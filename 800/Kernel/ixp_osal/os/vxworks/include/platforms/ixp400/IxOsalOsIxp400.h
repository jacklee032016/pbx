/**
 * @file IxOsalOsIxp400.h 
 *
 * @brief vxworks and IXP400 specific defines
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


/* IXP400 -specific, vxWorks defines */
#ifndef IxOsalOsIxp400_H
#define IxOsalOsIxp400_H

#include <ixp425.h>
#include <ixp425IntrCtl.h>

/* Base Address */
#define IX_OSAL_IXP400_INTC_PHYS_BASE           IXP425_INTC_BASE
#define IX_OSAL_IXP400_GPIO_PHYS_BASE           IXP425_GPIO_BASE
#define IX_OSAL_IXP400_UART1_PHYS_BASE          IXP425_UART1_BASE
#define IX_OSAL_IXP400_UART2_PHYS_BASE          IXP425_UART2_BASE
#define IX_OSAL_IXP400_ETHA_PHYS_BASE           IXP425_EthA_BASE  /* MAC on NPE B */
#define IX_OSAL_IXP400_ETHB_PHYS_BASE           IXP425_EthB_BASE  /* MAC on NPE C */
#define IX_OSAL_IXP400_NPEA_PHYS_BASE           IXP425_NPEA_BASE
#define IX_OSAL_IXP400_NPEB_PHYS_BASE           IXP425_NPEB_BASE
#define IX_OSAL_IXP400_NPEC_PHYS_BASE           IXP425_NPEC_BASE
#define IX_OSAL_IXP400_PERIPHERAL_PHYS_BASE     IXP425_PERIPHERAL_BASE
#define IX_OSAL_IXP400_QMGR_PHYS_BASE           IXP425_QMGR_BASE
#define IX_OSAL_IXP400_OSTS_PHYS_BASE           IXP425_OSTS
#define IX_OSAL_IXP400_USB_PHYS_BASE            IXP425_USB_BASE
#define IX_OSAL_IXP400_EXP_BUS_PHYS_BASE        IXP425_EXPANSION_BUS_BASE2
#define IX_OSAL_IXP400_EXP_BUS_BOOT_PHYS_BASE   IXP425_EXPANSION_BUS_BASE1
#define IX_OSAL_IXP400_EXP_BUS_CS0_PHYS_BASE    IXP425_EXPANSION_BUS_CS0_BASE
#define IX_OSAL_IXP400_EXP_BUS_CS1_PHYS_BASE    IXP425_EXPANSION_BUS_CS1_BASE
#define IX_OSAL_IXP400_EXP_BUS_CS4_PHYS_BASE    IXP425_EXPANSION_BUS_CS4_BASE
#define IX_OSAL_IXP400_EXP_BUS_REGS_PHYS_BASE   IXP425_EXP_CONFIG_BASE
#define IX_OSAL_IXP400_PCI_CFG_PHYS_BASE        IXP425_PCI_CONFIG_BASE
#define IX_OSAL_IXP400_SDRAM_CFG_PHYS_BASE      IXP425_SDRAM_CONFIG_BASE
#define IX_OSAL_IXP400_PMU_PHYS_BASE            IXP425_PMU_BASE



/* map sizes to be used when requesting memory with IX_OSAL_MEM_MAP */
#define IX_OSAL_IXP400_QMGR_MAP_SIZE           (0x4000)	    /**< Queue Manager map size */
#define IX_OSAL_IXP400_EXP_REG_MAP_SIZE        (0x1000)	    /**< Exp Bus Registers map size */
#define IX_OSAL_IXP400_UART1_MAP_SIZE          (0x1000)	    /**< UART1 map size */
#define IX_OSAL_IXP400_UART2_MAP_SIZE          (0x1000)	    /**< UART2 map size */
#define IX_OSAL_IXP400_PMU_MAP_SIZE            (0x1000)	    /**< PMU map size */
#define IX_OSAL_IXP400_OSTS_MAP_SIZE           (0x1000)	    /**< OS Timers map size */
#define IX_OSAL_IXP400_NPEA_MAP_SIZE           (0x1000)	    /**< NPE A map size */
#define IX_OSAL_IXP400_NPEB_MAP_SIZE           (0x1000)	    /**< NPE B map size */
#define IX_OSAL_IXP400_NPEC_MAP_SIZE           (0x1000)	    /**< NPE C map size */
#define IX_OSAL_IXP400_ETHA_MAP_SIZE           (0x1000)	    /**< Eth A map size */
#define IX_OSAL_IXP400_ETHB_MAP_SIZE           (0x1000)	    /**< Eth B map size */
#define IX_OSAL_IXP400_ETH_NPEA_MAP_SIZE       (0x1000)	    /**< Eth NPEA map size */
#define IX_OSAL_IXP400_USB_MAP_SIZE            (0x1000)	    /**< USB map size */
#define IX_OSAL_IXP400_GPIO_MAP_SIZE           (0x1000)	    /**< GPIO map size */
#define IX_OSAL_IXP400_EXP_BUS_MAP_SIZE        (0x08000000) /**< Expansion bus map size */
#define IX_OSAL_IXP400_EXP_BUS_CS0_MAP_SIZE    (0x01000000) /**< CS0 map size */
#define IX_OSAL_IXP400_EXP_BUS_CS1_MAP_SIZE    (0x01000000) /**< CS1 map size */
#define IX_OSAL_IXP400_EXP_BUS_CS4_MAP_SIZE    (0x01000000) /**< CS4 map size */


#define IX_OSAL_IXP400_PERIPHERAL_MAP_SIZE      IXP425_PERIPHERAL_SIZE
#define IX_OSAL_IXP400_EXP_CONFIG_MAP_SIZE      IXP425_EXP_CONFIG_SIZE

/*
 * Interrupt Levels
 */
#define IX_OSAL_IXP400_NPEA_IRQ_LVL							IXP425_INT_LVL_NPEA
#define IX_OSAL_IXP400_NPEB_IRQ_LVL							IXP425_INT_LVL_NPEB
#define IX_OSAL_IXP400_NPEC_IRQ_LVL							IXP425_INT_LVL_NPEC
#define IX_OSAL_IXP400_QM1_IRQ_LVL							IXP425_INT_LVL_QM1
#define IX_OSAL_IXP400_QM2_IRQ_LVL							IXP425_INT_LVL_QM2
#define IX_OSAL_IXP400_TIMER1_IRQ_LVL						IXP425_INT_LVL_TIMER1
#define IX_OSAL_IXP400_GPIO0_IRQ_LVL						IXP425_INT_LVL_GPIO0
#define IX_OSAL_IXP400_GPIO1_IRQ_LVL						IXP425_INT_LVL_GPIO1
#define IX_OSAL_IXP400_PCI_INT_IRQ_LVL					IXP425_INT_LVL_PCI_INT
#define IX_OSAL_IXP400_PCI_DMA1_IRQ_LVL					IXP425_INT_LVL_PCI_DMA1
#define IX_OSAL_IXP400_PCI_DMA2_IRQ_LVL					IXP425_INT_LVL_PCI_DMA2
#define IX_OSAL_IXP400_TIMER2_IRQ_LVL						IXP425_INT_LVL_TIMER2	
#define IX_OSAL_IXP400_USB_IRQ_LVL							IXP425_INT_LVL_USB
#define IX_OSAL_IXP400_UART2_IRQ_LVL						IXP425_INT_LVL_UART2
#define IX_OSAL_IXP400_TIMESTAMP_IRQ_LVL				IXP425_INT_LVL_TIMESTAMP
#define IX_OSAL_IXP400_UART1_IRQ_LVL						IXP425_INT_LVL_UART1
#define IX_OSAL_IXP400_WDOG_IRQ_LVL							IXP425_INT_LVL_WDOG
#define IX_OSAL_IXP400_AHB_PMU_IRQ_LVL					IXP425_INT_LVL_AHB_PMU
#define IX_OSAL_IXP400_XSCALE_PMU_IRQ_LVL				IXP425_INT_LVL_XSCALE_PMU
#define IX_OSAL_IXP400_GPIO2_IRQ_LVL						IXP425_INT_LVL_GPIO2
#define IX_OSAL_IXP400_GPIO3_IRQ_LVL						IXP425_INT_LVL_GPIO3
#define IX_OSAL_IXP400_GPIO4_IRQ_LVL						IXP425_INT_LVL_GPIO4
#define IX_OSAL_IXP400_GPIO5_IRQ_LVL						IXP425_INT_LVL_GPIO5
#define IX_OSAL_IXP400_GPIO6_IRQ_LVL						IXP425_INT_LVL_GPIO6
#define IX_OSAL_IXP400_GPIO7_IRQ_LVL						IXP425_INT_LVL_GPIO7
#define IX_OSAL_IXP400_GPIO8_IRQ_LVL						IXP425_INT_LVL_GPIO8
#define IX_OSAL_IXP400_GPIO9_IRQ_LVL						IXP425_INT_LVL_GPIO9
#define IX_OSAL_IXP400_GPIO10_IRQ_LVL						IXP425_INT_LVL_GPIO10
#define IX_OSAL_IXP400_GPIO11_IRQ_LVL						IXP425_INT_LVL_GPIO11
#define IX_OSAL_IXP400_GPIO12_IRQ_LVL						IXP425_INT_LVL_GPIO12
#define IX_OSAL_IXP400_SW_INT1_IRQ_LVL					IXP425_INT_LVL_INT1
#define IX_OSAL_IXP400_SW_INT2_IRQ_LVL					IXP425_INT_LVL_INT2


/* USB interrupt level mask */
#define IX_OSAL_IXP400_INT_LVL_USB							IXP425_INT_LVL_USB

/* USB IRQ */
#define IX_OSAL_IXP400_USB_IRQ		        			IXP425_INT_LVL_USB

#define IX_OSAL_IXP400_QMGR_LE_DC_VIRT					IXP425_QMGR_LE_DC_VIRT
#define IX_OSAL_IXP400_EXP_CONFIG_LE_DC_VIRT		IXP425_EXP_CONFIG_LE_DC_VIRT
#define IX_OSAL_IXP400_PERIPHERAL_LE_DC_VIRT		IXP425_PERIPHERAL_LE_DC_VIRT

#define IX_OSAL_OEM_APB_CLOCK 	   							(66)

/* USB interrupt level mask */
#define IX_OSAL_IXP400_INT_LVL_USB							IXP425_INT_LVL_USB

/* USB IRQ */
#define IX_OSAL_IXP400_USB_IRQ		        			IXP425_INT_LVL_USB

#define IX_OSAL_IXP400_QMGR_LE_DC_VIRT					IXP425_QMGR_LE_DC_VIRT
#define IX_OSAL_IXP400_EXP_CONFIG_LE_DC_VIRT		IXP425_EXP_CONFIG_LE_DC_VIRT
#define IX_OSAL_IXP400_PERIPHERAL_LE_DC_VIRT		IXP425_PERIPHERAL_LE_DC_VIRT


/* 
 * TimestampGet 
 */
PUBLIC UINT32 ixOsalOsIxp400TimestampGet (void);

/*
 * Timestamp
 */
#define IX_OSAL_OEM_TIMESTAMP_GET ixOsalOsIxp400TimestampGet

/*
 * Timestamp resolution
 */
PUBLIC UINT32 ixOsalOsIxp400TimestampResolutionGet (void);

#define IX_OSAL_OEM_TIMESTAMP_RESOLUTION_GET ixOsalOsIxp400TimestampResolutionGet


/*
 * required by FS but is not really platform-specific for vxworks.
 */
#define IX_OSAL_OEM_SYS_CLOCK_RATE_GET ixOsalSysClockRateGet


/*
 * required by FS but is not really platform-specific.
 */
#define IX_OSAL_OEM_TIME_GET(pTv) ixOsalTimeGet(pTv)

/*
 * OS name retrieval
 */
#define IX_OSAL_OEM_OS_NAME_GET(name, limit) \
ixOsalOsIxp400NameGet((INT8*)(name), (INT32) (limit))

/*
 * OS version retrieval
 */
#define IX_OSAL_OEM_OS_VERSION_GET(version, limit) \
ixOsalOsIxp400VersionGet((INT8*)(version), (INT32) (limit))

/*
 * Returns OS name string
 */
PUBLIC IX_STATUS
ixOsalOsIxp400NameGet(INT8* osName, INT32 maxSize);

/*
 * Returns OS version string
 */
PUBLIC IX_STATUS
ixOsalOsIxp400VersionGet(INT8* osVersion, INT32 maxSize);

/* 
NOTE: Include the apppropriate (IXP400 specific) Platform system Header file here 
			Platform - ixp425, ixp465 and etc
*/
#ifndef __ixp46X
#include  "ixp425/IxOsalOsIxp425Sys.h"
#else
#include "IxOsalOsIxp465Sys.h"
#endif

#endif /*IxOsalOsIxp400_H */
