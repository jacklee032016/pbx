/*
* $Id: pbx_state_dialing.c,v 1.17 2007/07/25 17:56:26 lizhijie Exp $
*/

/*
* In DIALING state, Digit, onhook and timeout events are process
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#define		EXT_TONE_IF		1

/* only when I am EXT, eg. EXT and PSTN device goto this state when IP dialing is found */
INT8U	ext_dialing_state_ringing(INT8U devIndex)
{/* EXT-->IP */
//	if(IS_IP_DEVICE(deviceStatus[devIndex].peerId) )
	ip_call_tx_caller_digits_signal(devIndex);

	decodingchannel_free(devIndex);
	
	return STATE_TRYING;
}


/* state 00|62; dialing process */
INT8U	ext_dialing_state_digits(INT8U devIndex)
{
	INT8U	data n, dest = PHONE_ERROR, channel;
	ext_transfer_digits	xdata	*transfer = NULL;

	if( (channel = decodingchannel_find(devIndex)) == PBX_VALUE_INVALIDATE)
		return STATE_BUSY;

	if(decodingChannels[channel].digits.length == 1)
	{
		if(IS_EXT_DEVICE(devIndex))
		{
			pbx_if_ringback_tone_play(devIndex, FALSE);
		}
		else if(IS_PSTN_LINE_DEVICE(devIndex))
		{
			pbx_if_play_record(devIndex, FALSE);
		}
	}
	
#if 0	
	if(flag_p1)
	{
		n=check_tefu(codingChannels[m]);		//是否特服号码
		if(n==0)
		{
			flag_p1=0;
			n=tefu_process(codingChannels[m]);	/*n=0,系统忙，n=1,处理失败,n=2处理成功*/
			if(n==0)
			{
				hasDecodedNumberFlags=hasDecodedNumberFlags|h;
				decodingchannel_broken(j);
			}
			else if(n==1)
			{
				decodingchannel_free(j);
				pbx_if_busytone_play(j);
				status[j][0]=03;
			}
			else
				decodingchannel_free(j);
		}
	}
#endif

	dest = call_dialing_check_dest(devIndex, &(decodingChannels[channel].digits));
	if(dest==PHONE_PBX_PROGRAM)
	{
		if(PBX_PROGRAM_MODE_IS_OFF() )
		{/* DTMF decoding channel is keep */
			return STATE_PBX_PROGRAM;
		}
		else
		{
			/* free DTMF decoding Channel */
			decodingchannel_free(devIndex);
			if(PBX_CONFERENCE_IS_ON() )
			{/* this is the begin of voice connect for call conference */
				return pbx_conference_begin(devIndex);
			}
			
			/* only one EXT can been activate in state of PROGRAM */
			return STATE_BUSY;
		}
	}
	
	if(IS_EXT_DEVICE(dest) )
	{/* -->EXT, then call transfer check  */
		if(dest == devIndex)
		{
			dest = PHONE_ERROR;
		}
		else if(NO_DISTURB(dest) )
		{
			dest = PHONE_ERROR;
		}
		else if(DEVICE_IS_BUSY(dest) )
		{
			if(EXT_IS_CFG_AS_HOLDER(dest) && deviceStatus[dest].status == STATE_IN_CALL )
			{/* CallWait/CallHold support */
				decodingchannel_free(devIndex);
				deviceStatus[devIndex].peerId = dest;
				holdId[dest] = devIndex;

				return STATE_WAIT;
			}
			else
			{
				dest = PHONE_ERROR;
			}
		}
		else if( IS_EXT_DEVICE(extConfigs.secretBranchs[dest]) &&
			devIndex != extConfigs.secretBranchs[dest] )
		{
			dest = extConfigs.secretBranchs[dest];
		}
		else
		{
			transfer = call_transfer_check( dest );
		}
	}

	if(transfer!= NULL)
	{
#if 0	
		if(transfer->type == EXT_TRANSFER_TYPE_LOCAL)
		{
			if(IS_EXT_DEVICE(PBX_GET_HIGH_NIBBLE(transfer->digits.value[0]) ) )
			{
				dest = PBX_GET_HIGH_NIBBLE(transfer->digits.value[0]);
			}
		}
		else
#endif			
		{
			if(transfer->type == EXT_TRANSFER_TYPE_IP)
			{
				dest = PHONE_IP;
				/* copy transfer dest phone number to redial in this state */
				/* copy digits into caller side */
				COPY_DIGITS(&decodingChannels[channel].digits, &transfer->digits);
				DEVICE_EVENT_EMIT(devIndex, PBX_EVENT_DIGIT);
			}
			else if(transfer->type == EXT_TRANSFER_TYPE_PSTN)
			{
				dest = PHONE_PSTN;
			}
			else
			{
				dest = PHONE_ERROR;
			}

		}
	}

	if(dest == PHONE_IP)
	{
		if(IS_UART_NOT_SENDABLE())
		{
			DEVICE_EVENT_EMIT(devIndex, PBX_EVENT_DIGIT);
			return STATE_CONTINUE;
		}
		else
		{
			dest = pbx_ip_find_free_device();
			if(dest != PBX_VALUE_INVALIDATE)
			{
				dest = GET_IP_DEVICE_INDEX(dest);
				deviceStatus[devIndex].timeout = DIALING_TIMER_LENGTH;
				if(transfer==NULL)
				{/* redial manually */
					decodingChannels[channel].digits.length = 0;
				}
#if 0				
				else
				{/* -->EXT-->Transfer-->IP */
				}
#endif				
			}
			else
			{
				dest = PHONE_ERROR;
			}
		}
	}
	else if(dest == PHONE_PSTN)
	{
#if 0		
		n = is_had_zhanghao(j);
		if(n==0xff)
#endif				
		if(IS_EXT_DEVICE(devIndex) )
		{
			n = extConfigs.authorities[devIndex];
			if( n < EXT_AUTHOR_INTERNAL )
			{
				dest = pbx_line_allocate_channel();
				if(dest == PBX_VALUE_INVALIDATE)
				{
					dest = PHONE_ERROR;
				}
			}
		}
	}

	/* and send phone number of transfered call */
	/* PSTN may enter into CONTINUE for more better effect */
	if(( IS_PSTN_LINE_DEVICE(dest)) && (transfer != NULL||IS_IP_DEVICE(devIndex)))
	{
//			if( callerid_allocate_channel(dest, &(transfer->digits) )==PBX_VALUE_INVALIDATE)
		if(transfer != NULL )
			n = callerid_allocate_channel(dest, transfer->digits.value, transfer->digits.length );
		else
		{
			/* for IP-->PSTN, line number is assigned by IP side with UART CMD */
			PSTN_SET_BUSY(dest);
			pbx_row_changed( dest);
			PBX_ROW_BUSY(dest);
		
#if 0		
			decodingChannels[channel].digits.value[0] = 0x24;
			decodingChannels[channel].digits.value[1] = 0x3F;
			decodingChannels[channel].digits.length = 3;
#endif		
			n = callerid_allocate_channel(dest, decodingChannels[channel].digits.value, decodingChannels[channel].digits.length );
		}

		if( n == PBX_VALUE_INVALIDATE)
		{
			decodingchannel_free(devIndex);
			return STATE_BUSY;
		}
	}
	
	if(IS_EXT_DEVICE(devIndex) && IS_EXT_DEVICE(dest) )
	{
//		callerid_allocate_channel(dest, (phone_digits_t xdata *)(extConfigs.phoneNumbers+devIndex ));
		callerid_allocate_channel(dest, extConfigs.phoneNumbers[devIndex].value, extConfigs.phoneNumbers[devIndex].length );
	}

	if((dest == PHONE_ERROR) || (dest == devIndex/*for EXT transfer */) )
	{
		decodingchannel_free(devIndex);
//		deviceStatus[devIndex].peerId = PBX_VALUE_INVALIDATE;
		return STATE_BUSY;
	}
	
	if(dest == PHONE_INCOMPLATE  )
	{
		goto stay_in_this_state;
	}
	
	deviceStatus[devIndex].peerId = dest;
	deviceStatus[dest].peerId = devIndex;
	DEVICE_EVENT_EMIT(dest, PBX_SIGNAL_RINGING);
		
	if( IS_IP_DEVICE(dest))// ||( IS_PSTN_LINE_DEVICE(dest) && transfer != NULL )  )
	{/* continue in this state to decode DTMF digit for IP dest, and quit this state with RINGING signal  */
//			return STATE_RE_DIALING;
		goto stay_in_this_state;
	}


	decodingchannel_free(devIndex);

	return STATE_TRYING;

stay_in_this_state:
	deviceStatus[devIndex].timeout = DIALING_TIMER_LENGTH;
	return STATE_CONTINUE;
	
}

INT8U	ext_dialing_state_onhook(INT8U devIndex)
{
	if(IS_IP_DEVICE(deviceStatus[devIndex].peerId) )/* IP device has been in RINGING state */
	{
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
	}
	
	decodingchannel_free(devIndex);
	pbx_if_ringback_tone_play(devIndex, FALSE);
	
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(EXT_FLASH_IS_ON(devIndex))
		{/* made waiter into busy state */
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_SIGNAL_CANCEL);
		}
		
		return STATE_IDLE;
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* for PSTN device, delay for onhook check */
		return STATE_BUSY;
	}
	return STATE_IDLE;
}

INT8U	ext_dialing_state_timeout(INT8U devIndex)
{/* also can be timeout event from Waiter ONHOOK */
	decodingchannel_free(devIndex);
	pbx_if_ringback_tone_play(devIndex, FALSE);

	return STATE_BUSY;
}


void ext_dialing_state_enter(INT8U devIndex)
{
	DEVICE_TIMER_START(devIndex, DIALING_TIMER_LENGTH);
	
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		/* play record alert voice here */
		pbx_if_play_record(devIndex, TRUE);
	}
	else if(IS_EXT_DEVICE(devIndex))
	{
		pbx_if_ringback_tone_play(devIndex, TRUE);
		if( extConfigs.delayOutTime[devIndex]> 0)
		{
			deviceStatus[devIndex].timeout = extConfigs.delayOutTime[devIndex];
		}
	}
	else if(IS_IP_DEVICE(devIndex) && IS_EXT_DEVICE(deviceStatus[devIndex].peerId) )
	{/* for IP-->EXT, make statemachine continue */
		DEVICE_EVENT_EMIT(devIndex, PBX_EVENT_DIGIT);
	}
	/* for IP-->PSTN, wait digits from UART */
	
	return;
}

