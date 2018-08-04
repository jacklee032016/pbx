/*
 * $Id: as_dsp_tone_gen.c,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
*/
#include "assist_globals.h"
#include "as_user_dsp.h"
#include "as_dsp_private.h"
#include "pbx_globals.h"

int as_dsp_play_dial(as_dsp_engine *dsp,int fd)
{
	int res;
	int event;

	while(1)
	{
		res = as_dsp_arbtones(dsp, fd, 350.0, 440.0, 100, 1);
		event  = as_lib_event_get(fd);
		if(event==AS_EVENT_DTMFDIGIT)
		{
			return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
		}
	}
	return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
}

int as_dsp_play_busy(as_dsp_engine *dsp,int fd)
{
	int res;
	int i;
	int event;

	while(1)
	{
		for(i=0; i<5; i++)
		{
			res = as_dsp_arbtones(dsp, fd, 480.0,620.0, 100, 1);
			event  = as_lib_event_get(fd);
			if(event==AS_EVENT_ONHOOK)
			{
				return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
			}
		}	
		for(i=0; i<5; i++)
		{
			res = as_dsp_silence(dsp, fd,  100);
			event  = as_lib_event_get(fd);
			if(event==AS_EVENT_ONHOOK)
			{
				return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
			}
		}	

	}

	return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
}

int as_dsp_play_ringback(as_dsp_engine *dsp,int fd)
{
	int res;

	while(1)
	{
		res = as_dsp_arbtones(dsp, fd, 440.0,480.0, 2000, 1);
		res = as_dsp_silence(dsp, fd,  4000);

	}
	printf("finished\r\n");
	return(as_dsp_finish_audio(dsp, fd,0) );  /* finish writing audio */
}

