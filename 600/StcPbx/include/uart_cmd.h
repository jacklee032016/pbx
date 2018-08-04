/*
* $Id: uart_cmd.h,v 1.23 2007/07/25 17:48:34 lizhijie Exp $
*/
#ifndef	__UART_CMD_H__
#define	__UART_CMD_H__


#define	OLD_SIGNAL				0

#define	STC_VER_MAJOR_0		'0'
#define	STC_VER_MAJOR_1		'1'
#define	STC_VER_MINOR_0		'7'
#define	STC_VER_MINOR_1		'0'

#define	PBX_MAGIC_CHAR_1					0x55
#define	PBX_MAGIC_CHAR_2					0xAA
#define	PBX_MAGIC_STRING_LENGTH			2


#define	SET_BIT(value, bit)	((value) << (bit))
#define	GET_BIT(value, bit)	(((value)>>(bit))&0x01)

#define	STC_VERSION_LENGTH				4
#define	STC_SYSTIME_LENGTH				6

#define		STC_BLACKLIST_COUNT			6

#define		MISC_BIT_TRUNK_MODE			0
#define		MISC_BIT_NODIAL				1
#define		MISC_BIT_LINE2LINE				2
#define		MISC_BIT_DIFFER_RINGING		3
#define		MISC_BIT_CALL_ID				4
#define		MISC_BIT_GROUP_CALL			5
#define		MISC_BIT_SPECIAL_FIRST		6
#define		MISC_BIT_RESERVED				7

#define		ACCOUNT_BIT_ONHOOK			6
#define		ACCOUNT_BIT_FREE				7

/* bit 7 of EXTs' group is ON/OFF flags */
#define		EXT_GROUP_FLAG_BIT			7

/***************** constants for configuration ***********************/
#define		EXT_FIRST_PSTN					0xFD
#define		EXT_FIRST_IP					0xFC

#define		TRUNK_MODE_DIRECT			1
#define		TRUNK_MODE_CONSOLE			0

#define		NODIAL_ACTION_AUTO			1
#define		NODIAL_ACTION_BROKEN			0

#define		LINE2LINE_OPEN					1
#define		LINE2LINE_FORBIDDEN			0

#define		CALLID_MODE_DTMF				1	/* PSTN CallerID is enable */
#define		CALLID_MODE_FSK				0	/* PSTN CallerID is disable, default */

#define		DIALOUT_FIRST_PSTN			0
#define		DIALOUT_FIRST_IP				1

#define		MECHANISM_CHINA				1
#define		MECHANISM_JAPAN				0

typedef	enum
{
	EXT_AUTHOR_ALL				=	1,
	EXT_AUTHOR_LONG_DISTANCE	=	2,
	EXT_AUTHOR_LOCAL				=	3,	/* local PSTN */
	EXT_AUTHOR_PSTN				=	4,	/* PSTN */
	EXT_AUTHOR_INTERNAL			=	5,	/* internal, eg, local extension */
	EXT_AUTHOR_DENY_ALL			=	6	/* Deny all call */
}PBX_AUTH_LEVEL;


#define		DIAL_DIGIT_ZERO					10			/* 0x0a, '0' */
#define		DIAL_DIGIT_ASTREIC				11			/* 0x0b, '*' */
#define		DIAL_DIGIT_SPECIAL					12			/* 0x0c, '#' */

#define	CALL_TIME_FOREVER				0

typedef	enum
{
#if 0
	EXT_TRANSFER_TYPE_LOCAL	=	0xCC,
#endif	
	EXT_TRANSFER_TYPE_IP		=	0xBB,		/* call transfer's dest is IP */
	EXT_TRANSFER_TYPE_PSTN	=	0xAA,		/* call transfer's dest is PSTN */
	EXT_TRANSFER_TYPE_UNKNOWN = 0XDD
}EXT_TRANSFER_TYPE;


typedef	enum
{
		SIGNAL_TYPE_JAPANESE				=	0,
		SIGNAL_TYPE_CHINESE				=	1,
		SIGNAL_TYPE_UNKNOWN				/* This must be the last item */
}signal_type_t;


#define		PBX_100MS							1
#define		PBX_50MS							1
#define		PBX_1_SECOND						10*PBX_100MS
#define		PBX_1_MINUTE						60*PBX_1_SECOND

#define		PBX_VALUE_INVALIDATE				0xFF


/**************** 1.  Configuration Items *****************/
/* 1.1. PBX --> IP */
#define	CMD_CFG_PBX_IP_BEGIN						0xF0
#define	CMD_CFG_PBX_IP_END						0xF2
#define	CMD_CFG_PBX_IP_VERSION					0xF4

/* 1.2. IP --> PBX */
#define	CMD_CFG_IP_PBX_GET_ALL_CFGS				0xF0
#define	CMD_CFG_IP_PBX_RESET_ISP					0xF2	/* reset STC to ISP in order to update IAP */
#define	CMD_CFG_IP_PBX_FACTORY_DEFAULT			0xF4

/* 1.3. IP <--> PBX*/
/* 1.3.1. Global Items */
#define	CMD_CFG_TRACK_MODE						0xA0
#define	CMD_CFG_NODIAL_ACTION					0xA2
#define	CMD_CFG_LINE2LINE							0xA4
#define	CMD_CFG_DIFFER_RINGING					0xA6
#define	CMD_CFG_CALLER_ID_MODE					0xA8
#define	CMD_CFG_GROUP_CALL						0xAA
#define	CMD_CFG_SPECIAL_FIRST					0xAC

#define	CMD_CFG_PSTN_PREFIX						0xB0
#define	CMD_CFG_IP_PREFIX							0xB2
#define	CMD_CFG_NIGHT_SERVICE_TIME				0xB4
#define	CMD_CFG_SYS_PASSWD						0xB6
#define	CMD_CFG_LINE2LINE_PASSWD				0xB8

#define	CMD_CFG_SYS_DATE							0xBA
#define	CMD_CFG_SIGNAL_TYPE						0xBC

/* Indexed Items */
#define	CMD_CFG_PSTN_FLAG						0xC0		/* Open/Close, for every PSTN Line*/
#define	CMD_CFG_PSTN_GROUP						0xC2
#define	CMD_CFG_PSTN_CONSOLE						0xC4

#define	CMD_CFG_SPECIAL_SERVICE					0xC6

#define	CMD_CFG_ACCOUNT_PASSWD					0xC8		/* passwd(phone digits) are rx/tx by this command */
#define	CMD_CFG_ACCOUNT_AUTHOR					0xCA		/* authority are rx/tx by this command */

/* EXTs' */
#define	CMD_CFG_EXT_PHONENUMER					0xD0
#define	CMD_CFG_EXT_AUTHORITY					0xD2
#define	CMD_CFG_EXT_CALLER_ID					0xD4		/* on or off */
#define	CMD_CFG_EXT_TIMEOUT						0xD6		/* time length for a call */
#define	CMD_CFG_EXT_NODISTURB					0xD8
#define	CMD_CFG_EXT_CALLWAIT						0xDA		/* ON or OFF */
#define	CMD_CFG_EXT_DELAY_2_LINE					0xDC		/* in unit of 0.1s */
#define	CMD_CFG_EXT_DIALOUT_MODE				0xDE		
#define	CMD_CFG_EXT_LEFT_TIME					0xE0
#define	CMD_CFG_EXT_GROUP						0xE2
#define	CMD_CFG_EXT_DIFFER_RINGING				0xE3

#define	CMD_CFG_EXT_BUSY_EXT						0xE4
#define	CMD_CFG_EXT_SECRECT_EXT					0xE6
#define	CMD_CFG_EXT_TRANSFER_LEFT				0xE8
#define	CMD_CFG_EXT_TRANSFER_NIGHT				0xEA
#define	CMD_CFG_EXT_TRANSFER_DENY				0xEC
#define	CMD_CFG_EXT_TRANSFER_UNCONDITIONAL	0xEE

#define	CMD_UART_DEBUG							0xFA

/****************  Call Command/Response Items *****************/

/**************** 2.  PBX--> IP *****************/
/* 1.2. Call Request/Response */
/* as callee */
#define	CMD_CALL_PBX_IP_OFFHOOK					0x85	/* an Extension offhooked, both channel Index and Phone Index is send  */
#define	CMD_CALL_PBX_IP_CHANNEL_INDEX			0x86	/* PBX feedback Channel Index, IP send ringback tone */
#define	CMD_CALL_PBX_IP_I_AM_BUSY				0x87	/* PBX busy, IP send busy tone */
/* as caller */
#define	CMD_CALL_PBX_IP_DIALING					0x91	/* PBX want to call IP and send Channel Index */
#define	CMD_CALL_PBX_IP_PHONENUMBER				0x92	/* Callee Phone number */
/* both caller and callee */
#define	CMD_CALL_PBX_IP_ONHOOK					0x93	/* an Extension has onhooked */



#if UART_DEBUG
#define	UART_DEBUG_DTMF							0x00
#define	UART_DEBUG_EVENT							0x01
#define	UART_DEBUG_ROW_RESOURCE				0x02
#define	UART_DEBUG_COL_RESOURCE					0x03
#define	UART_DEBUG_SELFTEST						0x04
#define	UART_DEBUG_STSTIME						0x05
#endif

/**************** 3.  IP --> PBX *****************/

/* IP <--> PBX */
/* caller --> Callee */
#define	CMD_CALL_DIALING							0X20	/* begin a new call from both side */
#define	CMD_CALL_PHONE_DIGITS					0X22	/* send out phone number of callee */
#define	CMD_CALL_CALLERID_DIGITS					0X24	/* send out phone number of caller in order to CALLER Display */
#define	CMD_CALL_CANCEL							0x26	/* Caller Notify Callee this call is canceled by user */
/* callee --> caller */
#define	CMD_CALL_BUSY_HERE						0x28
#define	CMD_CALL_TRYING							0X2A	/* Callee notify Caller to play ringback tone */
#define	CMD_CALL_OFFHOOK							0X2C	/* callee offhook */
/* end a call as RX/TX */
#define	CMD_CALL_ONHOOK							0X2E	/* want to end this call */

/* PBX --> IP */
#define	CMD_CALL_CHANNEL_INDEX					0X30	/* STC feedback Channel ID to IP when IP is play as caller */

/* IP --> PBX */
#define	CMD_CALL_RESET_ALL						0X40	/* IP notify PBX reset all call when IP is startup */
/*
* STC PBX need process total 9 signals in any direction
*/

/**************** 5.  SelfTest Items *****************/
typedef	enum
{
	SELF_TEST_TYPE_NONE	=	0,
	SELF_TEST_TYPE_RTC	=	1,					/* RTC chip */
	SELF_TEST_TYPE_NVRAM,							/* EEPROM */
	SELF_TEST_TYPE_RINGER	,						/* ringing on one or all phone */
	SELF_TEST_TYPE_ARRAY,							/* Switch array, DTMF encoder and DTMF decoder chip */
	SELF_TEST_TYPE_CALLERID,						/* Chip for PSTN Line caller ID decoding */
	SELF_TEST_TYPE_PCM_CHANNEL,					/* decode digit send on PCM channel by IP */
}SELFTEST_TYPE;

#define	SELF_TEST_MAX_NUMBER						SELF_TEST_TYPE_PCM_CHANNEL

typedef	enum
{
	SELFTEST_CAUSE_NOCHIP		=	1,
	SELFTEST_CAUSE_BUSY			=	2,
	SELFTEST_CAUSE_ERROR_DIGIT	=	3,
	SELFTEST_CAUSE_TIMEOUT		=	4,
	SELFTEST_CAUSE_INVALIDATE_PARAM	=	5,	
	SELFTEST_CAUSE_UNKNOWN	
}SELFTEST_CAUSE;

#define	SELFTEST_TIMER_LENGTH					100*PBX_100MS

#define	CMD_SELFTEST_REQUEST						0X50	/* IP-->PBX request of self test */
#define	CMD_SELFTEST_RESULT						0x50	/* PBX-->IP response OK */
#define	CMD_SELFTEST_BUSY							0x52	/* PBX-->IP response fail */


/**************** 4.  Record Items *****************/
#define	CMD_RECORD_REQUEST						0X60	/* IP-->PBX request voice record */
#define	CMD_RECORD_RESPONSE						CMD_RECORD_REQUEST	/* PBX freedback Channel Index */
#define	CMD_RECORD_END							0X62	/* IP-->PBX voice record ended now */


#endif

