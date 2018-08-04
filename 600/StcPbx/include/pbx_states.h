/*
* $Id: pbx_states.h,v 1.4 2007/06/27 07:48:07 lizhijie Exp $
*/
#ifndef	__PBX_STATE_H__
#define	__PBX_STATE_H__

#define	STATE_FXS_OFFSET			0
#define	STATE_FXO_OFFSET			8
#define	STATE_IP_OFFSET			12

#define	PBX_STATE_INVALIDATE				0xFF



#define	STATE_IP_IDLE						86
#define	STATE_IP_WAIT_PHONENUMBER		87
#define	STATE_IP_WAIT_CONNECT			88
#define	STATE_IP_SOUND_ALERT				89
#define	STATE_IP_DIAL						90
#define	STATE_IP_PLAY_RINGBACK			91
#define	STATE_IP_CONNECTED				92
#define	STATE_IP_WAIT_FLASH				93
#define	STATE_IP_FLASH_MUSIC				94
#define	STATE_IP_SEND_PHONENUMBER		95	/* send dialed phone number to IP */
#define	STATE_IP_RINGING					96
#define	STATE_IP_CONNECTED_CALLEE		97
#define	STATE_IP_MUSIC					98
#define	STATE_IP_CONNECTED_N				99

#define	STATE_IP_TRYING					102
#define	STATE_IP_SEND_ONHOOK_2			108

#define	STATE_IP_SEND_ONHOOK				112
#define	STATE_IP_PCM_SELFTEST			113
#define	STATE_IP_IP_RECORD				114


#define	STATE_EXT_DIALING					0
#define	STATE_EXT_DIALING_2				62
#define	STATE_EXT_PLAY_RINGBACK			2
#define	STATE_EXT_PLAY_BUSY				3
#define	STATE_EXT_IDLE						19

#define	STATE_CONTINUE					0	/* not enter into new state */
/* Caller's States */
#define	STATE_IDLE							1
#define	STATE_DIALING						2	/* rx all digits */
#define	STATE_RE_DIALING					3	/* ReDial when dest is PSTN line or IP  */
#define	STATE_TRYING						4	/* send out phone number, start timer and wait response */

/* Callee's States */
#define	STATE_RINGING						6
#define	STATE_BUSY							7	/* error state, send out busy tone */
#define	STATE_IN_CALL						8
#define	STATE_WAIT						9

/* PBX Program by Phone*/
#define	STATE_PBX_PROGRAM				10
#define	STATE_PBX_RERING					11

#define ADD_ELEMENT(header, element)	\
	if (header == NULL){					\
		header  = element;				\
		element->next   = NULL;			\
	}									\
	else	{								\
		element->next   = header;		\
		header = element;				\
	}

#endif

