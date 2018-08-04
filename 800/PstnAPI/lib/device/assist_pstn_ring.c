/*
 * $Log: assist_pstn_ring.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.11  2006/04/19 12:28:39  lizhijie
 * add CAR support and others
 *
 * Revision 1.10  2006/04/19 12:23:49  lizhijie
 * no message
 *
 * Revision 1.9  2006/03/24 11:07:46  lizhijie
 * add a C-language file for DTMF and silence data and compile it into libpstn.so
 *
 * Revision 1.8  2006/03/24 02:38:29  lizhijie
 * onhook function :comment 'while'
 *
 * Revision 1.7  2006/03/23 05:22:23  wangwei
 * no message
 *
 * Revision 1.6  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.5  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.4  2006/03/20 06:34:30  lizhijie
 * no message
 *
 * Revision 1.3  2006/03/16 08:41:29  lizhijie
 * add FSK support and FSK test programs in IXP4xx platform
 *
 * Revision 1.2  2006/03/16 08:36:59  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: assist_pstn_ring.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "as_dev_ctl.h"
#include "as_ring_param.h"

#include "assist_lib.h"

#define AS_HOOK_DEBUG		1


/* called by callee thread and monitor the off hook event with timeout */
int as_ring_with_dtmf_caller_id( int  fd  , unsigned char *call_id)
{
	int res;
	int y;
	AS_DIAL_OPERATION dtmf_dial_str ;
	
	y = AS_START;
	res = ioctl( fd, AS_CTL_HOOK, &y);

//	sleep(3);
	as_lib_wait_ringer_off(fd);
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
		as_error_msg("Uninit the dev device in %s\n" ,__FUNCTION__ );
		return -1;
	}	
#if AS_DEBUG	
	x = AS_ONHOOK;
	res = ioctl( fd,  AS_CTL_HOOK, &x);
#endif

	x = AS_START;
	res = ioctl(fd,  AS_CTL_HOOK, &x);
#if AS_DEBUG
	if (res) 
	{
		switch(errno) 
		{
			case EBUSY:
			case EINTR:
				/* Wait just in case */
				as_error_msg( "ring the phone BUSY\n" );
				usleep(10000);
				break;
			case EINPROGRESS:
				as_error_msg( "ring the phone IN PROGRESS\n" );
				res = 0;
				break;
			default:
				as_error_msg( "Couldn't ring the phone: %s\n", strerror(errno));
				break;
		}
	}
#endif	
	return res;
}

/* send CAR on FXS device */
int as_car_on_fxs( int  fd )
{
//	int count = 0;
	int x;
	int res =0;
		
	if( fd <0)
	{
		as_error_msg("Uninit the dev device in %s\n" ,__FUNCTION__ );
		return -1;
	}	
#if AS_DEBUG	
	x = AS_ONHOOK;
	res = ioctl( fd,  AS_CTL_HOOK, &x);
#endif

	x = AS_RING_CAR;
	res = ioctl(fd,  AS_CTL_HOOK, &x);
#if AS_DEBUG
	if (res) 
	{
		switch(errno) 
		{
			case EBUSY:
			case EINTR:
				/* Wait just in case */
				as_error_msg( "ring the phone BUSY\n" );
				usleep(10000);
				break;
			case EINPROGRESS:
				as_error_msg( "ring the phone IN PROGRESS\n" );
				res = 0;
				break;
			default:
				as_error_msg( "Couldn't ring the phone: %s\n", strerror(errno));
				break;
		}
	}
#endif	
	return res;
}

int as_stop_ring(int fd)
{
	int hook;
	int res;
	
	hook = AS_RINGOFF;
	res = ioctl(fd, AS_CTL_HOOK, &hook);
	if(res<0)
	{
		printf("error in HOOK IOCTL : %s\r\n", strerror(errno) );
		exit(1);
	}
	return 0;
}

/* set device to ONHOOK status, eg. reset the device */
int as_lib_onhook(int fd)
{
	int res = 0;
	int hook = AS_ONHOOK;

//	while (res == -1)
	{
		res = ioctl(fd, AS_CTL_HOOK, &hook);
		//printf("OnHook operator\n");
		if(res <0 )
			printf("error in AS_ONHOOK : %s with fd : %d\r\n", strerror(errno), fd);
	}
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
			as_error_msg( "Event '%d' received\r\n", res );
#endif		
	}while(res != AS_EVENT_RINGOFFHOOK );
#if AS_HOOK_DEBUG	
	as_error_msg("RING OFFHOOK \r\n" );
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

int as_lib_wait_ringer_off(int fd)
{
	int res;
	
	do
	{
		res = as_lib_event_get( fd);
		if (res < 0) /* if error, return as such */
		{
			res = AS_RINGOFF;  /* make sure ringer is off */
			  /* do the ioctl to turn ringer off. if its already off it will ret. error. its okay. */
			ioctl( fd, AS_CTL_HOOK, &res );
			return (-1);
		}			
#if AS_HOOK_DEBUG
		else if(res )
			as_error_msg( "Event '%d' received\r\n", res );
#endif		
	}while( (res != AS_EVENT_RINGEROFF) && (res != AS_EVENT_RINGOFFHOOK ) );

	//	usleep(500000);  /* sleep at least 1/2 sec */
	return 0;
}


