/**
 * @file IxCryptoAccCCDMgmt_p.h
 *
 * @date October-03-2002
 *
 * @brief  Private header file for Crypto Context database(CCD) management 
 *         module
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


#ifndef IxCryptoAccCCDMgmt_p_H
#define IxCryptoAccCCDMgmt_p_H


/*
 * Put the user defined include files required.
 */
#include "IxOsal.h"
#include "IxCryptoAcc_p.h"
#include "IxCryptoAccQAccess_p.h"


/* NPE Crypto Param structure size */
#define IX_CRYPTO_NPE_CRYPTO_PARAM_SIZE \
            (((sizeof (IxCryptoNpeCryptoParam) +  \
            (IX_OSAL_CACHE_LINE_SIZE - 1)) /    \
            IX_OSAL_CACHE_LINE_SIZE) *          \
            IX_OSAL_CACHE_LINE_SIZE )

/* Crypto Context Size */
#define IX_CRYPTO_CTX_SIZE  ((sizeof (IxCryptoAccCryptoCtx)) + \
                                IX_CRYPTO_NPE_CRYPTO_PARAM_SIZE)

/* Extra Crypto param structure needed for hash key and reverse AES key
 * generation
 */
#define IX_CRYPTO_ACC_KEY_CRYPTO_PARAM_NUM (IX_CRYPTO_ACC_NUM_OF_Q *\
                                                 IX_QMGR_Q_SIZE64)

/*
 * Global variables
 */
extern IxCryptoAccCryptoCtx ixCryptoCtx[IX_CRYPTO_ACC_MAX_ACTIVE_SA_TUNNELS];
                                            /**< Crypto Context database */
extern IxCryptoAccKeyCryptoParam ixKeyCryptoParam[IX_CRYPTO_ACC_KEY_CRYPTO_PARAM_NUM];
                         /**< Extra NPE Crypto Param for key generation */
                                                 

/**
 * @fn ixCryptoCCDMgmtInit
 *
 * @brief Initialize CCD management module. A pool of memory will be 
 *        allocated in this function to hold the information 
 *        (IxCryptoNpeCryptoParam) shared across NPE and IxCryptoAcc 
 *        software component. The CCD database will be initialized. 
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoCCDMgmtInit (void);



/**
 * @fn ixCryptoCCDMgmtCryptoCtxGet
 *
 * @brief Get a Crypto Context buffer from the CCD pool. The CryptoCtxId
 *        will be returned via the pCtxId pointer. CryptoCtxId will serve
 *        as index of the array to the CCD database.
 *
 * @param UINT32* [out] pCtxId - Crypto Context ID pointer
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_EXCEED_MAX_TUNNELS
 *
 */
IxCryptoAccStatus 
ixCryptoCCDMgmtCryptoCtxGet (UINT32 *pCtxId);



/**
 * @fn ixCryptoCCDMgmtCryptoCtxRelease
 *
 * @brief Release Crypto Context
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoCCDMgmtCryptoCtxRelease (UINT32 cryptoCtxId);



/**
 * @fn      ixCryptoCCDMgmtKeyCryptoParamGet
 *
 * @brief   Get a key ID from key crypto param pool. The keyId will serve
 *          as index of the array to the key crypto param database.
 *
 * @param UINT32* [out] pKeyId - Key ID pointer
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoCCDMgmtKeyCryptoParamGet (UINT32 *pKeyId);



/**
 * @fn      ixCryptoCCDMgmtKeyCryptoParamRelease
 *
 * @brief   Release Crypto Param structure back to the key crypto param pool.
 *
 * @param UINT32 [in] keyId - Key ID
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoCCDMgmtKeyCryptoParamRelease (UINT32 keyId);



/**
 * @fn ixCryptoCCDMgmtCryptoCtxReleaseAll
 *
 * @brief Release all Crypto Contexts
 *
 * @return none
 *
 */
void
ixCryptoCCDMgmtCryptoCtxReleaseAll (void);



/**
 * @fn ixCryptoCCDMgmtCtxValidCheck
 *
 * @brief To check whether the CryptoCtxId provided is valid and whether 
 *        the Crypto Context has been registered.
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_CRYPTO_CTX_NOT_VALID
 *
 *
 * Remove INLINE keyword to resolve cannot inline warning (vxWorks). SCR1421
 */
IxCryptoAccStatus 
ixCryptoCCDMgmtCtxValidCheck (UINT32 cryptoCtxId);



/**
 * @fn ixCryptoCCDMgmtShow
 *
 * @brief To show number of Crypto Contexts have been registered
 *
 * @return none
 *
 */
void
ixCryptoCCDMgmtShow (void);



/**
 * @fn ixCryptoCCDMgmtCryptoCtxShow
 *
 * @brief To show contents of Crypto Context which has been registered
 *
 * @param UINT32 [in] cryptoCtxId - Crypto Context ID
 *
 * @return none
 *
 */
void
ixCryptoCCDMgmtCryptoCtxShow (UINT32 cryptoCtxId);



/**
 * @fn ixCryptoCCDMgmtNpeCryptoParamPoolFree
 *
 * @brief To free the memory allocated to NPE crypto param pool through 
 *        malloc function
 *
 * @return none
 *
 */
void 
ixCryptoCCDMgmtNpeCryptoParamPoolFree (void);



/**
 * @def IX_CRYPTO_CTX_IS_ALL_TASK_DONE
 *
 * @brief To check if there is any pending task in the queues for the 
 *        Crypto Context specified. If TRUE, no pending tasks.
 *
 * @param UINT32 [in] reqCount - Number of requests have been issued
 * @param UINT32 [in] reqDoneCount - Number of requests have been completed
 *
 * @return TRUE/FALSE
 */
#define IX_CRYPTO_CTX_IS_ALL_TASK_DONE(reqCount, reqDoneCount)\
            (reqCount == reqDoneCount)


#endif /* IxCryptoAccCCDMgmt_p_H */
