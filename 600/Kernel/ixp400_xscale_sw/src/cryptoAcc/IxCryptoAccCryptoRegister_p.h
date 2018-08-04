/**
 * @file IxCryptoAccCryptoRegister_p.h
 *
 * @date October-03-2002
 *
 * @brief  Private header file for Crypto Register Module
 *
 *
 * Design Notes:
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


#ifndef IxCryptoAccCryptoRegister_p_H
#define IxCryptoAccCryptoRegister_p_H


/*
 * Put the user defined include files required.
 */
#include "IxOsal.h"
#include "IxCryptoAcc_p.h"


#define IX_CRYPTO_HMAC_IPAD_VALUE     0x36    /**< ipad value specified in 
                                               * RFC 2104 
                                               */
#define IX_CRYPTO_HMAC_OPAD_VALUE     0x5C    /**< opad value specified in 
                                               * RFC 2104 
                                               */
#define IX_CRYPTO_AES128_KEY_LEN_IN_WORDS  4  /**< 128-bit key (4 words)
                                               */
#define IX_CRYPTO_AES192_KEY_LEN_IN_WORDS  6  /**< 192-bit key (6 words)
                                               */
#define IX_CRYPTO_AES256_KEY_LEN_IN_WORDS  8  /**< 256-bit key (8 words)
                                               */
#define IX_CRYPTO_NPE_CRYPT_CFG_CIPHER_MODE_POS  12  /**< Cipher mode position
                                                      * in NPE Crypt Config
                                                      */ 
#define IX_CRYPTO_NPE_CRYPT_CFG_CIPHER_ALGO_POS  11  /**< Cipher Algo position
                                                      * in NPE Crypt Config
                                                      */ 
#define IX_CRYPTO_NPE_CRYPT_CFG_ENCRYPT_POS      10  /**< Encrypt dir position
                                                      * in NPE Crypt Config
                                                      */ 
#define IX_CRYPTO_NPE_CRYPT_CFG_CRYPT_MODE_POS   8   /**< Crypt mode position
                                                      * in NPE Crypt Config
                                                      */ 
                                                

/**
 * @fn ixCryptoRegisterCipherAlgoRegister
 *
 * @brief This function is responsible to register the cipher algorithm 
 *        selected by client to the IxCryptoAcc software component. 
 *        Parameters to the algorithm will be checked against the cipher 
 *        algorithm standard. If the parameters provided are not compliant
 *        to the standard or not supported by IxCryptoAcc software 
 *        component, error will be reported to the client.
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID points to Crypto
 *        Context in CCD databse.
 * @param IxCryptoAccCtx* [in] pAccCtx - Crypto context parameters needed 
 *        in registration
 * @param UINT32* [inout] npeCryptoParamIndex - Index to the array of NPE 
 *        Crypto Param structure array.
 * @param IxCryptoNpeOperationStatus [in] operStatus - NPE operation status 
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *             - IX_CRYPTO_ACC_STATUS_CIPHER_ALGO_NOT_SUPPORTED
 *             - IX_CRYPTO_ACC_STATUS_CIPHER_MODE_NOT_SUPPORTED
 *             - IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_KEY_LEN
 *             - IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_IV_LEN
 *             - IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_BLOCK_LEN
 *
 */
IxCryptoAccStatus
ixCryptoRegisterCipherAlgoRegister (
    UINT32 cryptoCtxId,
    IxCryptoAccCtx *pAccCtx,
    UINT32 *npeCryptoParamIndex,
    IxCryptoNpeOperationStatus operStatus); 
 
 
 
/**
 * @fn ixCryptoRegisterAuthAlgoRegister
 *
 * @brief This function is responsible to register the authentication 
 *        algorithm selected by client to the IxCryptoAcc software 
 *        component. Parameters to the algorithm will be checked against 
 *        the authentication algorithm standard. If the parameters provided
 *        are not compliant to the standard or not supported by IxCryptoAcc
 *        software component, error will be reported to the client.
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID points to Crypto
 *        Context in CCD databse.
 * @param IxCryptoAccCtx* [in] pAccCtx - Crypto context parameters needed 
 *        in registration
 * @param IX_OSAL_MBUF* [in] pMbufPrimaryChainVar - Empty mbuf for the use of
 *        primary chaining variable generation. This mbuf cannot be NULL
 *        if selected authentication algorithm is HMAC-SHA1 or HMAC-MD5. 
 *        Otherwise, the mbuf pointer must be NULL if WEP-CRC is selected.
 * @param IX_OSAL_MBUF* [in] pMbufSecondaryChainVar - Empty mbuf for the use of
 *        secondary chaining variable generation. This mbuf cannot be NULL
 *        if selected authentication algorithm is HMAC-SHA1 or HMAC-MD5. 
 *        Otherwise, the mbuf pointer must be NULL if WEP-CRC is selected.
 * @param UINT32* [inout] npeCryptoParamIndex - Index to the array of NPE 
 *        Crypto Param structure array.
 * @param IxCryptoNpeOperationStatus [in] operStatus - NPE operation status
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *             - IX_CRYPTO_ACC_STATUS_AUTH_ALGO_NOT_SUPPORTED
 *             - IX_CRYPTO_ACC_STATUS_AUTH_INVALID_DIGEST_LEN
 *             - IX_CRYPTO_ACC_STATUS_AUTH_INVALID_KEY_LEN
 *
 */
IxCryptoAccStatus
ixCryptoRegisterAuthAlgoRegister (
    UINT32 cryptoCtxId,
    IxCryptoAccCtx *pAccCtx,
    IX_OSAL_MBUF *pMbufPrimaryChainVar,
    IX_OSAL_MBUF *pMbufSecondaryChainVar,
    UINT32 *npeCryptoParamIndex,
    IxCryptoNpeOperationStatus operStatus);

 
 
/**
 * @fn ixCryptoRegisterChainVarMbufPrepare
 *
 * @brief Prepare input for NPE to generate initial chaining variables,
 *        the input are stored in the mbufs. Hash key is padded to 64 bytes 
 *        and each byte is XORed with 0x36 for primary chaining variables. 
 *        (key XORed with ipad, where ipad is the byte 0x36 repeated 64 
 *        times). Hash key is padded to 64 bytes and each byte is XORed with 
 *        0x5C for secondary chaining variables. (key XORed with opad, where 
 *        opad is the byte 0x5C repeated 64 times).Please refer to RFC2104
 *        (HMAC: Keyed-Hashing for Message Authentication) for details.
 *
 * @param IX_OSAL_MBUF* [in] pMbufPrimaryChainVar - Empty mbuf for the use of
 *        primary chaining variable generation.
 * @param IX_OSAL_MBUF* [in] pMbufSecondaryChainVar - Empty mbuf for the use of
 *        secondary chaining variable generation. 
 * @param IxCryptoAccCtx* [in] pAccCtx - Crypto context parameters needed 
 *        in registration
 *
 * @return none
 *
 * 
 */
void
ixCryptoRegisterChainVarMbufPrepare (
    IX_OSAL_MBUF *pMbufPrimaryChainVar,
    IX_OSAL_MBUF *pMbufSecondaryChainVar,
    IxCryptoAccCtx *pAccCtx);

 
 /**
 * @fn ixCryptoRegisterChainVariablesGenerate
 *
 * @brief Generate primary or secondary chaining variables (depends on the
 *        input of mbuf) through NPE. The result of computation will be 
 *        stored in data structure IxCryptoNpeCryptoParam, which is 
 *        associated with the cryptoCtxId. Only one initial chaining 
 *        variable is computed per each function call.
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID points to Crypto
 *        Context in CCD databse.
 * @param IX_OSAL_MBUF* [in] pMbufChainVar - Empty mbuf for the use of chaining
 *        variable generation.
 * @param UINT32 [in] initLength - Length of initial values in NPE Crypto
 *        Param structure
 * @param UINT32 [in] npeCryptoParamAddr - Start address of NPE Crypto 
 *        Param structure
 * @param UINT32 [in] chainVarAddr - Address for NPE to store the result of
 *        chaining variable computation
 * @param IxCryptoNpeOperationStatus [in] operStatus - NPE operation status
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_QUEUE_FULL
 *
 * 
 */
IxCryptoAccStatus
ixCryptoRegisterChainVariablesGenerate (
    UINT32 cryptoCtxId,
    IX_OSAL_MBUF *pMbufChainVar,
    UINT32 initLength,
    UINT32 npeCryptoParamAddr,
    UINT32 chainVarAddr,
    IxCryptoNpeOperationStatus operStatus);


 
 /**
 * @fn ixCryptoRegisterRevAesKeyGenerate
 *
 * @brief Generate reverse AES key needed for AES decryption operation 
 *        through the NPE. The result of computation will be stored in data
 *        structure IxCryptoNpeCryptoParam, which is associated with the 
 *        cryptoCtxId.
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID points to Crypto
 *        Context in CCD databse.
 * @param UINT32 [in] initLength - Length of initial values in NPE Crypto
 *        Param structure
 * @param UINT32 [in] fwdAesKeyAddr - Address for forward AES key
 * @param UINT32 [in] keyLength - AES key length in bytes
 * @param IxCryptoNpeOperationStatus [in] operStatus - NPE operation status
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_QUEUE_FULL
 *
 * 
 */
IxCryptoAccStatus
ixCryptoRegisterRevAesKeyGenerate (
    UINT32 cryptoCtxId,
    UINT32 initLength,
    UINT32 fwdAesKeyAddr,
    UINT32 keyLength,
    IxCryptoNpeOperationStatus operStatus);



#endif /* IxCryptoAccCryptoRegister_p_H */
