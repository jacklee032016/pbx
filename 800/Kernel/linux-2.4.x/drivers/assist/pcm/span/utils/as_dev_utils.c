/*
 * $Revision: 1.1.1.1 $
 * $Log: as_dev_utils.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.1  2005/06/27 06:02:38  lizhijie
 * no message
 *
 * $Author: lizhijie $
 * $Id: as_dev_utils.c,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
*/

#include "asstel.h"

#include <linux/slab.h>

/* sleep in user space until woken up. Equivilant of tsleep() in BSD */
int  as_schedule_delay(wait_queue_head_t *q)
{
	/* declare a wait_queue_t variable as 'wait', and add 'current' task into it */
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(q, &wait);
	current->state = TASK_INTERRUPTIBLE;

	if (!signal_pending(current)) 
		schedule();
	
	/* here, this task is waken up, and going to continue */
	current->state = TASK_RUNNING;
	remove_wait_queue(q, &wait);

	if (signal_pending(current)) 
		return -ERESTARTSYS;

	return(0);
}

