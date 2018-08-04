/**
 * @file IxCryptoAccCodelet.c
 *
 * @date October-31-2002
 *
 * @brief This file contains the implementation of the Crypto Access Codelet.
 *
 * Descriptions of the functions used in this codelet is contained in
 * IxCryptoAccCodelet_p.h
 *
 *
 * 
 * @par
 * IXP400 SW Release Crypto version 2.0
 * 
 * -- Intel Copyright Notice --
 * 
 * @par
 * Copyright 2002-2005 Intel Corporation All Rights Reserved.
 * 
 * @par
 * The source code contained or described herein and all documents
 * related to the source code ("Material") are owned by Intel Corporation
 * or its suppliers or licensors.  Title to the Material remains with
 * Intel Corporation or its suppliers and licensors.
 * 
 * @par
 * The Material is protected by worldwide copyright and trade secret laws
 * and treaty provisions. No part of the Material may be used, copied,
 * reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way except in accordance with the
 * applicable license agreement .
 * 
 * @par
 * No license under any patent, copyright, trade secret or other
 * intellectual property right is granted to or conferred upon you by
 * disclosure or delivery of the Materials, either expressly, by
 * implication, inducement, estoppel, except in accordance with the
 * applicable license agreement.
 * 
 * @par
 * Unless otherwise agreed by Intel in writing, you may not remove or
 * alter this notice or any other notice embedded in Materials by Intel
 * or Intel's suppliers or licensors in any way.
 * 
 * @par
 * For further details, please see the file README.TXT distributed with
 * this software.
 * 
 * @par
 * -- End Intel Copyright Notice --
 */

/*
 * System include files.
 */


#ifdef __vxworks
#include <end.h>    /* END drivers */
#include <endLib.h> /* END drivers */
#endif /* def __vxworks */

/*
 * User include files.
 */
#include "IxOsal.h" 
#include "IxQMgr.h"
#include "IxNpeDl.h"
#include "IxCryptoAccCodelet.h"
#include "IxCryptoAccCodelet_p.h"

#if defined(__wince) && defined(IX_USE_SERCONSOLE)
#include "IxSerConsole.h"
#define printf ixSerPrintf
#define gets ixSerGets
#endif


/** Bits in byte */
#define BITS_IN_BYTE    8

/** QMgr dispatcher function pointer */
PRIVATE IxQMgrDispatcherFuncPtr ixCryptoAccCodeletDispatcherFunc;

/** QMgr dispatcher active flag */
PRIVATE volatile BOOL ixCryptoAccCodeletDispatcherPollStop = FALSE;

/** Private functions prototype */
PRIVATE void ixCryptoAccCodeletRegisterCB (
            UINT32 cryptoCtxId,
            IX_OSAL_MBUF *pMbuf, 
            IxCryptoAccStatus status);

PRIVATE void ixCryptoAccCodeletPerformCB (
            UINT32 cryptoCtxId,
            IX_OSAL_MBUF *pSrcMbuf, 
            IX_OSAL_MBUF *pDestMbuf,
            IxCryptoAccStatus status);


/** Mbuf Pool */
PRIVATE IX_OSAL_MBUF *ixCryptoAccCodeletCryptoBufPool;
PRIVATE IX_OSAL_MBUF *ixCryptoAccCodeletCryptoFreeBufQHead = NULL;
PRIVATE IX_OSAL_MBUF *ixCryptoAccCodeletCryptoFreeBufQTail = NULL;
PRIVATE UINT8 *ixCryptoAccCodeletCryptoBufData;
PRIVATE UINT32 ixCryptoAccCodeletCryptoFreeBufQSize = 0;

/** Registration results  */
BOOL forwardRegisterCBCalled = FALSE;
BOOL reverseRegisterCBCalled = FALSE;

/** Callback function */
IxCryptoAccRegisterCompleteCallback registerCB;
IxCryptoAccPerformCompleteCallback performCB;

/** Length of MBuf payload (in bytes) */
PRIVATE UINT32 packetLength;

/** Performance log variables declaration 
 * Notes:
 * Throughput of the operation performed in codelets is calculated once every 
 * 1000 packets. The result is kept in the performanceLog array and is 
 * printed out after all the crypto perform service requests completed. Number
 * of requests is determined by the PERFORMANCE_WINDOW_SIZE. 1 unit in the 
 * window size represent 1000 requests (packets)
 */
PRIVATE UINT32 performanceLog [PERFORMANCE_WINDOW_SIZE];
PRIVATE UINT32 performanceNumEntries = 0;
PRIVATE UINT32 totalPacket = 0;
PRIVATE UINT32 timeStamp = 0;
PRIVATE UINT32 rateRatio;

/** Guard for once-only init code, when initialised this variable is set to 
 * TRUE 
 */
PRIVATE BOOL ixCryptoAccCodeletInitialised = FALSE;

/** Flag used when shutting down services, when TRUE, the function will 
 * shutdown 
 */
PRIVATE BOOL trafficFlowHalt = FALSE;

/* This flag is set when an error is returned by the ixCryptoApi while
 * doing perform operation. If this flag gets set then performance statistics
 * are not printed by the codelet.
 */
PRIVATE BOOL codeletPerformError = FALSE;

/** Initialised both CtxId to an invalid number as 0 (zero) is a valid ID.
 * The ID ranges from 0 to (IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS - 1), hence
 * IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS is invalid.
 */
PRIVATE UINT32 forwardCtxId = IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS;
PRIVATE UINT32 reverseCtxId = IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS;

/** Forward and reverse context declaration */
PRIVATE IxCryptoAccCtx forwardCtx, reverseCtx; 
INT32  codeletSrvIndex;

/** Random IV value and IV should remain constant on both
 * directions; encrypt and decrypt.
 */
#define IX_CRYPTO_ACC_CODELET_MAX_IV_LEN     IX_CRYPTO_ACC_AES_CCM_IV_512

PRIVATE UINT8 IV[IX_CRYPTO_ACC_CODELET_MAX_IV_LEN]
        = {  0,2,4,6,1,3,5,7,
             3,4,1,2,6,7,5,1,
             1,7,1,0,0,0,0,3,       
             3,4,3,4,4,5,6,7,       
             10,11,12,13,14,15,16,17,
             18,19,20,21,22,23,24,25,
             26,27,28,29,30,29,28,27,
             26,25,24,23,22,21,20,19};  

/** Random cipher key */
PRIVATE UINT8 cipherKey[IX_CRYPTO_ACC_MAX_CIPHER_KEY_LENGTH] 
        = {0x01,0x02,0x03,0x04,
           0x05,0x06,0x07,0x08,
           0x09,0x0A,0x0B,0x0C,
           0x0D,0x0E,0x0F,0x10,
           0x11,0x12,0x13,0x14,
           0x15,0x16,0x17,0x18,
           0x19,0x1A,0x1B,0x1C,
           0x1D,0x1E,0x1F,0x20};

/** SHA1 key from test case 1 in RFC 2202 */
PRIVATE UINT8 authKey[IX_CRYPTO_ACC_MAX_AUTH_KEY_LENGTH]
        = {0x0B,0x0B,0x0B,0x0B,
           0x0B,0x0B,0x0B,0x0B,
           0x0B,0x0B,0x0B,0x0B,
           0x0B,0x0B,0x0B,0x0B,
           0x0B,0x0B,0x0B,0x0B};

/** List of services configured for demonstration purposes */
IxCryptoCodeletServiceParam ixCryptoAccCodeletService[]={
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "DES(ECB) "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "DES(CBC) "
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "3DES(ECB) "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "3DES(CBC) "
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "AES(ECB) "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CBC_IV_128,
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "AES(CBC) "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CTR,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CTR_IV_128,
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                          /* authentication key size is ignored */
        0,                          /* Digest length is ignored */
        0,                          /* aad length is ignored */
        FALSE,                      /* Inplace mode of operation */
        FALSE,
        "AES(CTR) "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT_AUTH,
        IX_CRYPTO_ACC_OP_AUTH_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CCM,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CCM_IV_512 ,
        IX_CRYPTO_ACC_AUTH_CBC_MAC,
        0,                          /* authentication key size is ignored */
        IX_CRYPTO_ACC_CCM_DIGEST_64,
        IX_CRYPTO_ACC_CCM_AAD_LEN_384,
        FALSE,
        FALSE,
        "AES CCM for 802.11i"
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_ARC4,
        IX_CRYPTO_ACC_MODE_NULL,    /* cipher mode is ignored when using ARC4 */
        IX_CRYPTO_ACC_ARC4_KEY_128,
        IX_CRYPTO_ACC_ARC4_BLOCK_8,
        0,                         /* IV length is ignored */
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                        /* authentication key size is ignored */
        0,
        0,                        /* aad length is ignored */
        FALSE,
        TRUE,                     /* Codelet should use XScaleWepPerform
                                   * function to service this request.
                                   */
        "ARC4 on XScale"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_ARC4,
        IX_CRYPTO_ACC_MODE_NULL,    /* cipher mode is ignored when using ARC4 */
        IX_CRYPTO_ACC_ARC4_KEY_128,
        IX_CRYPTO_ACC_ARC4_BLOCK_8,
        0,                         /* IV length is ignored */
        IX_CRYPTO_ACC_AUTH_NULL,    /* Do not use any authentication algo */
        0,                        /* authentication key size is ignored */
        0,
        0,                        /* aad length is ignored */
        FALSE,
        FALSE,                    /* Codelet should use NpeWepPerform
                                   * function to service this request.
                                   */
        "ARC4 on WAN-NPE"
    },

    {
        IX_CRYPTO_ACC_OP_AUTH_CALC, /* Authentication only request */
        IX_CRYPTO_ACC_OP_AUTH_CHECK,
        IX_CRYPTO_ACC_CIPHER_NULL,  /* Do not use any cipher algorithm */
        IX_CRYPTO_ACC_MODE_NULL,    /* Cipher mode is ignored */
        0,                          /* Cipher key size is ignored */
        0,                          /* Cipher algorithm block size is ignored*/
        0,                          /* IV is ignored */
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "HMAC using MD5"
    },
    {
        IX_CRYPTO_ACC_OP_AUTH_CALC, /* Authentication only request */
        IX_CRYPTO_ACC_OP_AUTH_CHECK,
        IX_CRYPTO_ACC_CIPHER_NULL,  /* Do not use any cipher algorithm */
        IX_CRYPTO_ACC_MODE_NULL,    /* Cipher mode is ignored */
        0,                          /* Cipher key size is ignored */
        0,                          /* Cipher algorithm block size is ignored*/
        0,                          /* IV is ignored */
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "HMAC using SHA1"
    },

    {
        IX_CRYPTO_ACC_OP_AUTH_CALC, /* Authentication only request */
        IX_CRYPTO_ACC_OP_AUTH_CHECK,
        IX_CRYPTO_ACC_CIPHER_NULL,  /* Do not use any cipher algorithm */
        IX_CRYPTO_ACC_MODE_NULL,    /* Cipher mode is ignored */
        0,                          /* Cipher key size is ignored */
        0,                          /* Cipher algorithm block size is ignored*/
        0,                          /* IV is ignored */
        IX_CRYPTO_ACC_AUTH_WEP_CRC,
        0,                        /* authentication key size is ignored */
        IX_CRYPTO_ACC_WEP_CRC_DIGEST_32,
        0,                        /* aad length is ignored */
        FALSE,
        TRUE,                     /* Codelet should use XScaleWepPerform
                                   * function to service this request.
                                   */
        "WEP CRC on XScale"
    },
    {
        IX_CRYPTO_ACC_OP_AUTH_CALC, /* Authentication only request */
        IX_CRYPTO_ACC_OP_AUTH_CHECK,
        IX_CRYPTO_ACC_CIPHER_NULL,  /* Do not use any cipher algorithm */
        IX_CRYPTO_ACC_MODE_NULL,    /* Cipher mode is ignored */
        0,                          /* Cipher key size is ignored */
        0,                          /* Cipher algorithm block size is ignored*/
        0,                          /* IV is ignored */
        IX_CRYPTO_ACC_AUTH_WEP_CRC,
        0,                        /* authentication key size is ignored */
        IX_CRYPTO_ACC_WEP_CRC_DIGEST_32,
        0,                        /* aad length is ignored */
        FALSE,
        FALSE,                    /* Codelet should use NpeWepPerform
                                   * function to service this request.
                                   */
        "WEP CRC on WAN-NPE"
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "DES(ECB) with MD5 "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "DES(CBC) with MD5"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "DES(ECB) with SHA1"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_DES_KEY_64,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "DES(CBC) with SHA1"
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "3DES(ECB) with MD5 "
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "3DES(CBC) with MD5"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "3DES(ECB) with SHA1"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_3DES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_3DES_KEY_192,
        IX_CRYPTO_ACC_DES_BLOCK_64,
        IX_CRYPTO_ACC_DES_IV_64,
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "3DES(CBC) with SHA1"
    },

    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(ECB) with MD5"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CBC_IV_128,
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(CBC) with MD5"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CTR,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CTR_IV_128,
        IX_CRYPTO_ACC_AUTH_MD5,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_MD5_KEY_128,  /* Authentication Key size */
        IX_CRYPTO_ACC_MD5_DIGEST_128, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(CTR) with MD5"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_ECB,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        0,                          /* IV is ignored in ECB mode */
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(ECB) with SHA1"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CBC,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CBC_IV_128,
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(CBC) with SHA1"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT,  /* Encrypt only request */
        IX_CRYPTO_ACC_OP_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_AES,
        IX_CRYPTO_ACC_MODE_CTR,
        IX_CRYPTO_ACC_AES_KEY_128,
        IX_CRYPTO_ACC_AES_BLOCK_128,
        IX_CRYPTO_ACC_AES_CTR_IV_128,
        IX_CRYPTO_ACC_AUTH_SHA1,     /* Authentication algorithm is MD5 */
        IX_CRYPTO_ACC_SHA1_KEY_160,  /* Authentication Key size */
        IX_CRYPTO_ACC_SHA1_DIGEST_160, /* Digest length size */
        0,                           /* aad length is ignored */
        FALSE,                       /* Inplace mode of operation */
        FALSE,
        "AES(CTR) with SHA1"
    },

     {
        IX_CRYPTO_ACC_OP_ENCRYPT_AUTH,
        IX_CRYPTO_ACC_OP_AUTH_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_ARC4,
        IX_CRYPTO_ACC_MODE_NULL,    /* cipher mode is ignored when using ARC4 */
        IX_CRYPTO_ACC_ARC4_KEY_128,
        IX_CRYPTO_ACC_ARC4_BLOCK_8,
        0,                         /* IV length is ignored */
        IX_CRYPTO_ACC_AUTH_WEP_CRC,
        0,                        /* authentication key size is ignored */
        IX_CRYPTO_ACC_WEP_CRC_DIGEST_32,
        0,                        /* aad length is ignored */
        FALSE,
        TRUE,                     /* Codelet should use XScaleWepPerform
                                   * function to service this request.
                                   */
        "ARC4 + WEP CRC on XScale"
    },
    {
        IX_CRYPTO_ACC_OP_ENCRYPT_AUTH,
        IX_CRYPTO_ACC_OP_AUTH_DECRYPT,
        IX_CRYPTO_ACC_CIPHER_ARC4,
        IX_CRYPTO_ACC_MODE_NULL,    /* cipher mode is ignored when using ARC4 */
        IX_CRYPTO_ACC_ARC4_KEY_128,
        IX_CRYPTO_ACC_ARC4_BLOCK_8,
        0,                         /* IV length is ignored */
        IX_CRYPTO_ACC_AUTH_WEP_CRC,
        0,                        /* authentication key size is ignored */
        IX_CRYPTO_ACC_WEP_CRC_DIGEST_32,
        0,                        /* aad length is ignored */
        FALSE,
        FALSE,                    /* Codelet should use NpeWepPerform
                                   * function to service this request.
                                   */
        "ARC4 + WEP CRC on WAN-NPE"
    }
};


#define   IX_CRYPTOACC_CODELET_STOP           (-1)
#define   IX_CRYPTOACC_CODELET_SRV_LIST        0
#define   IX_CRYPTOACC_CODELET_MAX_SRV_INDEX    \
            (sizeof(ixCryptoAccCodeletService)  \
            /sizeof(IxCryptoCodeletServiceParam))


/**
 * @fn ixCryptoAccCodeletBufIsQEmpty (void)
 *
 * @brief This inline function is used to check whether the buffer
 *        queue is empty.
 *
 */
INLINE BOOL
ixCryptoAccCodeletBufIsQEmpty (void)
{
    BOOL status = FALSE;
    if (0 == ixCryptoAccCodeletCryptoFreeBufQSize)
    { 
        status = TRUE;
    }
    return status;
}


/**
 * @fn ixCryptoAccCodeletBufIsQFull (void)
 *
 * @brief This inline function is used to check whether the buffer
 *        queue is full.
 *
 */
INLINE BOOL
ixCryptoAccCodeletBufIsQFull(void)
{
    BOOL status = FALSE;
    if (IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE 
        == ixCryptoAccCodeletCryptoFreeBufQSize)
    { 
        status = TRUE;
    }
    return status;
}


/**
 * @fn ixCryptoAccCodeletCryptoMbufToQTailAdd (IX_OSAL_MBUF *pMbufToAdd)
 *
 * @brief This function adds a mbuf to the tail of the queue.
 *
 */
void ixCryptoAccCodeletCryptoMbufToQTailAdd (IX_OSAL_MBUF *pMbufToAdd)
{
    IX_OSAL_MBUF *pMbuf;
    
    if (!ixCryptoAccCodeletBufIsQFull())
    {
        pMbuf = ixCryptoAccCodeletCryptoFreeBufQTail;
        if (NULL != ixCryptoAccCodeletCryptoFreeBufQTail) 
        {
            /* Add pMbufToAdd in the last mbuf in the chain */
            IX_OSAL_MBUF_NEXT_PKT_IN_CHAIN_PTR(pMbuf) = pMbufToAdd;    
        } /* End of if ixCryptoAccCodeletCryptoFreeBufQTail */
    
        /* Update the ixCryptoAccCodeletFreeBufQ with the last mbuf */
        ixCryptoAccCodeletCryptoFreeBufQTail = pMbufToAdd;
        IX_OSAL_MBUF_NEXT_PKT_IN_CHAIN_PTR(pMbufToAdd) = NULL;
        ixCryptoAccCodeletCryptoFreeBufQSize++;
    } 
    else /* Buffer queue is full */
    {
        printf("\nBuffer Queue is full\n");
    } /* End of if-else ixCryptoAccCodeletBufIsQFull() */
} /* End of ixCryptoAccCodeletCryptoMbufToQTailAdd () function */


/**
 * @fn ixCryptoAccCodeletCryptoMbufFromQHeadRemove (IX_OSAL_MBUF **pMbufToRem)
 *
 * @brief This function removes a mbuf from the head of the queue.
 */
void ixCryptoAccCodeletCryptoMbufFromQHeadRemove(IX_OSAL_MBUF **pMbufToRem)
{
    if (!ixCryptoAccCodeletBufIsQEmpty())
    {
        *pMbufToRem = ixCryptoAccCodeletCryptoFreeBufQHead;
        ixCryptoAccCodeletCryptoFreeBufQHead 
            = IX_OSAL_MBUF_NEXT_PKT_IN_CHAIN_PTR((*pMbufToRem));
        
        /* Unchain the mbuf */
        IX_OSAL_MBUF_NEXT_PKT_IN_CHAIN_PTR(*pMbufToRem) = NULL;        

        ixCryptoAccCodeletCryptoFreeBufQSize--;
    } 
    else
    {
        *pMbufToRem = NULL;
        printf ("\nBuffer Queue is empty\n");   
    } /* end of if-else ixCryptoAccCodeletBufIsQEmpty */
} /* End of ixCryptoAccCodeletCryptoMbufFromQHeadRemove () function */


/**
 * @fn IX_STATUS ixCryptoAccCodeletCryptoMemPoolInit (UINT32 bufferSize)
 *
 * @brief Initialise Crypto Access Codelet MBUF pool
 */
IX_STATUS ixCryptoAccCodeletCryptoMemPoolInit (UINT32 bufferSize)
{
    UINT32 bufNo;
    IX_OSAL_MBUF *pMbuf;
    UINT32 mBlkSize = IX_OSAL_CACHE_LINE_SIZE * ((sizeof (IX_OSAL_MBUF) + 
                        (IX_OSAL_CACHE_LINE_SIZE - 1)) /
                        IX_OSAL_CACHE_LINE_SIZE);    
    UINT32 mDataSize = IX_OSAL_CACHE_LINE_SIZE * (( bufferSize + 
                        (IX_OSAL_CACHE_LINE_SIZE - 1)) /
                        IX_OSAL_CACHE_LINE_SIZE);

    ixCryptoAccCodeletCryptoBufPool = NULL;
    ixCryptoAccCodeletCryptoBufData = NULL;
    
    if (NULL == (ixCryptoAccCodeletCryptoBufPool 
                    = IX_CRYPTO_ACC_CODELET_DRV_DMA_MALLOC (
                        IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE
                        * mBlkSize)))
    {
        printf ("Error allocating mBuf pool for Crypto Codelet!\n");
        return IX_FAIL;
    }

    if (NULL == (ixCryptoAccCodeletCryptoBufData 
                    = IX_CRYPTO_ACC_CODELET_DRV_DMA_MALLOC(
                        IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE *
                        mDataSize)))
    {
        IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE (ixCryptoAccCodeletCryptoBufPool);

        printf ("Error allocating mBuf data pool for Crypto Codelet!\n");
        return IX_FAIL;
    }

    pMbuf = ixCryptoAccCodeletCryptoBufPool;
    /* Initilise mbuf pool head pointer */
    ixCryptoAccCodeletCryptoFreeBufQHead  = pMbuf;
    
    /* Format our mBufs */
    for (bufNo = 0;
        bufNo < IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE;
        bufNo++)
    {
        /* Assign memory block to mData pointer in mbuf */ 
        IX_OSAL_MBUF_MDATA (pMbuf) 
            = &ixCryptoAccCodeletCryptoBufData[bufNo * mDataSize];
    
        /* Initialise mbuf length */
        IX_OSAL_MBUF_MLEN(pMbuf) = mDataSize;
        
        /* No chains */
        IX_OSAL_MBUF_NEXT_BUFFER_IN_PKT_PTR(pMbuf)= NULL;
    
        /* Add newly formated buffer to our free queue */
        ixCryptoAccCodeletCryptoMbufToQTailAdd (pMbuf);
        pMbuf = (IX_OSAL_MBUF *)((UINT8 *) pMbuf + mBlkSize);
    }

    return IX_SUCCESS;
} /* End of ixCryptoAccCodeletCryptoMemPoolInit () function */


/**
 * @fn IX_STATUS ixCryptoAccCodeletCryptoMemPoolFree (UINT32 bufferSize)
 *
 * @brief Free all resources in Crypto Access Codelet MBUF pool
 *
 */
void ixCryptoAccCodeletCryptoMemPoolFree (UINT32 bufferSize)
{
    IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE (ixCryptoAccCodeletCryptoBufPool);

    IX_CRYPTO_ACC_CODELET_DRV_DMA_FREE (ixCryptoAccCodeletCryptoBufData);
    
    ixCryptoAccCodeletCryptoFreeBufQHead = NULL;
    ixCryptoAccCodeletCryptoFreeBufQTail = NULL;
    ixCryptoAccCodeletCryptoFreeBufQSize = 0;
} /* End of ixCryptoAccCodeletMemPoolFree () function */


/**
 * @fn void ixCryptoAccCodeletDispatcherPoll (void)
 *
 * @brief This function polls the queues when poll mode set in the 
 *        function ixCryptoAccCodeletDispatcherStart. 
 */
void ixCryptoAccCodeletDispatcherPoll (void)
{
    /* Set dispatcher stop flag to FALSE */
    ixCryptoAccCodeletDispatcherPollStop = FALSE;
    
    while (1)
    {
        /* Exit thread if stop flag is set to TRUE */
        if (ixCryptoAccCodeletDispatcherPollStop)
        {
            break;  /* Exit the thread */
        }

#ifndef __vxworks
        /* In linux platform, the QMgr dispatcher thread needs to be put into 
         * sleep mode for 1 milisecond after each loop, in order to allow  
         * others thread to run.
         */
        ixOsalSleep (1);
#endif /* ndef __vxworks */

        /* The crypto NPE uses queues 29 and 30. */
        (*ixCryptoAccCodeletDispatcherFunc) (IX_QMGR_QUELOW_GROUP);
    }
    
} /* End of function ixCryptoAccCodeletDispatcherPoll() */


/**
 * @fn ixCryptoAccCodeletDispatcherStart (BOOL useInterrupt)
 *
 * @brief  This function starts the Queue manager dispatcher.
 *
 * @return IX_STATUS 
 */
IX_STATUS ixCryptoAccCodeletDispatcherStart (BOOL useInterrupt)
{
    IxOsalThread dispatchtid;
    IxOsalThreadAttr threadAttr;
    char *pThreadName = "QMgr Dispatcher";

    /* Get QMgr dispatcher function pointer */
    ixQMgrDispatcherLoopGet (&ixCryptoAccCodeletDispatcherFunc);
    
    if (useInterrupt)   /* Interrupt mode */
    {
        printf ("\nStarting Interrupt mode for cryptoAcc!\n");
        
        /* Hook the QM QLOW dispatcher to the interrupt controller. 
         * The crypto NPE uses queues 29 and 30.
         */
        if (IX_SUCCESS != ixOsalIrqBind (IX_OSAL_IXP400_QM1_IRQ_LVL,
                              (IxOsalVoidFnVoidPtr)
                              ixCryptoAccCodeletDispatcherFunc,
                              (void *)IX_QMGR_QUELOW_GROUP))
        {
            printf ("Failed to bind to QM1 interrupt\n");
            return IX_FAIL;
        }
    }
    else  /* Poll mode */
    {
        printf ("\nStarting Polling mode for cryptoAcc!\n");
        
        /* Set attribute of thread */
        threadAttr.name = pThreadName;
        threadAttr.stackSize 
            = IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_THREAD_STACK_SIZE;
        threadAttr.priority = IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_PRIORITY;
        
        if (IX_SUCCESS != 
            ixOsalThreadCreate (
                &dispatchtid,
                &threadAttr,
                (IxOsalVoidFnVoidPtr) ixCryptoAccCodeletDispatcherPoll,
                NULL))
        {
            printf ("Error spawning dispatch task\n");
            return IX_FAIL;
        }

        if (IX_SUCCESS != 
            ixOsalThreadStart (&dispatchtid))
        {
            printf ("Error starting dispatch task\n");
            return IX_FAIL;
        }
    }

    return IX_SUCCESS;
} /* End of ixCryptoAccCodeletDispatcherStart () function */


/**
 * @fn ixCryptoAccCodeletRegisterCtx(
 *         INT32  srvIndex,
 *         IxCryptoAccCtx *forwardCtx,
 *         IxCryptoAccCtx *reverseCtx,
 *         UINT32 *forwardCtxId,
 *         UINT32 *reverseCtxId)
 *
 * @brief  This function registers to the Crypto Access Component for
 *         the specified service index. The srvIndex passed into this 
 *         function is an index into the ixCryptoAccCodeletService array.
 */
IX_STATUS ixCryptoAccCodeletRegisterCtx (
            INT32  srvIndex,
            IxCryptoAccCtx *pForwardCtx,
            IxCryptoAccCtx *pReverseCtx,
            UINT32 *pForwardCtxId,
            UINT32 *pReverseCtxId)
{
    IxCryptoAccStatus status;
    IX_OSAL_MBUF *pPrimaryMbuf = NULL;
    IX_OSAL_MBUF *pSecondaryMbuf = NULL;
    UINT16 i = 0;
    
    if ( (NULL == pForwardCtx)  || 
         (NULL == pReverseCtx)  ||
         (NULL == pForwardCtxId)  ||
         (NULL == pReverseCtxId))
    {
        printf ("\n Input Param is NULL ");
        return IX_FAIL;
    }

    /* Initialize forward and reverse contexts */
    pForwardCtx->operation = ixCryptoAccCodeletService[srvIndex].frwdOperation;
    pReverseCtx->operation = ixCryptoAccCodeletService[srvIndex].revOperation ;

    pForwardCtx->cipherCtx.cipherAlgo 
        = pReverseCtx->cipherCtx.cipherAlgo 
        = ixCryptoAccCodeletService[srvIndex].cipherAlgo;
        
    pForwardCtx->cipherCtx.cipherMode 
        = pReverseCtx->cipherCtx.cipherMode 
        = ixCryptoAccCodeletService[srvIndex].cipherMode;
        
    pForwardCtx->cipherCtx.cipherKeyLen 
        = pReverseCtx->cipherCtx.cipherKeyLen 
        = ixCryptoAccCodeletService[srvIndex].cipherKeyLen;
        
    pForwardCtx->cipherCtx.cipherBlockLen 
        = pReverseCtx->cipherCtx.cipherBlockLen 
        = ixCryptoAccCodeletService[srvIndex].cipherBlockLen;
        
    pForwardCtx->cipherCtx.cipherInitialVectorLen 
        = pReverseCtx->cipherCtx.cipherInitialVectorLen 
        = ixCryptoAccCodeletService[srvIndex].cipherInitialVectorLen;
        
    ixOsalMemCopy (pForwardCtx->cipherCtx.key.cipherKey,
        cipherKey,
        ixCryptoAccCodeletService[srvIndex].cipherKeyLen);

    ixOsalMemCopy (pReverseCtx->cipherCtx.key.cipherKey,
        cipherKey,
        ixCryptoAccCodeletService[srvIndex].cipherKeyLen);
    
    pForwardCtx->authCtx.authAlgo 
        = pReverseCtx->authCtx.authAlgo 
        = ixCryptoAccCodeletService[srvIndex].authAlgo;
        
    pForwardCtx->authCtx.authDigestLen 
        = pReverseCtx->authCtx.authDigestLen 
        = ixCryptoAccCodeletService[srvIndex].authDigestLen;
        
    pForwardCtx->authCtx.authKeyLen 
        = pReverseCtx->authCtx.authKeyLen 
        = ixCryptoAccCodeletService[srvIndex].authKeyLen;
        
    pForwardCtx->authCtx.aadLen
        = pReverseCtx->authCtx.aadLen
        = ixCryptoAccCodeletService[srvIndex].aadLen;

    ixOsalMemCopy (pForwardCtx->authCtx.key.authKey,
        authKey,
        ixCryptoAccCodeletService[srvIndex].authKeyLen);

    ixOsalMemCopy (pReverseCtx->authCtx.key.authKey,
        authKey,
        ixCryptoAccCodeletService[srvIndex].authKeyLen);
    
    pForwardCtx->useDifferentSrcAndDestMbufs 
        = pReverseCtx->useDifferentSrcAndDestMbufs 
        = ixCryptoAccCodeletService[srvIndex].useDifferentSrcAndDestMbufs;

    /* Primary Mbuf and Secondary Mbuf is required for authentication 
     * and combined service for forward context
     */
    if (( IX_CRYPTO_ACC_OP_AUTH_CALC 
        ==ixCryptoAccCodeletService[srvIndex].frwdOperation) || 
        ( IX_CRYPTO_ACC_OP_ENCRYPT_AUTH 
        == ixCryptoAccCodeletService[srvIndex].frwdOperation))
    {
        /* If auth algo is CBC MAC or WEP CRC then the primary and
         * secondary mbufs are not required and should be NULL.
         */
        if( ( IX_CRYPTO_ACC_AUTH_CBC_MAC 
            != ixCryptoAccCodeletService[srvIndex].authAlgo)
            &&( IX_CRYPTO_ACC_AUTH_WEP_CRC 
            !=ixCryptoAccCodeletService[srvIndex].authAlgo))
        {
            ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pPrimaryMbuf);
            ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pSecondaryMbuf);

            /* Check if the mbufs are NULL */
            if ((NULL == pPrimaryMbuf) || (NULL == pSecondaryMbuf))
            {
                printf ("\nUnable to allocate mbufs for chaining variables\n");
                return IX_FAIL;
            }
        }
    }

    /* Register the forward context */
    status = ixCryptoAccCtxRegister (
                 pForwardCtx,
                 pPrimaryMbuf,
                 pSecondaryMbuf,
                 registerCB,
                 performCB,
                 pForwardCtxId);
                 
    if (IX_CRYPTO_ACC_STATUS_SUCCESS != status)
    {
        printf ("Forward registration Failed. Status = %d\n", status);
        return IX_FAIL;
    }

    i = 0;

    /* Wait for the forward register callback to return */
    while (!forwardRegisterCBCalled)
    {
        ixOsalSleep (IX_CRYPTOACC_CODELET_REGISTER_WAIT_TIME);
        
        /* Wait for IX_CRYPTOACC_CODELET_MAX_TIME_WAIT times before return 
         * fail status 
         */
        if (IX_CRYPTOACC_CODELET_MAX_TIME_WAIT < i)
        {
            printf ("\nRegistration incomplete, timeout for forward callback\n");
            return IX_FAIL;
        }
        i++;
    } /* end of while (!forwardRegisterCBCalled) */

    /* Primary Mbuf and Secondary Mbuf are required for authentication 
     * and combined service for reverse context
     */
    if (( IX_CRYPTO_ACC_OP_AUTH_CALC 
        ==ixCryptoAccCodeletService[srvIndex].frwdOperation) || 
        ( IX_CRYPTO_ACC_OP_ENCRYPT_AUTH 
        == ixCryptoAccCodeletService[srvIndex].frwdOperation))
    {
        /* If auth algo is CBC MAC or WEP CRC then the primary and
         * secondary mbufs are not required and should be NULL.
         */
        if( ( IX_CRYPTO_ACC_AUTH_CBC_MAC 
            != ixCryptoAccCodeletService[srvIndex].authAlgo)
            &&( IX_CRYPTO_ACC_AUTH_WEP_CRC 
            !=ixCryptoAccCodeletService[srvIndex].authAlgo))
        {
            ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pPrimaryMbuf);
            ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pSecondaryMbuf);
    
            /* Check if the mbufs are NULL */
            if ((NULL == pPrimaryMbuf) || (NULL == pSecondaryMbuf))
            {
                printf ("\nUnable to allocate mbufs for chaining variables!\n");
                return IX_FAIL;
            }
        }
    }
    
    /* Register the reverse context */
    status = ixCryptoAccCtxRegister (
                 pReverseCtx,
                 pPrimaryMbuf,
                 pSecondaryMbuf,
                 registerCB,
                 performCB,
                 pReverseCtxId);
                 
    if (IX_CRYPTO_ACC_STATUS_SUCCESS != status)
    {
        printf ("Reverse registration Failed. Status = %d\n", status);
        return IX_FAIL;
    }

    i = 0;

    /* Wait for the reverse register callback to return */
    while (!reverseRegisterCBCalled)
    {
        ixOsalSleep (IX_CRYPTOACC_CODELET_REGISTER_WAIT_TIME);
        
        if (IX_CRYPTOACC_CODELET_MAX_TIME_WAIT < i)
        {
            printf ("\nRegistration incomplete, timeout for reverse callback\n");
            return IX_FAIL;
        }
        i++;
    } /* end of while (!reverseRegisterCBCalled) */
    
    return IX_SUCCESS;
} /* End of ixCryptoAccCodeletRegisterCtx () function */



/**
 * @fn ixCryptoAccCodeletPerform( UINT32 pktLen)
 *
 * @brief This function invokes the "appropriate" perform function
 *        of the cryptoAcc component. XScaleWepPerform function is invoked
 *        if the flag is set for the service index. This function uses 
 *        the global variable "codeletSrvIndex", set by the codeletMain 
 *        function to access the requested service's parameters.
 */
IX_STATUS ixCryptoAccCodeletPerform (UINT32 pktLen)
{
    IxCryptoAccStatus status;
    IX_OSAL_MBUF *pMbuf;
    UINT32 pktNo;
    UINT32 j;
    UINT8  data;

    if (IX_CRYPTO_ACC_CIPHER_AES 
        == ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo)
    {
        /* Exception: When the cipher mode is CCM, then there is no
         * restriction on the packet length being a multiple of
         * block size. 
         */
        if(IX_CRYPTO_ACC_MODE_CCM 
            != ixCryptoAccCodeletService[codeletSrvIndex].cipherMode)
        {
            /* Check if packet length is multiple of cipher block length */
            if (0 != (pktLen %  IX_CRYPTO_ACC_AES_BLOCK_128))
            {
	        printf ("\nPacket length is not mulitple of AES cipher "
                    "block length - 16 bytes.\n\n");
                ixCryptoAccCodeletUsageShow ();
                return IX_FAIL;
            }
        }
    }

    if ( (IX_CRYPTO_ACC_CIPHER_DES 
        ==  ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo)
        || (IX_CRYPTO_ACC_CIPHER_3DES 
        == ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo))
    {
        /* Check if packet length is multiple of cipher block length */
        if (0 != (pktLen %  IX_CRYPTO_ACC_DES_BLOCK_64))
        {
	    printf ("\nPacket length is not mulitple of DES cipher block "
                "length - 8 bytes.\n\n");
            ixCryptoAccCodeletUsageShow ();
            return IX_FAIL;
        }
    }
        
    /* If the cipher algorithm is ARC4, block length is 1 byte, therefore no
     * check is required on the packet length.
     */

    /* Check, if the request should be performed by XScaleWepPerform 
     * function. Note that the perform done call back is not called if
     * XScaleWepPerform function is invoked.
     */
    if(TRUE == ixCryptoAccCodeletService[codeletSrvIndex].invokeXScaleWepPerform )
    {
        /* Get an mbuf */
        ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pMbuf);

        /* Check if the mbuf is NULL */
        if (NULL == pMbuf)
        {
            printf ("\nUnable to allocate mbuf for data\n");
            return IX_FAIL;
        }
        
        /* copy data into mBuf from the ASCII table 0 (0x30) to z (0x7a)*/
        data = (char) 0x30;
        for (j = 0; j < pktLen; j++)
        {
            * (IX_OSAL_MBUF_MDATA(pMbuf) + j) = data++;             
            if ((char) 0x7b == data)
            {
                data =(char) 0x30;
            }
        }
       
        /* Call XScaleWepPerform function */
        status = ixCryptoAccCodeletXScaleWepPerform(pMbuf,pktLen);

        if(IX_SUCCESS!= status)
        {
            printf ("\n XScaleWepPeformed failed...!!!!");
            codeletPerformError= TRUE;
        }
        
	ixOsalSleep (5);  /* Added sleep to solve the RPC timeout problem */
        printf ("\nWaiting for operation done [<10 min for the biggest packet]\n");

        ixCryptoAccCodeletCryptoMbufToQTailAdd(pMbuf);
        
        /* Wait for all buffers being returned in callbacks */
        while (!ixCryptoAccCodeletBufIsQFull())
        {
            ixOsalSleep(IX_CRYPTOACC_CODELET_PERFORM_WAIT_TIME);
            printf (".");
        }
        
    }
    else
    {
        for(pktNo = 0; 
            pktNo < IX_CRYPTOACC_CODELET_CRYPTO_MBUF_POOL_SIZE;
            pktNo++)
        {
            /* Get an mbuf */
            ixCryptoAccCodeletCryptoMbufFromQHeadRemove (&pMbuf);

            /* Check if the mbuf is NULL */
            if (NULL == pMbuf)
            {
                printf ("\nUnable to allocate mbuf for data\n");
                return IX_FAIL;
            }
        
            /* copy data into mBuf from the ASCII table 0 (0x30) to z (0x7a)*/
            data = (char) 0x30;
            for (j = 0; j < pktLen; j++)
            {
                * (IX_OSAL_MBUF_MDATA(pMbuf) + j) = data++;             
                if ((char) 0x7b == data)
                {
                    data =(char) 0x30;
                }
            }
            /* This while loop will always call perform until it is  not 
             * successful but will continue to retry perform if the status 
             * is queue_full. Start with forwardCtxId and the reverseCtxId
             * will be used in the perform callback.
             */
            do
            {
                if (0 == pktNo)
                {
                    /* Start the timeStamp before Perform */
                    timeStamp = ixOsalTimestampGet();
                }
                if( IX_CRYPTO_ACC_CIPHER_ARC4 
                    != ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo
                    && IX_CRYPTO_ACC_AUTH_WEP_CRC 
                    != ixCryptoAccCodeletService[codeletSrvIndex].authAlgo)
                {
                    status = ixCryptoAccAuthCryptPerform (
                        forwardCtxId,
                        pMbuf,
                        NULL,
                        0,
                        pktLen,    
                        0, 
                        pktLen,
                        pktLen,
                        IV);
                }
                else
                {
                    /* If this request involves ARC4 or WEP_CRC, invoke 
                     * NPE wep perform function 
                     */
                    status= ixCryptoAccNpeWepPerform(forwardCtxId,
                                                     pMbuf,
                                                     NULL,
                                                     0, 
                                                     pktLen,
                                                     pktLen,
                                                     cipherKey);

                }
            
            } while (IX_CRYPTO_ACC_STATUS_QUEUE_FULL == status);
            /* End of do-while loop */
        
            printf ("Placed %dth Packet in to flow\n", pktNo + 1);

        } /* End of for (i) loop */
    
        printf ("\nWaiting for operation done [<10 min for the biggest packet]\n");

        /* Wait crypto services done. The task is put into sleep state while 
         * waiting for all the callbacks returned
         */        
        do
        {
            ixOsalSleep(IX_CRYPTOACC_CODELET_PERFORM_WAIT_TIME);            
        } while (!trafficFlowHalt);
    
        /* Wait for all buffers being returned in callbacks */
        while (!ixCryptoAccCodeletBufIsQFull())
        {
            ixOsalSleep(IX_CRYPTOACC_CODELET_PERFORM_WAIT_TIME);
            printf (".");
        }
    } /* end of else on operation type */
   
    ixOsalSleep (5);
    printf ("\nOperation DONE!\n");

    /* Unregister contexts */
    ixCryptoAccCodeletUnregisterCtx();

    return IX_SUCCESS;
}


/**
 * @fn ixCryptoAccCodeletUnregisterCtx (void)
 *
 * @brief  This function unregisters all the contexts registered.
 */
void ixCryptoAccCodeletUnregisterCtx (void)
{
    if (IX_CRYPTO_ACC_STATUS_SUCCESS != 
        ixCryptoAccCtxUnregister (forwardCtxId))
    {
        printf ("\nFailed to unregister forward context %d\n", forwardCtxId);
    }

    if (IX_CRYPTO_ACC_STATUS_SUCCESS != 
        ixCryptoAccCtxUnregister (reverseCtxId))
    {
        printf ("\nFailed to unregister reverse context %d\n", reverseCtxId);
    }

    forwardRegisterCBCalled = FALSE;
    reverseRegisterCBCalled = FALSE;
    
    /* Initialised to an invalid number, zero is a valid ID */
    forwardCtxId = IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS;
    reverseCtxId = IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS;
}

/**
 * @fn  ixCryptoAccCodeletInit (IxCryptoAccCfg ixCryptoAccCodeletCfg)
 * @brief This function initialises the required components depending upon
 *        the ixCryptoAccCodeletCfg.
 *
 */
PRIVATE IX_STATUS ixCryptoAccCodeletInit (IxCryptoAccCfg ixCryptoAccCodeletCfg)
{
    if (ixCryptoAccCodeletInitialised) 
    {
        printf ("\nCrypto codelet already initialised\n");
        return IX_SUCCESS;
    }
    
#ifdef __vxworks
    /* When the ixe drivers are running, the codelets
     * cannot run.
     */
    if (NULL != endFindByName ("ixe", 0))
    {
        printf ("FAIL : Driver ixe0 detected\n");
        return IX_FAIL;
    }
    if (NULL != endFindByName ("ixe", 1))
    {
        printf ("FAIL : Driver ixe1 detected\n");
        return IX_FAIL;
    }
#endif

    /* Initialise stop flag to TRUE */
    ixCryptoAccCodeletDispatcherPollStop = TRUE;

    /* Load NPE C image if required */
    if( (IX_CRYPTO_ACC_CFG_CRYPTO_NPE_ACC_EN == ixCryptoAccCodeletCfg) ||
        (IX_CRYPTO_ACC_CFG_CRYPTO_WEP_NPE_ACC_EN == ixCryptoAccCodeletCfg))
    {
        printf ("\nInitializing NPE C...");
        /* Initialise NPE C */
        if (IX_SUCCESS != ixNpeDlNpeInitAndStart (
            IX_CRYPTOACC_CODELET_NPEC_IMAGE_ID))
        {
            printf ("Error initialising NPE C for crypto processing\n");
            return IX_FAIL;
        }
        printf ("..done\n");
    }

    /* Load NPE A image if required */
    if( (IX_CRYPTO_ACC_CFG_WEP_NPE_ACC_EN == ixCryptoAccCodeletCfg ) ||
        (IX_CRYPTO_ACC_CFG_CRYPTO_WEP_NPE_ACC_EN == ixCryptoAccCodeletCfg))
    {
        printf ("\nInitializing NPE A...");
        /* Initialise NPE A */
        if (IX_SUCCESS != ixNpeDlNpeInitAndStart (
            IX_CRYPTOACC_CODELET_NPEA_IMAGE_ID))
        {
            printf ("Error initialising NPE A for WEP processing\n");
            return IX_FAIL;
        }
        printf ("..done\n");
    }

    /* Initialise Queue Manager */
    if (IX_SUCCESS != ixQMgrInit ())
    {
        printf ("Error initialising queue manager!\n");
        return IX_FAIL;
    }

    if(IX_SUCCESS != ixCryptoAccConfig( ixCryptoAccCodeletCfg))
    {
        printf ("Error failed to configure crypto acc component");
        return IX_FAIL;
    }

    /* Initialise cryptoAcc component */
    if (IX_CRYPTO_ACC_STATUS_SUCCESS != ixCryptoAccInit ())
    {
        printf ("Error initialising Crypto access component!\n");
        return IX_FAIL;
    }

    /* Start the Queue Manager dispatcher */   
    if ( IX_SUCCESS != ixCryptoAccCodeletDispatcherStart (
        IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE))
    {
        printf ("Error starting queue manager dispatcher!\n");
        return IX_FAIL;
    }
    
    /* set flag to TRUE to indicate codelet has been initialised successfully */
    ixCryptoAccCodeletInitialised = TRUE;
    printf ("\nIxCryptoAcc Codelet Initialization complete!\n\n");

    return IX_SUCCESS;
} /* End of ixCryptoAccCodeletInit () function */


/** 
 * @fn ixCryptoAccCodeletRegisterCB(
 *         UINT32 cryptoCtxId,
 *         IX_OSAL_MBUF *pMbuf, 
 *         IxCryptoAccStatus status)
 *
 * @brief  Cryptographic Context registration callback function to be called
 *         in QMgr Dispatcher context when the registration completed.
 *
 * @param "UINT32 [in] cryptoCtxId" - Crypto Context ID
 * @param "IX_OSAL_MBUF* [in] pMbuf" - mbuf pointer
 * @param "IxCryptoAccStatus [in] status" - status of callback
 *
 * @return None
 */
PRIVATE void ixCryptoAccCodeletRegisterCB (
        UINT32 cryptoCtxId,
        IX_OSAL_MBUF *pMbuf, 
        IxCryptoAccStatus status)
{
    BOOL *pFlagToUse;

    /* Determine callback flag to be used from the context crypto registered. 
     * If forward context, forwardRegisterCBCalled is used; otherwise 
     * reverseRegisterCBCalled is used.  
     */
    if (cryptoCtxId == forwardCtxId)
    {
        pFlagToUse = &forwardRegisterCBCalled;
    }
    else /* (cryptoCtxId != forwardCtxId) */
    {   
        if (cryptoCtxId == reverseCtxId)
        {
            pFlagToUse = &reverseRegisterCBCalled;
        }
        else
        {
            ixOsalLog (
                IX_OSAL_LOG_LVL_ERROR,
                IX_OSAL_LOG_DEV_STDERR,
                "\nInvalid ContextId %d\n", 
                cryptoCtxId, 0, 0, 0, 0, 0);
            return;
        } /* end of if-else (cryptoCtxId == reverseCtxId) */
    } /* end of if-else (cryptoCtxId == forwardCtxId) */

    /* We are expecting two callbacks for authentication and combined service 
     * The pMbuf could be null for encrypt/decrypt ONLY operation.
     */
    if (NULL != pMbuf)
    {
        /* Must return the buffer to the pool irrespective of the status */
        ixCryptoAccCodeletCryptoMbufToQTailAdd (pMbuf);
    }

    if (IX_CRYPTO_ACC_STATUS_SUCCESS == status)
    {
        *pFlagToUse = TRUE;
        ixOsalLog (
            IX_OSAL_LOG_LVL_MESSAGE, 
            IX_OSAL_LOG_DEV_STDOUT,
            "RegistrationCB is successful. Context %d\n", 
            cryptoCtxId, 
            0, 0, 0, 0, 0);
    } /* End of if (IX_CRYPTO_ACC_STATUS_SUCCESS == status) */
    else 
    {
        if (IX_CRYPTO_ACC_STATUS_WAIT == status)
        {
            ixOsalLog (
                IX_OSAL_LOG_LVL_MESSAGE, 
                IX_OSAL_LOG_DEV_STDOUT,
                "Register not completed yet; Context %d\n", 
                cryptoCtxId, 
                0, 0, 0, 0, 0);
        }
        else
        {
            ixOsalLog (
                IX_OSAL_LOG_LVL_WARNING,
                IX_OSAL_LOG_DEV_STDOUT, 
                "Registration failed; Context %d\n", 
                cryptoCtxId, 0, 0, 0, 0, 0);
        }
    } /* End of if else (IX_CRYPTO_ACC_STATUS_SUCCESS == status) */
} /* End of ixCryptoAccCodeletRegisterCB () function */


/**
 * @fn ixCryptoAccCodeletPerformCB (
 *        UINT32 cryptoCtxId,
 *        IX_OSAL_MBUF *pSrcMbuf, 
 *        IX_OSAL_MBUF *pDestMbuf,
 *        IxCryptoAccStatus status)
 *
 * @brief  Crypto services request callback fuction to be called when the 
 *         crypto service s request completed. Performance rate is calculated 
 *         in this function.
 *
 * @param "UINT32 [in] cryptoCtxId" - Crypto Context ID
 * @param "IX_OSAL_MBUF* [in] pSrcMbuf" - source mbuf pointer
 * @param "IX_OSAL_MBUF* [in] pDestMbuf" - destination mbuf pointer
 * @param "IxCryptoAccStatus [in] status" - status of callback
 *
 * @return None
 */
PRIVATE void ixCryptoAccCodeletPerformCB (
        UINT32 cryptoCtxId,
        IX_OSAL_MBUF *pSrcMbuf, 
        IX_OSAL_MBUF *pDestMbuf,
        IxCryptoAccStatus status)
{
    IxCryptoAccStatus retStatus;
    UINT32 timeNow;     /**< Current timestamp */
    UINT32 timeDiff;    /**< Time taken to process 1000 packets in XSCALE TICK 
                         * unit
                         */ 
    switch (status)
    {
        case IX_CRYPTO_ACC_STATUS_SUCCESS:
            if (IX_CRYPTOACC_CODELET_BATCH_LEN <= totalPacket)
            {
                timeNow = ixOsalTimestampGet();
                
                /* Check if the timer wrap over, get the time taken and  
                 * divided it by 66 to get microseconds as XScale tick is 66MHz 
                 */
                if (timeNow < timeStamp)
                {
                    timeDiff = ((0xffffffff - timeStamp + timeNow + 1)/
                        IX_CRYPTOACC_CODELET_XSCALE_TICK);
                }
                else
                {
                    timeDiff = ((timeNow - timeStamp) /
                        IX_CRYPTOACC_CODELET_XSCALE_TICK);
                } /* end of if-else (timeNow < timeStamp) */
                
                /* Store performance rate for every 1000 packets */
                if (PERFORMANCE_WINDOW_SIZE > performanceNumEntries)
                {
                    performanceLog[performanceNumEntries] = rateRatio / timeDiff;
                    performanceNumEntries++;
                }
        
                /* Reset totalPacket for another measurement */
                totalPacket = 0;
        
                /* Start the timeStamp */
                timeStamp = timeNow;
            }
            else
            {
                totalPacket++;
            } /* End of if-else totalPacket */
            break;
      
        case IX_CRYPTO_ACC_STATUS_AUTH_FAIL:
            ixOsalLog (
                IX_OSAL_LOG_LVL_WARNING,
                IX_OSAL_LOG_DEV_STDOUT,
                "Authentication Failed\n",
                0, 0, 0, 0, 0, 0);
            codeletPerformError = TRUE;
            break;
    
        case IX_CRYPTO_ACC_STATUS_FAIL:
            ixOsalLog (
                IX_OSAL_LOG_LVL_WARNING,
                IX_OSAL_LOG_DEV_STDOUT,
                "Operation Failed\n", 
                0, 0, 0, 0, 0, 0);
            codeletPerformError = TRUE;
            break;
    
        default:
            ixOsalLog (
                IX_OSAL_LOG_LVL_ERROR,
                IX_OSAL_LOG_DEV_STDERR,
                "Error in status message, %d\n", 
                status, 0, 0, 0, 0, 0);
            codeletPerformError = TRUE;
    } /* End of switch (status) */

    /* Sample size more than user-specified size, halt the traffic */    
    if (PERFORMANCE_WINDOW_SIZE <= performanceNumEntries)
    {
        trafficFlowHalt = TRUE;
    }

    /* If we are shutting down, return mbuf to the pool and return */
    if (trafficFlowHalt)
    {
        /* Return the buffer(s) to the pool */
        if (NULL != pSrcMbuf)
        {
            ixCryptoAccCodeletCryptoMbufToQTailAdd (pSrcMbuf);
        }
        if (NULL != pDestMbuf)
        {
            ixCryptoAccCodeletCryptoMbufToQTailAdd (pDestMbuf);
        }
        return;
    }

    /* Switch Contexts ID, based on the context ID to determine what operation 
     * should be done on the packet. If cryptoCtxId is a forwardCtxId, the 
     * packet is encrypted / authenticated, send the packet to NPE again for
     * decryption / auth_check, and vise-versa.
     */
    if (cryptoCtxId == forwardCtxId)
    {
        /* The perform will keep on trying to write to the queue and will wait
         * if the queue is full
         */
        do
        {
            if(IX_CRYPTO_ACC_CIPHER_ARC4 
                != ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo
                && IX_CRYPTO_ACC_AUTH_WEP_CRC 
                != ixCryptoAccCodeletService[codeletSrvIndex].authAlgo)
            {
                retStatus = ixCryptoAccAuthCryptPerform (
                    reverseCtxId,
                    pSrcMbuf,
                    pDestMbuf,
                    0,
                    packetLength,
                    0,
                    packetLength,
                    packetLength,
                    IV);
            }
            else
            {
               /* If this request involves ARC4 or WEP_CRC, invoke 
                * NPE wep perform function 
                */
               retStatus= ixCryptoAccNpeWepPerform(reverseCtxId, 
                                                   pSrcMbuf,
                                                   pDestMbuf,
                                                   0, 
                                                   packetLength,
                                                   packetLength,
                                                   cipherKey);
            }
        } while (IX_CRYPTO_ACC_STATUS_QUEUE_FULL == retStatus);
        /* End of do-while loop */
    }
    else    /* cryptoCtxId == reverseCtxId */
    {
        do
        {
            if( IX_CRYPTO_ACC_CIPHER_ARC4 
                != ixCryptoAccCodeletService[codeletSrvIndex].cipherAlgo
                && IX_CRYPTO_ACC_AUTH_WEP_CRC 
                != ixCryptoAccCodeletService[codeletSrvIndex].authAlgo)
            {
                retStatus = ixCryptoAccAuthCryptPerform (
                            forwardCtxId,
                            pSrcMbuf,
                            pDestMbuf,
                            0,
                            packetLength,
                            0,
                            packetLength,
                            packetLength,
                            IV);    
           }else
           {
               /* If this request involves ARC4 or WEP_CRC, invoke 
                * NPE wep perform function 
                */
               retStatus= ixCryptoAccNpeWepPerform(forwardCtxId,
                                                   pSrcMbuf,
                                                   pDestMbuf,
                                                   0, 
                                                   packetLength,
                                                   packetLength,
                                                   cipherKey);

           }
                 
        } while (IX_CRYPTO_ACC_STATUS_QUEUE_FULL == retStatus);
        /* End of while loop */
    } /* End of if else (cryptoCtxId == forwardCtxId) */
} /* End of ixCryptoAccCodeletPerformCB () function */


/**
 * @fn ixCryptoAccCodeletMain (
 *          INT32 srvIndex,
 *          UINT32 packetLen)
 *
 * @brief  This is the entry point function to the codelet to choose the 
 *         operation for the codelet and packet length to be used. 
 *         This is the main function of the codelet where crypto contexts
 *         registration and crypto perform services are done. Based on the  
 *         selected operation and selected packet length, packets are sent to 
 *         cryptoAcc for processing. 
 */
IX_STATUS ixCryptoAccCodeletMain (
               INT32 srvIndex,
               UINT32 packetLen)
{
    UINT32 i;
    UINT32 bufferSize; 

    if(IX_CRYPTOACC_CODELET_SRV_LIST == srvIndex) 
    {
        ixCryptoAccCodeletUsageShow ();
        return IX_SUCCESS;
    }
    
    if(IX_CRYPTOACC_CODELET_STOP == srvIndex)
    {
        /* Stop the codelet if it was initialized */
        if (ixCryptoAccCodeletInitialised) 
        {
            ixCryptoAccCodeletDispatcherStop (
                IX_CRYPTOACC_CODELET_QMGR_DISPATCHER_MODE);
            ixCryptoAccCodeletInitialised = FALSE;
            printf ("\n Codelet Halted");
            return IX_SUCCESS;
        }
        
        return IX_SUCCESS;
    }

    if( (IX_CRYPTOACC_CODELET_MAX_SRV_INDEX+1) <= (UINT32)srvIndex)
    {
        printf ("\nInvalid operation type selected!\n\n");
        ixCryptoAccCodeletUsageShow ();
        return IX_FAIL;
    }

    if ((IX_CRYPTOACC_CODELET_MAX_CRYPTO_PKT_SIZE < packetLen) 
        || (IX_CRYPTOACC_CODELET_MIN_CRYPTO_PKT_SIZE > packetLen))
    {
       printf ("\n Packetlength should  %d <= packetLength <= %d",
            IX_CRYPTOACC_CODELET_MIN_CRYPTO_PKT_SIZE,
            IX_CRYPTOACC_CODELET_MAX_CRYPTO_PKT_SIZE);

       return IX_FAIL;
    }    

    codeletSrvIndex = srvIndex-1;
    
    /* Initialize NPE's, Qmgr and IxCryptoAcc component */
    if (IX_SUCCESS !=  ixCryptoAccCodeletInit(IX_CRYPTOACC_CODELET_CRYPTOACC_CFG))
    {
        printf ("Initialisation Failed.\n");
        return IX_FAIL;
    }
    
    ixOsalSleep (5);
    registerCB = ixCryptoAccCodeletRegisterCB;
    performCB = ixCryptoAccCodeletPerformCB;
    
    printf ("************************************************************\n");
    printf ("\tSecurity Hardware Accelerator Codelet\n");
    printf ("************************************************************\n\n");
    ixOsalSleep (5);

    /* Initialize codeletPerformError flag */
    codeletPerformError = FALSE;

    /* Store packet length into global variable for the use in perform CB */
    packetLength = packetLen;

    /* Calculate the buffer size. It's the sum of packet length and
     * auth digest length (if it's combined mode of operation).
     */

    bufferSize = packetLen ;

    if (IX_CRYPTO_ACC_OP_ENCRYPT !=
          ixCryptoAccCodeletService[codeletSrvIndex].frwdOperation)
    {
        bufferSize += ixCryptoAccCodeletService[codeletSrvIndex].authDigestLen;
    }

    /* Precompute the rate ratio used performance calculation */
    rateRatio = (bufferSize * BITS_IN_BYTE * IX_CRYPTOACC_CODELET_BATCH_LEN);
   
    /* Alloc Mbufs pool */
    if (IX_SUCCESS 
        != ixCryptoAccCodeletCryptoMemPoolInit (bufferSize))
    {
        printf ("Mbuf pool init failed\n");
        return IX_FAIL;
    }

    /* Reset performance entries counter */
    performanceNumEntries = 0;
    
    /* Reset stop flag */
    trafficFlowHalt = FALSE;

    /* Registration */
    if (IX_SUCCESS != ixCryptoAccCodeletRegisterCtx(
        codeletSrvIndex,
        &forwardCtx,
        &reverseCtx,
        &forwardCtxId,
        &reverseCtxId))
    {
        printf ("\nRegistration Failed.\n");

        /* Free resources */
        ixCryptoAccCodeletCryptoMemPoolFree(bufferSize);

        return IX_FAIL;
    }

    if( IX_SUCCESS == ixCryptoAccCodeletPerform(packetLength))
    {
        if( TRUE != codeletPerformError)
        {
	    ixOsalSleep (10);
            /* Dump performance data */
            for (i = 0; i < PERFORMANCE_WINDOW_SIZE; i++)
            {
                printf ("[%2d] The rate is %d Mbits/sec\n", 
                    i, performanceLog[i]);
            }
        }
        else
        {
            printf ("\n Perform callback had errors.Statistics not printed");
        }
    }else
    {
        printf ("\n ixCryptoAccCodeletPerform failed. Statistics not printed");
    }

    /* Free resources */
    ixCryptoAccCodeletCryptoMemPoolFree(bufferSize);

    return IX_SUCCESS;

} /* end of function ixCryptoAccCodeletMain () */


/**
 * @fn ixCryptoAccCodeletXScaleWepPerform 
 * @brief This function calls the ixCryptoAccXScaleWepPerform
 * function for doing ARC4/WEP_CRC requests on XScale. It also gathers
 * performance statistics.
 */

PRIVATE IX_STATUS ixCryptoAccCodeletXScaleWepPerform(
                    IX_OSAL_MBUF *pMbuf,
                    UINT32 wepPacketLength)
{
    UINT32 timeNow;     /**< Current timestamp */
    UINT32 timeDiff;    /**< Time taken to process 1000 packets in XSCALE TICK 
                         * unit
                         */ 
    IxCryptoAccStatus status ;

    /* Check if pMbuf is NULL */
    if(NULL == pMbuf )
    {
        printf ("\n Mbuf is NULL");
        return IX_CRYPTO_ACC_STATUS_FAIL;
    }

    /* Fill all the entries in the performance window */
    for(performanceNumEntries=0;
        performanceNumEntries< PERFORMANCE_WINDOW_SIZE;
        performanceNumEntries++)
    {
        /* Get the current time stamp before starting the batch */
        timeStamp = ixOsalTimestampGet();

        for( totalPacket=0; totalPacket < IX_CRYPTOACC_CODELET_BATCH_LEN; 
             totalPacket+=2)
        {
            status= ixCryptoAccXScaleWepPerform(forwardCtxId,
                                                pMbuf,
                                                NULL,
                                                0, 
                                                wepPacketLength,
                                                wepPacketLength,
                                                cipherKey);
            
            /* Check whether the request invocation was success or not */
            if(IX_CRYPTO_ACC_STATUS_SUCCESS == status)
            {
                status= ixCryptoAccXScaleWepPerform(reverseCtxId,
                                                pMbuf,
                                                NULL,
                                                0, 
                                                wepPacketLength,
                                                wepPacketLength,
                                                cipherKey);

                /* Check whether the request invocation was success or not */
                if(IX_CRYPTO_ACC_STATUS_SUCCESS != status)
                {
                    printf ("\n XScaleWepPerform failed in the reverse direction ");
                    return status;
                }
            }
            else
            {
                printf ("\n XScaleWepPerform failed with status = %d", status);
                return status;
            }
        }/* end of For(totalPacket) */
        timeNow = ixOsalTimestampGet();
                
        /* Check if the timer wrap over, get the time taken and  
         * divided it by 66 to get microseconds as XScale tick is 66MHz 
         */
        if (timeNow < timeStamp)
        {
            timeDiff = ((0xffffffff - timeStamp + timeNow + 1)/
                        IX_CRYPTOACC_CODELET_XSCALE_TICK);
        }
        else
        {
            timeDiff = ((timeNow - timeStamp) /
                        IX_CRYPTOACC_CODELET_XSCALE_TICK);
        } /* end of if-else (timeNow < timeStamp) */
                
        /* Store performance rate for every 1000 packets */
        if (PERFORMANCE_WINDOW_SIZE > performanceNumEntries)
        {
            performanceLog[performanceNumEntries] = rateRatio / timeDiff;
        }

    }/* end of performanceEntries */
    
    return IX_SUCCESS;
}

/**
 * @fn ixCryptoAccCodeletDispatcherStop (BOOL useInterrupt)
 *
 * @brief Stop QMgr dispatcher thread if QMgr dispatcher runs in poll mode 
 *        or unbind QMgr dispatcher from interrupt if it runs in interrupt mode.
 *
 */
IX_STATUS ixCryptoAccCodeletDispatcherStop (BOOL useInterrupt)
{
    if (useInterrupt)   /* Interrupt mode */
    {
        /* 
         * Unhook the QM QLOW dispatcher from the interrupt controller. 
         */
        if (IX_SUCCESS != ixOsalIrqUnbind (IX_OSAL_IXP400_QM1_IRQ_LVL))
        {
            printf ("Failed to unbind to QM1 interrupt\n");
            return IX_FAIL;
        }
    }
    else /* poll mode */
    {
        if (!ixCryptoAccCodeletDispatcherPollStop)
        {
            /* Set stop flag to TRUE to stop the thread */
            ixCryptoAccCodeletDispatcherPollStop = TRUE;
        }
    } /* end of if (useInterrupt) */
    return (IX_SUCCESS);
} /* end of ixCryptoAccCodeletDispatcherStop () */


/**
 * @fn ixCryptoAccCodeletUsageShow (void)
 *
 * @brief Display user guide for Linux platform or VxWorks platform
 */
void ixCryptoAccCodeletUsageShow (void)
{
    UINT32 i;
    printf ("\nCrypto Access Component Codelet User Guide\n");
    printf ("==========================================\n\n");
    
#ifdef __vxworks    
    printf ("  >ixCryptoAccCodeletMain (serviceIndex, packetLen)\n\n");
#elif __wince
    printf ("  Provide operationType and  packetLen\n\n");
#else
    printf ("  >insmod ixp400_codelets_cryptoAcc.o serviceIndex=<serviceIndex> "
        "packetLen=<packetLen>\n\n");
#endif

    printf ("\n%4d : Stop the codelet\n", IX_CRYPTOACC_CODELET_STOP);    
    printf ("\n%4d : List configurations available\n", IX_CRYPTOACC_CODELET_SRV_LIST);

    for(i=0 ; i <IX_CRYPTOACC_CODELET_MAX_SRV_INDEX; i++)
    { 
        printf ("%4d : %s\n", i+1,ixCryptoAccCodeletService[i].infoString);
    }
    printf ("\n\n");
    
} /* end of ixCryptoAccCodeletUsageShow () */


#ifdef __wince
int readNumber(void)
{
    char line[256];
    gets(line);
    return atoi(line);
}

int    wmain(int argc, WCHAR **argv)
{
    int cryptoAccCodeletOperationNr,cryptoAccCodeletPacketLength;
    BOOL cryptoAccCodeletRun = TRUE;
    
    ixCryptoAccCodeletUsageShow ();
    
    while(cryptoAccCodeletRun)
    {
        printf("\n");
        printf("Enter operation number: ");
        cryptoAccCodeletOperationNr = readNumber();
    
        /* Stops the codelet. */
        if (IX_CRYPTOACC_CODELET_STOP == cryptoAccCodeletOperationNr)
        {
            ixCryptoAccCodeletMain(cryptoAccCodeletOperationNr, 
                IX_CRYPTOACC_CODELET_STOP);
        }
        /* Lists the set of services demonstrated */
        else
        if (IX_CRYPTOACC_CODELET_SRV_LIST == cryptoAccCodeletOperationNr)
        {
            ixCryptoAccCodeletMain(cryptoAccCodeletOperationNr, 
                IX_CRYPTOACC_CODELET_SRV_LIST);
        }
        /* Execute services between service Index 1 to 30 */
        else
        if ((IX_CRYPTOACC_CODELET_SRV_LIST < cryptoAccCodeletOperationNr) &&
            (IX_CRYPTOACC_CODELET_MAX_SRV_INDEX >= cryptoAccCodeletOperationNr))
        {
            printf("Enter packet length: ");
            cryptoAccCodeletPacketLength = readNumber();
            ixCryptoAccCodeletMain(cryptoAccCodeletOperationNr, 
                cryptoAccCodeletPacketLength);
        }
        else
        {
            printf("\nInvalid service index -- exit the codelet!\n");
            cryptoAccCodeletRun = FALSE;
        }
    }
}
#endif

/* End of IxCryptoAccCodelet.c */

