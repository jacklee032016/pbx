/**
 * @file IxCryptoAccCryptoPerform_p.h
 *
 * @date 03-October-2002
 *
 * @brief Header file for the perform module.
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


#ifndef IXCRYPTOACCCRYPTOPERFORM_P_H
#define IXCRYPTOACCCRYPTOPERFORM_P_H

/*
 * Os/System dependancies.
 */
#include "IxOsal.h"

/*
 * Intermodule dependancies
 */


/*
 * #defines for function return types, etc.
 */
 
/* Number of functions for WEP Xscale Assembly functionality */
#define IX_CRYPTO_ACC_NUM_WEP_FUNCTION    6


/**
 * @brief structure to store WEP computaion state information
 *
 */
typedef struct {
    UINT8   *pSi;
    UINT32  sboxIndex;
    UINT32  *pCrcTable;
    UINT32  crcValue;
} IxCryptoPerformWepState;


/**
 * @brief Function prototype for XScale Assembly WEP function 
 *
 * @param   UINT8 [in] *pSbox - pointer to Sbox for ARC4 computation
 * @param   IX_OSAL_MBUF [in] *pMbuf - source Mbuf pointer.
 * @param   IxCryptoPerformWepState [in] *pWepState - State for ongoing CRC 
 *          computations, including the current CRC value and a pointer to the 
 *          CRC LUT.
 */
typedef void (*IxCryptoPerformWepFunction) (
    UINT8   *pSbox,
    IX_OSAL_MBUF *pMbuf,
    IxCryptoPerformWepState *pWepState);


/* Assembly code functions wrapper prototypes */

/**
 * @fn      ARC4_InitSbox
 * @breif   Initialize Sbox for ARC4 computation 
 * @note    Please refer to IxCryptoPerformWepFunction for details
 */
void ARC4_InitSbox (
    UINT8 *pSbox,
    UINT8 *pKey);


/**
 * @fn      ARC4_Crypt
 * @breif   ARC4 encryption/decryption wrapper 
 * @note    Please refer to IxCryptoPerformWepFunction for details
 *
 */
void ARC4_Crypt (
    UINT8   *pSbox,
    IX_OSAL_MBUF *pMbuf,
    IxCryptoPerformWepState *pWepState);


/**
 * @fn      ARC4_EncryptWithCRC
 * @breif   ARC4 encryption with CRC wrapper 
 * @note    Please refer to IxCryptoPerformWepFunction for details
 *
 */
void ARC4_EncryptWithCRC (
    UINT8   *pSbox,
    IX_OSAL_MBUF *pMbuf,
    IxCryptoPerformWepState *pWepState);


/**
 * @fn      ARC4_DecryptWithCRC
 * @breif   ARC4 decryption with CRC wrapper
 * @note    Please refer to IxCryptoPerformWepFunction for details
 *
 */
void ARC4_DecryptWithCRC (
    UINT8   *pSbox,
    IX_OSAL_MBUF *pMbuf,
    IxCryptoPerformWepState *pWepState);


/**
 * @fn      CRC_Compute
 * @breif   CRC computation function
 * @note    Please refer to IxCryptoPerformWepFunction for details.
 *          pSbox will not be used in the function, it is included
 *          only for consistency with others XScale assembly 
 *          functions.     
 *
 */
void CRC_Compute (
    UINT8   *pSbox,
    IX_OSAL_MBUF *pMbuf,
    IxCryptoPerformWepState *pWepState);
    

/**
 *
 * @fn ixCryptoPerformBufferOffsetBoundaryCheck
 *
 * @brief   To check the boundary condition of the buffer offset provided by
 *          the client for crypto hardware accelerator service request.
 *
 * @note This function will ensure that the boundary conditions are satisfied:
 *       Auth offset <= Crypt offset <= Crypt offset + Crypt length <= Auth 
 *       offset + Auth length.
 *       It also checks that offsets/lengths given fit inside mBufs passed.
 *       The boundary check in this function will be activated under DEBUG mode
 *       ONLY in order to optimize the performance in the data path.
 *
 * @param   UINT32 [in] npeOperation - NPE Operation in NPE operation mode of
 *          struct IxCryptoNpeOperationMode.
 * @param   IX_OSAL_MBUF [in] *pSrcMbuf - Source Mbuf pointer.
 * @param   IX_OSAL_MBUF [in] *pDesMbuf - Destination Mbuf pointer.
 * @param   UINT16 [in] AuthStartOffset - Start address of the data to be 
 *          authenticated.
 * @param   UINT16 [in] AuthDataLen - The length of data to be authenticated.
 * @param   UINT16 [in] CryptStartOffset - Start address of the data to be 
 *          encrypted/decrypted.
 * @param   UINT16 [in] CryptDataLen - The length of data to be en/decrypted.
 * @param   UINT16 [in] icvOffset - Start offset of the ICV. Validated only in
 *                      CCM Mode.
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - successfully checked the 
 *              valid buffer offset and length.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - one of the parameters is invalid.
 *
 */

IxCryptoAccStatus 
ixCryptoPerformBufferOffsetBoundaryCheck (
    UINT32 npeOperation,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16 authStartOffset,
    UINT16 authDataLen,
    UINT16 cryptStartOffset,
    UINT16 cryptDataLen,
    UINT16 icvOffset);

/**
 *
 * @fn ixCryptoPerformCipherBlkLengthCheck
 *
 * @brief   To check the data length against the pre-determined cipher block
 *          length (8 or 16 bytes) stated in the Security Hardware Accelerator 
 *          Function Spec.
 *  
 * @note This function will ensure the correct block size for symmetric 
 *       en/decryption. The size for DES is 8 bytes while for AES' block size 
 *       is of 16 bytes.
 *       The block length check in this function will be activated under DEBUG
 *       mode ONLY in order to optimize the performance in the data path.
 *
 * @param   UINT32 [in] npeOperation - NPE Operation in NPE operation mode of
 *          struct IxCryptoNpeOperationMode.
 * @param   UINT16 [in] cipherBlkLen - the length of the cipher block.
 * @param   UINT16 [in] cryptStartOffset - the crypt start offset.
 * @param   UINT16 [in] cryptDataLen - the length of the data or plaintext..
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Successful block length check.
 * @return @li  IX_CRYPTO_ACC_STATUS_CIPHER_INVALID_BLOCK_LEN - Invalid block
 *              length for encryption.
 *
 */

IxCryptoAccStatus 
ixCryptoPerformCipherBlkLengthCheck (
    UINT32 npeOperation,
    UINT16 cipherBlkLen,
    UINT16 cryptStartOffset,    
    UINT16 cryptDataLen);


/**
 *
 * @fn ixCryptoPerformWepBufferOffsetBoundaryCheck
 *
 * @brief   To check the boundary condition of the buffer offset provided by
 *          the client for WEP service request.
 *
 * @note This function will ensure that the boundary conditions are satisfied:
 *       (start offset + data length <= ICV Offset) && (data length > 0).
 *       It also checks that offsets/lengths given fit inside mBufs passed.
 *       The boundary check in this function will be activated under DEBUG mode
 *       ONLY in order to optimize the performance in the data path.
 *
 * @param   UINT32 [in] npeOperation - NPE Operation in NPE operation mode of
 *          struct IxCryptoNpeOperationMode.
 * @param   IX_OSAL_MBUF [in] *pSrcMbuf - Source Mbuf pointer.
 * @param   IX_OSAL_MBUF [in] *pDesMbuf - Destination Mbuf pointer.
 * @param   UINT16 [in] startOffset - Start address of the data to be 
 *          encrypted/decrypted.
 * @param   UINT16 [in] dataLen - The length of data to be en/decrypted.
 * @param   UINT16 [in] icvOffset - Start address of the ICV field.
 * @param   UINT16 [in] icvLen - ICV length / CRC-32 result length
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - successfully checked the 
 *              valid buffer offset and length.
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - one of the parameters is invalid.
 *
 */
IxCryptoAccStatus 
ixCryptoPerformWepBufferOffsetBoundaryCheck (
    UINT32 npeOperation,
    IX_OSAL_MBUF *pSrcMbuf,
    IX_OSAL_MBUF *pDestMbuf,
    UINT16 startOffset,
    UINT16 dataLen,
    UINT16 icvOffset,
    UINT16 icvLen);
    
    
/**
 *
 * @fn ixCryptoPerformXScaleWepPerform
 *
 * @brief   Performs ARC4 cryption and/or WEP ICV (32-bit CRC) computation.
 *
 * @note    If ARC4 encryption and WEP ICV computation are simulateously
 *          selected, the ICV is generated, appended to the mbuf (the
 *          caller must insure that sufficient space exists at the end
 *          of the mbuf), and then encrypted along with the rest of the
 *          message.  If ARC4 decryption and WEP ICV computation are
 *          simultaneously selected, the entire message is decrypted,
 *          including the ICV, a CRC is computed over the message, and
 *          the resulting CRC is compared to the ICV field.
 *
 * @param   UINT32 [in] wepOperation - WEP opcode.
 * @param   IX_OSAL_MBUF [in] *pMbuf - source (and destination) Mbuf pointer.
 * @param   UINT16 [in] wepStartOffset - Start address of the data to be 
 *          processed.
 * @param   UINT16 [in] wepDataLength - Length of data to be processed.
 * @param   UINT16 [in] wepIcvOffset - Start address of ICV
 * @param   UINT8 [in] *pKey - pointer to ARC4 per packet key (must
 *          aligned to a 32-byte boundary).
 * @param   UINT8 [in] *pSbox - pointer to a pre-allocated S-box (must
 *          256 bytes in length and aligned to a 512-byte boundary).
 *
 * @return @li  IX_CRYPTO_ACC_STATUS_SUCCESS - Operation completed 
 *              successfully (for reverse direction CRC operations, this
 *              implies a valid CRC).
 * @return @li  IX_CRYPTO_ACC_STATUS_FAIL - Operation failed.
 * @return @li  IX_CRYPTO_ACC_STATUS_AUTH_FAIL - CRC verification failed.
 *
 */
IxCryptoAccStatus 
ixCryptoPerformXScaleWepPerform (
    UINT32    wepOperation,
    IX_OSAL_MBUF  *pMbuf,
    UINT16    wepStartOffset,
    UINT16    wepDataLength,
    UINT16    wepIcvOffset,
    UINT8    *pKey,
    UINT8    *pSbox);
    
#endif /* ifndef IXCRYPTOACCCRYPTOPERFORM_P_H */



