/**
 * @file IxOsalOsIxp465Sys.h
 *
 * @brief vxworks and IXP465 specific defines
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

#ifndef IxOsalOsIxp465Sys_H
#define IxOsalOsIxp465Sys_H

#ifndef IxOsalOsIxp400_H
#error "Error: IxOsalOsIxp465Sys.h cannot be included directly before IxOsalOsIxp400.h!"
#endif

/* Base Address */
#define IX_OSAL_IXP400_EXP_FUSE                (IXP425_EXP_CONFIG_BASE + 28)
#define IX_OSAL_IXP400_ETH_NPEA_PHYS_BASE      0x0C800C000  
#define IX_OSAL_IXP400_TIMESYNC_PHYS_BASE      IXP400_TIMESYNC_BASE
#define IX_OSAL_IXP400_PARITYEN_PHYS_BASE      (0xCC00E51C)
#define IX_OSAL_IXP400_I2C_PHYS_BASE           IXP400_I2C_BASE
#define IX_OSAL_IXP400_SSP_PHYS_BASE           IXP400_SSP_BASE
#define IX_OSAL_IXP400_SDRAM_BASE              IXP400_SDRAM_BASE


/* Map sizes */
#define IX_OSAL_IXP400_I2C_MAP_SIZE            (0x10)       /**< I2C map size */
#define IX_OSAL_IXP400_SSP_MAP_SIZE            (0x14)       /**< SSP map size */
#define IX_OSAL_IXP400_TIMESYNC_MAP_SIZE       (0xA0)       /**< Time Sync map size */
#define IX_OSAL_IXP400_PARITYEN_MAP_SIZE       (0x100000)   /**< Parity map size */

/* IRQ Interrupt levels */
#define IX_OSAL_IXP400_USB_HOST_IRQ_LVL         (32)
#define IX_OSAL_IXP400_I2C_IRQ_LVL              (33)
#define IX_OSAL_IXP400_SSP_IRQ_LVL              (34)
#define IX_OSAL_IXP400_TSYNC_IRQ_LVL            (35)
#define IX_OSAL_IXP400_EAU_DONE_IRQ_LVL         (36)
#define IX_OSAL_IXP400_SHA_HASHING_DONE_IRQ_LVL (37)
#define IX_OSAL_IXP400_RSVD_38_IRQ_LVL          (38)
#define IX_OSAL_IXP400_RSVD_39_IRQ_LVL          (39)
#define IX_OSAL_IXP400_RSVD_40_IRQ_LVL          (40)
#define IX_OSAL_IXP400_RSVD_41_IRQ_LVL          (41)
#define IX_OSAL_IXP400_RSVD_42_IRQ_LVL          (42)
#define IX_OSAL_IXP400_RSVD_43_IRQ_LVL          (43)
#define IX_OSAL_IXP400_RSVD_44_IRQ_LVL          (44)
#define IX_OSAL_IXP400_RSVD_45_IRQ_LVL          (45)
#define IX_OSAL_IXP400_RSVD_46_IRQ_LVL          (46)
#define IX_OSAL_IXP400_RSVD_47_IRQ_LVL          (47)
#define IX_OSAL_IXP400_RSVD_48_IRQ_LVL          (48)
#define IX_OSAL_IXP400_RSVD_49_IRQ_LVL          (49)
#define IX_OSAL_IXP400_RSVD_50_IRQ_LVL          (50)
#define IX_OSAL_IXP400_RSVD_51_IRQ_LVL          (51)
#define IX_OSAL_IXP400_RSVD_52_IRQ_LVL          (52)
#define IX_OSAL_IXP400_RSVD_53_IRQ_LVL          (53)
#define IX_OSAL_IXP400_RSVD_54_IRQ_LVL          (54)
#define IX_OSAL_IXP400_RSVD_55_IRQ_LVL          (55)
#define IX_OSAL_IXP400_RSVD_56_IRQ_LVL          (56)
#define IX_OSAL_IXP400_RSVD_57_IRQ_LVL          (57)
#define IX_OSAL_IXP400_SWCP_IRQ_LVL             (58)
#define IX_OSAL_IXP400_RSVD_59_IRQ_LVL          (59)
#define IX_OSAL_IXP400_AQM_IRQ_LVL              (60)
#define IX_OSAL_IXP400_MCU_IRQ_LVL              (61)
#define IX_OSAL_IXP400_EBC_IRQ_LVL              (62)
#define IX_OSAL_IXP400_RSVD_63_IRQ_LVL          (63)


/* Platform specific Macros */
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


