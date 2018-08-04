/*
* $Id: pbxTypes.h,v 1.6 2007/09/13 20:31:55 lizhijie Exp $
* data type used in PBX program all are defined in this file
*/
#ifndef	__PBX_TYPE_H__
#define	__PBX_TYPE_H__

typedef bit					BOOL;
typedef unsigned char			INT8U;
typedef signed char			INT8S;
typedef unsigned int			INT16U;
typedef signed int				INT16S;
typedef unsigned long			INT32U;
typedef signed long			INT32S;
typedef float					FP32;
typedef double				FP64;

#define	TRUE				1
#define	FALSE				0

#define	ON					1
#define	OFF					0

#define	YES					1
#define	NO					0

#ifndef	NULL
#define	NULL				0
#endif

#define	TIMEOUT_CAR_LENGTH	5*PBX_50MS

typedef	enum
{
	PBX_STATE_CONTINUE	=	0,	/* not enter into new state */
/* Caller's States */
	PBX_STATE_IDLE			=	1,	/* 1 */
	PBX_STATE_DIALING		=	2,	/* 2 : rx all digits */
	PBX_STATE_RE_DIALING	=	3,	/* 3 : ReDial when dest is PSTN line or IP  */
	PBX_STATE_TRYING		=	4,	/* 4 : send out phone number, start timer and wait response */

/* Callee's States */
	PBX_STATE_CAR			=	5,
	PBX_STATE_RINGING		=	6,	/* 5 */
	PBX_STATE_BUSY		=	7,	/* 6 : error state, send out busy tone */
	PBX_STATE_IN_CALL		=	8,	/* 7 */
	PBX_STATE_WAIT		=	9,	/* 8 */

/* PBX Program by Phone*/
	PBX_STATE_PROGRAM	=	10,	/* 9 */
	PBX_STATE_PBX_RERING	=	11,	/* 11 */
}pbx_state_t;


typedef	enum
{
	PBX_EVENT_OFFHOOK	=	0,
	PBX_EVENT_ONHOOK		=	1,
	PBX_EVENT_DIGIT		=	2,
	PBX_EVENT_FLASH		=	3,

	PBX_EVENT_RINGER_ON	=	4,		/* ringer begin */
	PBX_EVENT_RINGER_OFF	=	5,		/* ringer end */
	PBX_EVENT_CAR			=	6,		/* Japan */

	PBX_EVENT_TIMEOUT		=	7,

	PBX_SIGNAL_RINGING	=	8,		/* notify PEER(callee) ringing now */
	PBX_SIGNAL_CANCEL		=	9,		/* notify PEER(callee) cancel this call by caller or peer has been onhook when in INCALL state */
	PBX_SIGNAL_OK			=	10,		/* notify PEER(caller) I has offhook, so call begin */
	PBX_SIGNAL_WAITING	=	11,
	PBX_SIGNAL_RERING		=	12,		/* ringing the phone with its phone number as caller ID */
	PBX_EVENT_UNKNOWN	=	0xFF
}call_event_t;

typedef	enum
{
	DEV_EVENT_TONE_DIALING,
	DEV_EVENT_TONE_BUSY,
	DEV_EVENT_TONE_RINGBACK,
	DEV_EVENT_TONE_CALLWAIT,
	DEV_EVENT_MUSIC,
	DEV_EVENT_RINGING,
	DEV_EVENT_CAR_RINGING,
	DEV_EVENT_OK,
	DEV_EVENT_CANCEL,
	DEV_EVENT_CALLER_DIGITS,
	DEV_EVENT_CALLEE_DIGITS,
	DEV_EVENT_TIMEOUT,			/* timeout of current action for EXT devices */
	DEV_EVENT_STOP,				/* stop current action for EXT devices */
	DEV_EVENT_UNKNOWN
}dev_event_t;

typedef	enum
{
	TONE_TYPE_DIAL 	=	0,
	TONE_TYPE_BUSY,
	TONE_TYPE_RINGBACK,
	TONE_TYPE_CALLWAIT,
	TONE_TYPE_PDT,		/* Private Dial Tone for Japan, refer to specs p.22 */
	TONE_TYPE_MAXNUM,
	TONE_TYPE_NONE,
}tone_type_t;

#define		TONE_MAX_STEPS					4
#define		TONE_MAX_COUNT					TONE_TYPE_MAXNUM

typedef	enum
{
	RINGER_TYPE_INTERNAL,
	RINGER_TYPE_EXTERNAL,
	RINGER_TYPE_CAR,
	RINGER_TYPE_MAX_NUM,
	RINGER_TYPE_NONE,
}ringer_type_t;

#define	RINGER_MAX_COUNT						RINGER_TYPE_MAX_NUM


typedef	enum
{
	PBX_CFG_IF_PHONE = 0,
	PBX_CFG_IF_UART	
}PBX_CFG_IF;


typedef	struct
{
	INT8U					status;
	INT8U					peerId;
	INT8U					flashPeerId;

	INT8U					phyPeerId;
	
	INT8U					timeout;			/* in unit is 100ms, not more 25.6 seconds can be used */
	INT8U					timeoutCounter;	/* total number of timeout event , so about 100 minutes can be delay */
	
	call_event_t				currentEvent;	/* newest event for this device(Extension|Lines|IP)*/
#if 0
	dev_event_t				devEvent;
#endif
}pbx_call_device_t;

typedef	enum
{
	EXT_STATE_IDLE,
	EXT_STATE_OFFHOOK,	
	EXT_STATE_RINGER_ON,
	EXT_STATE_RINGEROFF_PENDING,	
	EXT_STATE_RINGER_OFF,
}pbx_ext_state_t;

typedef	struct
{
	INT8U					hookStatus;
	INT8U					hookTimer[PBX_FXS_COUNT];

	/* current status of EXTs' relay : U25 */
	INT8U					relayCtrl;				/* EXTs' Relay Control Status */
	/* current Status of tone for every EXTs : U30 */
	INT8U					toneCtrl;
	
	INT8U					ringbackStatus;				/* ringback tone channel status */
	INT8U					musicStatus;				/* music channel status */

	INT8U					status[PBX_FXS_COUNT];
	INT8U					timeout[PBX_FXS_COUNT];
	INT8U					timeoutCounter[PBX_FXS_COUNT];

	INT8U					holdId[PBX_FXS_COUNT];	/* for CallWait/CallKeep for every EXT */
}pbx_ext_device_t;

typedef enum
{
	PSTN_HW_STATE_ONHOOK		=	0,		/*  */
	PSTN_HW_STATE_OFFHOOK,	
#if WITH_PSTN_HW_RINGER_DETECT
	PSTN_HW_STATE_RINGER_ON,
	PSTN_HW_STATE_RINGER_OFF,
#else
	PSTN_HW_STATE_RINGER_ON_PENDING,		/* PSTN ringer is in period of OFF */
	PSTN_HW_STATE_RINGER_OFF_PENDING,
	PSTN_HW_STATE_RINGER_OFF_DETECT,
	PSTN_HW_STATE_RINGER_BLOCKED,			/* CAR is processed now */
#endif
	PSTN_HW_STATE_ONHOOK_PENDING,			/* */
	PSTN_HW_STATE_UNKNOWN,
}pstn_hw_state_t;


typedef	struct
{
	/* current status of PSTN Lines' relay and RST1(reset to 8816) and BELL1 : U27 */
	INT8U					relayCtrl;				/* PSNTs' Relay Control Status , HC574, U27 */
	/* current Status of PSTN fsk decoder and WDI,EPMRST,PDOWN : U31 */
	INT8U					fskCtrl;

	INT8U					busyDetected;
	INT8U					cutbuf[PBX_FXO_COUNT];
	INT16U					vadTimeCount[PBX_FXO_COUNT];
	INT8U 					busyNum[PBX_FXO_COUNT];

	INT8U					hwStatus[PBX_FXO_COUNT];
	INT8U					timeout[PBX_FXO_COUNT];
#if WITH_PSTN_HW_RINGER_DETECT
	INT8U					ringerOnCounter[PBX_FXO_COUNT];
	INT8U					ringerOffCounter[PBX_FXO_COUNT];
#else
	INT8U					ringerCounter[PBX_FXO_COUNT];
#endif	
}pbx_pstn_device_t;

typedef	struct
{
	INT8U					currentCmd;
	INT8U					extIndex;
}program_context;

typedef	struct
{
	INT8U					second;
	INT8U					minute;
	INT8U					hour;
	INT8U					date;			/* 1~31 */
	INT8U					month;
	INT8U					day;			/* monday ~ sunday */
	INT8U					year;
	INT8U					protect;
}sys_time;


/* data structure stored in data memory area */
typedef	struct
{
	INT8U					Counter50ms;
	INT8U					Counter100ms;
	INT8U					Counter500ms;
	INT8U					CounterMinutes;

	INT8U					dtmfDecodingStatus;			/* status for DTMF decoding channels */
	INT8U					buzzerTimeout;				/* in unit of 50ms */
	
	/* 16 bits are used to represent 16 rows which from 0 to 15 */
	INT16U					rowFrees;
	INT8U					columnFrees;
	
	INT8U					nvRamWriteFlags;
	INT8U					selfTestFlags;				/* refer to SELFTEST_TYPE_XXXX */

	program_context			programContext;				/* phone programming */

	sys_time					sysTime;
}pbx_hw_status;


#define  CMD_VALUE_MAX   14

typedef	struct
{
	INT8U					cmd;
	INT8U					id;
	INT8U					value[CMD_VALUE_MAX];
}uart_cmd_t;

typedef	struct
{
	INT8U					ipGetConfigIndex;
	INT8U					ipGetConfigItemIndex;

	INT8U					cmdSeq;
	
	INT8U					responseInIndex;				/* used by Main Loop */
	INT8U					responseOutIndex;				/* used by ISR */
	INT8U					responseBuffersPosition;

	/* low nibble is index for ISR, high nibble is index for main loop */
	INT8U					requestInIndex;				/* used by ISR */
	INT8U					requestOutIndex;				/* used by Main Loop */
	INT8U					requestBuffersPosition;
}uart_t;

typedef	struct
{
	INT8U					length;
	INT8U					value[PHONE_DIGIT_BUFFER_SIZE];
}phone_digits_t;/*10 bytes */

typedef	struct
{
	INT8U					length;
	INT8U					value[PHONE_NUMBER_BUF_SIZE];
}phone_number_t;/*3 bytes */


typedef	struct
{
	INT8U					owner;	/* which device use this DTMF decoding Channel */
	phone_digits_t				digits;
//	INT8U					digitIndex;
}decode_channel_t;


/* total 10 bytes */
typedef	struct
{
	INT8U					type;	/* PSTN/IP */
	phone_digits_t				digits;
}ext_transfer_digits;/* 11 bytes */

typedef struct
{
	INT8U					owner;
	INT8U					colChannel;

	INT8U					timeout;
	
	INT8U					point;

	phone_digits_t				digits;
}encode_channel_t;


typedef	struct
{
	INT8U					major1;
	INT8U					major2;
	
	INT8U					minor1;
	INT8U					minor2;

	INT8U					dateBuild[12];
}pbx_ver_t;



typedef	struct
{
/* bit 7 :free flags; bit6~4 of first byte is authority level; bit 3: onhook flags; bit2~bit0 bits is passwd(phone digits) of this account */
	INT8U					authority;
	phone_number_t			passwd;
}account_t;/* 4 bytes */

typedef	struct
{
	INT8U					length;				/* length of FSK packet which is the second byte parsed */
	INT8U					fskIndex;

	INT8U					bitCount;
	INT8U					currentDigits;
	
	INT8U					currentChannel;
}fsk_decoder;

typedef	struct
{
	INT8U					beginHour;
	INT8U					beginMiniute;

	INT8U					endHour;
	INT8U					endMinutes;
}night_service_time_t;



typedef	struct
{
	INT8U					msgicString[PBX_MAGIC_STRING_LENGTH];

	INT8U					globalOptions;
	signal_type_t				signalType;

	INT8U					pstnPrefix;
	phone_number_t			ipPrefix;									/* 8 bytes */
	phone_number_t			specialServices[PBX_SPECIAL_SERVICE_COUNT];	/* 26 bytes */

	phone_number_t			consolePhoneNumber;
	INT8U					extIndexOfConsole;
	
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
#if WITH_EXT_DIFFER_RINGING	
	INT8U					isDifferRingFlags;
#endif

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
	INT8U		stepCount;
//	INT8U		loopCount;
	INT8U		steps[TONE_MAX_STEPS];
}pbx_tone;

typedef	struct
{
#if 0
	INT8U		busy_on_time;
	INT8U		busy_off_time;
	INT8U		ringback_on_timer;
	INT8U		ringback_off_timer;
#else
	INT8U		ringer_on_time;
	INT8U		ringer_off_time;
	pbx_tone		tones[TONE_MAX_COUNT]; 		/* as order defined in tone_type_t */
	pbx_tone		ringers[RINGER_MAX_COUNT];
#endif

}pstn_param_t;

typedef	struct
{
	pbx_state_t			state;
	INT8U 				(code *handle)(INT8U devIndex);
}dev_timer_handler;


struct	_transition_t;

struct _transition_t
{
	call_event_t			event;
	INT8U 				(code *handle)(INT8U devIndex);
//	struct _transition_t	*next;
};

typedef	struct _transition_t	transition_t;

struct	_transition_table_t
{
	INT8U				state;
	
	INT8U				size;
	transition_t	code		*eventHandlers;
	
	void					(code *enter_handle)(INT8U devIndex);
};

typedef	struct	_transition_table_t			statemachine_t;


struct	_uart_call_rx_t;

struct _uart_call_rx_t
{
	INT8U		cmd;
	void 		(code *handle)(INT8U xdata *value);
};

typedef	struct _uart_call_rx_t	uart_call_rx_t;



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


/******************  types for Phone Program ******************/
typedef	enum
{
	PROGRAM_CMD_NONE					=	0,

	/* following are manipulate commands */
	PROGRAM_CMD_OP_GET_PHONENUMBER,
	PROGRAM_CMD_OP_RECORD,
	PROGRAM_CMD_OP_RECORD_PLAY,

	PROGRAM_CMD_OP_FACTORY_DEFAULT,
	PROGRAM_CMD_OP_RESET,

	PROGRAM_CMD_OP_REPLACE,
	PROGRAM_CMD_OP_CALL_CONFERENCE,
	
	/* following are configuration commands */
	PROGRAM_CMD_CFG_TRACK_MODE,
	PROGRAM_CMD_CFG_NODIAL_ACTION,
	PROGRAM_CMD_CFG_LINE2LINE,
	PROGRAM_CMD_CFG_DIFFER_RING,
	PROGRAM_CMD_CFG_GROUP_CALL,
	PROGRAM_CMD_CFG_SPECIAL_FIRST,
	PROGRAM_CMD_CFG_CALLERID_MODE,
	
	PROGRAM_CMD_CFG_SIGNAL_TYPE,

	PROGRAM_CMD_CFG_IP_PREFIX,
	PROGRAM_CMD_CFG_PSTN_PREFIX,

	/* 2 integers as parameters */
	PROGRAM_CMD_CFG_LINE_STATUS,
	PROGRAM_CMD_CFG_ACCOUNT_AUTHOR,
	PROGRAM_CMD_CFG_PSTN_GROUP,
	PROGRAM_CMD_CFG_PSTN_CONSOLE,	/* an EXT as console of one PSTN line */

	PROGRAM_CMD_CFG_EXT_AUTHORITY,
	PROGRAM_CMD_CFG_EXT_CALLER_ID,
	
	PROGRAM_CMD_CFG_BUSY_EXT,
	PROGRAM_CMD_CFG_SECRET_EXT,
	PROGRAM_CMD_CFG_EXT_GROUP,
	PROGRAM_CMD_CFG_EXT_DIALOUT_MODE,

	PROGRAM_CMD_CFG_EXT_DELAY_2_LINE,
	PROGRAM_CMD_CFG_EXT_CALLWAIT,
	PROGRAM_CMD_CFG_EXT_NODISTURB,
	PROGRAM_CMD_CFG_EXT_DIFFER_RINGING,

	PROGRAM_CMD_CFG_PASSWD_LINE2LINE,
	PROGRAM_CMD_CFG_PASSWD_SYSTEM,
	PROGRAM_CMD_CFG_PASSWD_ACCOUNT,
	PROGRAM_CMD_CFG_SPECIAL_NUMBER,
	PROGRAM_CMD_CFG_EXT_PHONE_NUMBER,

	PROGRAM_CMD_CFG_EXT_LEFT_TIME,
	PROGRAM_CMD_CFG_EXT_TIMEOUT,
	
	PROGRAM_CMD_CFG_TRANSFER_LEFT,
	PROGRAM_CMD_CFG_TRANSFER_NIGHT,
	PROGRAM_CMD_CFG_TRANSFER_DENY,		/* black list */
	PROGRAM_CMD_CFG_TRANSFER_UNCONDITIONAL,
	PROGRAM_CMD_CFG_NIGHT_SERVICE_TIME,

	PROGRAM_CMD_MAX_NUMBER,
}PBX_PROGRAM_CMD;

#define	PBX_RERING_COUNT			3


typedef	struct _pbx_program_handle_t
{
	INT8U		digits;			/* 2 digits which is used to determine operation */
	INT8U		cmd;
	INT8U 		(code *handle)(INT8U extIndex,INT8U cmd, phone_digits_t xdata *dialingDigits);
}pbx_program_handle_t;

/* cancel operation is handled by every command */
typedef struct _pbx_program_exit_handle_t
{
	INT8U		cmd;
	INT8U 		(code *handle)(INT8U extIndex);
}pbx_program_exit_handle_t;


#endif

