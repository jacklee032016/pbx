/*
 * $Id: assist_slic_ring.c,v 1.3 2007/03/16 20:40:29 lizhijie Exp $
 */
#include "as_dev_ctl.h"
#include "aslib.h"
#include "pbx_globals.h"
#include <sys/ioctl.h>
#include <stdio.h>

int as_lib_event_get(int fd);

#define AS_HOOK_DEBUG		1


/* called by callee thread and monitor the off hook event with timeout */
int as_ring_with_dtmf_caller_id( int  fd  , unsigned char *call_id)
{
	int res;
	int y;
	AS_DIAL_OPERATION dtmf_dial_str ;
	
	y = AS_RING;
	res = ioctl( fd, AS_CTL_HOOK, &y);

	dtmf_dial_str.op = AS_DIAL_OP_REPLACE;
	sprintf(dtmf_dial_str.dialstr, "%s" , call_id );

	res = ioctl( fd,  AS_CTL_SET_DTMF_STR , &dtmf_dial_str);
	
	return res;
}

int as_ring_on_hook( int  fd )
{
//	int count = 0;
	int x;
	int res =0;
		
	if( fd <0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Uninit the dev device in %s" ,__FUNCTION__ );
		return -1;
	}	
#if AS_DEBUG	
	x = AS_ONHOOK;
	res = ioctl( fd,  AS_CTL_HOOK, &x);
#endif

	x = AS_RING;
	res = ioctl(fd,  AS_CTL_HOOK, &x);
#if AS_DEBUG
	if (res) 
	{
		switch(errno) 
		{
			case EBUSY:
			case EINTR:
				/* Wait just in case */
				AS_MSG_DEBUG(AS_LOG_INFO, "ring the phone BUSY" );
				usleep(10000);
				break;
			case EINPROGRESS:
				AS_MSG_DEBUG(AS_LOG_INFO,  "ring the phone IN PROGRESS" );
				res = 0;
				break;
			default:
				AS_MSG_DEBUG(AS_LOG_INFO, "Couldn't ring the phone: %s", strerror(errno));
				break;
		}
	}
#endif	
	return res;
}

/* set device to ONHOOK status, eg. reset the device */
int as_lib_onhook(int fd)
{
	int res;
	int i;
	
	i = AS_ONHOOK;
	res = ioctl(fd,AS_CTL_HOOK,&i) ;
	return res;
}

/* set the device to OFFHOOK status, eg. begin to communicate */
int as_lib_offhook(int fd)
{
	int res;
	int i;
	
	i = AS_OFFHOOK;
	res = ioctl(fd,AS_CTL_HOOK,&i) ;
	return res;
}


/* wait RINGOFF event, 
 * when it used on a FXO device, this is first RINGEROFF
 * when is used on a FXS device, this is OFFHOOK
*/
void as_lib_wait_offhook(int fd)
{
	int res;
	
	do
	{
		res = as_lib_event_get( fd);
#if AS_HOOK_DEBUG
		if(res )
		{
			AS_MSG_DEBUG(AS_LOG_INFO, "Event '%d' received", res );
		}	
#endif		
	}while(res != AS_EVENT_RINGOFFHOOK );
	
#if AS_HOOK_DEBUG	
	AS_MSG_DEBUG(AS_LOG_INFO, "RING OFFHOOK" );
#endif
}

void as_lib_wait_onhook(int fd)
{
	int res;
	
	do
	{
		res = as_lib_event_get( fd);
	}while(res != AS_EVENT_ONHOOK );
}

