/**
 * @file IxParityENAccNpePE_p.h
 *
 * @author Intel Corporation
 * @date 26 July 2004
 *
 * @brief Private header file for NPE Parity Detection Enabler sub-
 * component of the IXP400 Parity Error Notifier access component.
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

#ifndef IXPARITYENACCNPEPE_P_H
#define IXPARITYENACCNPEPE_P_H

#include "IxOsal.h"

/*
 * #defines and macros used in this file.
 */

/* IRQ Levels for the NPE-A/B/C */
#define IRQ_IXP400_INTC_PARITYENACC_NPEA  IX_OSAL_IXP400_NPEA_IRQ_LVL
#define IRQ_IXP400_INTC_PARITYENACC_NPEB  IX_OSAL_IXP400_NPEB_IRQ_LVL 
#define IRQ_IXP400_INTC_PARITYENACC_NPEC  IX_OSAL_IXP400_NPEC_IRQ_LVL

/*
 * Base Addresses for NPE Registers
 *
 * The offset 0x2C is for the Status and Control Registers from the
 * base address of each of the NPE register's memory space
 */

#define IXP400_PARITYENACC_NPEA_BASEADDR  (IX_OSAL_IXP400_NPEA_PHYS_BASE + 0x2C)
#define IXP400_PARITYENACC_NPEB_BASEADDR  (IX_OSAL_IXP400_NPEB_PHYS_BASE + 0x2C)
#define IXP400_PARITYENACC_NPEC_BASEADDR  (IX_OSAL_IXP400_NPEC_PHYS_BASE + 0x2C)

/*
 * Address Ranges for each NPE registers
 * (Memory size = Register size x Number of registers)
 */
#define IXP400_PARITYENACC_NPE_MEMMAP_SIZE     (0x04 * 0x02)

/* NPE registers Relative Offset values */
#define IXP400_PARITYENACC_NPE_STATUS_OFFSET   (0x00)
#define IXP400_PARITYENACC_NPE_CONTROL_OFFSET  (0x04)

/*
 * Base Addresses for Expansion Bus Controller Registers
 *
 * The offset 0x24 is for the Expansion Bus Config Register #1 from the
 * base address
 */
#define IXP400_PARITYENACC_EBC_BASEADDR  (IX_OSAL_IXP400_EXP_BUS_REGS_PHYS_BASE + 0x24)

/* 
 * Address Range EBC registers
 * (Memory size = Register size x Number of registers)
 */
#define IXP400_PARITYENACC_EBC_MEMMAP_SIZE     (0x04)

/*
 * Mask values to access NPE registers
 */

/* Masks for bits in NPE Status Register */
#define IXP400_PARITYENACC_NPE_STATUS_IMEM_PARITY   (1 << 31)
#define IXP400_PARITYENACC_NPE_STATUS_DMEM_PARITY   (1 << 30)
#define IXP400_PARITYENACC_NPE_STATUS_EXT_ERROR     (1 << 29)
#define IXP400_PARITYENACC_NPE_STATUS_STDATA        (0x00FF)

/* Masks for bits in NPE Control Register */

/* Parity Polarity Write Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_PPWE         (1 << 29)
/* IMEM Parity Enable Write Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_IPEWE        (1 << 28)
/* DMEM Parity Enable Write Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_DPEWE        (1 << 27)
/* External Error Enable Write Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_EEEWE        (1 << 26)
/* Parity Polarity */
#define IXP400_PARITYENACC_NPE_CONTROL_PP           (1 << 21)
/* IMEM Parity Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_IPE          (1 << 20)
/* DMEM Parity Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_DPE          (1 << 19)
/* External Error Enable */
#define IXP400_PARITYENACC_NPE_CONTROL_EEE          (1 << 18)

/* Masks for bits in Expansion Bus Config Register #1 */
#define IXP400_PARITYENACC_NPE_EXPCNFG1_NPEA_ERREN  (1 << 12)
#define IXP400_PARITYENACC_NPE_EXPCNFG1_NPEB_ERREN  (1 << 13)
#define IXP400_PARITYENACC_NPE_EXPCNFG1_NPEC_ERREN  (1 << 14)

/* Values as interpreted from Expansion Bus Config Register #1 */
#define IXP400_PARITYENACC_NPE_ERREN_DISABLE        (0)
#define IXP400_PARITYENACC_NPE_ERREN_ENABLE         (1)

/* Helper macro for NPE Configuration */
#define IXP400_PARITYENACC_NPE_PE_CONFIG_INIT              \
    {                                                      \
        { 0,0 }, /* NPE Registers */                       \
        { 0,0 }, /* NPE Register Contents */               \
        (IxParityENAccInternalCallback) NULL,              \
        { 0, (IxParityENAccNpePEIsr) NULL } /* ISR Info */ \
    }

/*
 * Typedefs used in this file
 */

/* NPE Control and Status Registers virutal addresses */
typedef struct  /* IxParityENAccNpePERegisters */
{
    UINT32 npeStatusRegister;  /* Status register virtual address */
    UINT32 npeControlRegister; /* Control register virtual address */
} IxParityENAccNpePERegisters;

/* NPE ISR type */  
typedef IxParityENAccPEIsr IxParityENAccNpePEIsr;

/* NPE Interrupt Service Routine Info */
typedef struct  /* IxParityENAccNpePEIsrInfo */
{
    UINT32  npeInterruptId;        /* NPE Interrupt Identifier */
    IxParityENAccNpePEIsr npeIsr;  /* ISR for handling interrupts */
} IxParityENAccNpePEIsrInfo;

/* NPE Parity Error Status */
typedef struct  /* IxParityENAccNpePEParityErrorStatus */
{
    UINT32 npeStatusRegisterValue;   /* Status register contents */
    UINT32 npeControlRegisterValue;  /* Control register contents */
} IxParityENAccNpePEParityErrorStatus;

/* NPE Configuration Information */
typedef struct  /* IxParityENAccNpePEConfig */
{
    /* NPE Control and Status Registers virutal addresses */
    IxParityENAccNpePERegisters          npePERegisters;
    /* Contents of Status and Control Registers */
    IxParityENAccNpePEParityErrorStatus  npeParityErrorStatus;
    /* Internal Callback Routine */
    IxParityENAccInternalCallback        npePECallback;
    /*Interrupt Service Routine Info. */
    IxParityENAccNpePEIsrInfo            npeIsrInfo;
} IxParityENAccNpePEConfig;


/*
 * Variable declarations
 */
static IxParityENAccNpePEConfig 
       ixParityENAccNpePEConfig[IXP400_PARITYENACC_PE_NPE_MAX] =
{
    /*  NPE-A config Info */
    IXP400_PARITYENACC_NPE_PE_CONFIG_INIT,
    /*  NPE-B config Info */
    IXP400_PARITYENACC_NPE_PE_CONFIG_INIT,
    /*  NPE-C config Info */
    IXP400_PARITYENACC_NPE_PE_CONFIG_INIT
};
static UINT8 
        ixParityENAccNpePEErrorHandlingEnable[IXP400_PARITYENACC_PE_NPE_MAX] = 
{
   IXP400_PARITYENACC_NPE_ERREN_DISABLE,
   IXP400_PARITYENACC_NPE_ERREN_DISABLE,
   IXP400_PARITYENACC_NPE_ERREN_DISABLE 
};
 
/*
 * NPE sub-module local functions declarations
 */

/* NPE-A Interrupt Service Routine */
void
ixParityENAccNpePENpeAIsr(void);

/* NPE-B Interrupt Service Routine */
void
ixParityENAccNpePENpeBIsr(void);

/* NPE-C Interrupt Service Routine */
void
ixParityENAccNpePENpeCIsr(void);

/* Get parity error status into internal datastructures */
void
ixParityENAccNpePEParityErrorStatusGet (void);

#endif /* IXPARITYENACCNPEPE_P_H */
