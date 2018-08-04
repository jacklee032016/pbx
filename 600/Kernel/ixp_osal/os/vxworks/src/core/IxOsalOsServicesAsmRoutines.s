/**
 * @file IxOsalOsServicesAsmRoutines.s (vxWorks)
 *
 * @brief  Fastmutex implementation.
 * 
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



#ifdef __XSCALE__

/*
 * System defines and include files
 */
#define _ASMLANGUAGE
#include <arch/arm/arm.h>

.balign 4

.global ixOsalOemFastMutexTryLock

/* Quickest mutex. r0 must contain a local memory address.
   C prototype:
   IX_STATUS ixOsalOemFastMutexTryLock(IxOsalFastMutex *mutex);
   mutex must be a local memory address (use IxOsalFastMutex).
   *mutex must be either 0 or 1 when this function is called.
   if *mutex is 0, *mutex will be set to 1 and the function returns 0;
   else *mutex will stay 1 and the function returns 1. */
_ARM_FUNCTION(ixOsalOemFastMutexTryLock)
	mov		r1, #1
	swp		r2, r1, [r0] /* r2 <= {r0} and {r0} <= r1 */
	mov		r0, r2
	mov		pc, lr


#endif
