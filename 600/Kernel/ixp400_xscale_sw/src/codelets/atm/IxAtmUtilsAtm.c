/*
 * FileName:    IxAtmAtmUtilsAtm.c
 * Author: Intel Corporation
 * Created:     15 May 2002
 * Description: Atm Codelet utilities for Atm.
 *
 *
 * Design Notes:
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
 * User defined include files
 */
#if defined(__wince) && defined(IX_USE_SERCONSOLE)
	#include "IxSerConsole.h"
	#define printf ixSerPrintf
	#define gets ixSerGets
#endif


#include "IxAtmm.h"
#include "IxAtmSch.h"
#include "IxAtmdAcc.h"
#include "IxNpeMh.h"
#include "IxNpeDl.h"
#include "IxQMgr.h"
#include "IxOsal.h"
#include "IxAtmCodelet_p.h"

/*
 * #defines and macros
 */
#define DISCONNECT_RETRY_DELAY     (25)
#define DISCONNECT_RETRY_COUNT     (200)
#define UTOPIA_PORT_RATE           (53 * 1000000)
#define SINGLE_PORT_SPHY_IMAGE_ID  IX_NPEDL_NPEIMAGE_NPEA_HSS0_ATM_SPHY_1_PORT
#define SINGLE_PORT_MPHY_IMAGE_ID  IX_NPEDL_NPEIMAGE_NPEA_HSS0_ATM_MPHY_1_PORT
#define MULTI_PORT_MPHY_IMAGE_ID   IX_NPEDL_NPEIMAGE_NPEA_ATM_MPHY_12_PORT

#if IX_UTOPIAMODE == 1
#define IX_ATM_CODELET_NPEA_IMAGE_ID SINGLE_PORT_SPHY_IMAGE_ID 
#define IX_ATM_CODELET_PHY_MODE IX_ATMM_SPHY_MODE
#else
#define IX_ATM_CODELET_NPEA_IMAGE_ID MULTI_PORT_MPHY_IMAGE_ID 
#define IX_ATM_CODELET_PHY_MODE IX_ATMM_MPHY_MODE
#endif

/*
 * Typedefs
 */
typedef struct
{
    BOOL inUse;
    IxAtmConnId connId;
    IxAtmLogicalPort port;
} ChannelInfo;

/*
 * Variable declarations global to this file. Externs are followed by
 * statics.
 */
static ChannelInfo rxChannelInfo[IX_ATM_MAX_NUM_VC];
static ChannelInfo txChannelInfo[IX_ATM_MAX_NUM_VC];

static IxAtmSchedulerVcId rxVcId[IX_ATM_MAX_NUM_VC];
static IxAtmNpeRxVcId rxNpeVcIdTable[IX_ATM_MAX_NUM_VC];

static IxAtmSchedulerVcId txVcId[IX_ATM_MAX_NUM_VC];

static IxAtmLogicalPort rxPort[IX_ATM_MAX_NUM_VC];
static IxAtmLogicalPort txPort[IX_ATM_MAX_NUM_VC];

/*
 * Forward declarartions
 */
PRIVATE IX_STATUS
rxVcDisconnect(IxAtmConnId rxConnId);

PRIVATE IX_STATUS
txVcDisconnect(IxAtmConnId txConnId);

PRIVATE IX_STATUS
txFreeChannelGet (UINT32 *channelId);

PRIVATE IX_STATUS
rxFreeChannelGet (UINT32 *channelId);

PRIVATE IX_STATUS
rxChannelFind (IxAtmConnId connId, UINT32 *channelId);

PRIVATE IX_STATUS
txChannelFind (IxAtmConnId connId, UINT32 *channelId);

/*
 * Function definitions.
 */
PUBLIC IX_STATUS
ixAtmUtilsAtmVcRegisterConnect (IxAtmLogicalPort port,
				 unsigned vpi,
				 unsigned vci,
				 IxAtmdAccAalType aalType,
				 IxAtmServiceCategory atmService,
				 IxAtmRxQueueId rxQueueId,
				 IxAtmdAccRxVcRxCallback rxCallback,
				 UINT32 minimumReplenishCount,
				 IxAtmdAccTxVcBufferReturnCallback bufferFreeCallback,
				 IxAtmdAccRxVcFreeLowCallback rxFreeLowCallback,
				 IxAtmdAccUserId userId,
				 IxAtmConnId *rxConnId,
				 IxAtmConnId *txConnId)
{
    IX_STATUS retval;
    IxAtmmVc txVc;
    IxAtmmVc rxVc;
    UINT32 rxFreeQueueSize;
    unsigned rxChannelIdx;
    unsigned txChannelIdx;

    if (rxFreeChannelGet(&rxChannelIdx) == IX_FAIL)
    {
        ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "No free Rx channell\n", 0, 0, 0, 0, 0, 0);
        return IX_FAIL;
    }

    if (txFreeChannelGet(&txChannelIdx) == IX_FAIL)
    {
        ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "No free Tx channell\n", 0, 0, 0, 0, 0, 0);
        return IX_FAIL;
    }

    rxPort[rxChannelIdx] = port;
    txPort[txChannelIdx] = port;

    ixOsalMemSet(&txVc, 0, sizeof(txVc));
    ixOsalMemSet(&rxVc, 0, sizeof(rxVc));

    /* Setup Tx Vc descriptor */
    txVc.vpi = vpi;
    txVc.vci = vci;
    txVc.direction = IX_ATMM_VC_DIRECTION_TX;
    txVc.trafficDesc.atmService = atmService;
    txVc.trafficDesc.pcr = UTOPIA_PORT_RATE;

    /* Setup tx VC */
    retval = ixAtmmVcRegister (port,
			       &txVc,
			       &txVcId[txChannelIdx]);

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Fail, register Tx VC \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    /* Setup Rx Vc descriptor */
    rxVc.vpi = vpi;
    rxVc.vci = vci;
    rxVc.direction = IX_ATMM_VC_DIRECTION_RX;
    rxVc.trafficDesc.atmService = atmService;

    /* Setup rx VC */
    retval = ixAtmmVcRegister (port,
			       &rxVc,
			       &rxVcId[rxChannelIdx]);

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Fail, register Rx VC \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    /* Connect Tx to the Vc */
    retval = ixAtmdAccTxVcConnect (port,
				   vpi,
				   vci,
				   aalType,
				   userId,
				   bufferFreeCallback,
				   txConnId);

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Fail, connect to Tx VC \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    txChannelInfo[txChannelIdx].connId = *txConnId;
    txChannelInfo[txChannelIdx].port = port;

    /* Connect Rx to the VC */
    retval = ixAtmdAccRxVcConnect (port,
				   vpi,
				   vci,
				   aalType,
				   rxQueueId,
				   userId,
				   rxCallback,
				   minimumReplenishCount,
				   rxConnId,
				   &rxNpeVcIdTable[rxChannelIdx]);

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Fail, connect to Rx VC \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    rxChannelInfo[rxChannelIdx].connId = *rxConnId;
    rxChannelInfo[rxChannelIdx].port = port;

    retval = ixAtmdAccRxVcFreeEntriesQuery(*rxConnId,
                                           &rxFreeQueueSize);
    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Fail, get the no. entries in rx free queue\n",0,0,0,0,0,0);
	return retval;
    }

 
    /* Replenish Rx buffers if callback is not NULL*/
    if (rxFreeLowCallback != NULL)
    {
	rxFreeLowCallback(userId);
	
	retval = ixAtmdAccRxVcFreeLowCallbackRegister (*rxConnId,
						       rxFreeQueueSize / 4,
						       rxFreeLowCallback);
    }
	
    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to register RxFreeLowCalback \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    /* Register replish more callback */

    retval = ixAtmdAccRxVcEnable (*rxConnId);

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to enable Rx VC \n", 0, 0, 0, 0, 0, 0);
	return retval;
    }

    return IX_SUCCESS;
}

/* ---------------------------------------------------
 */
PRIVATE IX_STATUS
rxVcDisconnect(IxAtmConnId rxConnId)
{
    IX_STATUS retval;
    UINT32 retryCount;

    retval =  ixAtmdAccRxVcDisable(rxConnId);
    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "rx disable failed (%d)\n", retval, 0, 0, 0, 0, 0);
	return retval;
    }

    retryCount = 0;
    do
    {
	retval = ixAtmdAccRxVcTryDisconnect(rxConnId);
	if (retval == IX_ATMDACC_RESOURCES_STILL_ALLOCATED)
	{
	    ixOsalSleep(DISCONNECT_RETRY_DELAY);
	}
    }
    while ((retryCount++ < DISCONNECT_RETRY_COUNT)
	   && (retval == IX_ATMDACC_RESOURCES_STILL_ALLOCATED));

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "rx disconnect failed (%d)\n",
		    retval, 0, 0, 0, 0, 0);
	return retval;
    }

    return IX_SUCCESS;
}

/* ---------------------------------------------------
 */
PRIVATE IX_STATUS
txVcDisconnect(IxAtmConnId txConnId)
{
    IX_STATUS retval;
    UINT32 retryCount;

    retryCount = 0;
    do
    {
	retval = ixAtmdAccTxVcTryDisconnect(txConnId);
	if (retval == IX_ATMDACC_RESOURCES_STILL_ALLOCATED)
	{
	    ixOsalSleep(DISCONNECT_RETRY_DELAY);
	}
    }
    while ((retryCount++ < DISCONNECT_RETRY_COUNT)
	   && (retval == IX_ATMDACC_RESOURCES_STILL_ALLOCATED));

    if (retval != IX_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "tx disconnect failed (%d)\n",
		    retval, 0, 0, 0, 0, 0);
	return retval;
    }

    return IX_SUCCESS;
}

/* ---------------------------------------------------
 */
PUBLIC IX_STATUS
ixAtmUtilsAtmVcUnregisterDisconnect (IxAtmConnId rxConnId, IxAtmConnId txConnId)
{
    IX_STATUS retval = IX_SUCCESS;
    UINT32 rxChannelIdx;
    UINT32 txChannelIdx;

    if (rxChannelFind(rxConnId, &rxChannelIdx) == IX_SUCCESS)
    {
        retval = rxVcDisconnect(rxConnId);
        if (retval != IX_SUCCESS)
        {
            ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to disconnect an rx channel\n",
			0, 0, 0, 0, 0, 0);
            return IX_FAIL;
        }

        retval = ixAtmmVcDeregister(rxPort[rxChannelIdx], rxVcId[rxChannelIdx]);
        if (retval != IX_SUCCESS)
        {
            ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to unregister an rx channel\n",
			0, 0, 0, 0, 0, 0);
            return IX_FAIL;
        }
        rxChannelInfo[rxChannelIdx].inUse = FALSE;
    }

    if (txChannelFind(txConnId, &txChannelIdx) == IX_SUCCESS)
    {
        retval = txVcDisconnect(txConnId);
        if (retval != IX_SUCCESS)
        {
            ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to disconnect a tx channel\n",
			0, 0, 0, 0, 0, 0);
            return IX_FAIL;
        }

        retval = ixAtmmVcDeregister(txPort[txChannelIdx], txVcId[txChannelIdx]);
        if (retval != IX_SUCCESS)
        {
            ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to unregister a tx channel\n",
			0, 0, 0, 0, 0, 0);
            return IX_FAIL;
        }
        txChannelInfo[txChannelIdx].inUse = FALSE;
    }

    return IX_SUCCESS;
}

/* ---------------------------------------------------
 */
PUBLIC IX_STATUS
ixAtmUtilsAtmAllVcsDisconnect (void)
{
    IX_STATUS retval;
    int channelIdx;

    for (channelIdx=0; channelIdx<IX_ATM_MAX_NUM_VC; channelIdx++)
    {
        retval = ixAtmUtilsAtmVcUnregisterDisconnect (rxChannelInfo[channelIdx].connId, 
						       txChannelInfo[channelIdx].connId);
        if (retval != IX_SUCCESS)
        {
	    ixOsalLog(IX_OSAL_LOG_LVL_ERROR,IX_OSAL_LOG_DEV_STDERR, "Failed to unregister a channel\n",
			0, 0, 0, 0, 0, 0);
	    return IX_FAIL;
        }
    }
    return IX_SUCCESS;
}

/* ---------------------------------------------------
 */
PUBLIC IX_STATUS
ixAtmUtilsAtmImageDownload (unsigned numPorts, 
			     IxAtmmPhyMode *phyMode)
{
    IX_STATUS retval = IX_SUCCESS;

    retval = ixNpeDlNpeInitAndStart (IX_ATM_CODELET_NPEA_IMAGE_ID);
    if (retval != IX_SUCCESS)
    {
	printf ("Failed to download NPEA image\n");
    }

    if (retval == IX_SUCCESS)
    {
	*phyMode = IX_ATM_CODELET_PHY_MODE;
    }
    else
    {
	printf("NPEA download failed \n");
    }

    return retval;
}

PRIVATE IX_STATUS
rxFreeChannelGet (UINT32 *channelId)
{
    int i;
    for (i = 0; i < IX_ATM_MAX_NUM_VC; i ++)
    {
	if (!rxChannelInfo[i].inUse)
	{	    
	    *channelId = i;
	    rxChannelInfo[i].inUse = TRUE;
	    return IX_SUCCESS;
	}
    }
    return IX_FAIL;
}

PRIVATE IX_STATUS
txFreeChannelGet (UINT32 *channelId)
{
    int i;
    for (i = 0; i < IX_ATM_MAX_NUM_VC; i ++)
    {
	if (!txChannelInfo[i].inUse)
	{	    
	    *channelId = i;
	    txChannelInfo[i].inUse = TRUE;
	    return IX_SUCCESS;
	}
    }
    return IX_FAIL;
}

PRIVATE IX_STATUS
rxChannelFind (IxAtmConnId connId, UINT32 *channelId)
{
    int i;
    for (i = 0; i < IX_ATM_MAX_NUM_VC; i ++)
    {
	if ((rxChannelInfo[i].inUse) && (rxChannelInfo[i].connId == connId))
	{	    
	    *channelId = i;
	    return IX_SUCCESS;
	}
    }
    return IX_FAIL;
}

PRIVATE IX_STATUS
txChannelFind (IxAtmConnId connId, UINT32 *channelId)
{
    int i;
    for (i = 0; i < IX_ATM_MAX_NUM_VC; i ++)
    {
	if ((txChannelInfo[i].inUse) && (txChannelInfo[i].connId == connId))
	{	    
	    *channelId = i;
	    return IX_SUCCESS;
	}
    }
    return IX_FAIL;
}
         
