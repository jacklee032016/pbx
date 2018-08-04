/*
 * $Log: assist_slic_tone.c,v $
 * Revision 1.2  2007/03/16 20:40:29  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
 * AS600 Pbx
 *
 * Revision 1.1.1.1  2006/07/12 16:16:47  lizhijie
 * rebuild 
 *
 * Revision 1.1.1.1  2005/03/14 07:38:05  lizhijie
 * PBX program
 *
 * Revision 1.1.1.1  2005/02/16 04:51:10  lizhijie
 * new pbx code
 *
 * Revision 1.1.1.1  2005/02/05 05:30:17  lizhijie
 * pbx from UA of vocal
 *
 * $Id: assist_slic_tone.c,v 1.2 2007/03/16 20:40:29 lizhijie Exp $
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "assist_globals.h"
#include "pbx_globals.h"

static int __as_play_tone(int fd, int tone)
{
	int res;

	if( fd <0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Uninit the dev device for dev" );
		return -1;
	}	
	
	res = ioctl(fd, AS_CTL_SENDTONE, &tone);
	if (res)
	{
		AS_MSG_LOG(AS_LOG_WARNING,  "Unable to play tone DIAL tone with ID of '%d'" , tone);
	}	
	return res;
}

int as_tone_play_stop( int fd ) 
{
	return __as_play_tone( fd ,  AS_TONE_STOP);
}

int as_tone_play_dial( int fd ) 
{
	return __as_play_tone( fd ,  AS_TONE_DIALTONE);
}

int as_tone_play_busy( int  fd ) 
{
	return __as_play_tone( fd ,  AS_TONE_BUSY);
}

int as_tone_play_ringback( int  fd ) 
{
	return __as_play_tone( fd,  AS_TONE_RINGTONE);
}

int as_tone_play_congestion( int  fd ) 
{
	return __as_play_tone( fd ,  AS_TONE_CONGESTION);
}

int as_tone_play_callwait( int  fd ) 
{
	return __as_play_tone( fd,  AS_TONE_CALLWAIT );
}

int as_tone_play_dialrecall( int  fd ) 
{
	return __as_play_tone( fd,  AS_TONE_DIALRECALL);
}

int as_tone_play_record(  int  fd  ) 
{
	return __as_play_tone( fd,  AS_TONE_RECORDTONE);
}

int as_tone_play_info(  int  fd  ) 
{
	return __as_play_tone(fd,  AS_TONE_INFO);
}

int as_tone_play_custom_1(  int  fd ) 
{
	return __as_play_tone( fd,  AS_TONE_CUST1);
}

int as_tone_play_custom_2( int  fd ) 
{
	return __as_play_tone(fd,  AS_TONE_CUST2);
}

int as_tone_play_stutter(  int  fd ) 
{
	return __as_play_tone(fd,  AS_TONE_STUTTER);
}


int as_tone_dev_play_dial(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_DIALTONE);
}

int as_tone_dev_play_ringback(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_RINGTONE);
}

int as_tone_dev_play_busy(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_BUSY);
}

int as_tone_dev_play_fastbusy(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_CONGESTION);
}

int as_tone_dev_play_callwait(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_CALLWAIT);
}

int as_tone_dev_play_stop(as_device_t *dev)
{
	return __as_play_tone( dev->fd ,  AS_TONE_STOP);
}


