/**
 * @file IxHssAccCCM.c
 *
 * @author Intel Corporation
 * @date 30-Jan-02
 *
 * @brief HssAccess Channelised Connection Manager Interface
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
 * Put the system defined include files required.
 */


/**
 * Put the user defined include files required.
 */

#include "IxOsal.h"
#include "IxHssAccError_p.h"
#include "IxQMgr.h"
#include "IxNpeA.h"

#include "IxHssAccCCM_p.h"
#include "IxHssAccPDM_p.h"
#include "IxHssAccCommon_p.h"
#include "IxHssAccNpeA_p.h"

/**
 * #defines and macros used in this file.
 */

#define IX_HSSACC_BYTE_SIZE 8

/**
 * Typedefs whose scope is limited to this file.
 */

typedef struct
{
    unsigned blk1;
    unsigned blk2;
} IxHssAccCCMTxBlkSize;

typedef struct
{
    unsigned connects;
    unsigned disconnects;
    unsigned enables;
    unsigned disables;
    unsigned rxCallbacks;
    unsigned emptyRxCallbacks;
} IxHssAccCCMStats;

/**
 * Variable declarations global to this file only.  Externs are followed by
 * static variables.
 */

static IxHssAccCCMParams ixHssAccCCMParams[IX_HSSACC_HSS_PORT_MAX];
static IxQMgrQId ixHssAccCCMQids[IX_HSSACC_HSS_PORT_MAX] = { IX_NPE_A_QMQ_HSS0_CHL_RX_TRIG,
                                                             IX_NPE_A_QMQ_HSS1_CHL_RX_TRIG};
static BOOL ixHssAccCCMConnectedState[IX_HSSACC_HSS_PORT_MAX] = { FALSE, FALSE};
static BOOL ixHssAccCCMEnabledState[IX_HSSACC_HSS_PORT_MAX] = { FALSE, FALSE};

/*
 * The following array holds the blk1/blk2 combinations for various block sizes, as
 * required by the NPE
 */
static IxHssAccCCMTxBlkSize ixHssAccCCMTxBlkSize[] = {
    {  8,  8 }, /* 16 byte blk size */
    { 12,  8 }, /* 20 byte blk size */
    { 12, 12 }, /* 24 byte blk size */
    { 16, 12 }, /* 28 byte blk size */
    { 16, 16 }, /* 32 byte blk size */
    { 20, 16 }, /* 36 byte blk size */
    { 20, 20 }, /* 40 byte blk size */
    { 24, 20 }, /* 44 byte blk size */
    { 24, 24 }  /* 48 byte blk size */
};

static IxHssAccCCMStats ixHssAccCCMStats;

/**
 * Extern function prototypes.
 */

/**
 * Static function prototypes.
 */

PRIVATE void
ixHssAccCCMEmptyRxCallback (IxHssAccHssPort hssPortId,
			    unsigned rxOffset, 
			    unsigned txOffset, 
			    unsigned numHssErrs);

/**
 * Function definition: ixHssAccCCMConnect
 */
IX_STATUS 
ixHssAccCCMConnect (IxHssAccHssPort hssPortId, 
		    unsigned bytesPerTSTrigger, 
		    UINT8 *rxCircular, 
		    unsigned numRxBytesPerTS, 
		    UINT32 *txPtrList, 
		    unsigned numTxPtrLists, 
		    unsigned numTxBytesPerBlk, 
		    IxHssAccChanRxCallback rxCallback)
{
    IX_STATUS status = IX_SUCCESS;
    unsigned index1;
    unsigned txBlk1Bytes;
    unsigned txBlk2Bytes;
    unsigned msgData;
    UINT32 data = 0;
    IxNpeMhMessage npeMhMsg;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Entering ixHssAccCCMConnect\n");

    /* check to see if already connected - one connection per client per port */
    if (ixHssAccCCMConnectedState[hssPortId] == TRUE)
    {
	/* report the error */
	IX_HSSACC_REPORT_ERROR ("ixHssAccCCMConnect - already connected\n");
	/* return error */
	status = IX_FAIL;
	
    }

    /* save the parameters for reference */
    ixHssAccCCMParams[hssPortId].bytesPerTSTrigger = bytesPerTSTrigger;
    ixHssAccCCMParams[hssPortId].rxCircular = rxCircular;
    ixHssAccCCMParams[hssPortId].numRxBytesPerTS = numRxBytesPerTS;
    ixHssAccCCMParams[hssPortId].txPtrList = txPtrList;
    ixHssAccCCMParams[hssPortId].numTxPtrLists = numTxPtrLists;
    ixHssAccCCMParams[hssPortId].numTxBytesPerBlk = numTxBytesPerBlk;

    /* ensure tmpRxCallback is not registered to any client callback */
    ixHssAccCCMParams[hssPortId].tmpRxCallback = NULL;

    if (rxCallback != NULL)
    {
	ixHssAccCCMParams[hssPortId].rxCallback = rxCallback;
	if (status == IX_SUCCESS)
	{
	    /* enable QMgr notifications on the specified QMQ */
	    status =  ixQMgrNotificationEnable (ixHssAccCCMQids[hssPortId], 
						IX_QMGR_Q_SOURCE_ID_NOT_E);
	    if (status != IX_SUCCESS)
	    {
		/* report the error */
		IX_HSSACC_REPORT_ERROR_WITH_ARG ("ixHssAccCCMConnect - "
		    "unexpected behaviour on QMQ notification "
		    "enable: return status = %d\n",
		    status,
		    0, 0, 0, 0, 0);
 		/* return error */
		status = IX_FAIL;
	    }
	}
    }	

    /* write the connection configurables to the NPE */
    if (status == IX_SUCCESS)
    {
	/* the rx buffer address */
	ixHssAccComNpeCmdMsgCreate (
	    IX_NPE_A_MSSG_HSS_CHAN_RX_BUF_ADDR_WRITE,
	    0, hssPortId, 0,
	    (UINT32) IX_HSSACC_PKT_MMU_VIRT_TO_PHY(rxCircular),
	    &npeMhMsg);
    
	status = ixHssAccComNpeCmdMsgSend (
	    npeMhMsg, 
	    FALSE, /* no response expected */
	    IX_NPE_A_MSSG_HSS_CHAN_RX_BUF_ADDR_WRITE);
    }

    if (status == IX_SUCCESS)
    {
	/* the rx buffer size and trigger period */
	data = (numRxBytesPerTS << IX_HSSACC_NPE_CHAN_RXSIZEB_OFFSET) | 
	    ((bytesPerTSTrigger/IX_HSSACC_BYTE_SIZE) << IX_HSSACC_NPE_CHAN_TRIG_OFFSET);
	ixHssAccComNpeCmdMsgCreate (
	    IX_NPE_A_MSSG_HSS_CHAN_RX_BUF_CFG_WRITE,
	    0, hssPortId, 0, data, &npeMhMsg);
    
	status = ixHssAccComNpeCmdMsgSend (
	    npeMhMsg, 
	    FALSE, /* no response expected */
	    IX_NPE_A_MSSG_HSS_CHAN_RX_BUF_CFG_WRITE);
    }

    /*
     * The NPE expects the TX Blk size to be written in blk1/blk2 combination
     * values, as specified by the NPE. A global array containing the 
     * blk1/blk2 values is indexed through the numTxBytesPerBlk value
     */
    if (status == IX_SUCCESS)
    {
	index1 = (numTxBytesPerBlk - IX_HSSACC_CHAN_TXBYTES_PER_BLK_MIN) / 
	    IX_HSSACC_CHAN_TXBYTES_PER_BLK_DIV;

	
	txBlk1Bytes = ixHssAccCCMTxBlkSize[index1].blk1;
	txBlk2Bytes = ixHssAccCCMTxBlkSize[index1].blk2;
	data = (txBlk1Bytes << IX_HSSACC_NPE_CHAN_TXBLK1B_OFFSET) |
	    ((txBlk1Bytes/IX_HSSACC_BYTES_PER_WORD) << IX_HSSACC_NPE_CHAN_TXBLK1W_OFFSET) |
	    (txBlk2Bytes << IX_HSSACC_NPE_CHAN_TXBLK2B_OFFSET) |
	    ((txBlk2Bytes/IX_HSSACC_BYTES_PER_WORD) << IX_HSSACC_NPE_CHAN_TXBLK2W_OFFSET);
	ixHssAccComNpeCmdMsgCreate (
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BLK_CFG_WRITE,
	    0, hssPortId, 0, data, &npeMhMsg);
    
	status = ixHssAccComNpeCmdMsgSend (
	    npeMhMsg, 
	    FALSE, /* no response expected */
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BLK_CFG_WRITE);
    }

    if (status == IX_SUCCESS)
    {
	/* the tx ptrList address */
	ixHssAccComNpeCmdMsgCreate (
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BUF_ADDR_WRITE,
	    0, hssPortId, 0,
	    (UINT32) IX_HSSACC_PKT_MMU_VIRT_TO_PHY(txPtrList),
	    &npeMhMsg);
    
	status = ixHssAccComNpeCmdMsgSend (
	    npeMhMsg, 
	    FALSE, /* no response expected */
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BUF_ADDR_WRITE);
    }

    if (status == IX_SUCCESS)
    {
	msgData = numTxPtrLists << IX_HSSACC_NPE_CHAN_TXBUFSIZE_OFFSET;
	/* the tx buf size */
	ixHssAccComNpeCmdMsgCreate (
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BUF_SIZE_WRITE,
	    0, hssPortId, 0, msgData, &npeMhMsg);
    
	status = ixHssAccComNpeCmdMsgSend (
	    npeMhMsg, 
	    FALSE, /* no response expected */
	    IX_NPE_A_MSSG_HSS_CHAN_TX_BUF_SIZE_WRITE);
    }

    if (status == IX_SUCCESS)
    {
	/* note the active connection */
	ixHssAccCCMConnectedState[hssPortId] = TRUE;
	ixHssAccCCMStats.connects++;
    }

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Exiting ixHssAccCCMConnect\n");
    return status;
}

/**
 * Function definition: ixHssAccCCMPortEnable
 */
IX_STATUS 
ixHssAccCCMPortEnable (IxHssAccHssPort hssPortId)
{
    IX_STATUS status = IX_SUCCESS;
    IxNpeMhMessage message;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Entering ixHssAccCCMPortEnable\n");

    /* check to see if already enabled */
    if (ixHssAccCCMEnabledState[hssPortId] == TRUE)
    {
	/* report the error */
	IX_HSSACC_REPORT_ERROR ("ixHssAccCCMPortEnable - already enabled\n");
	/* return error */
	status = IX_FAIL;
	
    }

    /* check to see if there is a valid connection on the hss port */
    if (ixHssAccCCMConnectedState[hssPortId] == FALSE)
    {
	/* report the error */
	IX_HSSACC_REPORT_ERROR ("ixHssAccCCMPortEnable - no valid connection on specified hssPortId\n");
	/* return error */
	status = IX_FAIL;
	
    }

    /* enable the npe flow */
    if (status == IX_SUCCESS)
    {
	/* 
	 * check if tmpRxCallback contains any client callback. If any,
	 * this means the client is trying to reenable the service for
	 * the second (or third, fourth, ...) time without disconnecting
	 * the service. Hence, use back the client callback that has been
	 * supplied during ixHssAccCCMConnect
	 */
	if (ixHssAccCCMParams[hssPortId].tmpRxCallback != NULL)
	{
	    ixHssAccCCMParams[hssPortId].rxCallback = 
		ixHssAccCCMParams[hssPortId].tmpRxCallback;
	}

	/* create the NpeMh message - NPE_A message format */
	ixHssAccComNpeCmdMsgCreate (IX_NPE_A_MSSG_HSS_CHAN_FLOW_ENABLE, 0, 
				       hssPortId, 0, 0, &message);
	/* send the message */
	status = ixHssAccComNpeCmdMsgSend (message, 
					   FALSE,
					   IX_NPE_A_MSSG_HSS_CHAN_FLOW_ENABLE);

	if (status == IX_SUCCESS)
	{
	    ixHssAccCCMEnabledState[hssPortId] = TRUE;
	    ixHssAccCCMStats.enables++;
	}
	else
	{
	    /* report the error */
	    IX_HSSACC_REPORT_ERROR ("ixHssAccCCMPortEnable - npe flow enable failed\n");
	    /* ensure not passing another components fail return type */
	    status = IX_FAIL;
	}
    }

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Exiting ixHssAccCCMPortEnable\n");
    return status;
}

/**
 * Function definition: ixHssAccCCMPortDisable
 */
IX_STATUS 
ixHssAccCCMPortDisable (IxHssAccHssPort hssPortId)
{
    IX_STATUS status = IX_SUCCESS;
    IxNpeMhMessage message;
    IxHssAccChanRxCallback currentClientCallback = 
	ixHssAccCCMParams[hssPortId].rxCallback;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Entering ixHssAccCCMPortDisable\n");

    /* check to see if enabled */
    if (ixHssAccCCMEnabledState[hssPortId] == FALSE)
    {
	/* report the error */
	IX_HSSACC_REPORT_ERROR ("ixHssAccCCMPortDisable - not enabled\n");
	/* return error */
	status = IX_FAIL;
	
    }

    /* disable the npe flow */
    if (status == IX_SUCCESS)
    {
	/* 
	 * temporary save client callback just in case client would like to
	 * reenable the service after disabling it for a while
	 */
	ixHssAccCCMParams[hssPortId].tmpRxCallback = 
	    ixHssAccCCMParams[hssPortId].rxCallback;

	/* stop client notifications from the sync QMQ - clear client callback */
	ixHssAccCCMParams[hssPortId].rxCallback = ixHssAccCCMEmptyRxCallback;

	/* create the NpeMh message - NPE_A message format */
	ixHssAccComNpeCmdMsgCreate (IX_NPE_A_MSSG_HSS_CHAN_FLOW_DISABLE, 0, 
				       hssPortId, 0, 0, &message);
	/* send the message */
	status = ixHssAccComNpeCmdMsgSend (message, 
					   FALSE,
					   IX_NPE_A_MSSG_HSS_CHAN_FLOW_DISABLE);

	if (status == IX_SUCCESS)
	{
	    ixHssAccCCMEnabledState[hssPortId] = FALSE;
	    ixHssAccCCMStats.disables++;
	}
	else
	{
	    /* report the error */
	    IX_HSSACC_REPORT_ERROR ("ixHssAccCCMPortDisable - npe flow disable failed\n");
	    /* re-assign the client callback */
	    ixHssAccCCMParams[hssPortId].rxCallback = currentClientCallback;
	    status = IX_FAIL;
	}
    }

    /* 
     * Note: QMQ notifications will stay enabled. This will help the QMQ
     * flushing - if an entry gets pushed on the queue during disabling of
     * the npe flow, the entry will need to be read from the queue. The
     * hssAccess trigger callback will stay attached.  The client callback
     * will just not be called. 
     */

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Exiting ixHssAccCCMPortDisable\n");
    return status;
}

/**
 * Function definition: ixHssAccCCMIsPortEnabled
 *
 * NOTE: hssPortId should be validated before calling this function 
 */
BOOL 
ixHssAccCCMIsPortEnabled (IxHssAccHssPort hssPortId)
{
    return ixHssAccCCMEnabledState[hssPortId];
}

/**
 * Function definition: ixHssAccCCMDisconnect
 */
IX_STATUS 
ixHssAccCCMDisconnect (IxHssAccHssPort hssPortId)
{
    IX_STATUS status = IX_SUCCESS;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Entering ixHssAccCCMDisconnect\n");

    /* check to see if connected */
    if (ixHssAccCCMConnectedState[hssPortId] == FALSE)
    {
	/* report the error */
	IX_HSSACC_REPORT_ERROR ("ixHssAccCCMDisconnect - not connected\n");
	/* return error */
	status = IX_FAIL;
    }

    /* check to see if disabled yet */
    if ((status == IX_SUCCESS) && (ixHssAccCCMEnabledState[hssPortId] == TRUE))
    {
	/* stop the service */
	status = ixHssAccCCMPortDisable (hssPortId);
    }

    if (status == IX_SUCCESS)
    {    
	/* 
	 * clear the client connection parameters saved for reference
	 * NOTE: don't want to zero whole structure. Want to leave the
	 * rxCallback set to the dummy callback for debug purposes
	 */
	ixHssAccCCMParams[hssPortId].bytesPerTSTrigger = 0;
	ixHssAccCCMParams[hssPortId].rxCircular = 0;
	ixHssAccCCMParams[hssPortId].numRxBytesPerTS = 0;
	ixHssAccCCMParams[hssPortId].txPtrList = 0;
	ixHssAccCCMParams[hssPortId].numTxPtrLists = 0;
	ixHssAccCCMParams[hssPortId].numTxBytesPerBlk = 0;
	ixHssAccCCMParams[hssPortId].tmpRxCallback = NULL;

	ixHssAccCCMConnectedState[hssPortId] = FALSE;
	ixHssAccCCMStats.disconnects++;
    }

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Exiting ixHssAccCCMDisconnect\n");
    return status;
}

/**
 * Function definition: ixHssAccCCMRxCallbackRun
 */
void 
ixHssAccCCMRxCallbackRun (IxHssAccHssPort hssPortId, 
			  unsigned rxOffset, 
			  unsigned txOffset, 
			  unsigned numHssErrs)
{
    /* 
     * call the client issued callback. NOTE: no need for error
     * check here. When disconnected a private callback will be
     * registered to gather statistics
     */
    ixHssAccCCMParams[hssPortId].rxCallback (hssPortId,
					     rxOffset, 
					     txOffset, 
					     numHssErrs);
    ixHssAccCCMStats.rxCallbacks++;
}

/**
 * Function definition: ixHssAccCCMEmptyRxCallback
 */
PRIVATE void
ixHssAccCCMEmptyRxCallback (IxHssAccHssPort hssPortId,
			    unsigned rxOffset, 
			    unsigned txOffset, 
			    unsigned numHssErrs)
{
    ixHssAccCCMStats.emptyRxCallbacks++;
}

/**
 * Function definition: ixHssAccCCMQidGet
 */
IxQMgrQId
ixHssAccCCMQidGet (IxHssAccHssPort hssPortId)
{
    /* parameters error checked before use */
    return ixHssAccCCMQids[hssPortId];
}

/**
 * Function definition: ixHssAccCCMShow
 */
void 
ixHssAccCCMShow (void)
{
    IxHssAccHssPort hssPortIndex;
    
    printf ("\nixHssAccCCMShow:\n");
    printf ("\t   connects: %d \t enables: %d \t     rxCallbacks: %d\n", 
	    ixHssAccCCMStats.connects,
	    ixHssAccCCMStats.enables,
	    ixHssAccCCMStats.rxCallbacks);
    printf ("\tdisconnects: %d \tdisables: %d \temptyRxCallbacks: %d\n\n", 
	    ixHssAccCCMStats.disconnects,
	    ixHssAccCCMStats.disables,
	    ixHssAccCCMStats.emptyRxCallbacks);

    for (hssPortIndex = IX_HSSACC_HSS_PORT_0; 
         hssPortIndex < hssPortMax; 
         hssPortIndex++)
    {
        printf ("\thssPort[%d]: %s, %s\n", 
		hssPortIndex,
		(ixHssAccCCMConnectedState[hssPortIndex] ? "Connected" : "Disconnected"),
		(ixHssAccCCMEnabledState[hssPortIndex] ? "Enabled" : "Disabled"));

        if (ixHssAccCCMConnectedState[hssPortIndex])
        {
	    printf ("\t\tbytesPerTSTrigger: %d\n", 
		    ixHssAccCCMParams[hssPortIndex].bytesPerTSTrigger);
	    printf ("\t\t       rxCircular: 0x%p\n", 
		    ixHssAccCCMParams[hssPortIndex].rxCircular);
	    printf ("\t\t  numRxBytesPerTS: %d\n", 
		    ixHssAccCCMParams[hssPortIndex].numRxBytesPerTS);
	    printf ("\t\t        txPtrList: 0x%p\n", 
		    ixHssAccCCMParams[hssPortIndex].txPtrList);
	    printf ("\t\t    numTxPtrLists: %d\n", 
		    ixHssAccCCMParams[hssPortIndex].numTxPtrLists);
	    printf ("\t\t numTxBytesPerBlk: %d\n", 
		    ixHssAccCCMParams[hssPortIndex].numTxBytesPerBlk);
	    printf ("\t\t       rxCallback: %p\n", 
		    ixHssAccCCMParams[hssPortIndex].rxCallback);
        }
    }
}

/**
 * Function definition: ixHssAccCCMStatsInit
 */
void 
ixHssAccCCMStatsInit (void)
{
    ixHssAccCCMStats.connects = 0;
    ixHssAccCCMStats.disconnects = 0;
    ixHssAccCCMStats.enables = 0;
    ixHssAccCCMStats.disables = 0;
    ixHssAccCCMStats.rxCallbacks = 0;
    ixHssAccCCMStats.emptyRxCallbacks = 0;
}

/**
 * Function definition: ixHssAccCCMInit
 */
IX_STATUS 
ixHssAccCCMInit (void)
{
    IxHssAccHssPort hssPortIndex;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Entering ixHssAccCCMInit\n");

    /* initialise the stats */
    ixHssAccCCMStatsInit ();

    for (hssPortIndex = IX_HSSACC_HSS_PORT_0; 
	 hssPortIndex < IX_HSSACC_HSS_PORT_MAX;
	 hssPortIndex++)
    {
	ixHssAccCCMConnectedState[hssPortIndex] = FALSE;
	ixHssAccCCMEnabledState[hssPortIndex] = FALSE;
    }

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Exiting ixHssAccCCMInit\n");
    return IX_SUCCESS;
}
