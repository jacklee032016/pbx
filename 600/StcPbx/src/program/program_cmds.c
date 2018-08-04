/*
* $Id: program_cmds.c,v 1.12 2007/07/25 17:55:38 lizhijie Exp $
*/
/*
* For all IP-->PBX command of SIP Call Process
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U _program_get_my_phonenumber(INT8U extIndex, INT8U cmd, phone_digits_t xdata *dialingDigits)
{/* only alert user ONHOOK but keep in PROGRAM */
	cmd = cmd;
	DEVICE_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	return STATE_CONTINUE;
}

INT8U _program_get_my_phonenumber_exit(INT8U extIndex)
{
#if 0
	if(deviceStatus[extIndex].status == STATE_BUSY)
	{
//		callerid_allocate_channel(extIndex, extConfigs.phoneNumbers[extIndex].value, extConfigs.phoneNumbers[extIndex].length );
		DEVICE_EVENT_EMIT(extIndex, PBX_SIGNAL_RINGING);
		return STATE_CONTINUE;
	}
	if(deviceStatus[extIndex].status == STATE_IDLE )
	{
		return STATE_PBX_RERING;
	}
#endif
	return STATE_CONTINUE;
	
}

INT8U 	_program_recording_start(INT8U extIndex, INT8U cmd, phone_digits_t xdata *dialingDigits)
{/* record can only be used in EXT 0  */
	cmd = cmd;
	decodingchannel_free(extIndex);
	if(extIndex == 0)
	{
#if 0	
	RECORDER_REC_STOP();
	NOP();
	NOP();
	NOP();
#endif	
		RECORDER_REC_START();
		DEVICE_TIMER_START(extIndex, RECORD_TIMER_LENGTH);
	}
	else
	{
		DEVICE_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	}

	return STATE_CONTINUE;
}

INT8U 	_program_recording_end(INT8U extIndex)//, phone_digits_t xdata *dialingDigits)
{
//	DISCONNECT(extIndex, CHANNEL_COL_RECORD);
	extIndex = extIndex;
//	STOP_RECORD();
//	RECORDER_RESET();
#if 1
//	RECORDER_PLAY_STOP();
	RECORDER_REC_STOP();
#else	
	RESET_PBX();
#endif	

	PBX_PROGRAM_MODE_CANCEL();
	return STATE_CONTINUE;
}

INT8U 	_program_playrecord_start(INT8U extIndex,INT8U cmd,  phone_digits_t xdata *dialingDigits)
{
	cmd = cmd;
	decodingchannel_free(extIndex);
	pbx_if_play_record(extIndex, TRUE);
	DEVICE_TIMER_START(extIndex, RECORD_TIMER_LENGTH);

	return STATE_CONTINUE;
}

INT8U 	_program_playrecord_end(INT8U extIndex)//, phone_digits_t xdata *dialingDigits)
{
	pbx_if_play_record(extIndex, FALSE);
	PBX_PROGRAM_MODE_CANCEL();
	return STATE_CONTINUE;
}

INT8U _program_reset_pbx(INT8U extIndex,INT8U cmd, phone_digits_t xdata *dialingDigits)
{
	if(cmd == PROGRAM_CMD_OP_CALL_CONFERENCE)
	{/* DTMF decoding Channel is kept now, so return to DIALING directly */
		dialingDigits->length = 0;
		
		CONFERENCE_CLEAN_MEMBER();
		return STATE_DIALING;
	}
	
	uart_rx_no_value(cmd);
#if 0
	CONNECT(extIndex, CHANNEL_COL_MUSIC);
	DEVICE_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	PBX_PROGRAM_MODE_CANCEL();
#else	
	programSucess(extIndex);
#endif
	return STATE_CONTINUE;
}

pbx_program_handle_t		code		enter_program_handlers[]=
{
	/* 1. global simple option */
/********** cfg itesm with dialed digits as '**1x' and gas only one parameter *********/
	{/*track mode : virtaul console or direct dial : '**10' */
		SET_BIT(1, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		CMD_CFG_TRACK_MODE,
		program_cfg_global_options,
	},
	{/*No Dial Action : '**11' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_NODIAL, 0),
		CMD_CFG_NODIAL_ACTION,
		program_cfg_global_options,
	},
	{/*Line2Line : '**12' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_LINE2LINE, 0),
		CMD_CFG_LINE2LINE,
		program_cfg_global_options,
	},
	{/*Differ Ringing : '**13' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_DIFFER_RINGING, 0),
		CMD_CFG_DIFFER_RINGING,
		program_cfg_global_options,
	},
	{/*Caller ID mode : FSK or DTMF : '**14' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_CALL_ID, 0),
		CMD_CFG_CALLER_ID_MODE,
		program_cfg_global_options,
	},
	{/*Group Call : '**15' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_GROUP_CALL, 0),
		CMD_CFG_GROUP_CALL,
		program_cfg_global_options,
	},
	{/*Special First : '**16' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_SPECIAL_FIRST, 0),
		CMD_CFG_SPECIAL_FIRST,
		program_cfg_global_options,
	},
	
	{/*Signal Type : '**17' */
		SET_BIT(1, 4)|SET_BIT(MISC_BIT_RESERVED, 0),
		CMD_CFG_SIGNAL_TYPE,
		program_cfg_global_options,
	},
	{/*IP Prefix : '**18' */
		SET_BIT(1, 4)|SET_BIT(8, 0),
		CMD_CFG_IP_PREFIX,
		program_cfg_global_options,
	},
	{/*PSTN Prefix : '**19' */
		SET_BIT(1, 4)|SET_BIT(9, 0),
		CMD_CFG_PSTN_PREFIX,
		program_cfg_global_options,
	},


	/* 2. global complex command */
	{/*System Password : '**20' */
		SET_BIT(2, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		CMD_CFG_SYS_PASSWD,	
		program_cfg_passwd,
	},
	{/*Line2Line Password : '**21' */
		SET_BIT(2, 4)|SET_BIT(1, 0),
		CMD_CFG_LINE2LINE_PASSWD,	
		program_cfg_passwd,
	},

	{/*Special Service Phone Number : '**22' */
		SET_BIT(2, 4)|SET_BIT(2, 0),
		CMD_CFG_SPECIAL_SERVICE,	
		program_cfg_index_value,
	},

	{/*Night Service Time  : '**23' */
		SET_BIT(2, 4)|SET_BIT(3, 0),
		CMD_CFG_NIGHT_SERVICE_TIME,
		program_cfg_night_service_time,
	},

	/* following are indexed value */
	{/*Account Password : '**24' */
		SET_BIT(2, 4)|SET_BIT(4, 0),
		CMD_CFG_ACCOUNT_PASSWD,	
		program_cfg_index_value,
	},
	{/*Account Authority : '**25' */
		SET_BIT(2, 4)|SET_BIT(5, 0),
		CMD_CFG_ACCOUNT_AUTHOR,	
		program_cfg_2_params,
	},

/********** cfg itesm with dialed digits as '**1x' and gas only one parameter *********/
	{/*Line Status : '**30' */
		SET_BIT(3, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		CMD_CFG_PSTN_FLAG,	
		program_cfg_2_params,
	},
	{/*PSTN CONSOLE : '**31' */
		SET_BIT(3, 4)|SET_BIT(1, 0),
		CMD_CFG_PSTN_CONSOLE,	
		program_cfg_2_params,
	},
	{/*PSTN GROUP : '**32' */
		SET_BIT(3, 4)|SET_BIT(2, 0),
		CMD_CFG_PSTN_GROUP,	
		program_cfg_2_params,
	},
	

	/* 4-5 following are EXT configurations */		
	{/*EXT phone number  : '**40' */
		SET_BIT(4, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		CMD_CFG_EXT_PHONENUMER,	
		program_cfg_index_value,
	},
	{/*EXT delay to line : '**41' */
		SET_BIT(4, 4)|SET_BIT(1, 0),
		CMD_CFG_EXT_DELAY_2_LINE,	
		program_cfg_index_time_value,
	},
	{/*EXTs CallWait : '**42' */
		SET_BIT(4, 4)|SET_BIT(2, 0),
		CMD_CFG_EXT_CALLWAIT,	
		program_cfg_2_params,
	},
	{/*EXT No Disturb  : '**43' */
		SET_BIT(4, 4)|SET_BIT(3, 0),
		CMD_CFG_EXT_NODISTURB,	
		program_cfg_2_params,
	},
	{/* EXT Differ Ringing : '**44' */
		SET_BIT(4, 4)|SET_BIT(4, 0),
		CMD_CFG_EXT_DIFFER_RINGING,	
		program_cfg_2_params,
	},
	{/*EXT CallerID : '**45' */
		SET_BIT(4, 4)|SET_BIT(5, 0),
		CMD_CFG_EXT_CALLER_ID,	
		program_cfg_2_params,
	},
	{/*EXT authority : '**46' */
		SET_BIT(4, 4)|SET_BIT(6, 0),
		CMD_CFG_EXT_AUTHORITY,	
		program_cfg_2_params,
	},
	{/*EXT Group : '**47' */
		SET_BIT(4, 4)|SET_BIT(7, 0),
		CMD_CFG_EXT_GROUP,	
		program_cfg_2_params,
	},
	{/*EXT Dial Out Mode : '**48' */
		SET_BIT(4, 4)|SET_BIT(8, 0),
		CMD_CFG_EXT_DIALOUT_MODE,	
		program_cfg_2_params,
	},
	{/*EXT timeout : '**49' */
		SET_BIT(4, 4)|SET_BIT(9, 0),
		CMD_CFG_EXT_TIMEOUT, 	
		program_cfg_index_time_value,
	},

	/* EXT Call Transfer */
	{/*Busy EXTs : '**60' */
		SET_BIT(6, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		CMD_CFG_EXT_BUSY_EXT,	
		program_cfg_2_params,
	},
	{/*Secret EXTs : '**61' */
		SET_BIT(6, 4)|SET_BIT(1, 0),
		CMD_CFG_EXT_SECRECT_EXT,	
		program_cfg_2_params,
	},


	{/*Left : '**62' */
		SET_BIT(6, 4)|SET_BIT(2, 0),
		CMD_CFG_EXT_TRANSFER_LEFT,	
		program_cfg_index_value,
	},
	{/*Left time : '**63' */
		SET_BIT(6, 4)|SET_BIT(3, 0),
		CMD_CFG_EXT_LEFT_TIME,	
		program_cfg_index_time_value,
	},

	{/*Night : '**64' */
		SET_BIT(6, 4)|SET_BIT(4, 0),
		CMD_CFG_EXT_TRANSFER_NIGHT,	
		program_cfg_index_value,
	},
	{/*Unconditional : '**65' */
		SET_BIT(6, 4)|SET_BIT(5, 0),
		CMD_CFG_EXT_TRANSFER_UNCONDITIONAL,
		program_cfg_index_value,
	},

	{/*Deny, eg. blacklist  : '**66' */
		SET_BIT(6, 4)|SET_BIT(6, 0),
		CMD_CFG_EXT_TRANSFER_DENY,
		program_cfg_index_value,
	},

	/* following are manipulate commands wjhich is begin from **80 */
	{/* record alert : '**80' */
		SET_BIT(8, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		PROGRAM_CMD_OP_RECORD,	
		_program_recording_start,
	},
	{/* play recorded alert : '**81' */
		SET_BIT(8, 4)|SET_BIT(1, 0),
		PROGRAM_CMD_OP_RECORD_PLAY,
		_program_playrecord_start,
	},

	{/* get phone number : '**90' */
		SET_BIT(9, 4)|SET_BIT(DIAL_DIGIT_ZERO, 0),
		PROGRAM_CMD_OP_GET_PHONENUMBER,	
		_program_get_my_phonenumber,
	},
		
	{/* Factory : '**91' */
		SET_BIT(9, 4)|SET_BIT(1, 0),
		CMD_CFG_IP_PBX_FACTORY_DEFAULT,
		_program_reset_pbx,
	},
	{/* reset PBX : '**92' */
		SET_BIT(9, 4)|SET_BIT(2, 0),
		CMD_CFG_IP_PBX_RESET_ISP,
		_program_reset_pbx,
	},

	{/* get the call for another EXT : '**98' */
		SET_BIT(9, 4)|SET_BIT(8, 0),
		PROGRAM_CMD_OP_REPLACE,
		program_cfg_2_params,
	},

	{/* Telephone Conference : '**99' */
		SET_BIT(9, 4)|SET_BIT(9, 0),
		PROGRAM_CMD_OP_CALL_CONFERENCE,
		_program_reset_pbx,
	},
};

pbx_program_exit_handle_t		code		exit_program_handlers[]=
{
	{/* */
		PROGRAM_CMD_OP_GET_PHONENUMBER,
		_program_get_my_phonenumber_exit,
	},
	
	{/* record alert */
		PROGRAM_CMD_OP_RECORD,
		_program_recording_end,
	},
	{/* play recorded alert */
		PROGRAM_CMD_OP_RECORD_PLAY,
		_program_playrecord_end,
	},
};

	
INT8U	pbx_program_enter_handle(INT8U extIndex, phone_digits_t xdata *dialingDigits)
{
	INT8U	data i;

#if 0
	uart_tx_int_value(CMD_UART_DEBUG, dialingDigits->value[1]);
#endif

	for(i=0; i< sizeof(enter_program_handlers)/sizeof(pbx_program_handle_t); i++)
	{
		if(dialingDigits->length == 3)
		{
			if(PBX_GET_HIGH_NIBBLE(enter_program_handlers[i].digits) == 
				PBX_GET_HIGH_NIBBLE(dialingDigits->value[1]) )
			{
				return STATE_CONTINUE;
			}
		}
		else
		{
			if(enter_program_handlers[i].digits == dialingDigits->value[1] )
			{
				PBX_PROGRAM_MODE_ENTER(enter_program_handlers[i].cmd, extIndex);
				
				return (enter_program_handlers[i].handle)(extIndex, enter_program_handlers[i].cmd, dialingDigits);
			}
		}
	}

	DEVICE_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	return STATE_CONTINUE;
}

	
INT8U	pbx_program_exit_handle(INT8U extIndex)//, phone_digits_t xdata *dialingDigits)
{
	INT8U	data i;

	if(PBX_PROGRAM_MODE_IS_OFF() || !IS_EXT_DEVICE(extIndex) )
		return STATE_CONTINUE;
	
	for(i=0; i< sizeof(exit_program_handlers)/sizeof(pbx_program_exit_handle_t); i++)
	{
		if(exit_program_handlers[i].cmd == pbxHwStatus.programContext.currentCmd )
		{
			return (exit_program_handlers[i].handle)(extIndex);
		}
	}

	if(PBX_PROGRAM_MODE_IS_ON(extIndex) )
	{/* for command which has no exit handler */
		PBX_PROGRAM_MODE_CANCEL();
	}

	DEVICE_EVENT_EMIT(extIndex, PBX_EVENT_TIMEOUT);
	
	return STATE_BUSY;
}


