/*
 * $Log: assist_pstn_tone.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.3  2006/03/20 06:34:30  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: assist_pstn_tone.c,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "as_dev_ctl.h"

#include "assist_lib.h"

typedef enum
{
//	AS_TONE_ZONE_DEFAULT 	=-1,	/* To restore default */
	AS_TONE_STOP			= 	-1,
	AS_TONE_DIALTONE		=	0,
	AS_TONE_BUSY			=	1,
	AS_TONE_RINGTONE		=	2,
	AS_TONE_CONGESTION	=	3,
	AS_TONE_CALLWAIT		=	4,
	AS_TONE_DIALRECALL	=	5,
	AS_TONE_RECORDTONE	=	6,
	AS_TONE_INFO			=	7,
	AS_TONE_CUST1			=	8,
	AS_TONE_CUST2			=	9,
	AS_TONE_STUTTER		=	10
}as_tone_type_t;

#define AS_TONE_MAX		16

static int __as_play_tone(int fd, int tone)
{
	int res;

	if( fd <0)
	{
		as_error_msg("Uninit the dev device for dev\n" );
		return -1;
	}	
	
	res = ioctl(fd, AS_CTL_SENDTONE, &tone);
	if (res)
		as_error_msg( "Unable to play tone DIAL tone with ID of '%d'\n" , tone);
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

int as_tone_play_notify_tone( int  fd ) 
{
	return as_tone_play_custom_1( fd);
}

int as_tone_play_accept_tone( int  fd ) 
{
	return as_tone_play_custom_1( fd);
}

int as_tone_play_custom_2( int  fd ) 
{
	return __as_play_tone(fd,  AS_TONE_CUST2);
}

int as_tone_play_income_id_tone( int  fd ) 
{
	return as_tone_play_custom_2( fd);
}


int as_tone_play_stutter(  int  fd ) 
{
	return __as_play_tone(fd,  AS_TONE_STUTTER);
}


