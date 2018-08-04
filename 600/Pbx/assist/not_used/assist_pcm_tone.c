/*
 * $Log: assist_pcm_tone.c,v $
 * Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
 * AS600 Pbx
 *
 * Revision 1.1.1.1  2006/07/12 16:16:47  lizhijie
 * rebuild 
 *
 * Revision 1.2  2005/08/04 09:49:12  lijie
 * no message
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
 * $Id: assist_pcm_tone.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "assist_globals.h"
#include "pbx_globals.h"

static int __pcm_play_tone(int devId, int tone)
{
	switch(tone)
	{
		case AS_TONE_DIALTONE:
//			pcm_start_tone(devId);
			return 0;
		case AS_TONE_BUSY:
			pcm_ip_busy(devId);
			return 0;
		case AS_TONE_RINGTONE:
			pcm_ip_idle(devId);
			return 0;
		default:
			return 0;
	}
}

int pcm_tone_play_ringbackstop(int devId)
{
	return 0;
}

int pcm_tone_play_stop( int fd ) 
{
	return __pcm_play_tone( fd ,  AS_TONE_STOP);
}

int pcm_tone_play_dial( int fd ) 
{
	return __pcm_play_tone( fd ,  AS_TONE_DIALTONE);
}

int pcm_tone_play_busy( int  fd ) 
{
	return __pcm_play_tone( fd ,  AS_TONE_BUSY);
}

int pcm_tone_play_ringback( int  fd ) 
{
	return __pcm_play_tone( fd,  AS_TONE_RINGTONE);
}

int pcm_tone_play_congestion( int  fd ) 
{
	return __pcm_play_tone( fd ,  AS_TONE_CONGESTION);
}

int pcm_tone_play_callwait( int  fd ) 
{
	return __pcm_play_tone( fd,  AS_TONE_CALLWAIT );
}

int pcm_tone_play_dialrecall( int  fd ) 
{
	return __pcm_play_tone( fd,  AS_TONE_DIALRECALL);
}

int pcm_tone_play_record(  int  fd  ) 
{
	return __pcm_play_tone( fd,  AS_TONE_RECORDTONE);
}

int pcm_tone_play_info(  int  fd  ) 
{
	return __pcm_play_tone(fd,  AS_TONE_INFO);
}

int pcm_tone_play_custom_1(  int  fd ) 
{
	return __pcm_play_tone( fd,  AS_TONE_CUST1);
}

int pcm_tone_play_custom_2( int  fd ) 
{
	return __pcm_play_tone(fd,  AS_TONE_CUST2);
}

int pcm_tone_play_stutter(  int  fd ) 
{
	return __pcm_play_tone(fd,  AS_TONE_STUTTER);
}


int pcm_tone_dev_dial_play(as_device_t *dev)
{
	printf("pcm tone dev play dial\r\n");
	printf("No Operation is needed when prompt PBX send out dial tone\r\n");
	return 0;
	//return __pcm_play_tone( dev->phyId ,  AS_TONE_DIALTONE);
	
}

int pcm_tone_dev_dial_stop(as_device_t *dev)
{
	printf("pcm tone dev stop dial\r\n");
	printf("No Operation is needed when prompt PBX to stop dial tone\r\n");
	return 0;
	//return __pcm_play_tone( dev->phyId ,  AS_TONE_DIALTONE);
	
}

int pcm_tone_dev_ringback_play(as_device_t *dev)
{
	printf("Note PBX to play ringback\r\n");
	return __pcm_play_tone( dev->phyId ,  AS_TONE_RINGTONE);
}

int pcm_tone_dev_ringback_stop(as_device_t *dev)
{
	printf("Note PBX to stop ringback and open voice channel\r\n");
	//return __pcm_play_tone( dev->phyId ,  AS_TONE_RINGTONE);
	return pcm_open_channel(dev->phyId);
}

int pcm_tone_dev_play_busy(as_device_t *dev)
{
	printf("pcm tone dev play busy\n");
	return __pcm_play_tone( dev->phyId ,  AS_TONE_BUSY);
}

int pcm_tone_dev_play_fastbusy(as_device_t *dev)
{
	printf("pcm tone dev play fastbusy\n");
	return __pcm_play_tone( dev->phyId ,  AS_TONE_CONGESTION);
}

int pcm_tone_dev_play_callwait(as_device_t *dev)
{
	printf("pcm tone dev play callwait\n");
	return __pcm_play_tone( dev->phyId ,  AS_TONE_CALLWAIT);
}

int pcm_tone_dev_play_stop(as_device_t *dev)
{
	printf("pcm tone dev play stop\n");
	return __pcm_play_tone( dev->phyId ,  AS_TONE_STOP);
}


