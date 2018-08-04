/*
* $Id: pbx_types.h,v 1.30 2007/07/25 17:48:34 lizhijie Exp $
* data type used in PBX program all are defined in this file
*/
#ifndef	__PBX_TYPE_H__
#define	__PBX_TYPE_H__

#define	TIMEOUT_CAR_LENGTH	5*PBX_100MS

typedef	enum
{
	PBX_EVENT_OFFHOOK	=	0,
	PBX_EVENT_ONHOOK,
	PBX_EVENT_DIGIT,
	PBX_EVENT_FLASH,

	PBX_EVENT_RINGER_ON,		/* ringer begin */
	PBX_EVENT_RINGER_OFF,		/* ringer end */
	PBX_EVENT_CAR,				/* Japan */

	PBX_EVENT_TIMEOUT,

	PBX_SIGNAL_RINGING,		/* notify PEER(callee) ringing now */
	PBX_SIGNAL_CANCEL,		/* notify PEER(callee) cancel this call by caller or peer has been onhook when in INCALL state */
	PBX_SIGNAL_OK,				/* notify PEER(caller) I has offhook, so call begin */
	PBX_SIGNAL_WAITING,
	PBX_SIGNAL_RERING,		/* ringing the phone with its phone number as caller ID */
	PBX_EVENT_UNKNOWN	=	0xFF
}pbx_event_t;

typedef	enum
{
	PBX_CFG_IF_PHONE = 0,
	PBX_CFG_IF_UART	
}PBX_CFG_IF;


typedef	struct
{
	INT8U	length;
	INT8U	value[PHONE_DIGIT_BUFFER_SIZE];
}phone_digits_t;/*10 bytes */

typedef	struct
{
	INT8U	length;
	INT8U	value[PHONE_NUMBER_BUF_SIZE];
}phone_number_t;/*3 bytes */

typedef	struct
{
	INT8U		status;
	INT8U		peerId;
	INT8U		flashPeerId;

	INT8U		phyPeerId;

	INT8U		timeout;			/* in unit is 100ms, not more 25.6 seconds can be used */
	INT8U		timeoutCounter;	/* total number of timeout event , so about 100 minutes can be delay */
	
	pbx_event_t	currentEvent;	/* newest event for this device(Extension|Lines|IP)*/
}device_status_t;

#define  CMD_VALUE_MAX   14

typedef	struct
{
	INT8U		cmd;
	INT8U		id;
	INT8U		value[CMD_VALUE_MAX];
}uart_cmd_t;

typedef	struct
{
	INT8U			owner;	/* which device use this DTMF decoding Channel */
	phone_digits_t		digits;
//	INT8U			digitIndex;
}decode_channel_t;

/* total 13 bytes */
typedef struct
{
	INT8U			destChannel;
	INT8U			encodingChannel;
	INT8U			point;

	phone_digits_t		digits;
}encode_data_t;

typedef	struct
{
	INT8U			srcChannel;
	phone_digits_t		digits;
}caller_id_t;

/* total 10 bytes */
typedef	struct
{
	INT8U				type;	/* PSTN/IP */
	phone_digits_t			digits;
}ext_transfer_digits;/* 11 bytes */


typedef	struct
{
/* bit 7 :free flags; bit6~4 of first byte is authority level; bit 3: onhook flags; bit2~bit0 bits is passwd(phone digits) of this account */
	INT8U				authority;
	phone_number_t		passwd;
}account_t;/* 4 bytes */

typedef	struct
{
	INT8U		length;				/* length of FSK packet which is the second byte parsed */
	INT8U		fskIndex;

	INT8U		bitCount;
	INT8U		currentDigits;
	
	INT8U		currentChannel;
}fsk_decoder;

typedef	struct
{
	INT8U	beginHour;
	INT8U	beginMiniute;

	INT8U	endHour;
	INT8U	endMinutes;
}night_service_time_t;

typedef	struct
{
	INT8U	pstnPrefix;
	INT8U	ipPrefix;
	INT8U	accounts[PBX_FXS_COUNT];	
}pbx_config_t;


struct	_transition_t;

struct _transition_t
{
	pbx_event_t		event;
	INT8U (code *handle)(INT8U devIndex);
//	struct _transition_t	*next;
};

typedef	struct _transition_t	transition_t;

struct	_transition_table_t
{
	INT8U				state;
	
	INT8U				size;
	transition_t	code		*eventHandlers;
	
	void	(code *enter_handle)(INT8U devIndex);
};

typedef	struct	_transition_table_t			statemachine_t;


typedef	struct
{
	INT8U					msgicString[PBX_MAGIC_STRING_LENGTH];

	INT8U					globalOptions;
	signal_type_t				signalType;

	INT8U					pstnPrefix;
	phone_number_t			ipPrefix;									/* 8 bytes */
	phone_number_t			specialServices[PBX_SPECIAL_SERVICE_COUNT];	/* 26 bytes */
	
	account_t					accounts[PBX_ACCOUNT_NUMBER];				/* 34 bytes */
	phone_number_t			passwordSystem;
	phone_number_t			passwordLine2Line;	/* 38 bytes */
	night_service_time_t		nightServiceTime;							/* 42 bytes */

	INT8U					trunkStatusFlags; 							/* 0 : PSTN line is disabled, 1 : enabled */
	INT8U					trunkBindingExtGroup[PBX_FXO_COUNT];		/* 47 bytes */
	INT8U					trunkVirtualConsoles[PBX_FXO_COUNT];		/* 51 bytes */

#if 0
	INT8U					currentTime[STC_SYSTIME_LENGTH];
#endif

	phone_digits_t				blackList[STC_BLACKLIST_COUNT];
}pbx_base_t;/* 44 bytes */


typedef	struct
{
	phone_number_t			phoneNumbers[PBX_FXS_COUNT];		/* 24 bytes */
	INT8U					callWaitFlags;			/* every bit for an EXT's callwait state */
	INT8U					noDisturbFlags;			/* every bit for an EXT */
	INT8U					isCallerIdOnFlags;		/* every bit for an EXT */
	INT8U					isDifferRingFlags;

#if 0
	INT16U	 				callLengths[PBX_FXS_COUNT];	/* unit of 0.1s */	/* 43 bytes */
#else
	INT8U	 				callLengths[PBX_FXS_COUNT];	/* unit of minutes */	/* 46 bytes */
#endif

	INT8U					busyBranchs[PBX_FXS_COUNT];
	INT8U					secretBranchs[PBX_FXS_COUNT];
	INT8U					authorities[PBX_FXS_COUNT];
	INT8U					delayOutTime[PBX_FXS_COUNT];		/* time, unit of 0.1s */
/* bit 7 : ON/OFF flag for EXTs' differ ringing; bit6~bit0 : group index for this EXT */
	INT8U					groups[PBX_FXS_COUNT];

/* 0xff : PSNT first; 0xfe : IP first; <10, Dial a digit first */
	INT8U					dialOutMethod[PBX_FXS_COUNT];
	INT8U					leftTranferTimes[PBX_FXS_COUNT];		/* unit of seconds, 102 bytes */
}pbx_ext_base_t;	/* 102 bytes */

typedef	struct
{/* this is structure is too big for C51 when copy memory area */
	ext_transfer_digits			leftTransfers[PBX_FXS_COUNT];
	ext_transfer_digits			nightTransfers[PBX_FXS_COUNT];
	ext_transfer_digits			unconditionalTransfers[PBX_FXS_COUNT];
	
#if 0
	ext_transfer_digits			denyCallInTransfers[PBX_FXS_COUNT];
#endif

}pbx_ext_transfer_t;/* 352 =4*88 bytes */


typedef	struct
{
	INT8U		busy_on_time;
	INT8U		busy_off_time;

	INT8U		ringer_on_time;
	INT8U		ringer_off_time;

	INT8U		ringback_on_timer;
	INT8U		ringback_off_timer;
}pstn_param_t;


struct	_uart_call_rx_t;

struct _uart_call_rx_t
{
	INT8U		cmd;
	void 		(code *handle)(INT8U xdata *value);
};

typedef	struct _uart_call_rx_t	uart_call_rx_t;

#include "pbx_program.h"

typedef	struct
{
	INT8U		second;
	INT8U		minute;
	INT8U		hour;
	INT8U		date;			/* 1~31 */
	INT8U		month;
	INT8U		day;			/* monday ~ sunday */
	INT8U		year;
	INT8U		protect;
}sys_time;

/* data structure stored in data memory area */
typedef	struct
{
	INT8U			Counter50ms;
	INT8U			Counter100ms;
	INT8U			Counter400ms;
	INT8U			CounterMinutes;

	INT8U			extHookStatus;

	/* current status of EXTs' relay : U25 */
	INT8U			extRelayCtrl;				/* EXTs' Relay Control Status */
	/* current Status of tone for every EXTs : U30 */
	INT8U			extToneCtrl;
	
	/* current status of PSTN Lines' relay and RST1(reset to 8816) and BELL1 : U27 */
	INT8U			pstnRelayCtrl;				/* PSNTs' Relay Control Status , HC574, U27 */
	/* current Status of PSTN fsk decoder and WDI,EPMRST,PDOWN : U31 */
	INT8U			pstnFskCtrl;

	
	INT8U			trackFreeStatus;				/* PSTN lines status */
	INT8U			dtmfDecodingStatus;			/* status for DTMF decoding channels */
	
	INT8U			ringbackStatus;				/* ringback tone channel status */
	INT8U			musicStatus;				/* music channel status */

	INT8U			nvRamWriteFlags;
	INT8U			selfTestFlags;				/* refer to SELFTEST_TYPE_XXXX */

	program_context	programContext;				/* phone programming */

	sys_time			sysTime;
}pbx_hw_status;


typedef struct
{
	INT8U			owner;
	INT8U			colChannel;

	INT8U			timeout;
	
	INT8U			point;

	phone_digits_t		digits;
}encode_channel_t;


typedef	struct
{
	INT8U			major1;
	INT8U			major2;
	
	INT8U			minor1;
	INT8U			minor2;

	INT8U			dateBuild[12];
}pbx_ver_t;



struct _pbx_selftest_handle_t
{
	INT8U		testType;/* refer to SELF_TEST_TYPE_XXX */
	BOOL 		(code *handle)(INT8U xdata *value);
};

typedef	struct _pbx_selftest_handle_t	pbx_selftest_handle_t;


struct _pbx_selftest_exit_handle_t
{
	INT8U		testType;/* refer to SELF_TEST_TYPE_XXX */
	INT8U 		(code *handle)(INT8U rowIndex, INT8U event);
};

typedef	struct _pbx_selftest_exit_handle_t	pbx_selftest_exit_handle_t;


#endif

