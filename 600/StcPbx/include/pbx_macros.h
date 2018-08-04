/*
* $Id: pbx_macros.h,v 1.31 2007/07/22 20:47:38 lizhijie Exp $
*/
#ifndef	__PBX_MACROS_H__
#define	__PBX_MACROS_H__

#define		PBX_FXS_COUNT					8
#define		PBX_FXO_COUNT					4
#define		PBX_IP_COUNT					4

#define		PBX_DEVICE_COUNT				(PBX_FXS_COUNT+PBX_FXO_COUNT+PBX_IP_COUNT)
#define		PBX_ROW_DEVICE_COUNT			(PBX_FXS_COUNT+PBX_FXO_COUNT)

#define		ENCODE_CHANNEL_NUMBER		4		/* only 2 HW encoding channel and 4 SW encoding channel */
#define		DECODE_CHANNEL_NUMBER		4		/* 8 DTMF decoder(LC7385), first 4 is connect to array, last 4 connecto EXT0~3 */

#define		SWITCH_ARRAY_WIDTH			16

#define		UART_BUFFER_COUNT			2
#define		UART_TX_BUFFER_COUNT			2	/* tx will need more buffer than rx */
#define		UART_BUFFER_SIZE				16


#define		CALL_TRANSFER_LENGTH			10	/* length of CallTransfer Phone number */
#define		EXT_PHONE_NUMBER_LENGTH		3

#define		CODING_CHANNLE_COUNT		8

#define		CODING_CHANNLE_BYTE_STATUS		0
#define		CODING_CHANNLE_BYTE_LENGTH		1	/* length of phone number just decoded */
#define		CODING_CHANNLE_BYTE_NUBMER		2	/* decoded phone numbers */


#define		EXT_TRANSDER_EXT_IS_NULL			0xFF

#define		PHONE_ERROR						PBX_VALUE_INVALIDATE
#define		PHONE_INCOMPLATE					0xFE
#define		PHONE_OK							0xFC
#define		PHONE_PSTN						0xFB
#define		PHONE_IP							0xFA
#define		PHONE_PBX_PROGRAM				0xF9

/* Array Columns defination */
/* raw0~row7 is for 8 Extensions; row9~row11 is for 4 PSTN Lines */
#define		CHANNEL_ROW_DTMF_ENCODE_1		12
#define		CHANNEL_ROW_DTMF_ENCODE_2		13
#define		CHANNEL_ROW_IP_R_1				14		/* used to play tone or music on IP channel */
#define		CHANNEL_ROW_IP_R_2				15		/* used to decode phone number on IP channel */

#define		CHANNEL_COL_OFFSET				0
/* Array Columns defination */
#define		CHANNEL_COL_DTMF_DECODE_1		(8+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_2		(9+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_3		(10+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_DTMF_DECODE_4		(11+CHANNEL_COL_OFFSET)

#define		CHANNEL_COL_RINGBACK_TONE		(12+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_MUSIC				(13+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_RECORD				(14+CHANNEL_COL_OFFSET)
#define		CHANNEL_COL_TONE					(15+CHANNEL_COL_OFFSET)


#define		ACCOUNT_FREE_BIT					0x80
#define		ACCOUNT_ONHOOK_BIT				0x08

#define		PBX_ACCOUNT_NUMBER				2
#define		PBX_SPECIAL_SERVICE_COUNT		6

/* CTS is p1.4 */
#define		UART_CTS_ENABLE					0
#define		UART_CTS_DISABLE					1

#define	WRITE_GLOBAL_MISC(result, old)	\
	do{	\
	if(wr24c08_single(sign2,ROM_ADDRESS_GLOBAL_MISC)) \
		result  = 1; \
	else	{ \
		sign2 = old; \
		result = 0;}	\
		}while(0)


#define	NO_DISTURB( extIndex )	\
	(GET_BIT(extConfigs.noDisturbFlags, extIndex) )

#define	PBX_READ_SYS_TIME( timeValue)	\
		TIME_BYTES_READ(0xbf, (timeValue), 7 )

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

#define	PHONE_DIGIT_BUFFER_SIZE			9
#define	PHONE_NUMBER_BUF_SIZE			2


#define	IS_ROW_DEVICE(devIndex )	\
		( ((devIndex)>= 0 ) && ((devIndex) < PBX_ROW_DEVICE_COUNT) )

#define	IS_EXT_DEVICE(devIndex )	\
		( ((devIndex)>= 0 ) && ((devIndex) < PBX_FXS_COUNT) )

#define	IS_PSTN_LINE_DEVICE(devIndex )	\
		( ((devIndex) >= PBX_FXS_COUNT) &&((devIndex)<STATE_IP_OFFSET) )

#define	IS_IP_DEVICE(devIndex )	\
		( ((devIndex) >= STATE_IP_OFFSET)&&((devIndex)<PBX_DEVICE_COUNT) )
		
/* channel Index is from 0 to 3 which is used in IP side */
#define	GET_IP_DEVICE_INDEX(channelIndex)		\
		((channelIndex)+STATE_IP_OFFSET)

/* ipIndex is 12~15 which is used in STC side, channel index is 0~3 which used in IP side */
#define	GET_IP_CHANNEL_INDEX(ipIndex)		\
		((ipIndex)-STATE_IP_OFFSET)

/* change device index used in deviceStatus into physical device index used in IP side before tx call signal */
#define	GET_PHY_DEVICE_ID(extLineIndex)	\
		(( (extLineIndex)< PBX_FXS_COUNT)?(extLineIndex):((extLineIndex)-PBX_FXS_COUNT+0x80) )

#define	GET_PSTN_NUMBER(devIndex)	\
		((devIndex) - PBX_FXS_COUNT)


#define	GET_DECODE_CHANNEL_INDEX(arrayIndex)	\
		((arrayIndex) + CHANNEL_COL_DTMF_DECODE_1)


#define	IS_UART_NOT_SENDABLE()	\
		(uartTxInIndex==PBX_VALUE_INVALIDATE)


#define	PBX_COLUMN_FREE(x)	\
		PBX_SET_FLAGS(columnFrees, (x))

#define	PBX_COLUMN_FREE_IP(x)	\
		PBX_COLUMN_FREE((x)-STATE_IP_OFFSET)

#define	PBX_COLUMN_BUSY(x)	\
		PBX_CLEAR_FLAGS(columnFrees, (x))

#define	PBX_ROW_BUSY(x)	\
		PBX_CLEAR_FLAGS(rowFrees, (x))

#define	PBX_ROW_FREE(x)	\
		PBX_SET_FLAGS(rowFrees, (x) )

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
do{	uartTxCmds[uartTxInIndex].value[0] = (value1);	\
	uart_tx_cmd((cmd), 1);						\
}while(0)


#define	uart_tx_2_int_value(cmd, value1, value2)	\
do{	uartTxCmds[uartTxInIndex].value[0] = (value1);	\
	uartTxCmds[uartTxInIndex].value[1] = (value2);	\
	uart_tx_cmd((cmd), 2);	\
}while(0)
#endif



#define	START_CGY(x)	\
do{	START_DIAL_TONE(x);	\
	deviceStatus[x].timeout =cgy_space;	\
	deviceStatus[x].timeoutCounter =0;	\
}while(0)


#define	PBX_CONNECT(row, col)	\
	do{if(deviceStatus[row].phyPeerId!= PBX_VALUE_INVALIDATE)	\
		PBX_DISCONNECT(row, deviceStatus[row].phyPeerId);	\
		CONNECT(row, col);			\
		deviceStatus[row].phyPeerId = col;	\
		connectTables[row] = col;			\
	}while(0)
	
#define	PBX_DISCONNECT(row, col)	\
	do{DISCONNECT(row, col);			\
		deviceStatus[row].phyPeerId = PBX_VALUE_INVALIDATE;	\
		connectTables[row] = PBX_VALUE_INVALIDATE;			\
	}while(0)
	

#define	DEVICE_EVENT_EMIT(devIndex, event)	\
	deviceStatus[(devIndex)].currentEvent = (event)


#define	DEVICE_TIMER_START(devIndex, duration)		\
	do{	deviceStatus[(devIndex)].timeout = (duration);	\
		deviceStatus[(devIndex)].timeoutCounter = 0;	\
		}while(0)

#define	DEVICE_TIMER_CANCEL(devIndex)		\
	do{	deviceStatus[(devIndex)].timeout = 0;	\
		deviceStatus[(devIndex)].timeoutCounter = 0;	\
		}while(0)

#define	EXT_FLASH_TIMER_IS_ON(extIndex ) \
	((ringerTimer[extIndex] > 0) && (GET_BIT(ringerOffPending,extIndex) == 0))

#define	EXT_FLASH_TIMER_SETUP(extIndex) \
	(ringerTimer[extIndex] = EVENT_FLASH_TIME)

#define	EXT_FLASH_TIMER_CANCEL(extIndex) \
	(ringerTimer[extIndex] = 0)

#define	EXT_RINGER_TIMER_IS_ON(extIndex) \
	( (GET_BIT(ringerOffPending,extIndex) != 0))

#define	EXT_RINGER_TIMER_SETUP(extIndex, value) \
	do{ringerTimer[extIndex] = value;	\
		PBX_SET_FLAGS(ringerOffPending, extIndex);	\
		}while(0)


#define	EXT_FLASH_PEEDING(devIndex ) \
	do{	deviceStatus[devIndex].flashPeerId = deviceStatus[devIndex].peerId; \
		}while(0)

#define	EXT_FLASH_ENTER(devIndex ) \
	do{	deviceStatus[devIndex].flashPeerId = deviceStatus[devIndex].peerId; \
		deviceStatus[devIndex].peerId = PBX_VALUE_INVALIDATE;		\
		}while(0)
		
//		deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = devIndex;	

#define	EXT_FLASH_CANCEL(devIndex ) \
	do{	deviceStatus[devIndex].peerId = deviceStatus[devIndex].flashPeerId ;\
		deviceStatus[devIndex].flashPeerId = PBX_VALUE_INVALIDATE;		\
		}while(0)

#define	EXT_FLASH_IS_OFF(devIndex)	\
	(deviceStatus[devIndex].flashPeerId == PBX_VALUE_INVALIDATE)


#define	EXT_FLASH_IS_PEEDING(devIndex)	\
	(deviceStatus[devIndex].flashPeerId == deviceStatus[devIndex].peerId )


#define	EXT_FLASH_IS_ON(devIndex)	\
	(deviceStatus[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	deviceStatus[devIndex].flashPeerId != deviceStatus[devIndex].peerId )

#define	EXT_FLASH_IS_FAIL(devIndex)	\
	(deviceStatus[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	deviceStatus[devIndex].peerId == PBX_VALUE_INVALIDATE && \
	deviceStatus[devIndex].flashPeerId != deviceStatus[devIndex].peerId)

#define	EXT_FLASH_IS_OK(devIndex)	\
	(deviceStatus[devIndex].flashPeerId != PBX_VALUE_INVALIDATE && \
	deviceStatus[devIndex].peerId != PBX_VALUE_INVALIDATE && \
	deviceStatus[devIndex].flashPeerId != deviceStatus[devIndex].peerId)


#define	DEVICE_IS_BUSY(devIndex)	\
	( deviceStatus[devIndex].status != STATE_IDLE )

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
	(IS_EXT_DEVICE(extIndex) && (holdId[extIndex]!= PBX_VALUE_INVALIDATE) )


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



#endif

