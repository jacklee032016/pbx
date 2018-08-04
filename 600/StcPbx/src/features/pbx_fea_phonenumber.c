/*
* $Id: pbx_fea_phonenumber.c,v 1.7 2007/07/25 17:50:45 lizhijie Exp $
*/


#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#include "_pbx_state_handles.h"


/* return physical index of phone(0~7), when 8 means invalidate */
INT8U pbx_find_ext_by_number(INT8U  xdata *phoneNumber, INT8U length )
{
	INT8U	data i;
	BOOL 	isJapanMode = FALSE;
	
	if ( PBX_GET_HIGH_NIBBLE(phoneNumber[0])== DIAL_DIGIT_ASTREIC)/* char '*' is the high 4 bits in second byte */
	{
		isJapanMode = TRUE;
		
		if(PBX_GET_LOW_NIBBLE (phoneNumber[0]) == DIAL_DIGIT_ASTREIC)
			return PHONE_PBX_PROGRAM;
	}

	for(i=0;i<PBX_FXS_COUNT;i++)
	{
		if(isJapanMode)
		{/* phone number is 3 , 2007.04.22 */
			if( length== 1)
				return PHONE_INCOMPLATE;
			
			if( (phoneNumber[0] & 0x0f )==((extConfigs.phoneNumbers[i].value[1]&0xf0)>>4) )
				return i;
		}
		else
		{
			if( length== 1)
			{
				if( (extConfigs.phoneNumbers[i].value[0]>>4)== (phoneNumber[0]>>4 ) )
					return PHONE_INCOMPLATE;
			}
			else if( length== 2 )
			{
				if( extConfigs.phoneNumbers[i].value[0]== phoneNumber[0] )
					return PHONE_INCOMPLATE;
			}
			else
			{
				if( (extConfigs.phoneNumbers[i].value[1] &0xf0)== (phoneNumber[1]&0xf0) )
					return i;
			}
		}
	}

	return	PHONE_ERROR;
}

void	pbx_check_night_service_time(void)
{
	unsigned int data i,j,k;
	BOOL	flag;
	
	nowIsNightServiceTime = FALSE;		
	pbx_systime_reload();
#if UART_DEBUG
	uartTxCmds[uartTxInIndex].value[0] = UART_DEBUG_STSTIME;
	uartTxCmds[uartTxInIndex].value[1] = pbxHwStatus.sysTime.second;
	uartTxCmds[uartTxInIndex].value[2] = pbxHwStatus.sysTime.minute;
	uartTxCmds[uartTxInIndex].value[3] = pbxHwStatus.sysTime.hour;
	uartTxCmds[uartTxInIndex].value[4] = pbxHwStatus.sysTime.date;
	uartTxCmds[uartTxInIndex].value[5] = pbxHwStatus.sysTime.month;
	uartTxCmds[uartTxInIndex].value[6] = pbxHwStatus.sysTime.day;
	uartTxCmds[uartTxInIndex].value[7] = pbxHwStatus.sysTime.year;
	uart_tx_cmd(CMD_UART_DEBUG, 8);
#endif

	if((pbxHwStatus.sysTime.minute > baseConfigs.nightServiceTime.beginMiniute && 
		pbxHwStatus.sysTime.hour > baseConfigs.nightServiceTime.beginHour &&
		pbxHwStatus.sysTime.hour < baseConfigs.nightServiceTime.endHour) ||
		(pbxHwStatus.sysTime.minute < baseConfigs.nightServiceTime.endMinutes && 
		pbxHwStatus.sysTime.hour > baseConfigs.nightServiceTime.beginHour &&
		pbxHwStatus.sysTime.hour < baseConfigs.nightServiceTime.endHour) )
	{
		nowIsNightServiceTime = TRUE;
	}

}

INT8U	pbx_conference_begin(INT8U master)
{
	INT8U data i;

	if(!PBX_CONFERENCE_IS_ON() || CONFERENCE_HAS_NO_MEMBER(master) )
		return STATE_BUSY;

	for(i=0; i< PBX_DEVICE_COUNT; i++)
	{
		if(IS_CONFERENCE_MEMBER(i) )
		{
			/* master is always on row of switch array */
#if 0			
			pbx_voice_channel_connect(pbxHwStatus.programContext.extIndex, i);
#endif
			DEVICE_EVENT_EMIT(i, PBX_EVENT_OFFHOOK);
		}
	}

	CONFERENCE_VOICE_SETUP_ON(master);
	return STATE_IN_CALL;
}


/* no matter what operation is needed, after ONHOOK, device is always in state of IDLE */
void pbx_conference_master_onhook(INT8U masterId)
{
	INT8U data i;

	if(!PBX_CONFERENCE_IS_ON() )
		return;

	if(deviceStatus[masterId].peerId != PBX_VALUE_INVALIDATE)
	{/* no matter what state peer is. master can be in state of DIALING, TRYING, BUSY and IN_CALL  */
		/* when in state of TRYING and IN_CALL, this peer is not in member list of call conference */
		DEVICE_EVENT_EMIT(deviceStatus[masterId].peerId, PBX_SIGNAL_CANCEL);
	}
	
	for(i=0; i< PBX_DEVICE_COUNT; i++)
	{
		if(i== masterId)
			continue;
		
		if(IS_CONFERENCE_MEMBER(i) )
		{
			DEVICE_EVENT_EMIT(i, PBX_SIGNAL_CANCEL);
			
			/* master is always on row of switch array */
			if(deviceStatus[masterId].status == STATE_IN_CALL)
				pbx_voice_channel_broken(masterId, i);
		}
	}

	if(deviceStatus[masterId].status == STATE_DIALING)
		decodingchannel_free(masterId);
	
	/* clear all member */	
	CONFERENCE_CLEAN_MEMBER();

	/* exit from program mode */
	PBX_PROGRAM_MODE_CANCEL();
}

