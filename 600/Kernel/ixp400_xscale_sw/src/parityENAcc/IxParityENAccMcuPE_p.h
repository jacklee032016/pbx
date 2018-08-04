/**
 * @file IxParityENAccMcuPE_p.h
 *
 * @author Intel Corporation
 * @date 26 July 2004
 *
 * @brief Private header file for MCU Parity Detection Enabler sub-
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

#ifndef IXPARITYENACCMCUPE_P_H
#define IXPARITYENACCMCUPE_P_H

#include "IxOsal.h"

/*
 * #defines and macros used in this file.
 */

/* IRQ Level of MCU */
#define IRQ_IXP400_INTC_PARITYENACC_MCU  (IX_OSAL_IXP400_MCU_IRQ_LVL)

/* 
 * Base Addresses for MCU Registers 
 * The ECCR register begins at offset 0xE51C
 */
#define IXP400_PARITYENACC_MCU_BASEADDR  (IX_OSAL_IXP400_SDRAM_CFG_PHYS_BASE + 0X0000E51C)

#if 0
///* 
// * Base Addresses for MCU Registers 
// * The ECCR register begins at offset 0x001C
// */
//#define IXP400_PARITYENACC_MCU_BASEADDR  (IX_OSAL_IXP400_SDRAM_CFG_PHYS_BASE + 0x0000001C)
#endif

/*
 * Address Ranges for Block and Port Level Registers
 * (Memory size = Register size x Number of registers)
 * Note: Number of registers includes a reserved test register
 */
#define IXP400_PARITYENACC_MCU_MEMMAP_SIZE        (0x04 * 0x07)

/* MCU Registers Offset Values */
#define IXP400_PARITYENACC_MCU_ECCR_OFFSET        (0x00)
#define IXP400_PARITYENACC_MCU_ELOG0_OFFSET       (0x04)
#define IXP400_PARITYENACC_MCU_ELOG1_OFFSET       (0x08)
#define IXP400_PARITYENACC_MCU_ECAR0_OFFSET       (0x0C)
#define IXP400_PARITYENACC_MCU_ECAR1_OFFSET       (0x10)
#define IXP400_PARITYENACC_MCU_MCISR_OFFSET       (0x18)

/*
 * Mask values to access various ECC registers
 */

/* Masks for bits in ECC Control Register */
#define IXP400_PARITYENACC_MCU_ECC_EN_MASK        (1 << 3)
#define IXP400_PARITYENACC_MCU_SBIT_CORRECT_MASK  (1 << 2)
#define IXP400_PARITYENACC_MCU_MBIT_REPORT_MASK   (1 << 1)
#define IXP400_PARITYENACC_MCU_SBIT_REPORT_MASK   (1 << 0)

/* Masks for bits in ECC Log Registers */
#define IXP400_PARITYENACC_MCU_ERR_MASTER_MASK    (0x00FF0000)
#define IXP400_PARITYENACC_MCU_ERR_RW_MASK        (0x00001000)
#define IXP400_PARITYENACC_MCU_ERR_SMBIT_MASK     (0x00000100)
#define IXP400_PARITYENACC_MCU_ERR_SYNDROME_MASK  (0x000000FF)

/* Masks for bits in ECC Address Registers */
#define IXP400_PARITYENACC_MCU_ERR_ADDRESS_MASK   (0xFFFFFFFC)

/* Masks for bits in Interrupt Status Register */
#define IXP400_PARITYENACC_MCU_ERROR0_MASK        (1 << 0)
#define IXP400_PARITYENACC_MCU_ERROR1_MASK        (1 << 1)
#define IXP400_PARITYENACC_MCU_ERRORN_MASK        (1 << 2)

/*
 * Values to identified from ECC registers' contents
 */

/* Values as interpreted from ECC Log Registers */
#define IXP400_PARITYENACC_MCU_ERR_MASTER_CORE_BIU  (0x00000000)
#define IXP400_PARITYENACC_MCU_ERR_MASTER_IB_BUS    (0x00800000)
#define IXP400_PARITYENACC_MCU_ERR_RW_READ          (0)
#define IXP400_PARITYENACC_MCU_ERR_RW_WRITE         (1)
#define IXP400_PARITYENACC_MCU_ERR_SMBIT_SGL        (0)
#define IXP400_PARITYENACC_MCU_ERR_SMBIT_MLT        (1)

/*
 * Typedefs used in this file
 */

/* MCU Control, Status and Other Registers virutal addresses */
typedef struct  /* IxParityENAccMcuPERegisters */
{
    UINT32  mcuEccr;  /* ECC Control Register virtual address */
    UINT32  mcuElog0; /* ECC Log Register_0 virtual address */
    UINT32  mcuElog1; /* ECC Log Register_1 virtual address */
    UINT32  mcuEcar0; /* ECC Address Registers_0 virtual address */
    UINT32  mcuEcar1; /* ECC Address Registers_1 virtual address */
    UINT32  mcuMcisr; /* Memory Controller Interrupt Status Register
                       * virtual address
                       */
} IxParityENAccMcuPERegisters;

/* MCU ISR type */  
typedef IxParityENAccPEIsr IxParityENAccMcuPEIsr;

/* MCU Interrupt Service Routine Info */
typedef struct  /* IxParityENAccMcuPEISRInfo */
{
    UINT32  mcuInterruptId;        /* MCU Interrupt Identifier */
    IxParityENAccMcuPEIsr  mcuIsr; /* ISR for handling interrupts */
} IxParityENAccMcuPEIsrInfo;

/*
 * MCU ECC Error Status
 *
 * Data is captured while retrieving the parity error context and
 * is also used for clearing the interrupt condtions based on the
 * parity error source returned from the client application.
 */
typedef struct  /* IxParityENAccMcuPEParityErrorStatus */
{
    UINT32  mcuEccrValue;  /* ECC Control Register contents */
    UINT32  mcuElog0Value; /* ECC Log Register_0 contents */
    UINT32  mcuElog1Value; /* ECC Log Register_1 contents */
    UINT32  mcuEcar0Value; /* ECC Address Register_0 contents */
    UINT32  mcuEcar1Value; /* ECC Address Register_1 contents */
    UINT32  mcuMcisrValue; /* Memory Controller Interrupt Status Register
                            * contents
                            */
} IxParityENAccMcuPEParityErrorStatus;

/* MCU Configuration Information */
typedef struct  /* IxParityENAccMcuPEConfig */
{
    /* MCU Control and Status Registers virtual addresses */
    IxParityENAccMcuPERegisters  mcuPERegisters;

    /* Contents of Status and Control Register */
    IxParityENAccMcuPEParityErrorStatus mcuParityErrorStatus;

    /* Internal Callback Routine */
    IxParityENAccInternalCallback  mcuPECallback;

    /* Interrupt Service Routine Info */
    IxParityENAccMcuPEIsrInfo  mcuIsrInfo;
} IxParityENAccMcuPEConfig;

/*
 * Variable declarations
 */

/* MCU Configuration Information */
PRIVATE IxParityENAccMcuPEConfig ixParityENAccMcuPEConfig =
{
    { 0,0,0,0,0,0 }, /* Registers virtual addresses */
    { 0,0,0,0,0,0 }, /* Registers contents */
    (IxParityENAccInternalCallback) NULL,  /* Internal Callback Routine */
    { 0, (IxParityENAccMcuPEIsr) NULL }    /* ISR Info */
};

/*
 * Local functions declarations
 */

/* MCU interrupt service routine */
void 
ixParityENAccMcuPEIsr (void);

/* Get parity error status into internal data structures */
void 
ixParityENAccMcuPEParityErrorStatusGet (void);

/*
 *  Identify the Multi & Single bit parity errors from 
 *  the internal data structures
 */
IXP400_PARITYENACC_INLINE IX_STATUS
ixParityENAccMcuPEParityErrorStatusInterpret (
    BOOL *parityError0,
    BOOL *parityError1,
    BOOL *parityErrorN,
    UINT32 *paritySource0,
    UINT32 *paritySource1);

IXP400_PARITYENACC_INLINE IX_STATUS
ixParityENAccMcuPEParityErrorStatusInterpret (
    BOOL *parityError0,
    BOOL *parityError1,
    BOOL *parityErrorN,
    UINT32 *paritySource0,
    UINT32 *paritySource1)
{
    register IxParityENAccMcuPEParityErrorStatus *mcuPEStatus = 
                &ixParityENAccMcuPEConfig.mcuParityErrorStatus;

    /*
     * Input parameter validation is not done in the local functions 
     * to avoid extra code.
     */

    /* Identify the parity errors detected from Interrupt Status Register */
    *parityError0 = IXP400_PARITYENACC_VAL_BIT_CHECK(mcuPEStatus->mcuMcisrValue,
                        IXP400_PARITYENACC_MCU_ERROR0_MASK);

    *parityError1 = IXP400_PARITYENACC_VAL_BIT_CHECK(mcuPEStatus->mcuMcisrValue,
                        IXP400_PARITYENACC_MCU_ERROR1_MASK);

    *parityErrorN = IXP400_PARITYENACC_VAL_BIT_CHECK(mcuPEStatus->mcuMcisrValue,
                        IXP400_PARITYENACC_MCU_ERRORN_MASK);

    /* Identify ECC/parity error #0 is of Single or Multi bit type */
    if (TRUE == *parityError0)
    {
        *paritySource0 = (FALSE == IXP400_PARITYENACC_VAL_BIT_CHECK(mcuPEStatus->mcuElog0Value, 
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_MASK)) ?
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_SGL : 
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_MLT;
    } /* end of if */

    /* Identify ECC/parity error #1 is of Single or Multi bit type */
    if (TRUE == *parityError1)
    {
        *paritySource1 = (FALSE == IXP400_PARITYENACC_VAL_BIT_CHECK(mcuPEStatus->mcuElog1Value, 
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_MASK)) ?
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_SGL : 
                                       IXP400_PARITYENACC_MCU_ERR_SMBIT_MLT;
    } /* end of if */

    return IX_SUCCESS;
} /* end of ixParityENAccMcuPEParityErrorStatusInterpret() function */

/* Transform parity error #0 or #1 status as needed by Main sub-module */
void
ixParityENAccMcuPEParityErrorStatusTransform (
    IxParityENAccMcuPEParityErrorContext *ixMcuPECMsg,
    UINT32 mcuElogNValue,
    UINT32 mcuEcarNValue);

#endif /* IXPARITYENACCMCUPE_P_H */
