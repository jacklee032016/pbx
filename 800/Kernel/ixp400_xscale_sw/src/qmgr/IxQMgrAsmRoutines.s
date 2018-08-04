/**
 * @file    IxQMgrAsmRoutines.s
 *
 * @author Intel Corporation
 * @date    20-Jul-2002
 *    
 * @brief   This file contains the XSCALE implementation of the fastest way to
 * to search a bit in a bitfield.
 *
 * See also: XSCALE processor instruction set documentation (clz)
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


#ifndef __linux
#ifdef __XSCALE__

/*
 * System defines and include files
 */
#define _ASMLANGUAGE
#include <arch/arm/arm.h>

.balign 4

.global ixQMgrCountLeadingZeros
/* input: r0 must contain the 32 bits bitfield where bits are to be searched.
   output: r0 contains the number of leading zero bits (or 32 if the word is nul)

   C prototype:
   unsigned int ixQMgrCountLeadingZeros(UINT32 r0);
*/
_ARM_FUNCTION(ixQMgrCountLeadingZeros)
	clz		r0, r0
	mov		pc, lr

#endif

#endif /* !__linux */

