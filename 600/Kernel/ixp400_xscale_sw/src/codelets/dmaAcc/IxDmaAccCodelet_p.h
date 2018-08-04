 /**
 * @file IxDmaAccCodelet_p.h
 *
 * @author Intel Corporation
 * @date 18 November 2002
 *
 * @brief This file contains some private data structures and
 * defines for the Dma Access Codelet.
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

#ifndef IXDMAACCCODELET_P_H
#define IXDMAACCCODELET_P_H

/** Test pattern content and array size */
#define IX_DMA_CODELET_TESTPATTERN_LIST { 0x12,0x34,0x56,0x78,0x9A,0xBC,0xDE,0xF0 } 
#define IX_DMA_CODELET_TESTPATTERN_LENGTH (8) 

/** Number of milliiseconds to poll the dispatcher */
#define IX_DMA_CODELET_MS_SLEEP (5)

/** Number of words for test transfer */
#define IX_DMA_CODELET_TEST_MAXLENGTH (65535)

/** Number of bytes to show when doing memory dump */
#define IX_DMA_CODELET_MEMDUMPSIZE (32)

/** Priority of queue manager dispatch loop */
#define IX_DMA_CODELET_QMR_PRIORITY (150)

/** Stack size of queue manager dispatch */
#define IX_DMA_CODELET_QMGR_STACK_SIZE (10240)

/** Version ID for NPE A Dma Code */
#define IX_DMA_CODELET_NPE_A_IMAGEID IX_NPEDL_NPEIMAGE_NPEA_DMA

/** Version ID for NPE B Dma Code */
#define IX_DMA_CODELET_NPE_B_IMAGEID IX_NPEDL_NPEIMAGE_NPEB_DMA

/** Version ID for NPE C Dma Code */
#define IX_DMA_CODELET_NPE_C_IMAGEID IX_NPEDL_NPEIMAGE_NPEC_DMA

/* Loop 100 runs for performance purpose */
#define PERFORMANCE_LOOP_NUM 100

/* Struct for start and stop time */
typedef struct {
  UINT32 startTime [PERFORMANCE_LOOP_NUM];
  UINT32 stopTime [PERFORMANCE_LOOP_NUM];
} IxDmaAccCodeletTimeStore;

/**
 * @fn IX_STATUS ixDmaAccCodeletInit(IxDmaNpeId npeId)
 *
 * This function is the entry point to the Dma Access codelet.
 * It must be called before any of the demos as it initialises the necessary
 * components.
 *
 * @param npeID             Id for the Npe to setup for Dma function
 *                   @li    IX_DMA_NPEID_A for Npe A
 *                   @li    IX_DMA_NPEID_B for Npe B
 *                   @li    IX_DMA_NPEID_C for Npe C
 *
 * The initialisation sequence is as follows:
 *
 *	 Initialise Queue manager
 *	 Start Queue manager dispatcher loop
 *	 Download NPE microcode
 *	 Start NPEs
 *	 Initialise Dma Access component
 *
 * @return IX_SUCCESS - Codelet successfully initialised
 * @return IX_FAIL - Error initialising codelet
 */
IX_STATUS
ixDmaAccCodeletInit(IxDmaNpeId npeId);

/**
 * @fn ixDmaAccCodeletTestPerform( UINT16 transferLength, 
                                   IxDmaTransferMode transferMode,
				   IxDmaAddressingMode addressingMode, 
				   IxDmaTransferWidth transferWidth)
 *
 * This function performs a dma transfer with parameters provided by the client 
 * This function can only function after ixDmaAccCodeletInit is performed.
 *
 * @param transferLength 1 to 65535 bytes         
 * 
 * @param transferMode 
 *                  @li 0 = Copy 
 *                  @li 1 = Copy and Clear Source  
 *                  @li 2 = Copy and Byte Swap  
 *                  @li 3 = Copy and Reverse
 * 
 * @param addressMode 
 *                  @li 0 = Increment Source Addr and Increment Destination Addr");
 *                  @li 1 = Increment Source Addr and Fix Destination Addr");
 *                  @li 2 = Fix Source Addr and Increment Destination Addr");
 * 
 * @param transferWidth  : 
 *                  @li  0 = 32 bit src, 32 bit dest   
 *                  @li  1 = 32 bit src, 16 bit dest   
 *                  @li  2 = 32 bit src,  8 bit dest   
 *                  @li  3 = 16 bit src, 32 bit dest   
 *                  @li  4 = 16 bit src, 16 bit dest   
 *                  @li  5 = 16 bit src,  8 bit dest   
 *                  @li  6 =  8 bit src, 32 bit dest   
 *                  @li  7 =  8 bit src, 16 bit dest   
 *                  @li  8 =  8 bit src,  8 bit dest
 *                  @li  9 =  8 bit src,  Burst dest
 *                  @li 10 = 16 bit src,  Burst dest
 *                  @li 11 = 32 bit src,  Burst dest
 *                  @li 12 =  Burst src,  8 bit dest
 *                  @li 13 =  Burst src, 16 bit dest
 *                  @li 14 =  Burst src, 32 bit dest
 *                  @li 15 =  Burst src,  Burst dest
 *
 * @return none 
 */
IX_STATUS ixDmaAccCodeletTestPerform( UINT16 transferLength,
				      IxDmaTransferMode transferMode,
				      IxDmaAddressingMode addressingMode,
				      IxDmaTransferWidth transferWidth);

/**
 * @fn ixDmaAccCodeletShow()
 *
 * This function performs a memory dump at the source and destination   
 * addresses. This function is called before and after a Dma transfer
 * to enable the user to check that a transfer has taken place and
 * allows the user to examine the data at the destination after it has 
 * been transfered.   
 *
 * This function is used by ixDmaAccCodeletTestPerform()  
 *
 * @param none 
 *
 * @return none 
 */
/**
 * @fn ixDmaAccCodeletTestPatternReset()
 *
 * This function formats source and destination address blocks
 * with test patterns 
 *
 * This function is used by ixDmaAccCodeletTestPerform()  
 *
 * @param none 
 *
 * @return none 
 */

/**
 * @fn ixDmaAccCodeletDispatcherPoll()
 *
 * The function runs the dispatcher for a group of queues. 
 * Callbacks are made for interrupts that have occurred on 
 * queues within the group that have registered callbacks. 
 * This function may be called from interrupt or task context. 
 *
 * This function is used by ixDmaAccCodeletInit()  
 *
 * @param  void* argUnused Not used
 * @param  void** ptrRetObjUnused Not used
 *
 * @return none 
 */
/**
 * @fn ixDmaAccCodeletDispatcherStart()
 *
 * This function starts the queue dispatcher 
 * This function is used by ixDmaAccCodeletInit()  
 *
 * @param useInterrupt TRUE sets the dispatcher to be interrupt driven
 * @param useInterrupt FALSE sets the dispatcher to poll mode 
 *
 * @return none 
 */

/**
 * @fn ixDmaAccCodeletNpeInit()
 *
 * This function initialises the Npe 
 * This function is used by ixDmaAccCodeletInit()  
 *
 * @param npeId IX_NPEDL_NPEID_NPEA for Npe A 
 * @param npeId IX_NPEDL_NPEID_NPEB for Npe B 
 * @param npeId IX_NPEDL_NPEID_NPEC for Npe C 
 *
 * @return IX_SUCCESS Npe succesfully initialised 
 * @return IX_FAIL Npe failed to initialise 
 */

/**
 * @fn ixDmaAccCodeletCallback()
 *
 * This function is called by Dma Acc component when 
 * Dma transfer is done  
 *
 * @param IX_STATUS IX_SUCCESS Dma transfer succesful 
 * @param IX_STATUS IX_FAIL Dma transfer failed 
 *
 * @return none 
 */


#endif /* IXDMAACCCODELET_P_H */

