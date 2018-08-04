/*
* $Id: console.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#ifndef __CONSOLE_HXX__
#define __CONSOLE_HXX__

typedef enum 
{
	CONSOLE_MASTER	=	1,
	CONSOLE_SLAVE		=	2
}console_dev_t;

typedef enum
{
	console_audio_no	= 0,
	console_audio_greeting,
	console_audio_dial_tone,
	console_audio_parse_dtmf,	
	console_audio_ringback,
	console_audio_busy,
	console_audio_silence	
}console_audio_state_t;

#endif

