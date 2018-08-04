/*
 * $Id: assist_pcm_dtmf.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "assist_globals.h"

/* get an event of the device */
int pcm_lib_event_get(int fd)
{
	int j =-1;
	return j;
}


/* return 0: no dtmf digit is ready ;  not 0 : a dtmf digit is ready */
int pcm_dtmf_is_ready( int fd , const char *devicename)
{
	return 0;
}


/* set device to ONHOOK status, eg. reset the device */
int pcm_setup_onhook(int fd)
{
	return 1;
}



