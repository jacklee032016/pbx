/**
 * @file IxParityENAccPmuE_p.h
 *
 * @author Intel Corporation
 * @date 26 July 2004
 *
 * @brief Private header file for Performance Monitoring Unit Enabler
 * sub-component of the IXP400 Parity Error Notifier access component.
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

#ifndef IXPARITYENACCPMUE_P_H
#define IXPARITYENACCPMUE_P_H

#include "IxOsal.h"

/*
 * #defines and macros used in this file.
 */

/*
 * Base Addresses for PMU Registers
 * 
 * The offset of 0x18 is for the PMSR register needed for PMU 
 * sub-component access 
 */
#define IXP400_PARITYENACC_PMU_BASEADDR     (IX_OSAL_IXP400_PMU_PHYS_BASE + 0x18)

/* Address Range for PMU register */
#define IXP400_PARITYENACC_PMU_MEMMAP_SIZE  (0x04)

/* Mask values to access PMU Previous Master/Slave Register */
#define IXP400_PARITYENACC_PMU_PMSR_SOUTH_AHB_ERR     (1 << 31)
#define IXP400_PARITYENACC_PMU_PMSR_NORTH_AHB_ERR     (1 << 30)
#define IXP400_PARITYENACC_PMU_PMSR_PREV_SLAVE_SOUTH  (0x0000F000)
#define IXP400_PARITYENACC_PMU_PMSR_PREV_SLAVE_NORTH  (0x00000F00)
#define IXP400_PARITYENACC_PMU_PMSR_PREV_MASTER_SOUTH (0x000000F0)
#define IXP400_PARITYENACC_PMU_PMSR_PREV_MASTER_NORTH (0x0000000F)

/*
 * Values as interpreted from PMU Previous Master/Slave Register
 *
 * NOTE: Y and N indicates whether that particular PMU device can
 * be a Master/Slave for a given AHB transactions respectively.
 */

/*
 * Master/Slave for a South AHB Bus Transaction
 *
 * PMU Device # Name                Master  Slave
 * ------------ ---------------     ------  -----
 * 0            X-Scale BIU         Y       N
 * 1            PCI                 Y       Y
 * 2            AHB Bridge          Y       N
 * 3            Expansion Bus       Y       Y
 * 4            USBH                Y       Y
 * 5            MCU                 N       Y
 * 6            APB Bridge          N       Y
 * 7            AQM                 N       Y
 * 8            RSA                 N       Y
 */
#define IXP400_PARITYENACC_PMU_PMSS_XSCALE_BIU        (0)
#define IXP400_PARITYENACC_PMU_PMSS_PBC               (1)
#define IXP400_PARITYENACC_PMU_PMSS_AHB_BRIDGE        (2)
#define IXP400_PARITYENACC_PMU_PMSS_EBC               (3)
#define IXP400_PARITYENACC_PMU_PMSS_USBH              (4)
#define IXP400_PARITYENACC_PMU_PMSS_MCU               (5)
#define IXP400_PARITYENACC_PMU_PMSS_APB_BRIDGE        (6)
#define IXP400_PARITYENACC_PMU_PMSS_AQM               (7)
#define IXP400_PARITYENACC_PMU_PMSS_RSA               (8)

/*
 * Master/Slave for a North AHB Bus Transaction
 *
 * PMU Device # Name                Master  Slave
 * ------------ ---------------     ------  -----
 * 0            NPE-A               Y       N
 * 1            NPE-B               Y       N
 * 2            NPE-C               Y       N
 * 3            MCU                 N       Y
 * 4            AHB Bridge          N       Y
 */

#define IXP400_PARITYENACC_PMU_PMSN_NPE_A             (0)
#define IXP400_PARITYENACC_PMU_PMSN_NPE_B             (1)
#define IXP400_PARITYENACC_PMU_PMSN_NPE_C             (2)
#define IXP400_PARITYENACC_PMU_PMSN_MCU               (3)
#define IXP400_PARITYENACC_PMU_PMSN_AHB_BRIDGE        (4)


/* Macros to adjust the Master/Slave values read from PMSR register */
#define IXP400_PARITYENACC_VAL_READ_PMU_SLAVE_SOUTH(value)      \
/* Shift the value into LSBs */                                 \
(((value) & IXP400_PARITYENACC_PMU_PMSR_PREV_SLAVE_SOUTH) >> 12)

#define IXP400_PARITYENACC_VAL_READ_PMU_SLAVE_NORTH(value)      \
/* Shift the value into LSB */                                  \
(((value) & IXP400_PARITYENACC_PMU_PMSR_PREV_SLAVE_NORTH) >>  8)

#define IXP400_PARITYENACC_VAL_READ_PMU_MASTER_SOUTH(value)     \
/* Shift the value into first nibble of LSB */                  \
(((value) & IXP400_PARITYENACC_PMU_PMSR_PREV_MASTER_SOUTH) >> 4)

#define IXP400_PARITYENACC_VAL_READ_PMU_MASTER_NORTH(value)     \
/* The value is already in first nibble LSB */                  \
((value) & IXP400_PARITYENACC_PMU_PMSR_PREV_MASTER_NORTH)


/*
 * Variable declarations
 */

/* Previous Master and Slave Register virtual address */
static UINT32 ixParityENAccPmuEPmsr = 0;

/* Previous Master and Slave Register contents */
static UINT32 ixParityENAccPmuEPmsrStatus = 0;

/*
 * These static tables are used to map onto the Master/Slaves as to be
 * observed by the public API with the help of the above enums  and by
 * using the subscript/index.
 *
 * Example: Consider PCI being the master of the AHB transaction, then
 * the enum value to be returned is selected as following:
 *
 * enumVariable = ixParityENAccPmuESouthAhbMaster[<PCI device Id from PMSR>]
 * enumVariable = ixParityENAccPmuESouthAhbMaster[1]
 *                (i.e., IXP400_PARITYENACC_PMUE_AHBS_MST_PBC)
 */

/* Master on North AHB Bus */
static UINT32 ixParityENAccPmuENorthAhbMaster[] =
{
    IXP400_PARITYENACC_PMUE_AHBN_MST_NPE_A,       /* 0 */
    IXP400_PARITYENACC_PMUE_AHBN_MST_NPE_B,       /* 1 */
    IXP400_PARITYENACC_PMUE_AHBN_MST_NPE_C        /* 2 */
};

/* Master on South AHB Bus */
static UINT32 ixParityENAccPmuESouthAhbMaster[] =
{
    IXP400_PARITYENACC_PMUE_AHBS_MST_XSCALE,      /* 0 */
    IXP400_PARITYENACC_PMUE_AHBS_MST_PBC,         /* 1 */
    IXP400_PARITYENACC_PMUE_AHBS_MST_AHB_BRIDGE,  /* 2 */
    IXP400_PARITYENACC_PMUE_AHBS_MST_EBC,         /* 3 */
    IXP400_PARITYENACC_PMUE_AHBS_MST_USBH         /* 4 */
};

/* Slave on North AHB Bus */
static UINT32 ixParityENAccPmuENorthAhbSlave[] =
{
    IXP400_PARITYENACC_PMUE_AHBN_SLV_MCU,         /* 0 */
    IXP400_PARITYENACC_PMUE_AHBN_SLV_AHB_BRIDGE   /* 1 */
};

/* Slave on South AHB Bus */
static UINT32 ixParityENAccPmuESouthAhbSlave[] =
{
    IXP400_PARITYENACC_PMUE_AHBS_SLV_INVALID,     /* 0 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_PBC,         /* 1 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_INVALID,     /* 2 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_EBC,         /* 3 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_USBH,        /* 4 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_MCU,         /* 5 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_APB_BRIDGE,  /* 6 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_AQM,         /* 7 */
    IXP400_PARITYENACC_PMUE_AHBS_SLV_RSA          /* 8 */
};

#endif /* IXPARITYENACCPMUE_P_H */
