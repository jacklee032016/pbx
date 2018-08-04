/*

 * $Id: assist_pcm_ring.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include "as_dev_ctl.h"
#include "assist_lib.h"

#define AS_HOOK_DEBUG		1


/* called by callee thread and monitor the off hook event with timeout */
int pcm_ring_with_dtmf_caller_id( int  fd  , unsigned char *call_id)
{
	return 0;
}

int pcm_ring_on_hook( int  fd )
{
	return 0;
}

/* set device to ONHOOK status, eg. reset the device */
int pcm_lib_onhook(int fd)
{
	return 0;
}

/* set the device to OFFHOOK status, eg. begin to communicate */
int pcm_lib_offhook(int fd)
{
	return 1;
}


/* wait RINGOFF event, 
 * when it used on a FXO device, this is first RINGEROFF
 * when is used on a FXS device, this is OFFHOOK
*/
void pcm_lib_wait_offhook(int fd)
{
	return;
}

void pcm_lib_wait_onhook(int fd)
{
	return;
}


