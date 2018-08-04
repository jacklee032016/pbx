/**
 * @file IxOsalOs.h
 *
 * @brief linux-specific defines 
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

#ifndef IxOsalOs_H
#define IxOsalOs_H

#ifndef IX_OSAL_CACHED
#error "Uncached memory not supported in linux environment"
#endif

#include <linux/cache.h>
#include <linux/mm.h>
#include <linux/config.h>
#include <asm/pgalloc.h>

/**
 * Linux implementations of macros.
 */

#define IX_OSAL_OS_MMU_VIRT_TO_PHYS(addr) ((addr) ? virt_to_phys((void*)(addr)) : 0)

#define IX_OSAL_OS_MMU_PHYS_TO_VIRT(addr)  ((addr) ? phys_to_virt((unsigned int)(addr)) : 0)

#define IX_OSAL_OS_CACHE_INVALIDATE(addr, size)  ( invalidate_dcache_range((__u32)addr, (__u32)addr + size )) 

#define IX_OSAL_OS_CACHE_FLUSH(addr, size) ( clean_dcache_range((__u32)addr, (__u32)addr + size ))

#define printf	printk /* For backword compatibility, needs to move to better location */

#endif /* IxOsalOs_H */
