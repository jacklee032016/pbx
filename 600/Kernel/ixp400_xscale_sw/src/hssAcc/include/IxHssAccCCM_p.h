/**
 * @file IxHssAccCCM_p.h
 *
 * @author Intel Corporation
 * @date 11-DEC-2001
 * 
 * @brief This file contains the private API of the HSS Channelised
 * Connection Manager module
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

/**
 * @defgroup IxHssAccCCM_p IxHssAccCCM_p
 *
 * @brief The private API for the HssAccess Channelised Connection Manager
 * module
 * 
 * @{ */

#ifndef IXHSSACCCCM_P_H
#define IXHSSACCCCM_P_H

#include "IxHssAcc.h"

/**
 * Prototypes for interface functions.
 */

/**
 * #defines for function return types, etc.
 */

/**
 * @def IX_HSSACC_CHAN_TXBYTES_PER_BLK_DIV
 *
 * @brief Channelised TX bytes per blk divisor
 */
#define IX_HSSACC_CHAN_TXBYTES_PER_BLK_DIV     4

/**
 * @def IX_HSSACC_CHAN_TXBYTES_PER_BLK_MIN
 *
 * @brief Channelised TX bytes per blk minimum size
 */
#define IX_HSSACC_CHAN_TXBYTES_PER_BLK_MIN     16

/**
 * @def IX_HSSACC_CHAN_TXBYTES_PER_BLK_MAX
 *
 * @brief Channelised TX bytes per blk maximum size
 */
#define IX_HSSACC_CHAN_TXBYTES_PER_BLK_MAX     48

/**
 * @struct IxHssAccCCMParams
 * @brief Structure containing HSS channelised connection parameters
 *
 */
typedef struct
{
    unsigned bytesPerTSTrigger; /**< NPE->Xscale trigger rate : 8 = 1ms */
    UINT8 *rxCircular;          /**< address of memory to be used for rx data */
    unsigned numRxBytesPerTS;   /**< memory depth per TS at rxCircular */
    UINT32 *txPtrList;          /**< address of memory containing tx ptrs */
    unsigned numTxPtrLists;     /**< depth of tx ptr list */
    unsigned numTxBytesPerBlk;  /**< memory depth per tx ptr within txPtrList */
    IxHssAccChanRxCallback rxCallback; /**< client callback for returning rx 
					  info */
    IxHssAccChanRxCallback tmpRxCallback; /**< temporary storage to keep client
					     callback for returning rx info */
} IxHssAccCCMParams;

/**
 * @fn IX_STATUS ixHssAccCCMConnect (IxHssAccHssPort hssPortId, 
		    unsigned bytesPerTSTrigger, 
		    UINT8 *rxCircular, 
		    unsigned numRxBytesPerTS, 
		    UINT32 *txPtrList, 
		    unsigned numTxPtrLists, 
		    unsigned numTxBytesPerBlk, 
		    IxHssAccChanRxCallback rxCallback)
 *
 * @brief This function takes the client specified parameters, configures
 * them appropriately and communicates them to NPE-A
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two identical ports (0-1).
 * @param unsigned bytesPerTSTrigger (in) - The NPE will trigger the access
 * component after bytesPerTSTrigger have been received for all trunk
 * timeslots. This figure must be a multiple of 8 e.g. 8 for 1ms trigger,
 * 16 for 2ms trigger.
 * @param UINT8 *rxCircular (in) - A pointer to memory allocated by the
 * client to be filled by data received. The buffer at this address is part
 * of a pool of buffers to be accessed in a circular fashion.
 * @param unsigned numRxBytesPerTS (in) - The number of bytes allocated per
 * timeslot within the receive memory. This figure will depend on the
 * latency of the system. It needs to be deep enough for data to be read by
 * the client before the NPE re-writes over that memory e.g. if the client
 * samples at a rate of 40bytes per timeslot, numRxBytesPerTS may need to
 * be 40bytes * 3. This would give the client 3 * 5ms of time before
 * received data is over-written.
 * @param UINT32 *txPtrList (in) - The address of an area of contiguous
 * memory allocated by the client to be populated with pointers to data for
 * transmission. Each pointer list contains a pointer per active channel
 * @param unsigned numTxPtrLists (in) - The number of pointer lists in
 * txPtrList. This figure is dependent on jitter.
 * @param unsigned numTxBytesPerBlk (in) - The size of the Tx data, in
 * bytes, that each pointer within the txPtrList points to.
 * @param IxHssAccChanRxCallback rxCallback (in) - A client function
 * pointer to be called back to handle the actual tx/rx of channelised
 * data. If this is not NULL, an ISR will call this function. If this
 * pointer is NULL, it implies that the client will use a polling mechanism
 * to detect when the tx/rx of channelised data is to occur. The client
 * will use hssChanAccStatus for this.
 *
 * @return IX_STATUS
 */
IX_STATUS 
ixHssAccCCMConnect (IxHssAccHssPort hssPortId, 
		    unsigned bytesPerTSTrigger, 
		    UINT8 *rxCircular, 
		    unsigned numRxBytesPerTS, 
		    UINT32 *txPtrList, 
		    unsigned numTxPtrLists, 
		    unsigned numTxBytesPerBlk, 
		    IxHssAccChanRxCallback rxCallback);

/**
 * @fn IX_STATUS ixHssAccCCMPortEnable (IxHssAccHssPort hssPortId)
 *
 * @brief This function calls the function to enable a NPE channelised flow
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1). 
 *
 * @return IX_STATUS
 */
IX_STATUS 
ixHssAccCCMPortEnable (IxHssAccHssPort hssPortId);

/**
 * @fn IX_STATUS ixHssAccCCMPortDisable (IxHssAccHssPort hssPortId)
 *
 * @brief This function calls the function to disable a NPE channelised flow
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1).
 *
 * @return IX_STATUS
 */
IX_STATUS 
ixHssAccCCMPortDisable (IxHssAccHssPort hssPortId);

/**
 * @fn BOOL ixHssAccCCMIsPortEnabled (IxHssAccHssPort hssPortId)
 *
 * @brief This function checks the state of the specified port
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1).
 *
 * @return BOOL
 */
BOOL 
ixHssAccCCMIsPortEnabled (IxHssAccHssPort hssPortId);

/**
 * @fn IX_STATUS ixHssAccCCMDisconnect (IxHssAccHssPort hssPortId)
 *
 * @brief This function disconnects client from a channelised service
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1). 
 *
 * @return IX_STATUS
 */
IX_STATUS 
ixHssAccCCMDisconnect (IxHssAccHssPort hssPortId);

/**
 * @fn void ixHssAccCCMRxCallbackRun (IxHssAccHssPort hssPortId, 
         unsigned rxOffset, 
	 unsigned txOffset, 
	 unsigned numHssErrs)
 *
 * @brief This function executes the client callback passed at connect time
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1). 
 * @param unsigned rxOffset (in) - An offset to indicate to the client
 * where within the receive buffers the NPE has just written the received
 * data to.
 * @param unsigned txOffset (in) - An offset to indicate to the client from
 * where within the txPtrList the NPE is currently transmitting from.
 * @param unsigned numHssErrs (in) - The total number of HSS port errors
 * since initial port configuration.
 *
 * @return IX_STATUS
 */
void 
ixHssAccCCMRxCallbackRun (IxHssAccHssPort hssPortId, 
			  unsigned rxOffset, 
			  unsigned txOffset, 
			  unsigned numHssErrs);

/**
 * @fn IxQMgrQId ixHssAccCCMQidGet (IxHssAccHssPort hssPortId)
 *
 * @brief This function returns the QMQ id related to the specified hss port
 *
 * @param IxHssAccHssPort hssPortId (in) - The HSS port Id. There are two
 * identical ports (0-1). 
 *
 * @return IxQMgrQId
*/
IxQMgrQId 
ixHssAccCCMQidGet (IxHssAccHssPort hssPortId);

/**
 * @fn void ixHssAccCCMShow (void)
 *
 * @brief This function will display the current state of the IxHssAcc CCM
 * module
 *
 * @return void
 */
void 
ixHssAccCCMShow (void);

/**
 * @fn void ixHssAccCCMStatsInit (void)
 *
 * @brief This function will initialise the stats of the IxHssAcc CCM module
 *
 * @return void
 */
void 
ixHssAccCCMStatsInit (void);

/**
 * @fn IX_STATUS ixHssAccCCMInit (void)
 *
 * @brief This function will initialise the IxHssAcc CCM module
 *
 * @return
 *          - IX_SUCCESS Function executed successfully
 *          - IX_FAIL Function failed to execute
 */
IX_STATUS
ixHssAccCCMInit (void);

#endif /* IXHSSACCCCM_P_H */

/**
 * @} defgroup IxHssAccCCM_p
 */
