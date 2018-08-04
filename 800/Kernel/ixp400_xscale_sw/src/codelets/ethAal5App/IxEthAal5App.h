/**
 * @file IxEthAal5App.h
 *
 * @author Intel Corporation
 * @date 3 Apr 2002
 *
 * @brief This file contains the declaration of the IxEthAal5App
 * component's main functions, etc.
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
#ifndef IXETHAAL5APP_H
#define IXETHAAL5APP_H

/**
 * @defgroup Codelets IXP400 Codelets
 *
 * @brief IXP400 Codelets
 *
 * @{
 */

/**
 * @ingroup Codelets
 *
 * @defgroup IxEthAal5App IXP400 Ethenet Aal5 (IxEthAal5App) API 
 *
 * @brief IXP400 Ethernet Aal5 Codelet component API
 * 
 * @par
 * IxEthAal5App application is also called as IXP425 Mini Bridge application
 * which  bridges traffic between Ethernet and Utopia ports or Ethernet
 * and ADSL ports. It uses ixEthAcc,ixAtmdAcc, ixAtmm, ixAtmSch and ixQmgr
 * software components.
 *
 * <b> VxWorks User Guide </b><br>
 * ixEthAal5AppCodeletMain() function is used as a single point of execution
 * for EthAal5 Codelet, which allows the user to enter in 2 type of modes:
 * Utopia or ADSL, in order to operate together with ethernet.
 *
 * <pre>
 *  <i> Usage :
 *      ixEthAal5AppCodeletMain (modeType)
 *	  modeType:
 *                  1 = Utopia
 *                  2 = ADSL
 * </i>
 * </pre>
 *
 * <b> Linux User Guide </b><br>
 * ixEthAal5AppCodeletMain() function is used as a single point of execution 
 * for EthAal5 Codelet, which allows the user to enter in 2 type of modes:
 * Utopia or ADSL, in order to operate together with ethernet.
 *
 * <pre>
 *  <i> Usage :
 *      # insmod ixp400_codelets_ethAal5App.o modeType=<x>
 *      Where x:
 *                  1 = Utopia
 *                  2 = ADSL
 * </i>
 * </pre>
 *
 * Note for VxWorks and Linux Usage:
 * In order to observe the current traffic counters, the ixEAAShow() function
 * is executed every 15seconds. This applys to both Utopia and ADSL mode.
 *
 *
 * @par
 * <B>Features</B><br>
 * This Application currently supports 2 Ethernet ports and up to 8 Utopia 
 * phys,which are initialized by default at the start of application. Ethernet 
 * frames are transferred across ATM link (through Utopia interface) using AAL5
 * protocol and Ethernet frame encapsulation described by RFC 1483. MAC address
 * learning is performed on Ethernet frames, received by Ethernet ports and ATM
 * interface (encapsulated). Application filters packets basing on destination 
 * MAC addresses - packets are forwarded to other port only if the port has 
 * ever received packet/frame with the same source MAC address. Forwarding is 
 * done only between Ethernet and Utopia port. Two simplifications were made to
 * keep code simple: 
 *  - Application doesn't allow packet forwarding between Ethernet ports (nor 
 *    Utopia ports).
 *  - flooding (forwarding frames/packets with unknown MAC addresses) is
 *    not supported. Two IxEthAal5App will never transfer any packets between 
 *    each other, because initialy MAC data base is empty, so all packets will
 *    be filtered out. However there is function ixEAAAddMAC which can be used
 *    to add MAC address to the data base and assign it to one of available 
 *    ports. To enable simplified flooding see comments in ixEAAEthRxCallback.
 *  - This application can not be executed more than once. It doesn't 
 *    deinitialize itself. If user wishes to change configuration and run 
 *    application again the whole system (vxWorks) must be restarted.
 *  - currently Mac Learning/Filtering database in ixEthAcc component supports
 *    only Ethernet ports. For that reason it couldn't be used in this 
 *    application for learning Mac addresses from encapsulated Ethernet frames
 *    received from Utopia. In the near future ixEthAcc component will support
 *    all possible ports (including Utopia), but by this time a very simplified
 *    approach is used in this application: 
 *       only one Mac address is stored per VC (and there is one VC per Phy). 
 *       It means, that only one Mac address is supported simultaneously per
 *       Phy. This is done to keep code as simple as possible.
 *  - This application doesn't initialize any DSL connection. It initializes
 *    Utopia interface - if any DSL card is attached to the Richfield board
 *    it should be initialized before start of application (e.g. to initialize
 *    Adsl card call 'adslUp' function from windshell - adsl module must be
 *    separately loaded).
 *  - 2 protocols from RFC 1483 are recognized: The first packet received from 
 *    ATM will decide the behaviour of the application (ether bridged or 
 *    routed)
 *
 * @par
 * <B>From RFC 1483</B><br>
 *  VC Based Multiplexing of Routed Protocols
 *
 *  PDUs of routed protocols shall be carried as such in the Payload of
 *  the AAL5 CPCS-PDU.  The format of the AAL5 CPCS-PDU Payload field
 *  thus becomes:
 *
 * @verbatim
               Payload Format for Routed PDUs
               +-------------------------------+
               |             .                 |
               |         Carried PDU           |
               |    (up to 2^16 - 1 octets)    |
               |             .                 |
               |             .                 |
               +-------------------------------+
   @endverbatim
 *
 * VC Based Multiplexing of Bridged Protocols
 *
 *  PDUs of bridged protocols shall be carried in the Payload of the AAL5
 *  CPCS-PDU.
 *
 * @verbatim
               Payload Format for Bridged Ethernet/802.3 PDUs
               +-------------------------------+
               |         PAD 0x00-00           |
               +-------------------------------+
               |    MAC destination address    |
               +-------------------------------+
               |                               |
               |   (remainder of MAC frame)    |
               |                               |
               +-------------------------------+
               | LAN FCS (VC dependent option) |
               +-------------------------------+
   @endverbatim
 *  
 * Data flow is illustrated in the diagram below:
 * @verbatim
   +--------------------------------------------------------------------------+
   |       IXP425                                                             |
   | +------------------------------------------------------------------------+
   | |       XScale                                                           |
   | |                                                                        |
   | |    +-------------------------------------------------------------------+
   | |    |       ethAal5App                                                  |
   | |    |                                                                   |
   | |    |   +--------------------------+      +------------------------+    |
   | |    |   | ixEAAEthTxDoneCallback   |      | ixEAAAtmTDonexCallback |    |
   | |    |   | -----------------        |      | -----------------      |    |
   | |  +---->| After mbuffer data was   |      | After mbuffer data was |    |
   | |  | |   | sent, ixEthAcc component |   +--|sent, ixEthAcc component|<-+ |
   | |  | |   | returns buffer back. It  |   |  | returns buffer back. It|  | |
   | |  | |   | will be given back to    |   |  | will be given back to  |  | |
   | |  | |   | ixAtmdAcc component.     |   |  | ixAtmdAcc component.   |  | |
   | |  | |   +--------------------------+   |  +------------------------+  | |
   | |  | |                           |      |                              | |
   | |  | |                           |      |                              | |
   | |  | |   +--------------------+  |      |  +--------------------+      | |
   | |  | |   | ixEAAEthRxCallback |  |      |  | ixEAAAtmRxCallback |      | |
   | |  | |   | -----------------  |  |      |  | -----------------  |      | |
   | |  | | +>| Received frame is  |  |      |  | Ethernet frame is  |      | |
   | |  | | | | encapsulated into  |  |      |  | extracted from Atm |<-+   | |
   | |  | | | | Atm packet and sent|  |   +--+  | packet and sent    |  |   | |
   | |  | |Rx | to Utopia through  |  |   |     | to Eth phy through |  |   | |
   | |  | | | |ixAtmdAcc component.|  |   |     |ixEthAcc component. |  |   | |
   | |  | | | +--------------------+  |   |     +--------------------+  |   | |
   | |  | +---------------------|-----|---|--------------------|--------|---|-+
   | |  |   |         +--Tx-----|-----|---|--------------------+        |   | |
   | |  +---------+   |    +----|-----|---+  +---------+                |   | |
   | |  |ixEthAcc |<--+    |    |     +----->|ixAtmdAcc|-----Rx---------+   | |
   | |  |         |<-------+    +----Tx----->|         |--------------------+ |
   | +------------------------------------------------------------------------+
   |     |        \                               |
   |     |         \                              |
   | +-------+     +-------+                      |
   | | NPE B |     | NPE C |                      |
   | +-------+     +-------+                +-----------+
   |   |   |          |  |                  | Utopia    |
   |   |   |          |  |                  |           |
   |  Eth Mac        Eth Mac                |           |
   +-----------------------------------------------------------
       |                |                          |
       |                |                          |
     +---------+    +---------+                    |
     |Eth Phy 1|    |Eth Phy 2|                    
     +---------+    +---------+
   @endverbatim
 *     
 * @par
 * <B>Configuration Example:</B>
 * @verbatim
   +-----------+          +-----------------------+          +---------+
   | Smartbits | <------> | (Eth) IXP425 (Utopia) | <------> | Adtech  |
   +-----------+          +-----------------------+          +---------+
   @endverbatim 

 *   On smartbits set: dst. mac = MAC1  and src mac = MAC2 <br>
 *   On Adtech add AAL5 with eth. encapsulation, dst. mac = MAC2, src. MAC=MAC1 <br>
 *   Dependently which Phy is used Adtech must use same Vpi/Vci address as 
 *   assigned to Phy by ethAal5App. By default for Phy 0 it is 1/100 (Vpi/Vci),
 *   for Phy 1 2/100 and so on.
 *
 * Another option is: <br>
 * @verbatim
  +----------+     +-------------------+     +-------------------+     +----------+
  |Smartbits1| <-> |(Eth)IXP425(Utopia)| <-> |(Eth)IXP425(Utopia)| <-> |Smartbits2|
  +----------+     +-------------------+     +-------------------+     +----------+
   @endverbatim 
 *
 *   On smartbits1 set: dst. mac = MAC1  and src mac = MAC2 <br>
 *   On smartbits2 set: dst. mac = MAC2  and src mac = MAC1 <br>
 *   on IXP425 connected to Smartbits 1 execute in windshell: <br>
 *      ixEAAAddMAC( 1, MAC1[0], MAC1[1], ..., MAC1[5] ); <br>
 *   We use port 1 for Adsl card, however it may change in the future.
 *
 * @{
 */

#include "IxOsal.h"
#include "IxEthAcc.h"
#include "IxAtmTypes.h"

#include "EthAal5User.h"
#include "IxEthAal5App_p.h"

/**
 * @ingroup IxEthAal5App
 *
 * @def IX_EAA_NUM_BUFFERS_PER_ETH
 * @brief  This is the number of buffers, which can be stored in free buffer 
 * queue for each ethernet port 
 */
#ifndef IX_EAA_NUM_BUFFERS_PER_ETH
#define IX_EAA_NUM_BUFFERS_PER_ETH  64
#endif

/**
 * @ingroup IxEthAal5App
 *
 * @def IX_EAA_NUM_ATM_PORTS
 * @brief Define number of supported atm ports by this application
 *
 * User can define them in EthAal5User.h file to overwrite definitions below
 */
#ifndef  IX_EAA_NUM_ATM_PORTS
#if IX_UTOPIAMODE == 1
#define IX_EAA_NUM_ATM_PORTS     1
#else
#define IX_EAA_NUM_ATM_PORTS     8
#endif
#endif

/**
 * @ingroup IxEthAal5App
 *
 * @def IX_EAA_NUM_BUFFERS_PER_VC
 * @brief This is the number of buffers per atm port 
 *
 * User can define them in EthAal5User.h file to overwrite definitions below
 */
#ifndef IX_EAA_NUM_BUFFERS_PER_VC
#define IX_EAA_NUM_BUFFERS_PER_VC 16 
#endif

/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default VPI and VCI for 8 ports. 
 * User can define them in EthAal5User.h file to overwrite definitions below
 */
#ifndef IX_EAA_PORT1_VPI
#define IX_EAA_PORT1_VPI 1
#endif
#ifndef IX_EAA_PORT1_VCI
#define IX_EAA_PORT1_VCI 100
#endif
#ifndef IX_EAA_PORT2_VPI
#define IX_EAA_PORT2_VPI 2
#endif
#ifndef IX_EAA_PORT2_VCI
#define IX_EAA_PORT2_VCI 100
#endif
#ifndef IX_EAA_PORT3_VPI
#define IX_EAA_PORT3_VPI 3
#endif
#ifndef IX_EAA_PORT3_VCI
#define IX_EAA_PORT3_VCI 100
#endif
#ifndef IX_EAA_PORT4_VPI
#define IX_EAA_PORT4_VPI 4
#endif
#ifndef IX_EAA_PORT4_VCI
#define IX_EAA_PORT4_VCI 100
#endif
#ifndef IX_EAA_PORT5_VPI
#define IX_EAA_PORT5_VPI 5
#endif
#ifndef IX_EAA_PORT5_VCI
#define IX_EAA_PORT5_VCI 100
#endif
#ifndef IX_EAA_PORT6_VPI
#define IX_EAA_PORT6_VPI 6
#endif
#ifndef IX_EAA_PORT6_VCI
#define IX_EAA_PORT6_VCI 100
#endif
#ifndef IX_EAA_PORT7_VPI
#define IX_EAA_PORT7_VPI 7
#endif
#ifndef IX_EAA_PORT7_VCI
#define IX_EAA_PORT7_VCI 100
#endif
#ifndef IX_EAA_PORT8_VPI
#define IX_EAA_PORT8_VPI 8
#endif
#ifndef IX_EAA_PORT8_VCI
#define IX_EAA_PORT8_VCI 100
#endif

/**
 * @ingroup IxEthAal5App 
 * 
 * @brief Define default for MAC1 address for ixEAAAddMac() function 
 */ 
#define IX_EAA_MAC1 00
/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for MAC2 address for ixEAAAddMac() function
 */
#define IX_EAA_MAC2 00
/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for MAC3 address for ixEAAAddMac() function
 */
#define IX_EAA_MAC3 00
/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for MAC4 address for ixEAAAddMac() function
 */
#define IX_EAA_MAC4 00

/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for MAC5 address for ixEAAAddMac() function
 */
#define IX_EAA_MAC5 03
/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for MAC6 address for ixEAAAddMac() function
 */
#define IX_EAA_MAC6 01
/**
 * @ingroup IxEthAal5App
 *
 * @brief Define default for Port number for ixEAAAddMac() function
 */
#define IX_EAA_PORT0             0

/**
 * @ingroup IxEthAal5App
 *
 * @brief Define medium thread priority for ixEAAPollTask() function
 */
#define IX_ETHAAL5APP_THREAD_PRI_MEDIUM       160


/**
 * @ingroup IxEthAal5App
 *
 * @fn ixEthAal5AppCodeletMain(IxEAAModeType modeType)
 *
 * @brief This is the main function that executes the EthAal5App codelet
 *
 * It first calls IxEAAMain() function which initialize the MAC database, 
 * to be an in valid Mac addresses (i.e. contain 0xffs), QMGR, NpeMh, Eth phys
 * - 100Mbit, FULL DUPLEX,  NO AUTONEGOTIATION (User can change those settings 
 * accordingly to required configuration), ATM, and Utopia interface.
 *
 * If Linux is used, use interrupt mode - much faster under Linux than polling
 *
 * If vxWorks is used use poll mode - much faster under vxWorks than 
 * interrupts and start background QMgr queues poll
 *
 * After which the main has been called, this ixEthAal5AppCodeletMain() function 
* will add the MAC address using ixEAAAddMAC() function. For a single PHY 
 * utopia mode, only one port will be setup a single MAC address. However, if
 * multiple phy is used, 8 ports will be setup and each port is assigned with
 * a unique MAC addresses.<br>
 * For single utopia phy. The following is setup using ixEAAAddMAC() function
 * @verbatim
        ixEAAAddMAC(IX_EAA_PORT0,
		    IX_EAA_MAC1, 
		    IX_EAA_MAC2, 
		    IX_EAA_MAC3, 
		    IX_EAA_MAC4, 
		    IX_EAA_MAC5, 
		    IX_EAA_MAC6);     
    @endverbatim
 *
 * For multi phy utopia the port number and  MAC6 increments using a for-loop
 * @verbatim
    for (port = 0, nextMac = 0; port < IX_EAA_NUM_ATM_PORTS; port++, nextMac++)
    {
	ixEAAAddMAC(IX_EAA_PORT0 + port,
		    IX_EAA_MAC1,
		    IX_EAA_MAC2,
		    IX_EAA_MAC3,
		    IX_EAA_MAC4,
		    IX_EAA_MAC5,
		    IX_EAA_MAC6 + nextMac);
    }
    @endverbatim
 *
 *
 * Lastly, ixEthAal5AppCodeletMain() creates a thread which purposed to display 
 * the EthAal5App codelet counter every 15secs
 */
PUBLIC IX_STATUS
ixEthAal5AppCodeletMain(IxEAAModeType modeType);

#endif 
/* IXETHAAL5APP_H */

/** @} defgroup IxEthAal5App*/

/** @} defgroup Codelets*/
