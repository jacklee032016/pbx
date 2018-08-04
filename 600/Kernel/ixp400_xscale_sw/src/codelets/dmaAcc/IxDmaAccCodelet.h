 /**
 * @file IxDmaAccCodelet.h
 *
 * @date 18 November 2002
 *
 * @brief This file contains the interface for the Dma Access Codelet.
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

#ifndef IXDMAACCCODELET_H
#define IXDMAACCCODELET_H

#include "IxOsal.h"
#include "IxDmaAcc.h"

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
 * @defgroup IxDmaAccCodelet IXP400 DMA Access Codelet (IxDmaAccCodelet) API
 *
 * @brief IXP400 DMA Access component API
 *
 * This file contains a main interface of the Dma Access Codelet that 
 * initialises the DmaAcc codelet and execute Dma transfer using 
 * ixDmaAccCodeletTestPerform() function for various DMA transfer mode, 
 * addressing mode and transfer width. The block size used in this codelet
 * are 8,1024,16384,32768,65528 bytes. For each Dma configuration, the 
 * performance will be measured and the average rate (in Mbps) will be
 * displayed
 *
 * <b> VxWorks User Guide </b><br>
 *  <pre>
 *  <i> Usage :
 *      -> ixDmaAccCodeletMain()
 *
 * Note:
 * 1. Once the function is executed, the codelet will display the results <br>
 * 2. The formulae to calculate the rate is: <br>
 *    Rate (in Mbps)  = ( (length * 8) / (ticks / 66) ) 
 *
 * </i>
 * </pre>     
 *
 * <b> Linux User Guide </b><br>
 * <pre>
 * <i>  Usage :
 *      # insmod ixp400_codelets_dmaAcc.o
 *
 * Note: <br>
 * 1. Once the function is executed, the codelet will display the results <br>
 * 2. The formulae to calculate the rate is: <br>
 *    Rate (in Mbps)  = ( (length * 8) / (ticks / 66) ) 
 *
 * </i>
 * </pre>   
 *
 * <b> DmaAcc Codelet Features </b>
 *
 * The API ixDmaAccCodeletTestPerform() allows the user to perform a
 * Dma transfer of block size 0 to 65535 bytes between two locations
 * in the SRAM. The user can specify any combination of the following modes.   
 *
 *            DMA Transfer Modes 
 *	         1. Copy
 *	      	 2. Copy and Clear Source
 *	      	 3. Copy with Bytes Swap
 *	      	 4. Copy with Bytes Reversed
 *
 *	      DMA Addressing Modes
 *	      	 1. Incremental Source to Incremental Destination Addressess
 *	         2. Fixed Source to Incremental Destination Addressess
 *	      	 3. Incremental Source to Fixed Destination Addressess
 *
 *	      DMA Transfer Widths
 *	       	 1. 32-bit Transfer
 *	      	 2. 16-bit Transfer
 *	      	 3. 8-bit Transfer
 *	      	 4. Burst Transfer
 *
 *  NOTE : The user must initialise the system with ixDmaAccCodeletInit prior to 
 *         calling the function ixDmaAccCodeletiTestPerform()
 *
 * Performance will execute PERFORMANCE_NUM_LOOP (i.e. 100 runs) in order to 
 * calculate the average rate for each Dma transfer configuration
 *
 * @{
 */

/*
 * Defines
 */

/**
 * @ingroup IxDmaAccCodelet
 *
 * @def IX_DMA_CODELET_TRANSFER_LENGTH
 *
 * @brief The length of the transfer size if 128 bytes.
 * 
 * It can be changed for Dma transfer. The range is between 1-65535 bytes
 */
#define IX_DMA_CODELET_TRANSFER_LENGTH 128

/*
 * Prototypes for interface functions.
 */

/**
 * @ingroup IxDmaAccCodelet
 *
 * @fn void ixDmaAccCodeletMain(void)
 *
 * @brief This function is the entry point to the Dma Access codelet.
 * It will initialise the Dma codelet which in turn initialises the necessary
 * components.
 *
 * Once it has successfully initialise the Dma Codelet, this function will
 * continue to perform valid DMA transfer using IxDmaAccCodeletTestPerform()
 *
 * @param none
 *
 * @return none
 */
IX_STATUS ixDmaAccCodeletMain(void);

#endif /* IXDMAACCCODELET_H */

/** @} defgroup IxDmaAccCodelet*/

/** @} defgroup Codelet*/
