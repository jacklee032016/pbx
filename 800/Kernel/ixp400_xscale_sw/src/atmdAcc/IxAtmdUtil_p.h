/**
 * @file IxAtmdUtil_p.h
 *
 * @author Intel Corporation
 * @date 17 March 2002
 *
 * @brief IxAtmdAcc Misc utility functions
 *
 * This file contains the prototypes of misceallaneous functiobs
 * used in IxAtmdAcc
 * @li a function to manage UniqueIds
 * @li a function to compute the standard CCITT-16 crc
 * @li a function to retrive the chained mbufs after
 *   npe processing and the symmetrical function to process
 *   mbufs before submitting them to noe
 * @li utility functions to send commands to the npe through
 *   the npemh interface.
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

#ifndef IXATMDUTIL_P_H
#define IXATMDUTIL_P_H

#include "IxAtmdAcc.h"
#include "IxQMgr.h"
#include "IxNpeMh.h"

/**
* @brief Util initialisations
*/
IX_STATUS
ixAtmdAccUtilInit (void);

/**
* @brief Util stats display
*/
PUBLIC void
ixAtmdAccUtilStatsShow (void);

/**
* @brief Util stats reset
*/
PUBLIC void
ixAtmdAccUtilStatsReset (void);

/**
*  @brief QMgr Queues Initialisation
*/
IX_STATUS
ixAtmdAccUtilQueuesInit (void);

/**
* @brief Hash a port/vpi/vci according to ISO 3309
*/
unsigned int
ixAtmdAccUtilHashVc (IxAtmLogicalPort port,
                 unsigned int vpi,
                 unsigned int vci);
/**
* @brief Allocate a unique ID, used for connections security and sanity checks
*/
unsigned int
ixAtmdAccUtilUniqueIdGet (void);

/**
* @brief Set a QMgr callback
*/
IX_STATUS
ixAtmdAccUtilQmgrCallbackSet (IxQMgrQId qId,
                          unsigned int loThresholdLevel,
                          unsigned int hiThresholdLevel,
                          IxQMgrSourceId sourceId,
                          IxQMgrCallback callback,
                          IxQMgrCallbackId callbackId,
                          IxQMgrPriority priority);

/**
* @brief convert chained mbufs to NPE format
*
* This conversion includes
* @li local Endian to BIG endian
* @li virtual address space processing
* @li memory cache processing (flush memory)
*/
IX_OSAL_MBUF *
ixAtmdAccUtilMbufToNpeFormatConvert (IX_OSAL_MBUF* mbufPtr);

/**
* @brief convert chained mbufs from NPE format
*
* This conversion includes
* @li BIG endian to local endian
* @li virtual address space processing
* @li memory cache processing (invalidate memory, optional)
*/
IX_OSAL_MBUF *
ixAtmdAccUtilMbufFromNpeFormatConvert (IX_OSAL_MBUF* mbufPtr,
                                       BOOL invalidateCache);

/**
* @brief convert RX chained mbufs from NPE format
*
* This conversion includes
* @li BIG endian to local endian
* @li virtual address space processing
* @li memory cache processing (invalidate memory)
* @li length handling (first mbuf header and last mbuf length)
*/
IX_OSAL_MBUF *
ixAtmdAccUtilRxMbufFromNpeFormatConvert (IX_OSAL_MBUF* mbufPtr, 
                                         unsigned int pduLength,
                                         unsigned int *mbufCount);

/**
* @brief QMgr Id check and conversion to IxAtmdAcc enum
*/
IX_STATUS
ixAtmdAccUtilAtmdQIdGet (IxQMgrQId qMgrQueueId,
                         IxAtmRxQueueId *atmdQueueIdPtr);

/**
* @brief IxAtmRxQueueId check and conversion to QMgr enum
*/
IX_STATUS
ixAtmdAccUtilQmgrQIdGet (IxAtmRxQueueId atmdQueueId,
                         IxQMgrQId* qMgrQueueIdPtr);

/**
* @brief send to the NPE a disable message for this channel
*/
IX_STATUS
ixAtmdAccUtilNpeRxDisableSend(unsigned int npeVcId);

/**
* @brief send to the NPE an enable message for this channel
*/
IX_STATUS
ixAtmdAccUtilNpeRxEnableSend(unsigned int npeVcId);

/**
* @brief send a message to the NPE
*
* @param msgType (in) message type (shifted and ored with the first parameter)
* @param param0 (in) msg first parameter
* @param param1 (in) msg second parameter
*
* @return @li IX_SUCCESS the message is sent
* @return @li IX_FAIL the message is not sent
*/
IX_STATUS
ixAtmdAccUtilNpeMsgSend(unsigned int msgType,
                        unsigned int param0,
                        unsigned int param1);

/** ----------------------------------------------------
* @brief send a message to the NPE, wait for a response
*
* @param msgType (in) message type (shifted and ored with the first parameter)
* @param param0 (in) msg first parameter
* @param param1 (in) msg second parameter
* @param callback (in) msg callback
*
* @return @li IX_SUCCESS the message is sent
* @return @li IX_FAIL the message is not sent
*/
IX_STATUS
ixAtmdAccUtilNpeMsgWithResponseSend(unsigned int msgType,
                                    unsigned int param0,
                                    unsigned int param1,
                                    IxNpeMhCallback callback);

/**
* @brief  prototype to  get the txdone queue size
*
* @param numberOfPdusPtr (out) specifies the queue size
*
* @return @li IX_SUCCESS the size of the tx queue is retrieved
* @return @li IX_FAIL an unexpected error occured
*
*/
IX_STATUS
ixAtmdAccUtilQueueSizeQuery (IxQMgrQId qmgrQueueId,
                            unsigned int *queueSizePtr);

#endif /* IXATMDUTIL_P_H */


