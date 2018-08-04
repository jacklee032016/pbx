/*
* $Id: pbxMacros.h,v 1.12 2007/09/12 20:21:11 lizhijie Exp $
*/
#ifndef	__PBX_MACROS_H__
#define	__PBX_MACROS_H__

/* options for compiler */
#define	WITH_NVRAM_DEBUG						0
#define	WITH_UART_DEBUG						1

#define	WITH_OPTIMIZE_SIZE					1
#define	WITH_IP_VOICE_CHANNEL				1

#define	WITH_PSTN_HW_RINGER_DETECT			1
#define	WITH_FSK_MT88E39						1


/* SM8223AP can be Interupt external INT0 with falling edge or low level, default is low level */
#define	SM8223AP_FALLING_EDGE_SAMPLE		0

#if WITH_UART_DEBUG
#define	WITH_UART_DEBUG_DTMF				1
#define	WITH_UART_DEBUG_EVENT				0
#define	WITH_UART_DEBUG_STATE				0
#define	WITH_UART_DEBUG_ARRAY				0
#define	WITH_UART_DEBUG_SELFTEST			0
#define	WITH_UART_DEBUG_STSTIME				0
#define	WITH_UART_DEBUG_PSTN_RINGER		0
#define	WITH_UART_DEBUG_CAR					0
#define	WITH_UART_DEBUG_PSTN_FLOW			0
#define	WITH_UART_DEBUG_PSTN_FSK_DATA		0	
#define	WITH_UART_DEBUG_PSTN_BUSY_DETECT	0
#else
#define	WITH_UART_DEBUG_DTMF				0
#define	WITH_UART_DEBUG_EVENT				0
#define	WITH_UART_DEBUG_STATE				0
#define	WITH_UART_DEBUG_ARRAY				0
#define	WITH_UART_DEBUG_SELFTEST			0
#define	WITH_UART_DEBUG_STSTIME				0
#define	WITH_UART_DEBUG_PSTN_RINGER		0
#define	WITH_UART_DEBUG_CAR					0
#define	WITH_UART_DEBUG_PSTN_FLOW			0
#define	WITH_UART_DEBUG_PSTN_FSK_DATA		0	
#define	WITH_UART_DEBUG_PSTN_BUSY_DETECT	0
#endif

#define		DURATION_COUNT_RINGBACK			6		/* in unit of RINGBACK, eg. in unit of 10seconds */

#define		EVENT_FLASH_TIME					14*PBX_50MS	/* 700 ms */


#define		RECORD_TIMER_LENGTH				100*PBX_100MS
#define		DIALING_TIMER_LENGTH				200*PBX_100MS	/* timeout length of dialing, about 20 s */
#define		BUSY_TIMER_LENGTH					200*PBX_100MS	/* */
#define		RINGBACK_TIMER_LENGTH			100*PBX_500MS	/* is also used as RINGER timer */

#define		CAR_PENDING_TIMER_LENGTH		20*PBX_100MS	/* CAR wait primary Answer Signal */
#define		CAR_TIMER_LENGTH					60*PBX_100MS	/* CAR wait primary Answer Signal */

#define		TIMEOUT_NEVER						0


#define		UART_BUFFER_COUNT				2
#define		UART_TX_BUFFER_COUNT				2	/* tx will need more buffer than rx */
#define		UART_BUFFER_SIZE					16


#define		EXT_TRANSDER_EXT_IS_NULL			0xFF

#define		PHONE_ERROR						PBX_VALUE_INVALIDATE
#define		PHONE_INCOMPLATE					0xFE
#define		PHONE_OK							0xFC
#define		PHONE_PSTN						0xFB
#define		PHONE_IP							0xFA
#define		PHONE_PBX_PROGRAM				0xF9
#define		PHONE_SPECIAL_FIRST				0xF8



#define		ACCOUNT_FREE_BIT					0x80
#define		ACCOUNT_ONHOOK_BIT				0x08

#define		PBX_ACCOUNT_NUMBER				2
#define		PBX_SPECIAL_SERVICE_COUNT		6


/* CTS is p1.4 */
#define		UART_CTS_ENABLE					0
#define		UART_CTS_DISABLE					1

#define		PHONE_DIGIT_BUFFER_SIZE			9
#define		PHONE_NUMBER_BUF_SIZE			2


#define	NO_DISTURB( extIndex )	\
		(GET_BIT(extConfigs.noDisturbFlags, extIndex) )


#define	PBX_SET_FLAGS(flags, bitPosition)	\
		flags |= SET_BIT(0x01, (bitPosition) ) 

#define	PBX_CLEAR_FLAGS(flags, bitPosition)	\
		flags &= ~(SET_BIT(0x01, (bitPosition) ) )	

#define	PBX_CHECK_FLAGS(flags, bitPosition)	\
		( (flags&SET_BIT(0x01,(bitPosition) ) )!=0 )

#define	PBX_GET_LOW_NIBBLE(data)	\
		( (data)&0x0F)

#define	PBX_GET_HIGH_NIBBLE(data)	\
		( ((data)>>4)&0x0F)


#define	IS_ROW_DEVICE(devIndex )	\
		( ((devIndex)>= 0 ) && ((devIndex) < PBX_ROW_DEVICE_COUNT) )

#define	IS_EXT_DEVICE(devIndex )	\
		( ((devIndex)>= 0 ) && ((devIndex) < PBX_FXS_COUNT) )

#define	IS_PSTN_LINE_DEVICE(devIndex )	\
		( ((devIndex) >= PBX_FXS_COUNT) &&((devIndex)<PBX_ROW_DEVICE_COUNT) )

#define	IS_IP_DEVICE(devIndex )	\
		( ((devIndex) >= PBX_ROW_DEVICE_COUNT)&&((devIndex)<PBX_DEVICE_COUNT) )

#define	IS_CALL_DEVICE(devIndex) 	\
		( (devIndex>=0 )&&(devIndex <PBX_DEVICE_COUNT ))
		
/* channel Index is from 0 to 3 which is used in IP side */
#define	GET_IP_DEVICE_INDEX(channelIndex)		\
		((channelIndex)+PBX_ROW_DEVICE_COUNT)

/* ipIndex is 12~15 which is used in STC side, channel index is 0~3 which used in IP side */
#define	GET_IP_CHANNEL_INDEX(ipIndex)		\
		((ipIndex)-PBX_ROW_DEVICE_COUNT)

/* change device index used in callDevices into physical device index used in IP side before tx call signal */
#define	GET_PHY_DEVICE_ID(extLineIndex)	\
		(( (extLineIndex)< PBX_FXS_COUNT)?(extLineIndex):((extLineIndex)-PBX_FXS_COUNT+0x80) )

#define	GET_PSTN_NUMBER(devIndex)	\
		((devIndex) - PBX_FXS_COUNT)


#define	GET_DECODE_CHANNEL_INDEX(arrayIndex)	\
		((arrayIndex) + CHANNEL_COL_DTMF_DECODE_1)


#define	IS_UART_NOT_SENDABLE()	\
		(uart.responseInIndex==PBX_VALUE_INVALIDATE)


#define	PBX_COLUMN_FREE(x)	\
		PBX_SET_FLAGS(pbxHwStatus.columnFrees, (x))

#define	PBX_COLUMN_FREE_IP(x)	\
		PBX_COLUMN_FREE((x)-PBX_ROW_DEVICE_COUNT)

#define	PBX_COLUMN_BUSY(x)	\
		PBX_CLEAR_FLAGS(pbxHwStatus.columnFrees, (x))

#define	PBX_ROW_BUSY(x)	\
		PBX_CLEAR_FLAGS(pbxHwStatus.rowFrees, (x))

#define	PBX_ROW_FREE(x)	\
		PBX_SET_FLAGS(pbxHwStatus.rowFrees, (x) )

#define	IS_ODD(value)	\
	( ((value)&0x01) != 0 )

#if 0
#define	pbx_x_str_copy(dest, src, length )	\
do{	INT8U xdata i;					\
	for(i= 0; i< length; i++) {		\
		*(dest + i) = *(src+i);	}	\
}while(0)
#endif

#if WITH_OPTIMIZE_SIZE
#else
#define	uart_tx_int_value(cmd, value1)	\
do{	uartTxCmds[uart.responseInIndex].value[0] = (value1);	\
	uart_tx_cmd((cmd), 1);						\
}while(0)


#define	uart_tx_2_int_value(cmd, value1, value2)	\
do{	uartTxCmds[uart.responseInIndex].value[0] = (value1);	\
	uartTxCmds[uart.responseInIndex].value[1] = (value2);	\
	uart_tx_cmd((cmd), 2);	\
}while(0)
#endif

#define	DEVICE_EVENT_EMIT(devIndex, event)	\
	pbx_dev_handle(devIndex, event)
/*	
	callDevices[(devIndex)].devEvent = (event)
*/

#if WITH_UART_DEBUG_EVENT
	#define	CALL_EVENT_EMIT(devIndex, event)	\
 		do{uart_debug_2_int_value(UART_DEBUG_EVENT, devIndex, event);	\
 		callDevices[(devIndex)].currentEvent = (event);}while(0)
#else
	#define	CALL_EVENT_EMIT(devIndex, event)	\
 		callDevices[(devIndex)].currentEvent = (event)
#endif

#define	CALL_PEER_EVENT(devIndex, event)	\
	CALL_EVENT_EMIT(CALL_MY_PEER(devIndex), event)

#define	CALL_FLASH_EVENT(devIndex, event)	\
	CALL_EVENT_EMIT(CALL_MY_FLASH(devIndex), event)


#define	CALL_TIMER_START(devIndex, duration)		\
	do{	callDevices[(devIndex)].timeout = (duration);	\
		callDevices[(devIndex)].timeoutCounter = 0;	\
		}while(0)

#define	CALL_TIMER_RESTART(devIndex, duration)		\
	do{	callDevices[(devIndex)].timeout = duration;	\
		callDevices[(devIndex)].timeoutCounter++;	\
		}while(0)


#define	CALL_TIMER_CANCEL(devIndex)		\
	do{	callDevices[(devIndex)].timeout = 0;	\
		callDevices[(devIndex)].timeoutCounter = 0;	\
		}while(0)

#define	CALL_MY_PEER(devIndex)	(callDevices[devIndex].peerId)
#define	CALL_MY_FLASH(devIndex)	(callDevices[devIndex].flashPeerId)

#define	CALL_CHECK_STATUS(devIndex, state)		\
		(callDevices[devIndex].status == state)

#define	EXT_FLASH_TIMER_IS_ON(extIndex ) \
	(extDevices.hookTimer[extIndex] >0 )

#define	EXT_FLASH_TIMER_SETUP(extIndex) \
	do{extDevices.hookTimer[extIndex] = EVENT_FLASH_TIME;	\
		}while(0)


/*/		extDevices.status[extIndex] = EXT_STATE_ONHOOK_PENDING; \*/

#define	EXT_FLASH_TIMER_CANCEL(extIndex) \
	(extDevices.hookTimer[extIndex] = 0)


#define	EXT_FLASH_ENTER(devIndex ) \
	do{	callDevices[devIndex].flashPeerId = callDevices[devIndex].peerId; \
		callDevices[devIndex].peerId = PBX_VALUE_INVALIDATE;		\
		}while(0)
		
//		callDevices[callDevices[devIndex].peerId].flashPeerId = devIndex;	

#define	EXT_FLASH_CANCEL(devIndex ) \
	do{	callDevices[devIndex].peerId = callDevices[devIndex].flashPeerId ;\
		callDevices[devIndex].flashPeerId = PBX_VALUE_INVALIDATE;		\
		}while(0)

#define	EXT_FLASH_IS_OFF(devIndex)	\
	(callDevices[devIndex].flashPeerId == PBX_VALUE_INVALIDATE)


#define	EXT_FLASH_IS_PEEDING(devIndex)	\
	(callDevices[devIndex].flashPeerId == callDevices[devIndex].peerId )


#define	EXT_FLASH_IS_ON(devIndex)	\
	(callDevices[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	callDevices[devIndex].flashPeerId != callDevices[devIndex].peerId )

#define	EXT_FLASH_IS_FAIL(devIndex)	\
	(callDevices[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	callDevices[devIndex].peerId == PBX_VALUE_INVALIDATE && \
	callDevices[devIndex].flashPeerId != callDevices[devIndex].peerId)

#define	EXT_FLASH_IS_OK(devIndex)	\
	(callDevices[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	callDevices[devIndex].peerId != PBX_VALUE_INVALIDATE && \
	callDevices[devIndex].flashPeerId != callDevices[devIndex].peerId)


#define	DEVICE_IS_BUSY(devIndex)	\
	( callDevices[devIndex].status != PBX_STATE_IDLE )

#define	COPY_DIGITS( dest, src )	\
	do{	\
		pbx_x_str_copy((dest)->value, (src)->value, \
				(IS_ODD((src)->length))? ((src)->length/2+1):((src)->length/2) );	\
			(dest)->length = (src)->length;		\
	}while(0)		

#define	COPY_DIGITS_BUFFER( dest, buf, length )	\
	do{	\
		pbx_x_str_copy((dest)->value, (buf), \
				(IS_ODD((length)))? ((length)/2+1):( (length)/2) );	\
			(dest)->length = (length);		\
	}while(0)		


#define	PBX_IP_RECORD_MODE_ENTER()	\
	(flagIpRecord = TRUE)

#define	PBX_IP_RECORD_MODE_CANCEL()	\
	(flagIpRecord = FALSE)

#define	PBX_IP_RECORD_MODE_IS_ON()	\
	(flagIpRecord == TRUE)

#define	PBX_IP_RECORD_MODE_IS_OFF()	\
	(flagIpRecord == FALSE)


#define	PBX_SELFTEST_MODE_ENTER( type)	\
	( pbxHwStatus.selfTestFlags = type )

#define	PBX_SELFTEST_MODE_CANCEL()	\
	( pbxHwStatus.selfTestFlags = SELF_TEST_TYPE_NONE )

#define	PBX_SELFTEST_MODE_IS_ON()	\
	( pbxHwStatus.selfTestFlags != SELF_TEST_TYPE_NONE )

#define	PBX_SELFTEST_MODE_IS_OFF()	\
	( pbxHwStatus.selfTestFlags == SELF_TEST_TYPE_NONE )

#define	PBX_SELFTEST_FEEDBACK_OK( value)	\
	uart_tx_2_int_value(CMD_SELFTEST_REQUEST, pbxHwStatus.selfTestFlags, value )

#define	PBX_SELFTEST_FEEDBACK_FAIL( cause )	\
	uart_tx_2_int_value(CMD_SELFTEST_BUSY, pbxHwStatus.selfTestFlags, cause)


#define	PBX_PROGRAM_MODE_ENTER( cmd, extIndex)	\
	do{ pbxHwStatus.programContext.currentCmd = cmd;	\
		pbxHwStatus.programContext.extIndex = extIndex;\
		}while(0)

#define	PBX_PROGRAM_MODE_CANCEL()	\
	do{ pbxHwStatus.programContext.currentCmd = PROGRAM_CMD_NONE; \
		pbxHwStatus.programContext.extIndex = PBX_VALUE_INVALIDATE; \
		}while(0)

#define	PBX_PROGRAM_MODE_IS_ON(devIndex)	\
	( (pbxHwStatus.programContext.currentCmd != PROGRAM_CMD_NONE) && \
	(pbxHwStatus.programContext.extIndex == devIndex) )

#define	PBX_PROGRAM_MODE_IS_OFF()	\
	( pbxHwStatus.programContext.currentCmd == PROGRAM_CMD_NONE )

#define	PBX_PROGRAM_MODE_IS_GET_PHONE_NUMBER(devIndex)	\
	( (pbxHwStatus.programContext.currentCmd == PROGRAM_CMD_OP_GET_PHONENUMBER) && \
	(pbxHwStatus.programContext.extIndex == devIndex) )


/* for CallWait/CallHold */
#define	EXT_IS_HOLDER(extIndex)	\
	(IS_EXT_DEVICE(extIndex) && (extDevices.holdId[extIndex]!= PBX_VALUE_INVALIDATE) )


#define	EXT_IS_CFG_AS_HOLDER(extIndex)	\
	(IS_EXT_DEVICE(extIndex)&& (GET_BIT(extConfigs.callWaitFlags, extIndex)!= 0)  )


/* Call Conference Involved */
#define	PBX_CONFERENCE_IS_ON()	\
	( (pbxHwStatus.programContext.currentCmd == PROGRAM_CMD_OP_CALL_CONFERENCE) && \
	(pbxHwStatus.programContext.extIndex != PBX_VALUE_INVALIDATE) )


#define	IS_CONFERENCE_MASTER(devIndex)	\
	( (pbxHwStatus.programContext.currentCmd == PROGRAM_CMD_OP_CALL_CONFERENCE) && \
	(pbxHwStatus.programContext.extIndex == devIndex) )

#define	IS_CONFERENCE_MEMBER(devIndex)	\
	(GET_BIT(conferenceMember, devIndex) == 1 )

#define	CONFERENCE_HAS_NO_MEMBER(master) \
	( (conferenceMember&(~(SET_BIT(1, master))) )== 0)

#define	CONFERENCE_HAS_SOME_MEMBER(master) \
	( (conferenceMember&(~(SET_BIT(1, master))) )!= 0)

	
#define	CONFERENCE_ADD_MEMBER(devIndex)	\
	(conferenceMember = conferenceMember |SET_BIT(1, devIndex) )

#define	CONFERENCE_DEL_MEMBER(devIndex) \
	(conferenceMember = conferenceMember&(~(SET_BIT(0,devIndex) ) )  )

#define	CONFERENCE_CLEAN_MEMBER() \
	(conferenceMember = 0)

#define	CONFERENCE_VOICE_SETUP_ON(master)	\
	CONFERENCE_ADD_MEMBER(master)

#define	CONFERENCE_VOICE_IS_ON(master)	\
	IS_CONFERENCE_MEMBER(master)

#define	LOCAL_IS_JAPAN()	\
	(baseConfigs.signalType == SIGNAL_TYPE_JAPANESE )

#define	LOCAL_IS_CHINA()	\
	(baseConfigs.signalType == SIGNAL_TYPE_CHINESE )

#define	CAR_IS_SUPPORT(devIndex)	\
	(IS_EXT_DEVICE(devIndex) && LOCAL_IS_JAPAN() && \
			PBX_CHECK_FLAGS(extConfigs.isCallerIdOnFlags,devIndex))

#define	NVRAM_BIT_ID								0
#define	NVRAM_BIT_GLOBAL							1
#define	NVRAM_BIT_EXT								2
#define	NVRAM_BIT_TRANSFER_NIGHT				3
#define	NVRAM_BIT_TRANSFER_UNCONDITIONAL		4
#define	NVRAM_BIT_TRANSFER_LEFT					5

/* tag it as modified */
#define	NVRAM_MODIFY_GLOBAL()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)

#define	NVRAM_MODIFY_EXT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)

#define	NVRAM_MODIFY_NIGHT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)

#define	NVRAM_MODIFY_UNCONDITIONAL()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)

#define	NVRAM_MODIFY_LEFT()	\
	PBX_SET_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)

/* check */
#define	NVRAM_CHECK_GLOBAL()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)
	
#define	NVRAM_CHECK_EXT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)
	
#define	NVRAM_CHECK_NIGHT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)
	
#define	NVRAM_CHECK_UNCONDITIONAL()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)
	
#define	NVRAM_CHECK_LEFT()	\
	PBX_CHECK_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)

/* clear */
#define	NVRAM_CLEAR_GLOBAL()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_GLOBAL)
	
#define	NVRAM_CLEAR_EXT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_EXT)
	
#define	NVRAM_CLEAR_NIGHT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_NIGHT)
	
#define	NVRAM_CLEAR_UNCONDITIONAL()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_UNCONDITIONAL)
	
#define	NVRAM_CLEAR_LEFT()	\
	PBX_CLEAR_FLAGS(pbxHwStatus.nvRamWriteFlags, NVRAM_BIT_TRANSFER_LEFT)
	

#define	SET_GLOBAL_OPTION(flags, value, bit)	\
	do{	if ((value)==0 || (value)== PBX_VALUE_INVALIDATE ){	\
		PBX_CLEAR_FLAGS((flags), (bit));}	\
		else {	PBX_SET_FLAGS((flags), (bit)); }\
			NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_IP_PREFIX(digitValue)	\
	do{	baseConfigs.ipPrefix.length = 1;	\
		baseConfigs.ipPrefix.value[0] = digitValue;\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_PSTN_PREFIX(digitValue)	\
	do{	baseConfigs.pstnPrefix = digitValue;\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)

#define	SET_SIGNAL_TYPE(digitValue)	\
	do{	if( (digitValue != SIGNAL_TYPE_CHINESE) && (digitValue != SIGNAL_TYPE_JAPANESE) )\
		{		digitValue = SIGNAL_TYPE_JAPANESE;}\
			baseConfigs.signalType = digitValue;	\
		NVRAM_MODIFY_GLOBAL();	\
		}while(0)


#define	SET_LINE_STATUS(index, value)	\
	do{	if(index>=0 && index<PBX_FXO_COUNT)	{	\
		SET_GLOBAL_OPTION(baseConfigs.trunkStatusFlags, value, index);	}\
		}while(0)
	
/********************   UART Macros  **************************/
#define	UART_TX_CFG_BEGIN()	\
		uart_tx_cmd(CMD_CFG_PBX_IP_BEGIN,0)

#define	UART_TX_CFG_END()	\
		uart_tx_cmd(CMD_CFG_PBX_IP_END, 0)

#define	UART_TX_TRACK_MODE()	\
		uart_tx_int_value(CMD_CFG_TRACK_MODE, boolTrunkMode)

#define	UART_TX_NODIAL_ACTION()	\
		uart_tx_int_value(CMD_CFG_NODIAL_ACTION, boolNoDialAction)

#define	UART_TX_LINE_2_LINE()	\
		uart_tx_int_value(CMD_CFG_LINE2LINE, boolLine2Line)

#define	UART_TX_DIFFER_RINGING()	\
		uart_tx_int_value(CMD_CFG_DIFFER_RINGING, boolDifferRinging)

#define	UART_TX_CALLER_ID_MODE()	\
		uart_tx_int_value(CMD_CFG_CALLER_ID_MODE, boolCallIdMode )

#define	UART_TX_GROUP_CALL()	\
		uart_tx_int_value(CMD_CFG_GROUP_CALL, boolGroupCall)

#define	UART_TX_SPECIAL_SERVICE_FIRST()	\
		uart_tx_int_value(CMD_CFG_SPECIAL_FIRST, boolSpecialFirst)

/********************   PSTN Macros  **************************/
#if WITH_UART_DEBUG_PSTN_FLOW
enum
{
	PSTN_FLOW_BEGIN_RINGER_ON			=	0,
	PSTN_FLOW_BEGIN_CAR					=	1,
	PSTN_FLOW_RINGER_OFF					=	2,
	PSTN_FLOW_RINGER_ON					=	3,
	PSTN_FLOW_RINGER_TIMEOUT			=	4,
	PSTN_FLOW_BEGIN_CHINA_CALLER_ID	=	5,

	PSTN_FLOW_BEGIN_CALLER_ID			=	7,
};
#endif

#if WITH_UART_DEBUG_PSTN_FSK_DATA
enum
{
		FSK_DATA_ERR_START_BIT		=	0x0A,
		FSK_DATA_ERR_STOP_BIT		=	0X0B,
		FSK_DATA_ERR_FORMAT			=	0X0C,
		FSK_DATA_ERR_LENGTH			=	0X0D,
		FSK_DATA_RAW_DATA			=	0X0E,
};
#endif

#define	FSK_FORMAT_SDMF						0x04
#define	FSK_FORMAT_MDMF						0x80

/* month, day, hour, minute, every has 2 bytes */
#define	FSK_CHINESE_DATE_PARAM_LENGTH		8


#define	PSTN_MAX_GROUP_ID					4

#define	PSTN_IS_DISABLE(devIndex) \
		(!PBX_CHECK_FLAGS(baseConfigs.trunkStatusFlags, GET_PSTN_NUMBER(devIndex)) )

#define	PSTN_IS_ENABLE(devIndex) \
		(PBX_CHECK_FLAGS(baseConfigs.trunkStatusFlags, GET_PSTN_NUMBER(devIndex)) )

#define	PSTN_IS_ONHOOK(devIndex)	\
		(pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] == PSTN_HW_STATE_ONHOOK)
	
#define	PSTN_IS_OFFHOOK(devIndex)		\
		(pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] == PSTN_HW_STATE_OFFHOOK)

#define	PSTN_SET_ONHOOK(devIndex)	\
		(pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] = PSTN_HW_STATE_ONHOOK)

	
#define	PSTN_IS_CONSOLE_MODE()\
		(boolTrunkMode == TRUNK_MODE_CONSOLE)

#define	PSTN_IS_DIRECT_MODE()\
		(boolTrunkMode == TRUNK_MODE_DIRECT)

#define	PSTN_GROUP_CALL_IS_ON() \
		(boolGroupCall == TRUE)

#define	PSTN_MY_VIRTUAL_CONSOLE(devIndex)	\
		(baseConfigs.trunkVirtualConsoles[GET_PSTN_NUMBER(devIndex)] )

#define	PSTN_MY_GROUP_ID(devIndex)	\
		(baseConfigs.trunkBindingExtGroup[GET_PSTN_NUMBER(devIndex)] )


/* Direct Inward Dial */
#define	PSTN_IS_DID(devIndex)	\
		(PSTN_GROUP_CALL_IS_ON() ||(PSTN_IS_DIRECT_MODE() ) )//&& callDevices[]))

#define	PSTN_RINGER_IS_TIMEOUT(index) \
		(pstnDevices.timeout[index] <= 0)
		
#define	PSTN_HW_TIMER_START(index, timeout)	\
		(pstnDevices.timeout[index] = (timeout) )

#define	PSTN_HW_NEW_STATE(index, state, timeout)	\
	do{	pstnDevices.ringerCounter[index] = 0;	\
		pstnDevices.hwStatus[index] = state;	\
		PSTN_HW_TIMER_START(index, timeout);	\
		}while(0)

#define	PSTN_CALLID_IS_ON()	\
		(boolCallIdMode == TRUE)

#define	IS_SPECIAL_SERVICE_FIRST()		(boolSpecialFirst == TRUE)

#define	PSTN_LINE_2_LINE_IS_ON()	\
		(boolLine2Line == TRUE )

#endif

