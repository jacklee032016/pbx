/*
 * $Id: assist_slic_dtmf.c,v 1.2 2007/03/16 20:40:29 lizhijie Exp $
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "assist_globals.h"

/* get an event of the device */
int as_lib_event_get(int fd)
{
	int j =-1;
	
	if (ioctl(fd, AS_CTL_GETEVENT, &j) == -1) 
		return -1;
	return j;
}


/* return 0: no dtmf digit is ready ;  not 0 : a dtmf digit is ready */
int as_dtmf_is_ready( int fd , const char *devicename)
{
	int res;
	res = as_lib_event_get( fd);
	if (res <0 ) 
	{
		AS_MSG_LOG(AS_LOG_WARNING, "Get event failed in device '%s': %s!", devicename, strerror(errno));
		return 0;	
	}
	if( res==AS_EVENT_DTMFDIGIT)
	{
#if AS_EVENT_DEBUG			
		AS_MSG_DEBUG(AS_LOG_INFO, "Detect DTMF signal");
#endif	
		return 1;
	}
	return 0;
}


/* set device to ONHOOK status, eg. reset the device */
int as_setup_onhook(int fd)
{
	int res;
	int i;
	
	i = AS_ONHOOK;
	res = ioctl(fd,AS_CTL_HOOK,&i) ;
	return res;
}

