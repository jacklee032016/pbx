/**
 * @file IxNpeDlNpeMgrUtils_p.h
 *
 * @author Intel Corporation
 * @date 18 February 2002
 * @brief This file contains the private API for the NpeMgr module.
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


/**
 * @defgroup IxNpeDlNpeMgrUtils_p IxNpeDlNpeMgrUtils_p
 *
 * @brief The private API for the IxNpeDl NpeMgr Utils module
 * 
 * @{
 */

#ifndef IXNPEDLNPEMGRUTILS_P_H
#define IXNPEDLNPEMGRUTILS_P_H


/*
 * Put the user defined include files required.
 */
#include "IxNpeDl.h"
#include "IxOsalTypes.h"
#include "IxNpeDlNpeMgrEcRegisters_p.h"


/*
 * Function Prototypes
 */

/**
 * @fn IX_STATUS ixNpeDlNpeMgrInsMemWrite (UINT32 npeBaseAddress,
                                           UINT32 insMemAddress,
			                   UINT32 insMemData,
                                           BOOL verify)
 * 
 * @brief Writes a word to NPE Instruction memory
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] insMemAddress  - NPE instruction memory address to write
 * @param UINT32 [in] insMemData     - data to write to instruction memory
 * @param BOOL   [in] verify         - if TRUE, verify the memory location is
 *                                     written successfully.
 * 
 * This function is used to write a single word of data to a location in NPE 
 * instruction memory.  If the <i>verify</i> option is ON, NpeDl will read back
 * from the memory location to verify that it was written successfully
 * 
 * @pre
 *
 * @post
 *
 * @return
 *    - IX_FAIL if verify is TRUE and the memory location was not written
 *      successfully
 *    - IX_SUCCESS otherwise
 */ 
IX_STATUS
ixNpeDlNpeMgrInsMemWrite (UINT32 npeBaseAddress, UINT32 insMemAddress,
			  UINT32 insMemData, BOOL verify);


/**
 * @fn IX_STATUS ixNpeDlNpeMgrDataMemWrite (UINT32 npeBaseAddress,
                                            UINT32 dataMemAddress,
	                                    UINT32 dataMemData,
                                            BOOL verify)
 * 
 * @brief Writes a word to NPE Data memory
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] dataMemAddress - NPE data memory address to write
 * @param UINT32 [in] dataMemData    - data to write to NPE data memory
 * @param BOOL   [in] verify         - if TRUE, verify the memory location is
 *                                     written successfully.
 * 
 * This function is used to write a single word of data to a location in NPE 
 * data memory.  If the <i>verify</i> option is ON, NpeDl will read back from
 * the memory location to verify that it was written successfully
 * 
 * @pre
 *
 * @post
 *
 * @return
 *    - IX_FAIL if verify is TRUE and the memory location was not written
 *      successfully
 *    - IX_SUCCESS otherwise
 */ 
IX_STATUS
ixNpeDlNpeMgrDataMemWrite (UINT32 npeBaseAddress, UINT32 dataMemAddress,
			   UINT32 dataMemData, BOOL verify);


/**
 * @fn void ixNpeDlNpeMgrExecAccRegWrite (UINT32 npeBaseAddress,
                                          UINT32 regAddress,
                                          UINT32 regData)
 * 
 * @brief Writes a word to an NPE Execution Access register
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] regAddress     - NPE Execution Access register address
 * @param UINT32 [in] regData        - data to write to register
 * 
 * This function is used to write a single word of data to an NPE Execution
 * Access register.
 * 
 * @pre
 *
 * @post
 *
 * @return none
 */ 
void
ixNpeDlNpeMgrExecAccRegWrite (UINT32 npeBaseAddress, UINT32 regAddress,
			      UINT32 regData);


/**
 * @fn UINT32 ixNpeDlNpeMgrExecAccRegRead (UINT32 npeBaseAddress,
                                           UINT32 regAddress)
 * 
 * @brief Reads the contents of an NPE Execution Access register
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] regAddress     - NPE Execution Access register address
 * 
 * This function is used to read the contents of an NPE Execution
 * Access register.
 * 
 * @pre
 *
 * @post
 *
 * @return The value read from the Execution Access register
 */ 
UINT32
ixNpeDlNpeMgrExecAccRegRead (UINT32 npeBaseAddress, UINT32 regAddress);


/**
 * @fn void ixNpeDlNpeMgrCommandIssue (UINT32 npeBaseAddress,
                                       UINT32 command)
 * 
 * @brief Issues an NPE Execution Control command
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] command        - Command to issue
 * 
 * This function is used to issue a stand-alone NPE Execution Control command
 * (e.g. command to Stop NPE execution)
 * 
 * @pre
 *
 * @post
 *
 * @return none
 */ 
void
ixNpeDlNpeMgrCommandIssue (UINT32 npeBaseAddress, UINT32 command);


/**
 * @fn void ixNpeDlNpeMgrDebugInstructionPreExec (UINT32 npeBaseAddress)
 * 
 * @brief Prepare to executes one or more NPE instructions in the Debug
 *        Execution Stack level.
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * 
 * This function should be called once before a sequence of calls to 
 * ixNpeDlNpeMgrDebugInstructionExec().
 * 
 * @pre
 *
 * @post
 *     - ixNpeDlNpeMgrDebugInstructionPostExec() should be called to restore
 *       registers values altered by this function
 *
 * @return none
 */ 
void
ixNpeDlNpeMgrDebugInstructionPreExec (UINT32 npeBaseAddress);


/**
 * @fn IX_STATUS ixNpeDlNpeMgrDebugInstructionExec (UINT32 npeBaseAddress,
                                               UINT32 npeInstruction,
                                               UINT32 ctxtNum,
                                               UINT32 ldur)
 * 
 * @brief Executes a single instruction on the NPE at the Debug Execution Stack
 *        level
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] npeInstruction - Value to write to INSTR (Instruction)
 *                                     register
 * @param UINT32 [in] ctxtNum        - context the instruction will be executed
 *                                     in and which context store it may access
 * @param UINT32 [in] ldur           - Long Immediate Duration, set to non-zero
 *                                     to use long-immediate mode instruction
 * 
 * This function is used to execute a single instruction in the NPE pipeline at
 * the debug Execution Context Stack level. It won't disturb the state of other
 * executing contexts.  Its useful for performing NPE operations, such as
 * writing to NPE Context Store registers and physical registers, that cannot
 * be carried out directly using the Configuration Bus registers. This function
 * will return TIMEOUT status if NPE not responding due to NPS is hang / halt.
 * 
 * @pre 
 *     - The NPE should be stopped and in a clean state
 *     - ixNpeDlNpeMgrDebugInstructionPreExec() should be called once before
 *       a sequential of 1 or more calls to this function
 *
 * @post
 *     - ixNpeDlNpeMgrDebugInstructionPostExec() should be called after 
 *       a sequence of calls to this function
 *
 * @return 
 *    - IX_NPEDL_CRITICAL_NPE_ERR if execution of instruction failed / timeout
 *    - IX_SUCCESS otherwise 
 */ 
IX_STATUS
ixNpeDlNpeMgrDebugInstructionExec (UINT32 npeBaseAddress,
				   UINT32 npeInstruction,
				   UINT32 ctxtNum, UINT32 ldur);


/**
 * @fn void ixNpeDlNpeMgrDebugInstructionPostExec (UINT32 npeBaseAddress)
 * 
 * @brief Clean up after executing one or more NPE instructions in the
 *        Debug Stack Level
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * 
 * This function should be called once following a sequence of calls to 
 * ixNpeDlNpeMgrDebugInstructionExec().
 *
 * @pre 
 *    -  ixNpeDlNpeMgrDebugInstructionPreExec() was called earlier
 *
 * @post
 *    -  The Instruction Pipeline will cleared
 *
 * @return none
 */ 
void
ixNpeDlNpeMgrDebugInstructionPostExec (UINT32 npeBaseAddress);


/**
 * @fn IX_STATUS ixNpeDlNpeMgrPhysicalRegWrite (UINT32 npeBaseAddress,
                                                UINT32 regAddr,
                                                UINT32 regValue,
					        BOOL verify)
 * 
 * @brief Write one of the 32* 32-bit physical registers in the NPE data 
 *        register file
 *
 * @param UINT32 [in] npeBaseAddress - Base Address of NPE
 * @param UINT32 [in] regAddr        - number of the physical register (0-31)*
 * @param UINT32 [in] regValue       - value to write to the physical register
 * @param BOOL   [in] verify         - if TRUE, verify the register is written
 *                                     successfully.
 * 
 * This function writes a physical register in the NPE data register file.
 * If the <i>verify</i> option is ON, NpeDl will read back the register to 
 * verify that it was written successfully
 * *Note that release 1.0 of this software supports 32 physical
 * registers, but 64 may be supported in future versions.
 * 
 * @pre
 *    - The NPE should be stopped and in a clean state
 *    - ixNpeDlNpeMgrDebugInstructionPreExec() should be called once before
 *      a sequential of 1 or more calls to this function
 *
 * @post
 *    - Contents of REGMAP Context Store register for Context 0 will be altered
 *    - ixNpeDlNpeMgrDebugInstructionPostExec() should be called after 
 *      a sequence of calls to this function
 *
 * @return
 *    - IX_FAIL if verify is TRUE and the Context Register was not written
 *      successfully
 *    - IX_SUCCESS if Context Register was written successfully
 *    - IX_NPEDL_CRITICAL_NPE_ERR if Context Register was not written 
 *      successfully due to timeout error where NPE is not responding
 */
IX_STATUS
ixNpeDlNpeMgrPhysicalRegWrite (UINT32 npeBaseAddress, UINT32 regAddr,
			       UINT32 regValue, BOOL verify);


/**
 * @fn IX_STATUS ixNpeDlNpeMgrCtxtRegWrite (UINT32 npeBaseAddress,
                                            UINT32 ctxtNum,
                                            IxNpeDlCtxtRegNum ctxtReg,
                                            UINT32 ctxtRegVal,
                                            BOOL verify)
 * 
 * @brief Writes a value to a Context Store register on an NPE
 *
 * @param UINT32 [in] npeBaseAddress     - Base Address of NPE
 * @param UINT32 [in] ctxtNum            - context store to access
 * @param IxNpeDlCtxtRegNum [in] ctxtReg - which Context Store reg to write
 * @param UINT32 [in] ctxtRegVal         - value to write to the Context Store
 *                                         register
 * @param BOOL   [in] verify             - if TRUE, verify the register is
 *                                         written successfully.
 * 
 * This function writes the contents of a Context Store register in the NPE 
 * register file. If the <i>verify</i> option is ON, NpeDl will read back the
 * register to verify that it was written successfully
 * 
 * @pre
 *    - The NPE should be stopped and in a clean state
 *    - ixNpeDlNpeMgrDebugInstructionPreExec() should be called once before
 *      a sequential of 1 or more calls to this function
 *
 * @post
 *    - ixNpeDlNpeMgrDebugInstructionPostExec() should be called after 
 *      a sequence of calls to this function
 *
 * @return
 *    - IX_FAIL if verify is TRUE and the Context Register was not written
 *      successfully
 *    - IX_SUCCESS if Context Register was written successfully
 *    - IX_NPEDL_CRITICAL_NPE_ERR if Context Register was not written 
 *      successfully due to timeout error where NPE is not responding
 */
IX_STATUS
ixNpeDlNpeMgrCtxtRegWrite (UINT32 npeBaseAddress, UINT32 ctxtNum,
			   IxNpeDlCtxtRegNum ctxtReg, UINT32 ctxtRegVal,
			   BOOL verify);


/**
 * @fn void ixNpeDlNpeMgrUtilsStatsShow (void)
 *
 * @brief This function will display the statistics of the IxNpeDl NpeMgrUtils
 *        module
 *
 * @return none
 */
void
ixNpeDlNpeMgrUtilsStatsShow (void);


/**
 * @fn void ixNpeDlNpeMgrUtilsStatsReset (void)
 *
 * @brief This function will reset the statistics of the IxNpeDl NpeMgrUtils
 *        module
 *
 * @return none
 */
void
ixNpeDlNpeMgrUtilsStatsReset (void);


#endif  /* IXNPEDLNPEMGRUTILS_P_H */
