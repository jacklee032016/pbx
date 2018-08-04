/*
* $Id: pbxUtils.c,v 1.11 2007/09/13 20:32:15 lizhijie Exp $
*/


#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

INT8U checkSpecialFirst(INT8U  xdata *phoneNumber, INT8U length)
{
	INT8U data	i, res, result = PHONE_ERROR;

	if(!IS_SPECIAL_SERVICE_FIRST())
	{
		return PHONE_ERROR;
	}

	for(i=0; i < PBX_SPECIAL_SERVICE_COUNT; i++)
	{
		res = pbx_compare_digits(baseConfigs.specialServices[i].value, phoneNumber, length);
		if(res == PHONE_OK )
		{
			if( length == baseConfigs.specialServices[i].length )
			{
				return PHONE_SPECIAL_FIRST;
			}
			else
			{
				result = PHONE_INCOMPLATE;
			}
		}
	}

	return result;
}

/* return physical index of phone(0~7), when 8 means invalidate */
INT8U pbx_find_ext_by_number(INT8U  xdata *phoneNumber, INT8U length )
{
	INT8U	data i, res, result = PHONE_ERROR;
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
			res = pbx_compare_digits(extConfigs.phoneNumbers[i].value, phoneNumber, length);
			if( res == PHONE_OK )
			{
				if( length == extConfigs.phoneNumbers[i].length )
				{
					return i;
				}
				else
				{
					result = PHONE_INCOMPLATE;
				}
			}
		}
	}

	return	result;
}

INT8U	call_dialing_check_dest(INT8U srcDev, phone_digits_t xdata *dialingDigits)
{
	INT8U	data res, dest = PHONE_ERROR;
	
	/* EXT checking */
	dest = pbx_find_ext_by_number( dialingDigits->value, dialingDigits->length );
	
	if(dest!= PHONE_ERROR)
		return dest;

	/* IP Process */
	if(IS_EXT_DEVICE(srcDev) )
	{
	//		 if( pbx_compare_digits(baseConfigs.ipPrefix.value, decodingChannels[channel].digits.value, baseConfigs.ipPrefix.length) == PHONE_OK)
		if((PBX_GET_HIGH_NIBBLE(dialingDigits->value[0])== PBX_GET_HIGH_NIBBLE(baseConfigs.ipPrefix.value[0]) ) ||
			(extConfigs.dialOutMethod[srcDev]==EXT_FIRST_IP))	
		{
			return PHONE_IP;
		}

		/* PSTN Process */
		res = PBX_GET_HIGH_NIBBLE(dialingDigits->value[0]);
		if( ( res == baseConfigs.pstnPrefix ) ||
			(IS_EXT_DEVICE(srcDev) && (EXT_FIRST_PSTN == extConfigs.dialOutMethod[srcDev]) ) )
		{
			return PHONE_PSTN;
		}
	}
	else
//	if(IS_PSTN_LINE_DEVICE(srcDev) )
	{
		res = pbx_compare_digits(baseConfigs.passwordSystem.value, dialingDigits->value, dialingDigits->length);
		if(res == PHONE_OK )
		{
			if( dialingDigits->length == baseConfigs.passwordSystem.length)
			{
				return PHONE_IP;
			}
			else
			{
				return PHONE_INCOMPLATE;
			}
		}
		
		res = pbx_compare_digits(baseConfigs.passwordLine2Line.value, dialingDigits->value, dialingDigits->length);
		if(res == PHONE_OK )
		{
			if( dialingDigits->length == baseConfigs.passwordLine2Line.length)
			{
				return PHONE_PSTN;
			}
			else
			{
				return PHONE_INCOMPLATE;
			}
		}

		if(IS_PSTN_LINE_DEVICE(srcDev) )
		{
			res = pbx_compare_digits(baseConfigs.consolePhoneNumber.value, dialingDigits->value, dialingDigits->length);
			if(res == PHONE_OK )
			{
				if( dialingDigits->length == baseConfigs.consolePhoneNumber.length)
				{
					return baseConfigs.extIndexOfConsole;
				}
				else
				{
					return PHONE_INCOMPLATE;
				}
			}
		
		}
	}
	
	return checkSpecialFirst(dialingDigits->value, dialingDigits->length);
}

ext_transfer_digits xdata *call_transfer_check(INT8U extIndex)
{
	if( (extTransfers.unconditionalTransfers[extIndex].type !=EXT_TRANSFER_TYPE_UNKNOWN)
		&& (extTransfers.unconditionalTransfers[extIndex].digits.length> 0) )
	{
		return extTransfers.unconditionalTransfers + extIndex;
	}

	if(nowIsNightServiceTime)
	{
		if( (extTransfers.nightTransfers[extIndex].type !=EXT_TRANSFER_TYPE_UNKNOWN)
			&& (extTransfers.nightTransfers[extIndex].digits.length> 0) )
		{
			return  extTransfers.nightTransfers + extIndex;
		}
	}

	return NULL;		
}


void	pbx_check_night_service_time(void)
{
	nowIsNightServiceTime = FALSE;		
	pbx_systime_reload();

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
		return PBX_STATE_BUSY;

	for(i=0; i< PBX_DEVICE_COUNT; i++)
	{
		if(IS_CONFERENCE_MEMBER(i) )
		{
			/* master is always on row of switch array */
#if 0			
			voiceChannelConnect(pbxHwStatus.programContext.extIndex, i);
#endif
			CALL_EVENT_EMIT(i, PBX_EVENT_OFFHOOK);
		}
	}

	CONFERENCE_VOICE_SETUP_ON(master);
	return PBX_STATE_IN_CALL;
}


/* no matter what operation is needed, after ONHOOK, device is always in state of IDLE */
void pbx_conference_master_onhook(INT8U masterId)
{
	INT8U data i;

	if(!PBX_CONFERENCE_IS_ON() )
		return;

	if(CALL_MY_PEER(masterId) != PBX_VALUE_INVALIDATE)
	{/* no matter what state peer is. master can be in state of DIALING, TRYING, BUSY and IN_CALL  */
		/* when in state of TRYING and IN_CALL, this peer is not in member list of call conference */
		CALL_PEER_EVENT(masterId, PBX_SIGNAL_CANCEL);
	}
	
	for(i=0; i< PBX_DEVICE_COUNT; i++)
	{
		if(i== masterId)
			continue;
		
		if(IS_CONFERENCE_MEMBER(i) )
		{
			CALL_EVENT_EMIT(i, PBX_SIGNAL_CANCEL);
			
			/* master is always on row of switch array */
			if(CALL_CHECK_STATUS(masterId, PBX_STATE_IN_CALL) )
				voiceChannelBroken(masterId, i);
		}
	}

	if(CALL_CHECK_STATUS(masterId, PBX_STATE_DIALING) )
		dtmfDecodingChannelFree(masterId);
	
	/* clear all member */	
	CONFERENCE_CLEAN_MEMBER();

	/* exit from program mode */
	PBX_PROGRAM_MODE_CANCEL();
}

void	pbx_2_beep(void)
{
	pbx_buzzer_ctrl(TRUE);
	pbx_mdelay(100);
	pbx_buzzer_ctrl(FALSE);
	pbx_mdelay(600);

	pbx_buzzer_ctrl(TRUE);
	pbx_mdelay(100);
	pbx_buzzer_ctrl(FALSE);
}

void pbxBuzzer( INT8U ms50)/* unit of 50 ms */
{
	if(pbxHwStatus.buzzerTimeout>0)
		return;

	pbxHwStatus.buzzerTimeout = ms50;
	pbx_buzzer_ctrl(TRUE);
}

void	pbx_mdelay(INT8U ms)
{
	for(ms/=5; ms>0; ms--)
		delay5ms();
}

void	pbx_ring_all(INT8U	 whichOne)
{
	extDevices.relayCtrl =  whichOne;
	EXT_CTRL_RINGING();
	pbx_mdelay(250);
	WATCHDOG_FEED();
	pbx_mdelay(250);
	
	extDevices.relayCtrl = 0;
	EXT_CTRL_RINGING();
	WATCHDOG_FEED();
}

INT8U callTransfer(INT8U dest, INT8U  xdata *calleeDigits, INT8U length)
{
	INT8U data	devIndex = PBX_VALUE_INVALIDATE;

	if(dest == PHONE_IP)
	{
		devIndex = findFreeIpChannel();
		if(devIndex != PBX_VALUE_INVALIDATE)
		{
			devIndex = GET_IP_DEVICE_INDEX(devIndex);
		}
	}
	else if(dest == PHONE_PSTN)
	{
		devIndex = pstnDeviceAllocate();
	}

	if(devIndex != PBX_VALUE_INVALIDATE )
	{
		if( calleeDigits != NULL && length != 0 )
		{
			dtmfEncodeChannelAllocate(devIndex, calleeDigits, length );
		}
		return devIndex;
	}

	return PHONE_ERROR;
}

INT8U callSpecialFirst(INT8U extIndex)
{
	INT8U data	dest;
	INT8U data	i;
	
	dest = pstnDeviceAllocate();

	if(dest != PBX_VALUE_INVALIDATE)
		return dest;
	
	for(i=PBX_FXS_COUNT; i< PBX_ROW_DEVICE_COUNT; i++)
	{
		if(PSTN_IS_ENABLE(i) && PSTN_IS_OFFHOOK(i))
		{
			pstnDeviceOnhook(i);

			CALL_EVENT_EMIT(i, PBX_EVENT_ONHOOK);
			CALL_TIMER_START(extIndex, 15*PBX_100MS);//EVENT_FLASH_TIME);

			return PHONE_INCOMPLATE;
		}
	}

	return PHONE_ERROR;
}

