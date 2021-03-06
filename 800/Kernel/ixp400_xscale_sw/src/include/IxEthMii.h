/**
 * @file    IxEthMii.h
 *
 * @brief this file contains the public API of @ref IxEthMii component
 *
 * Design notes :  
 * The main intent of this API is to inplement MII high level fonctionalitoes
 * to support the codelets provided with the IXP400 software releases. It 
 * superceedes previous interfaces provided with @ref IxEThAcc component.
 * 
 * This API has been tested with the PHYs provided with the
 * IXP400 development platforms. It may not work for specific Ethernet PHYs
 * used on specific boards.
 *
 * This source code detects and interface the LXT972, LXT973 and KS6995
 * Ethernet PHYs.
 *
 * This source code should be considered as an example which may need
 * to be adapted for different hardware implementations.
 *
 * It is strongly recommended to use public domain and GPL utilities
 * like libmii, mii-diag for MII interface support.
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

#ifndef IxEthMii_H
#define IxEthMii_H

#include <IxTypes.h>

/**
 * @defgroup IxEthMii IXP400 Ethernet Phy Access (IxEthMii) API
 *
 * @brief ethMii is a library that does provides access to the 
 * Ethernet PHYs
 *
 *@{
 */

/**
 * @ingroup IxEthMii
 *
 * @fn  ixEthMiiPhyScan(BOOL phyPresent[], UINT32 maxPhyCount)
 *
 * @brief Scan the MDIO bus for PHYs
 *  This function scans PHY addresses 0 through 31, and sets phyPresent[n] to 
 *  TRUE if a phy is discovered at address n. 
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @pre The MAC on Ethernet Port 2 (NPE C) must be initialised, and generating the MDIO clock.
 *
 * @param phyPresent BOOL [in] - boolean array of IXP425_ETH_ACC_MII_MAX_ADDR entries
 * @param maxPhyCount UINT32 [in] - number of PHYs to search for (the scan will stop when
 *         the indicated number of PHYs is found).
 *
 * @return IX_STATUS
 * - IX_ETH_ACC_SUCCESS
 * - IX_ETH_ACC_FAIL : invalid arguments.
 *
 * <hr>
 */
PUBLIC IX_STATUS ixEthMiiPhyScan(BOOL phyPresent[], UINT32 maxPhyCount);

/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiPhyConfig(UINT32 phyAddr,
   		         BOOL speed100, 
 			 BOOL fullDuplex, 
 			 BOOL autonegotiate)
 *
 *
 * @brief Configure a PHY
 *   Configure a PHY's speed, duplex and autonegotiation status
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @pre The MAC on Ethernet Port 2 (NPE C) must be initialised, and generating the MDIO clock.
 *   
 * @param phyAddr UINT32 [in] 
 * @param speed100 BOOL [in] - set to TRUE for 100Mbit/s operation, FALSE for 10Mbit/s
 * @param fullDuplex BOOL [in] - set to TRUE for Full Duplex, FALSE for Half Duplex
 * @param autonegotiate BOOL [in] - set to TRUE to enable autonegotiation
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 *
 * <hr>
 */
PUBLIC IX_STATUS ixEthMiiPhyConfig(UINT32 phyAddr,
				    BOOL speed100, 
				    BOOL fullDuplex, 
				    BOOL autonegotiate);

/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiPhyLoopbackEnable(UINT32 phyAddr)
 *
 *
 * @brief Enable PHY Loopback in a specific Eth MII port
 *
 * @note When PHY Loopback is enabled, frames sent out to the PHY from the
 * IXP400 will be looped back to the IXP400. They will not be transmitted out
 * on the wire.
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @param phyAddr UINT32 [in] - the address of the Ethernet PHY (0-31)
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 * <hr>
 */
PUBLIC IX_STATUS
ixEthMiiPhyLoopbackEnable (UINT32 phyAddr);

/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiPhyLoopbackDisable(UINT32 phyAddr)
 *
 *
 * @brief Disable PHY Loopback in a specific Eth MII port
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *   
 * @param phyAddr UINT32 [in] - the address of the Ethernet PHY (0-31)
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 * <hr>
 */
PUBLIC IX_STATUS
ixEthMiiPhyLoopbackDisable (UINT32 phyAddr);

/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiPhyReset(UINT32 phyAddr)
 *
 * @brief Reset a PHY
 *   Reset a PHY
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @pre The MAC on Ethernet Port 2 (NPE C) must be initialised, and generating the MDIO clock.
 *   
 * @param phyAddr UINT32 [in] - the address of the Ethernet PHY (0-31)
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 *
 * <hr>
 */
PUBLIC IX_STATUS ixEthMiiPhyReset(UINT32 phyAddr);


/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiLinkStatus(UINT32 phyAddr, 
 			  BOOL *linkUp,
 			  BOOL *speed100, 
 			  BOOL *fullDuplex,
 		          BOOL *autoneg)
 *
 * @brief Retrieve the current status of a PHY
 *   Retrieve the link, speed, duplex and autonegotiation status of a PHY
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @pre The MAC on Ethernet Port 2 (NPE C) must be initialised, and generating the MDIO clock.
 *   
 * @param phyAddr UINT32 [in] - the address of the Ethernet PHY (0-31)
 * @param linkUp BOOL [out] - set to TRUE if the link is up
 * @param speed100 BOOL [out] - set to TRUE indicates 100Mbit/s, FALSE indicates 10Mbit/s
 * @param fullDuplex BOOL [out] - set to TRUE indicates Full Duplex, FALSE indicates Half Duplex
 * @param autoneg BOOL [out] - set to TRUE indicates autonegotiation is enabled, FALSE indicates autonegotiation is disabled
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 *
 * <hr>
 */
PUBLIC IX_STATUS ixEthMiiLinkStatus(UINT32 phyAddr, 
				     BOOL *linkUp,
				     BOOL *speed100, 
				     BOOL *fullDuplex,
				     BOOL *autoneg);

/**
 * @ingroup IxEthMii
 *
 * @fn ixEthMiiPhyShow (UINT32 phyAddr)
 *
 *
 * @brief Display information on a specified PHY
 *   Display link status, speed, duplex and Auto Negotiation status
 *
 * - Reentrant    - no
 * - ISR Callable - no
 *
 * @pre The MAC on Ethernet Port 2 (NPE C) must be initialised, and generating the MDIO clock.
 *   
 * @param phyAddr UINT32 [in] - the address of the Ethernet PHY (0-31)
 *
 * @return IX_STATUS
 * - IX_SUCCESS
 * - IX_FAIL : invalid arguments.
 *
 * <hr>
 */
PUBLIC IX_STATUS ixEthMiiPhyShow (UINT32 phyAddr);

#endif /* ndef IxEthMii_H */
/**
 *@}
 */
