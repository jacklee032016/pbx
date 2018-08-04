/**
 * @file IxCryptoAccQAccess_p.h
 *
 * @date October-03-2002
 *
 * @brief Private header file for Queue Access module. The only interface  
 *        to QMgr.
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


#ifndef IxCryptoAccQAccess_p_H
#define IxCryptoAccQAccess_p_H


/*
 * Os/System dependancies.
 */
#include "IxOsal.h"
#include "IxQMgr.h"
#include "IxQueueAssignments.h"
#include "IxCryptoAcc_p.h"


/*
 * #defines for function return types, etc.
 */
                     
/* Global variables */
extern IxCryptoAccStats ixCryptoStats;                     

/**
 * @typedef ixCryptoQCfgInfo
 * @brief Data structure for queue configuration
 *
 */
typedef struct 
{
   IxQMgrQId qId;
   INT8 *qName;
   IxQMgrCallback qCallback;
   IxQMgrCallbackId callbackTag;
   IxQMgrQSizeInWords qSize;
   IxQMgrQEntrySizeInWords qWords; 
   BOOL qNotificationEnableAtStartup;
   IxQMgrSourceId qConditionSource; 
   IxQMgrWMLevel AlmostEmptyThreshold;
   IxQMgrWMLevel AlmostFullThreshold;
} IxCryptoQCfgInfo;


/**
 * @fn ixCryptoQAccessInit
 *
 * @brief Queue configuration and callback registration function
 *
 * @param IxCryptoAccCfg [in] compCfg - IxCryptoAcc component configuration
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 * Reentrant     : no
 * ISR Callable  : no
 *
 */
IxCryptoAccStatus 
ixCryptoQAccessInit (IxCryptoAccCfg compCfg);


/**
 * @fn ixCryptoQAccessReqDoneQMgrCallback
 *
 * @brief Notification callback registered with QManager for crypto hardware
 *        acclerator service. Once the threshold of the interrupt source is 
 *        reached, the notification callback is triggered and is executed.
 *
 * @param IxQMgrId [in] queueId - Queue ID
 * @param IxQMgrCallbackId [in] callbackId - Notification callback ID
 *
 * @return none
 *
 */
void 
ixCryptoQAccessReqDoneQMgrCallback (
    IxQMgrQId queueId, 
    IxQMgrCallbackId callbackId);


/**
 * @fn      ixCryptoQAccessQueueWrite
 *
 * @brief   Write queue entry to hardware queue through QMgr
 *
 * @param IxQMgrId [in] queueId - Queue ID
 * @param UINT32* [out] pQEntry - Queue entry message
 *                                  31    - NPE operation status 
 *                                         (authentication Pass / Fail)
 *                                  30:28 - reserved
 *                                  27:0  - Q descriptor address
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_QUEUE_FULL
 *
 * Remove INLINE keyword to resolve cannot inline warning (vxWorks). SCR1421
 *
 */
IxCryptoAccStatus
ixCryptoQAccessQueueWrite (
    IxQMgrQId queueId,
    UINT32 *pQEntry);


/**
 * @fn      ixCryptoQAccessQueueStatusShow
 *
 * @brief   Show queue status
 *
 * @param IxQMgrId [in] queueId - Queue ID 
 *
 * @return IxCryptoAccStatus
 *             - IX_CRYPTO_ACC_STATUS_SUCCESS
 *             - IX_CRYPTO_ACC_STATUS_FAIL
 *
 */
IxCryptoAccStatus
ixCryptoQAccessQueueStatusShow(IxQMgrQId queueId);


/**
 * @fn ixCryptoQAccessWepReqDoneQMgrCallback
 *
 * @brief Notification callback registered with QManager for WEP service. Once  
 *        the threshold of the interrupt source is reached, the notification
 *        callback is triggered and is executed.
 *
 * @param IxQMgrId [in] queueId - Queue ID
 * @param IxQMgrCallbackId [in] callbackId - Notification callback ID
 *
 * @return none
 *
 */
void 
ixCryptoQAccessWepReqDoneQMgrCallback (
    IxQMgrQId queueId, 
    IxQMgrCallbackId callbackId);


#endif /* ndef IxCryptoAccQAccess_p_H */
