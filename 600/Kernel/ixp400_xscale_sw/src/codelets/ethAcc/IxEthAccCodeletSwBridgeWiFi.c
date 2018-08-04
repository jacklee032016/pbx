/**
 * @file IxEthAccCodeletSwBridgeWiFi.c
 *
 * @date 25 August 2004
 *
 * @brief This file contains the implementation of the Ethernet Access Codelet.
 *
 * This file implements a simple bridge between two Ethernet ports with a 
 * 802.3 <=> 802.11 frame header conversion set up at NPE level.
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
 * Put the system defined include files required.
 */

/*
 * Put the user defined include files required.
 */

#include "IxEthAcc.h"
#include "IxEthDB.h"
#include "IxOsal.h"
#include "IxEthAccCodelet.h"
#include "IxEthAccCodelet_p.h"

/* prototypes */
void ixEthAccCodeletWiFiBridgeRxCB(UINT32 cbTag, IX_OSAL_MBUF* mBufPtr, UINT32 reserved);
IX_STATUS ixEthAccCodeletSwWiFiBridgeStart(IxEthAccPortId wifiPortId, IxEthAccPortId bridgedPortId);
UINT32 ixEthAccCodeletWiFiAddressCompare(UINT8 *mac1, UINT8 *mac2);

PRIVATE IxEthDBMacAddr ixEthAccCodeletWiFiStaMacAddress   = { {0x00, 0x00, 0x11, 0x22, 0x33, 0x44} };
PRIVATE IxEthDBMacAddr ixEthAccCodeletWiFiApMacAddress    = { {0x00, 0x00, 0xA1, 0xA2, 0xA3, 0xA4} };
PRIVATE IxEthDBMacAddr ixEthAccCodeletWiFiGwMacAddress    = { {0x00, 0x00, 0xF9, 0xF8, 0xF7, 0xF6} };
PRIVATE IxEthDBMacAddr ixEthAccCodeletWiFiBBSIDMacAddress = { {0x00, 0xFF, 0xFE, 0xFD, 0xFC, 0xFB} };

PRIVATE UINT16 ixEthAccCodeletWiFiDurationID   = 0x1234;
PRIVATE UINT16 ixEthAccCodeletWiFiFrameControl = 0xABCD;

extern const char *mac2string(const unsigned char *mac);

/*
 * Function definition: ixEthAccCodeletSwBridgeWiFiStart()
 *
 * Configure WiFi header conversion and Start bridge datapath
 */
IX_STATUS ixEthAccCodeletSwBridgeWiFiStart(IxEthAccPortId wifiPortId, 
					       IxEthAccPortId bridgedPortId)
{
    IxEthDBFeature featureSet = 0;

    if (wifiPortId == bridgedPortId)
    {
	printf("SwBridgeWiFi: Cannot configure a Bridge Operation between port %u and port %u (ports must be different)\n",
	       wifiPortId, 
               bridgedPortId);
	return (IX_FAIL);
    }

    /* Perform basic port configuration (actual callbacks will be registered later, 
     * in ixEthAccCodeletSwWiFiBridgeStart) */
    if ( ixEthAccCodeletPortConfigure(wifiPortId,
                                      ixEthAccCodeletBridgeRxCB,
                                      NULL,
                                      ixEthAccCodeletBridgeTxCB,
                                      0 /* no callback tag */) != IX_SUCCESS)
    {
        printf("SwBridgeWiFi: Failed to configure Port %u\n",
               wifiPortId);
        return (IX_FAIL);
    }

    if ( ixEthAccCodeletPortConfigure(bridgedPortId,
                                      ixEthAccCodeletBridgeRxCB,
                                      NULL,
                                      ixEthAccCodeletBridgeTxCB,
                                      0 /* no callback tag */) != IX_SUCCESS)
    {
        printf("SwBridgeWiFi: Failed to configure Port %u\n",
               bridgedPortId);
        return (IX_FAIL);
    }

    /* Enable the Port in EthDB in order to configure and download the
     * WiFi Database 
     */ 
    if (ixEthDBPortEnable(wifiPortId) != IX_ETH_DB_SUCCESS)
    {
        printf("SwBridgeWiFi: Cannot enable port %u\n", wifiPortId);
        return (IX_FAIL);
    }

    if (ixEthDBPortEnable(bridgedPortId) != IX_ETH_DB_SUCCESS)
    {
        printf("SwBridgeWiFi: Cannot enable port %u\n", bridgedPortId);
        return (IX_FAIL);
    }

    /* check feature */
    if (IX_ETH_DB_SUCCESS != ixEthDBFeatureCapabilityGet(wifiPortId, &featureSet))
    {
        printf("SwBridgeWiFi: Could not get the feature capabilities of the selected WiFi port (%d)\n", wifiPortId);
        return IX_FAIL;
    }

    if ((featureSet & IX_ETH_DB_WIFI_HEADER_CONVERSION) == 0)
    {
        printf("SwBridgeWiFi: Selected WiFi port (%d) does not have an NPE image capable of 802.11 header conversion\n", wifiPortId);
        return IX_FAIL;
    }

    featureSet = 0;

    if (IX_ETH_DB_SUCCESS != ixEthDBFeatureCapabilityGet(bridgedPortId, &featureSet))
    {
        printf("SwBridgeWiFi: Could not get the feature capabilities of the selected bridge port (%d)\n", bridgedPortId);
        return IX_FAIL;
    }

    if ((featureSet & IX_ETH_DB_WIFI_HEADER_CONVERSION) != 0)
    {
        printf("SwBridgeWiFi: Selected bridge port (%d) should not have an NPE image capable of 802.11 header conversion\n", bridgedPortId);
        return IX_FAIL;
    }

    /* enable feature */
    if (IX_ETH_DB_SUCCESS != ixEthDBFeatureEnable(wifiPortId, IX_ETH_DB_WIFI_HEADER_CONVERSION, TRUE))
    {
        printf("SwBridgeWiFi: Failed to enable the WiFi header conversion feature, exiting\n");
        return IX_FAIL;
    }

    /* set the conversion table */
    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiStationEntryAdd(wifiPortId, &ixEthAccCodeletWiFiStaMacAddress))
    {
        printf("SwBridgeWiFi: Failed to add Station address, exiting\n");
        return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiAccessPointEntryAdd(wifiPortId, 
        &ixEthAccCodeletWiFiApMacAddress, 
        &ixEthAccCodeletWiFiGwMacAddress))
    {
        printf("SwBridgeWiFi: Failed to add AccessPoint address, exiting\n");
        return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiConversionTableDownload(wifiPortId))
    {
        printf("SwBridgeWiFi: Failed to download WiFi frame header conversion table, exiting\n");
        return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiBBSIDSet(wifiPortId, &ixEthAccCodeletWiFiBBSIDMacAddress))
    {
        printf("SwBridgeWiFi: Failed to set the BBSID value, exiting\n");
        return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiFrameControlSet(wifiPortId, ixEthAccCodeletWiFiFrameControl))
    {
        printf("SwBridgeWiFi: Failed to set the FrameControl value, exiting\n");
        return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBWiFiDurationIDSet(wifiPortId, ixEthAccCodeletWiFiDurationID))
    {
        printf("SwBridgeWiFi: Failed to set the Duration/ID value, exiting\n");
        return IX_FAIL;
    }

    /* display user instructions */
    printf("\n");
    printf("-------------------------------------------------------------------------------\n");
    printf("| Sending packets to port %d using [%s] as the destination MAC  |\n", wifiPortId, mac2string(ixEthAccCodeletWiFiStaMacAddress.macAddress));
    printf("| will convert their headers to 802.11 STA->AP frames. Capture the frames on  |\n");
    printf("| port %d with your test equipment to view the conversion result.              |\n", bridgedPortId);
    printf("|                                                                             |\n");
    printf("| Sending packets to port %d using [%s] as the destination MAC  |\n", wifiPortId, mac2string(ixEthAccCodeletWiFiApMacAddress.macAddress));
    printf("| will convert their headers to 802.11 AP->AP frames. The conversion uses     |\n");
    printf("| [%s] as gateway MAC address. Capture the frames on port %d    |\n", mac2string(ixEthAccCodeletWiFiGwMacAddress.macAddress), bridgedPortId);
    printf("| with your test equipment to view the conversion result.                     |\n");
    printf("|                                                                             |\n");
    printf("| Sending 802.11 frames generated as above to port %d, when bridged to port %d, |\n", bridgedPortId, wifiPortId);
    printf("| will convert them to 802.3 frames. Observe the output on port %d.            |\n", wifiPortId);
    printf("|                                                                             |\n");
    printf("| This test is using : [%s] as BBSID                           |\n", mac2string(ixEthAccCodeletWiFiBBSIDMacAddress.macAddress));
    printf("|                      0x%4x as FrameControl                                 |\n", ixEthAccCodeletWiFiFrameControl);
    printf("|                      0x%4x as Duration/ID                                  |\n", ixEthAccCodeletWiFiDurationID);
    printf("-------------------------------------------------------------------------------\n");
    
    
    return ixEthAccCodeletSwWiFiBridgeStart(wifiPortId, bridgedPortId);
}

/*
 * Function definition: ixEthAccCodeletSwBridgeStart()
 *
 * Start bridge datapath
 */
IX_STATUS ixEthAccCodeletSwWiFiBridgeStart(IxEthAccPortId wifiPortId, 
				       IxEthAccPortId bridgedPortId)
{
    UINT32 firstPortCbTag = wifiPortId | (bridgedPortId << 16);
    UINT32 secondPortCbTag = bridgedPortId | (wifiPortId << 16);

    /* Configure and register the traffic callbacks for both ports */
    if ( ixEthAccCodeletPortConfigure(wifiPortId, 
				      ixEthAccCodeletBridgeRxCB, 
				      (IxEthAccPortMultiBufferRxCallback) NULL,
				      ixEthAccCodeletBridgeTxCB,
                                      firstPortCbTag) != IX_SUCCESS)
    {
	printf("SwBridge: Failed to configure Port %u\n", wifiPortId);
	return (IX_FAIL);
    }

    if ( ixEthAccCodeletPortConfigure(bridgedPortId, 
				      ixEthAccCodeletWiFiBridgeRxCB, 
				      (IxEthAccPortMultiBufferRxCallback) NULL,
				      ixEthAccCodeletBridgeTxCB,
                                      secondPortCbTag) != IX_SUCCESS)
    {
	printf("SwBridge: Failed to configure Port %u\n", bridgedPortId);
	return (IX_FAIL);
    }

    /* Enable the traffic over both ports */
    if ( ixEthAccPortEnable(wifiPortId) != IX_SUCCESS)
    {
	printf("SwBridge: Failed to enable Port %u\n", wifiPortId);
	return (IX_FAIL);
    }

    if ( ixEthAccPortEnable(bridgedPortId) != IX_SUCCESS)
    {
	printf("SwBridge: Failed to enable Port %u\n", bridgedPortId);
	return (IX_FAIL);
    }

    return (IX_SUCCESS);
}

UINT32 ixEthAccCodeletWiFiAddressCompare(UINT8 *mac1, UINT8 *mac2)
{
    UINT32 local_index;

    for (local_index = 0 ; local_index < IX_IEEE803_MAC_ADDRESS_SIZE ; local_index++)
    {
        if (mac1[local_index] > mac2[local_index])
        {
            return 1;
        }
        else if (mac1[local_index] < mac2[local_index])
        {
            return -1;
        }
    }

    return 0;
}

/**
 * Function definition: ixEthAccCodeletWiFiBridgeRxCB()
 *
 * Receive callback for Bridge Operation. Received frames are retransmitted
 * on the port loaded with the WiFi header conversion image, and 802.11
 * frames are configured (by setting the link_prot flag) to be converted
 * into 802.3 format.
 */

void ixEthAccCodeletWiFiBridgeRxCB(UINT32 cbTag, 
			       IX_OSAL_MBUF* mBufPtr, 
			       UINT32 reserved)
{
    /* Transmit the buffer on the other port */
    UINT16 frameControl;
    UINT16 durationID;
    UINT8 *firstAddr;
    UINT16 linkProt = 0;

    IxEthAccPortId wifiPortId    = cbTag >> 16;
    IxEthAccPortId bridgedPortId = cbTag & 0xFFFF;

    ixEthAccCodeletStats[bridgedPortId].rxCount++;

    /* 802.11 frames generated by the codelet can be detected by
     * inspecting the Frame Control, Duration/ID and first 802.11 address */
    IX_OSAL_CACHE_INVALIDATE(IX_OSAL_MBUF_MDATA(mBufPtr), IX_OSAL_MBUF_MLEN(mBufPtr));

    frameControl = IX_OSAL_SWAP_BE_SHARED_SHORT(* ((UINT16 *) IX_OSAL_MBUF_MDATA(mBufPtr)));
    durationID   = IX_OSAL_SWAP_BE_SHARED_SHORT(*(((UINT16 *) IX_OSAL_MBUF_MDATA(mBufPtr)) + 1));
    firstAddr    = ((UINT8 *) IX_OSAL_MBUF_MDATA(mBufPtr)) + 4;

    if ((frameControl == ixEthAccCodeletWiFiFrameControl) && (durationID == ixEthAccCodeletWiFiDurationID))
    {
        if (ixEthAccCodeletWiFiAddressCompare(firstAddr, ixEthAccCodeletWiFiGwMacAddress.macAddress) == 0)
        {
            /* AP to AP frame, set link_prot to 11b << 4*/
            linkProt = IX_ETHACC_RX_APTYPE;
        }
        else if (ixEthAccCodeletWiFiAddressCompare(firstAddr, ixEthAccCodeletWiFiStaMacAddress.macAddress) == 0)
        {
            /* AP to STA frame, set link_prot to 10b << 4 */
            linkProt = IX_ETHACC_RX_STATYPE;
        }

        IX_ETHACC_NE_FLAGS(mBufPtr) &= ~IX_ETHACC_NE_LINKMASK;
        IX_ETHACC_NE_FLAGS(mBufPtr) |= linkProt;
    }

    if(ixEthAccPortTxFrameSubmit(wifiPortId, mBufPtr, IX_ETH_ACC_TX_DEFAULT_PRIORITY)
	!=IX_ETH_ACC_SUCCESS)
    {
	ixOsalLog(IX_OSAL_LOG_LVL_ERROR, IX_OSAL_LOG_DEV_STDOUT,
		    "Tx Buffer submission failed on port 1\n", 
		    0, 0, 0, 0, 0, 0);
    }
}

/*
 * Function definition: ixEthAccCodeletSwBridgeWiFiStop()
 *
 * Unconfigure WiFi and Stop bridge datapath
 */
IX_STATUS ixEthAccCodeletSwBridgeWiFiStop(IxEthAccPortId firstPortId, 
                                          IxEthAccPortId secondPortId)
{
    /* clears the WiFi address table */
    if (IX_ETH_DB_SUCCESS != ixEthDBDatabaseClear(IX_ETH_PORT_1, IX_ETH_DB_WIFI_RECORD))
    {
	printf("Failed to clear the WiFi database\n");
	return IX_FAIL;
    }

    if (IX_ETH_DB_SUCCESS != ixEthDBFeatureEnable(IX_ETH_PORT_1, IX_ETH_DB_WIFI_HEADER_CONVERSION, FALSE))
    {
        printf("Failed to disable the WiFi header conversion feature\n");
        return IX_FAIL;
    }

    return ixEthAccCodeletSwBridgeStop(firstPortId, secondPortId);
}
