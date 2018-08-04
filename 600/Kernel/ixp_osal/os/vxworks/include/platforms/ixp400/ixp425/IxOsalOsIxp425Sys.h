/**
 * @file IxOsalOsIxp425Sys.h
 *
 * @brief vxworks and IXP425 specific defines
 *
 * Design Notes:
 *
 * @par
 * IXP400 SW Release version  2.0
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

#ifndef IxOsalOsIxp425Sys_H
#define IxOsalOsIxp425Sys_H

#ifndef IxOsalOsIxp400_H
#error "Error: IxOsalOsIxp425Sys.h cannot be included directly before IxOsalOsIxp400.h"
#endif

#define IX_OSAL_IXP400_TIME_STAMP_RESOLUTION    (66666666) /* 66.66MHz */


/* Global memmap only visible to IO MEM module */

#ifdef IxOsalIoMem_C

static IxOsalMemoryMap ixOsalGlobalMemoryMap[] = {
    /*
     * Global BE and LE_AC map 
     */
    {
     IX_OSAL_STATIC_MAP,	/* type            */
     0x00000000,		/* physicalAddress */
     0xFFFFFFFF,		/* size            */
     0x00000000,		/* virtualAddress  */
     NULL,			/* mapFunction     */
     NULL,			/* unmapFunction   */
     0,				/* refCount        */
     IX_OSAL_BE | IX_OSAL_LE_AC,	/* coherency       */
     "global"			/* name            */
     },

    /*
     * QMgr LE_DC map 
     */
    {
     IX_OSAL_STATIC_MAP,	/* type            */
     IX_OSAL_IXP400_QMGR_PHYS_BASE,	/* physicalAddress */
     IX_OSAL_IXP400_QMGR_MAP_SIZE,	/* size            */
     IX_OSAL_IXP400_QMGR_LE_DC_VIRT,	/* virtualAddress  */
     NULL,			/* mapFunction     */
     NULL,			/* unmapFunction   */
     0,				/* refCount        */
     IX_OSAL_LE_DC,		/* coherency       */
     "qMgr LE_DC"		/* name            */
     },

    /*
     * Expansion Bus Config LE_DC map 
     */
    {
     IX_OSAL_STATIC_MAP,	/* type            */
     IX_OSAL_IXP400_EXP_BUS_REGS_PHYS_BASE,	/* physicalAddress */
     IX_OSAL_IXP400_EXP_CONFIG_MAP_SIZE,	/* size            */
     IX_OSAL_IXP400_EXP_CONFIG_LE_DC_VIRT,	/* virtualAddress  */
     NULL,			/* mapFunction     */
     NULL,			/* unmapFunction   */
     0,				/* refCount        */
     IX_OSAL_LE_DC,		/* coherency       */
     "Exp Cfg LE_DC"		/* name            */
     },

    /*
     * Peripherals LE_DC map 
     */
    {
     IX_OSAL_STATIC_MAP,	/* type            */
     IX_OSAL_IXP400_PERIPHERAL_PHYS_BASE,	/* physicalAddress */
     IX_OSAL_IXP400_PERIPHERAL_MAP_SIZE,	/* size            */
     IX_OSAL_IXP400_PERIPHERAL_LE_DC_VIRT,	/* virtualAddress  */
     NULL,			/* mapFunction     */
     NULL,			/* unmapFunction   */
     0,				/* refCount        */
     IX_OSAL_LE_DC,		/* coherency       */
     "peripherals LE_DC"	/* name            */
     }
};
#endif /* IxOsalIoMem_C visible */
#endif


