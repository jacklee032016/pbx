/**
 * @file IxHssAccPktRx.c
 *
 * @author Intel Corporation
 * @date 10 Jan 2002
 *
 * @brief This file contains the implementation of the private API for the
 * Rx Module.
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
#include "IxHssAccPktRx_p.h"
#include "IxOsal.h"
#include "IxHssAccError_p.h"
#include "IxHssAccPDM_p.h"
#include "IxHssAccPCM_p.h"
#include "IxHssAccCommon_p.h"

/*
 * Defines
 */

/*
 * Typedefs
 */
typedef struct
{    
    unsigned rxCallbacks;
    unsigned qReadFailures;
    unsigned rxFreeQWriteFails;
    unsigned rxQWriteOverflows;
    unsigned rxFreeReplenishs;
    unsigned maxEntriesInRxQ;
    unsigned rxInvalidDesc;
} IxHssAccPktRxStats;


/**
 * Variable declarations global to this file only.  
 * Externs are followed by static variables.
 */
static IxHssAccPktRxStats ixHssAccPktRxStats;

IxQMgrQId ixHssAccPktRxQId[IX_HSSACC_HSS_PORT_MAX] = 
{    IX_NPE_A_QMQ_HSS0_PKT_RX,       
     IX_NPE_A_QMQ_HSS1_PKT_RX
};

/*
 *Function :ixHssAccPktRxFreeLowCallback 
 */
void 
ixHssAccPktRxFreeLowCallback (IxQMgrQId qId, 
			      IxQMgrCallbackId cbId)
{
    /* This function is called from within an ISR */
    IxHssAccHdlcPort hdlcPortId = IX_HSSACC_PKT_CBID_HDLC_MASK & (unsigned)cbId;
    IxHssAccHssPort hssPortId  = cbId >> IX_HSSACC_PKT_CBID_HSS_OFFSET;
    IX_HSSACC_DEBUG_OFF (IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Entering "
					   "ixHssAccPktRxFreeLowCallback\n"));


    /* if the notification was from a Q that is a packetied RX Free Q*/
#ifndef NDEBUG
	if ((hssPortId < hssPortMax) && 
	    (hdlcPortId < IX_HSSACC_HDLC_PORT_MAX))
	{
	    ixHssAccPCMRxFreeLowCallbackRun (hssPortId, hdlcPortId);
	    
	}
	else
	{
	    IX_HSSACC_DEBUG_OFF 
		(IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxFreeReqCallback: The"
					 " hdlcPortId or hssPortId has been "
					 "corrupted by the NPE\n"));
	}
#else
	ixHssAccPCMRxFreeLowCallbackRun (hssPortId, hdlcPortId);
#endif
	IX_HSSACC_DEBUG_OFF 
	    (IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Exiting "
			       "ixHssAccPktRxFreeLowCallback \n"));
}


/*
 *Function :ixHssAccPktRxCallback
 */
void 
ixHssAccPktRxCallback (IxQMgrQId qId, 
		       IxQMgrCallbackId cbId)
{
    /* This function is called from within an ISR */
    IX_STATUS status;
    unsigned numOfEntriesInRxQ = 0;
    IxHssAccPDMDescriptor *desc;   /* descriptor ptr for reading from the Q */ 
    UINT32 pDesc = 0;

    IX_HSSACC_DEBUG_OFF
	(IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Entering "
			   "ixHssAccPktRxCallback \n"));
   
    do 
    {
	status = ixQMgrQRead (qId, &pDesc); 
        desc = (IxHssAccPDMDescriptor *)pDesc;
	if (status == IX_SUCCESS)
	{	    
#ifndef NDEBUG
	    if (desc != NULL)
	    {
#endif
		desc = (IxHssAccPDMDescriptor *) IX_HSSACC_PKT_MMU_PHY_TO_VIRT (desc);
		IX_HSSACC_PKT_DATA_CACHE_INVALIDATE(desc, sizeof(IxHssAccPDMDescriptor));

                /* endian conversion for the NpePacket Descriptor */
		desc->npeDesc.pRootMbuf = (IX_OSAL_MBUF *)(IX_OSAL_SWAP_BE_SHARED_LONG (
		    (UINT32)desc->npeDesc.pRootMbuf));
		desc->npeDesc.packetLength = (UINT16)(IX_OSAL_READ_BE_SHARED_SHORT (
	       	    (UINT16 *) &(desc->npeDesc.packetLength)));
	        desc->npeDesc.rsvdShort0 = (UINT16)(IX_OSAL_READ_BE_SHARED_SHORT (
	       	    (UINT16 *) &(desc->npeDesc.rsvdShort0)));	
		desc->npeDesc.pNextMbuf = (IX_OSAL_MBUF *)(IX_OSAL_SWAP_BE_SHARED_LONG (
		    (UINT32) desc->npeDesc.pNextMbuf)); 
		desc->npeDesc.pMbufData = (UINT8 *)(IX_OSAL_SWAP_BE_SHARED_LONG (
		    (UINT32) desc->npeDesc.pMbufData)); 
		desc->npeDesc.mbufLength = IX_OSAL_SWAP_BE_SHARED_LONG (
		    desc->npeDesc.mbufLength); 

		desc->npeDesc.pRootMbuf = ixHssAccPDMMbufFromNpeFormatConvert (
		    desc->npeDesc.pRootMbuf, TRUE);

#ifndef NDEBUG
		/* Is the hss Port Id lower than the max hssPortId */
		/* AND is the hdlc Port Id lower than the max hdlcportId */
		if (!(IX_HSSACC_ENUM_INVALID (desc->hssPortId, hssPortMax)) && 
		    !(IX_HSSACC_ENUM_INVALID (desc->hdlcPortId, IX_HSSACC_HDLC_PORT_MAX)))
		{
#endif
		    ixHssAccPCMRxCallbackRun (desc->hssPortId,
					      desc->hdlcPortId,
					      desc->npeDesc.pRootMbuf,
					      desc->npeDesc.errorCount,
					      desc->npeDesc.status,
					      desc->npeDesc.packetLength);
		    ixHssAccPDMDescFree (desc, IX_HSSACC_PDM_RX_POOL); 
		    ixHssAccPktRxStats.rxCallbacks++;
		    numOfEntriesInRxQ++;
		    
#ifndef NDEBUG		
		}
		else
		{
		    IX_HSSACC_DEBUG_OFF
			(IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxCallback: An invalid descriptor "
						 "was received.\n"));
		    ixHssAccPktRxStats.rxInvalidDesc++;
		}
	    }
	    else
	    {
		IX_HSSACC_DEBUG_OFF
		    (IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxCallback: An invalid descriptor "
					     "was received.\n"));
		ixHssAccPktRxStats.rxInvalidDesc++;
	    }
#endif
	}
	else if (status == IX_FAIL)
	{
	    IX_HSSACC_DEBUG_OFF
		(IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxCallback: ixQMgrQRead"
					 " failed while reading from PktRx Q.\n"));
	    ixHssAccPktRxStats.qReadFailures++;
	}
    } while (status == IX_SUCCESS);

    if (numOfEntriesInRxQ > ixHssAccPktRxStats.maxEntriesInRxQ)
    {
	ixHssAccPktRxStats.maxEntriesInRxQ = numOfEntriesInRxQ;
    }
    
    IX_HSSACC_DEBUG_OFF
	(IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
			   "Exiting ixHssAccPktRxCallback\n"));
}


/**
 * Function definition: ixHssAccPktRxShow
 */
void ixHssAccPktRxShow (void)
{
   
    printf ("\nixHssAccPktRxShow:\n");
    printf ("      rxs: %d \t    rxFreeRepls: %d\n",
	    ixHssAccPktRxStats.rxCallbacks,
	    ixHssAccPktRxStats.rxFreeReplenishs);
    printf ("readFails: %d \t writeOverFlows: %d \t  invalidDescs: %d\n",
	    ixHssAccPktRxStats.qReadFailures,
	    ixHssAccPktRxStats.rxQWriteOverflows,
	    ixHssAccPktRxStats.rxInvalidDesc);
    printf (" maxInRxQ: %d \t     writeFails: %d\n",
	    ixHssAccPktRxStats.maxEntriesInRxQ,
	    ixHssAccPktRxStats.rxFreeQWriteFails);
}


/**
 * Function definition: ixHssAccPktRxStatsInit
 */
void ixHssAccPktRxStatsInit (void)
{
    ixHssAccPktRxStats.rxCallbacks   = 0;
    ixHssAccPktRxStats.qReadFailures = 0;
    ixHssAccPktRxStats.rxFreeQWriteFails = 0;
    ixHssAccPktRxStats.rxQWriteOverflows = 0;
    ixHssAccPktRxStats.rxFreeReplenishs = 0;
    ixHssAccPktRxStats.maxEntriesInRxQ = 0;
    ixHssAccPktRxStats.rxInvalidDesc = 0;
}


/*
 *Function :ixHssAccPktRxFreeReplenish
 */
IX_STATUS 
ixHssAccPktRxFreeReplenish (IxHssAccHssPort hssPortId, 
			    IxHssAccHdlcPort hdlcPortId, 
			    IX_OSAL_MBUF *buffer)
{
    /* This function may be called from within an ISR */
    IX_STATUS status = IX_SUCCESS;
    IxHssAccPDMDescriptor *desc = NULL, *physDesc = NULL;
    UINT32 pPhysDesc = 0; 

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Entering "
		      "ixHssAccPktRxFreeReplenish \n");

    /* Check that the port is connected */
    if (ixHssAccPCMCheckReplenishOk (hssPortId, hdlcPortId))
    {
	/* Get a desc to attach the client supplied mbuf to*/
	status = ixHssAccPDMDescGet (hssPortId, hdlcPortId, 
				     IX_HSSACC_PDM_RX_POOL, &desc);
	if (status != IX_SUCCESS)
	{
	    return status;
	}
	
	/* Set up the desc as supplied by the NPE */
	desc->npeDesc.pRootMbuf     = ixHssAccPDMMbufToNpeFormatConvert (buffer);
	desc->npeDesc.status        = IX_HSSACC_PKT_OK;
	desc->npeDesc.chainCount    = 0;
	desc->npeDesc.packetLength  = 0;
	desc->npeDesc.pMbufData     = (UINT8 *) IX_OSAL_MBUF_MDATA(buffer);
	desc->npeDesc.mbufLength    = IX_OSAL_MBUF_MLEN(buffer);
	desc->npeDesc.pNextMbuf     = NULL;
	desc->npeDesc.errorCount    = 0;

        /* endian conversion for the NpePacket Descriptor */
	desc->npeDesc.pRootMbuf = (IX_OSAL_MBUF *)(IX_OSAL_SWAP_BE_SHARED_LONG (
	    (UINT32) desc->npeDesc.pRootMbuf));
	desc->npeDesc.pNextMbuf = (IX_OSAL_MBUF *)(IX_OSAL_SWAP_BE_SHARED_LONG (
	    (UINT32) desc->npeDesc.pNextMbuf)); 
	
	IX_HSSACC_PKT_DATA_CACHE_FLUSH (desc, sizeof(IxHssAccPDMDescriptor));
	physDesc  = (IxHssAccPDMDescriptor *) IX_HSSACC_PKT_MMU_VIRT_TO_PHY (desc);

	/* Write the desc to the appropiate RxFree Q*/
        pPhysDesc = (UINT32) physDesc;
	status = ixQMgrQWrite (ixHssAccPCMRxFreeQIdGet (hssPortId, hdlcPortId),
			       &pPhysDesc);

	if (status != IX_SUCCESS)
	{
	    if (status == IX_FAIL)
	    {
		IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxFreeReplenish:"
					"Writing an RX descriptor to the RXFREE"
					" Queue failed\n");
		ixHssAccPktRxStats.rxFreeQWriteFails++;
		status = IX_FAIL;
	    }
	    else if (status == IX_QMGR_Q_OVERFLOW)
	    {
		ixHssAccPktRxStats.rxQWriteOverflows++;
		status = IX_HSSACC_Q_WRITE_OVERFLOW;
	    }

            /* endian conversion for the NpePacket Descriptor */
	    desc->npeDesc.pRootMbuf = (IX_OSAL_MBUF *)(IX_OSAL_SWAP_BE_SHARED_LONG (
	        (UINT32) desc->npeDesc.pRootMbuf));

	    /* we need to restore the buffer to the original (virtual) address
	     * before we return
	     */
	    ixHssAccPDMMbufFromNpeFormatConvert(desc->npeDesc.pRootMbuf, FALSE);

	    ixHssAccPDMDescFree (desc, IX_HSSACC_PDM_RX_POOL);
	    return status;
	}
	ixHssAccPktRxStats.rxFreeReplenishs++;
    }
    else
    {
	IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxFreeReplenish:"
				"Called ixHssAccPktRxInternalFreeBufReplenish "
				"on a port that is not connected\n");
	return IX_FAIL;
    }
   
    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, "Exiting "
		      "ixHssAccPktRxFreeReplenish \n");
    return status;
}

/**
 * Function definition: ixHssAccPktRxInit
 */
IX_STATUS 
ixHssAccPktRxInit (void)
{
    IX_STATUS status = IX_SUCCESS;
    IxHssAccHssPort hssPortIndex;
    IxHssAccHdlcPort hdlcPortIndex;
    unsigned hssHdlcPortIdAsCbId;

    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Entering ixHssAccPktRxInit\n");

    /* initialise stats */
    ixHssAccPktRxStatsInit ();

    /* Set the Callback for the RxFree Q for this client */
    for (hssPortIndex = IX_HSSACC_HSS_PORT_0; 
	 hssPortIndex < hssPortMax; 
	 hssPortIndex++)
    {
	for (hdlcPortIndex = IX_HSSACC_HDLC_PORT_0; 
	     hdlcPortIndex < IX_HSSACC_HDLC_PORT_MAX; 
	     hdlcPortIndex++)
	{
	    hssHdlcPortIdAsCbId = (hssPortIndex << IX_HSSACC_PKT_CBID_HSS_OFFSET) | 
		hdlcPortIndex;
	    
	    status = ixQMgrNotificationCallbackSet (
		ixHssAccPCMRxFreeQIdGet (hssPortIndex, hdlcPortIndex),
		ixHssAccPktRxFreeLowCallback,
		hssHdlcPortIdAsCbId);

	    if (status != IX_SUCCESS)
	    {
		IX_HSSACC_REPORT_ERROR ("ixHssAccPktRxInit:"
					"ixQMgrNotificationCallbackSet failed\n");
		return IX_FAIL;
	    }
	}
    }

    for (hssPortIndex = IX_HSSACC_HSS_PORT_0; 
	 hssPortIndex < hssPortMax; 
	 hssPortIndex++)
    {
        /* Set the callback for the HSS Port 0/1 Rx Q */
        status = ixQMgrNotificationCallbackSet (
	    ixHssAccPktRxQId[hssPortIndex],
	    ixHssAccPktRxCallback,
	    hssPortIndex);
        if (status != IX_SUCCESS)
        {
	    /* report the error */
	    IX_HSSACC_REPORT_ERROR_WITH_ARG ("ixHssAccPktRxInit:"
	        "Setting callback for the HSS%d PktRx Q failed\n",
		hssPortIndex,
		0, 0, 0, 0, 0);
            /* return error */
            return IX_FAIL;
        }
    
        /* Enable notification for the HSS Port 0/1 Rx Q */
        status =  ixQMgrNotificationEnable (ixHssAccPktRxQId[hssPortIndex],
	                                    IX_QMGR_Q_SOURCE_ID_NOT_E);
        if (status != IX_SUCCESS)
        {
	    /* report the error */
	    IX_HSSACC_REPORT_ERROR_WITH_ARG ("ixHssAccPktRxInit:"
	        "Notification enable for the HSS%d PktRx Q failed\n",
		hssPortIndex,
		0, 0, 0, 0, 0);
            /* return error */
	    return IX_FAIL;
        }
    }
	
    IX_HSSACC_TRACE0 (IX_HSSACC_FN_ENTRY_EXIT, 
		      "Exiting ixHssAccPktRxInit\n");
    return status;
}