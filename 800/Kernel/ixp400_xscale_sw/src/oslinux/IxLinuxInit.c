/*
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
/* Non-Linux related headers */
#include <IxFeatureCtrl.h>
#include <IxNpeMh.h>

/* Linux initialization */
#ifdef __linux

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/uaccess.h>


/******************************************************
 * Include File Operations functions and declarations *
 * if Microcode is to be read in from a file          *
 ******************************************************/
#ifdef IX_NPEDL_READ_MICROCODE_FROM_FILE

/* #defines, variables and forward declarations for file operations */
#define DEV_IXNPE_MAJOR_NUMBER 241

typedef struct 
{
    UINT32* data;
    UINT32 size;
    struct IxNpeMicrocodeSegment* next;
} IxNpeMicrocodeSegment;

UINT32* ixNpeMicrocode_binaryArray;
PRIVATE IxNpeMicrocodeSegment* ixNpeDlSegmentedListHead = NULL;
PRIVATE IxNpeMicrocodeSegment* ixNpeDlSegmentedListTail = NULL;
PRIVATE UINT32 ixNpeDlMicrocodePageOrder, ixNpeDlTotalBytesReadIn = 0;

int ixNpe_dev_open (struct inode *inode, struct file *file);
int ixNpe_dev_release (struct inode *inode, struct file *file);
ssize_t ixNpe_dev_write (struct file *file, const char *buf, size_t byteCount, loff_t *offset);

struct file_operations ixNpe_dev_fops = {
    owner:    THIS_MODULE,
    
    open:     ixNpe_dev_open,
    release:  ixNpe_dev_release,
    
    write:    ixNpe_dev_write
};

#endif /* IX_NPEDL_READ_MICROCODE_FROM_FILE */


/* Module parameters */
static int livelock_dispatcher = 0;/* default: don't use livelock dispatcher*/

MODULE_PARM(livelock_dispatcher, "i");
MODULE_PARM_DESC(livelock_dispatcher, "If non-zero, use the livelock prevention qmgr dispatcher");

/* Init and cleanup functions for module */
static int __init ixp400_sw_init_module(void)
{
    /* Add initialization code here */
#ifdef DEBUG
    printk("\n\n"__FUNCTION__": addr=%p\n\n", ixp400_sw_init_module);
#endif

    /*
     * If livelock prevention dispatcher is requested - enable in FeatureCtrl
     */
    if (livelock_dispatcher != 0)
    {
        ixFeatureCtrlSwConfigurationWrite (IX_FEATURECTRL_ORIGB0_DISPATCHER,
                                           IX_FEATURE_CTRL_SWCONFIG_DISABLED);
    }

#ifdef IX_NPEDL_READ_MICROCODE_FROM_FILE
    /* Register driver for /dev/ixNpe */
    if (register_chrdev(DEV_IXNPE_MAJOR_NUMBER, "ixNpe", &ixNpe_dev_fops))
	printk (KERN_ERR "Failed to register driver for /dev/ixNpe\n");
#endif /* IX_NPEDL_READ_MICROCODE_FROM_FILE */

    printk(KERN_DEBUG "ixp400: Module init.\n");
    return 0;
}

static void __exit ixp400_sw_cleanup_module(void)
{
    /* Add cleanup code here */
    ixNpeMhUnload();

#ifdef IX_NPEDL_READ_MICROCODE_FROM_FILE
    /* Free memory Microcode images are in and unregister driver for ixNpe */
    free_pages((UINT32)ixNpeMicrocode_binaryArray, ixNpeDlMicrocodePageOrder);
    if (unregister_chrdev(DEV_IXNPE_MAJOR_NUMBER, "ixNpe"))
    {
	printk (KERN_ERR "Failed to unregister driver for /dev/ixNpe\n");
    }
#endif /* IX_NPEDL_READ_MICROCODE_FROM_FILE */

    printk(KERN_DEBUG "ixp400: Module uninit.\n");
}

module_init(ixp400_sw_init_module);
module_exit(ixp400_sw_cleanup_module);


/******************************************
 * Include File Operations functions if   *
 * Microcode is to be read in from a file *
 ******************************************/
#ifdef IX_NPEDL_READ_MICROCODE_FROM_FILE

/* File Operations support for /dev/ixNpe */
int ixNpe_dev_open (struct inode *inode, struct file *file)
{
    /* Test to see if we've read in Microcode already */
    if (ixNpeMicrocode_binaryArray != NULL)
    {
	printk (KERN_ERR "ixp400.o:  Microcode is already in memory.\n");
	return -EEXIST;
    }

    MOD_INC_USE_COUNT;   /* Increment use count to prevent premature rmmod-ing */
    ixNpeDlSegmentedListHead = NULL;  /* Reset linked list which keeps track of Microcode fragments */
    ixNpeDlSegmentedListTail = NULL;
    ixNpeDlTotalBytesReadIn = 0;
    return 0;
}

int ixNpe_dev_release (struct inode *inode, struct file *file)
{
    IxNpeMicrocodeSegment* temp = NULL;
    UINT32* currentPosition = NULL;
    UINT32 pagesNeeded, x;
    
    /* If we got data, allocate pages and copy piecemeal *
     * data into permanent memory as single block        */
    if (ixNpeDlTotalBytesReadIn > 0)
    {
        /* Calculate how many pages needed */
	pagesNeeded = ixNpeDlTotalBytesReadIn/PAGE_SIZE;

	if (ixNpeDlTotalBytesReadIn%PAGE_SIZE)
	{
	    pagesNeeded++;        /* Allow for lack of floating point division */
	}
	
	/* Shift right to find what order is needed for __get_free_pages */
	for (x = pagesNeeded; x > 0; x >>= 1)
	{
	    ixNpeDlMicrocodePageOrder++;
	}

	/* __get_free_pages returns a pointer cast as an unsigned long */
	ixNpeMicrocode_binaryArray = (UINT32 *)(__get_free_pages(GFP_KERNEL, ixNpeDlMicrocodePageOrder));
	if (ixNpeMicrocode_binaryArray == NULL)
	{
	    printk (KERN_ERR "ixp400.o:  Error, couldn't get enough memory to store Microcode in (need %d pages).\n", pagesNeeded);
	}
	else
	{
	    /* Copy each segment of data into a single image */
	    temp = ixNpeDlSegmentedListHead;
	    currentPosition = ixNpeMicrocode_binaryArray;
	    
	    while (temp != NULL)
	    {
		memcpy(currentPosition, temp->data, temp->size);
		
		/* increment pointers */
		currentPosition = (UINT32 *)((UINT32)currentPosition + temp->size);
		temp = (IxNpeMicrocodeSegment *) temp->next;       /* cast is for fussy compiler */
	    }
	}
    }
    
    /* Free the temporary list memory and decrement use count */
    while (ixNpeDlSegmentedListHead->next != NULL)
    {
	/* Move the head pointer down the list and free old element */
	temp = ixNpeDlSegmentedListHead;
	ixNpeDlSegmentedListHead = (IxNpeMicrocodeSegment *) ixNpeDlSegmentedListHead->next;
	
	kfree (temp->data);
	kfree (temp);
    }

    MOD_DEC_USE_COUNT;

    return 0;
}

ssize_t ixNpe_dev_write (struct file *file, const char *buf, size_t byteCount, loff_t *offset)
{
    /* Malloc memory to hold this segment of the Microcode */
    UINT32* currentData = kmalloc (byteCount, GFP_KERNEL);
    IxNpeMicrocodeSegment* currentSegment = kmalloc (sizeof(IxNpeMicrocodeSegment), GFP_KERNEL);

    /* Copy data from user space */
    if (copy_from_user(currentData, buf, byteCount))
    {
	/* Copy Failed:  Free memory just kmalloced */
	kfree (currentData);
	kfree (currentSegment);
	
	/* Reset total byte count to prevent page allocation in ixNpe_dev_release() */
	ixNpeDlTotalBytesReadIn = 0;
	return -EFAULT;
    }

    /* Copy was successful so update struct */
    currentSegment->data = currentData;
    currentSegment->size = byteCount;
    currentSegment->next = NULL;	
    ixNpeDlTotalBytesReadIn += byteCount;

    /* Add to linked list of microcode segments */
    if (ixNpeDlSegmentedListHead == NULL)
    {
	ixNpeDlSegmentedListHead = currentSegment;
	ixNpeDlSegmentedListTail = currentSegment;
    }
    else
    {
	ixNpeDlSegmentedListTail->next = (struct IxNpeMicrocodeSegment *)currentSegment;  /* cast is for fussy compiler */
	ixNpeDlSegmentedListTail = currentSegment;
    }

    /* Return the number of bytes we wrote */
    return byteCount;
}

#endif /* IX_NPEDL_READ_MICROCODE_FROM_FILE */

#else

#error Non-Linux compilation

#endif /* #ifdef __linux */
