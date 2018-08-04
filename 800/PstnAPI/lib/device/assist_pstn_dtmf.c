/*
 * $Log: assist_pstn_dtmf.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/03/21 09:50:58  lizhijie
 * debug with vpbx program
 *
 * Revision 1.3  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.2  2006/03/11 10:18:34  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: assist_pstn_dtmf.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <math.h>

#include "as_dev_ctl.h"
#include "assist_lib.h"

/* get an event of the device */
int as_lib_event_get(int fd)
{
	int j =-1;
	
	if (ioctl(fd, AS_CTL_GETEVENT, &j) == -1) 
		return -1;
	return j;
}


/* return 0: no dtmf digit is ready ;  not 0 : a dtmf digit is ready */
int as_dtmf_is_ready( int fd )
{
	int res;
	res = as_lib_event_get( fd);
	if (res <0 ) 
	{
		as_error_msg( "Get event failed in this FXS device : %s!\n", strerror(errno));
		return 0;	
	}
	if( res==AS_EVENT_DTMFDIGIT)
	{
#if AS_EVENT_DEBUG			
		as_error_msg("Detect DTMF signal\r\n");
#endif	
		return 1;
	}
	return 0;
}

/* return <0: no DIAL PULSE digit is ready ;  >= 0 : a DP digit is ready */
int as_get_dp_signal( int fd )
{
	int res;
	res = as_lib_event_get( fd);
	if (res <0 ) 
	{
		as_error_msg( "Get event failed in this FXS device : %s!\n", strerror(errno));
		return -1;	
	}
	if( res&AS_EVENT_PULSEDIGIT )
	{
#if AS_EVENT_DEBUG			
		as_error_msg("Detect PULSE signal\r\n");
#endif	
		return (res &(~AS_EVENT_PULSEDIGIT) );
	}
	return -1;
}

unsigned char as_get_dtmf_4_fxs(int fd)
{
	int signal;
	if (ioctl(fd, AS_CTL_GET_DTMF_DETECT, &signal) == -1) 
	{
		as_error_msg("IOCTL error\r\n");
		return 'X';
	}
//#if AS_EVENT_DEBUG			
//	as_error_msg("DTMF signal is '%c' \r\n", (unsigned char)signal);
//#endif

	return (unsigned char)signal;
}

