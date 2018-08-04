/*
* $Id: stateDialing.c,v 1.8 2007/09/12 20:21:22 lizhijie Exp $
*/

/*
* In DIALING state, Digit, onhook and timeout events are process
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

INT8U	ext_dialing_state_ringing(INT8U devIndex)
{
	dtmfDecodingChannelFree(devIndex);
	
	return PBX_STATE_TRYING;
}

INT8U	ext_dialing_state_digits(INT8U devIndex)
{
	INT8U	data n, dest = PHONE_ERROR, channel;
	ext_transfer_digits	xdata	*transfer = NULL;

	CALL_TIMER_START(devIndex, DIALING_TIMER_LENGTH);
	if( (channel = dtmfDecodingChannelFind(devIndex)) == PBX_VALUE_INVALIDATE)
		return PBX_STATE_BUSY;
	
	if(IS_CALL_DEVICE(CALL_MY_PEER(devIndex) ) )
	{
		DEVICE_EVENT_EMIT(CALL_MY_PEER(devIndex), DEV_EVENT_CALLEE_DIGITS);
		return PBX_STATE_CONTINUE;
	}
	
	if(decodingChannels[channel].digits.length == 1)
	{
		DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	}

	dest = call_dialing_check_dest(devIndex, &(decodingChannels[channel].digits));
	if(dest==PHONE_PBX_PROGRAM)
	{
		if(PBX_PROGRAM_MODE_IS_OFF() )
		{/* DTMF decoding channel is keep */
			return PBX_STATE_PROGRAM;
		}
		else
		{
			/* free DTMF decoding Channel */
			dtmfDecodingChannelFree(devIndex);
			if(PBX_CONFERENCE_IS_ON() )
			{/* this is the begin of voice connect for call conference */
				return pbx_conference_begin(devIndex);
			}
			
			/* only one EXT can been activate in state of PROGRAM */
			return PBX_STATE_BUSY;
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
			if(EXT_IS_CFG_AS_HOLDER(dest) && CALL_CHECK_STATUS(dest, PBX_STATE_IN_CALL)   )
			{/* CallWait/CallHold support */
				dtmfDecodingChannelFree(devIndex);
				CALL_MY_PEER(devIndex) = dest;
				extDevices.holdId[dest] = devIndex;

				return PBX_STATE_WAIT;
			}
			else
			{
				if( IS_EXT_DEVICE(extConfigs.busyBranchs[dest]) &&
					devIndex != extConfigs.busyBranchs[dest] && 
					CALL_CHECK_STATUS(extConfigs.busyBranchs[dest], PBX_STATE_IDLE) )
				{
					dest = extConfigs.busyBranchs[dest];
				}
				else
				{
					dest = PHONE_ERROR;
				}
			}
		}
		else if( IS_EXT_DEVICE(extConfigs.secretBranchs[dest]) &&
			devIndex != extConfigs.secretBranchs[dest] && 
			CALL_CHECK_STATUS(extConfigs.secretBranchs[dest], PBX_STATE_IDLE) )
		{
			dest = extConfigs.secretBranchs[dest];
		}
		else
		{
			transfer = call_transfer_check( dest );
		}
	}

	if(transfer!= NULL  )
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
		if(transfer->type == EXT_TRANSFER_TYPE_IP)
		{
			dest = PHONE_IP;
			/* copy transfer dest phone number to redial in this state */
			/* copy digits into caller side */
		}
		else if(transfer->type == EXT_TRANSFER_TYPE_PSTN)
		{
			dest = PHONE_PSTN;
		}
	}

	if(dest == PHONE_IP)
	{
		dest = callTransfer(dest, (transfer==NULL)?NULL:transfer->digits.value, (transfer==NULL)?0:transfer->digits.length);
		decodingChannels[channel].digits.length = 0;
	}
	else if(dest == PHONE_PSTN)
	{
		if(IS_EXT_DEVICE(devIndex) )
		{
			n = extConfigs.authorities[devIndex];
			if( n < EXT_AUTHOR_INTERNAL )
			{
				dest = callTransfer(dest, (transfer==NULL)?NULL:transfer->digits.value, (transfer==NULL)?0:transfer->digits.length);
			}
			else
			{
				dest = PHONE_ERROR;
			}
		}
		else
		{/* PSTN-->PSTN */
			if(PSTN_LINE_2_LINE_IS_ON() )
			{
				dest = pstnDeviceAllocate();
			}
			else
			{
				dest = PHONE_ERROR;
			}
		}
	}

	if(dest == PHONE_SPECIAL_FIRST )
	{
		dest = callSpecialFirst(devIndex);	
		if(IS_CALL_DEVICE(dest) )
		{
			dtmfEncodeChannelAllocate(dest, decodingChannels[channel].digits.value, decodingChannels[channel].digits.length );
		}
	}

	if((dest == PHONE_ERROR) || (dest == devIndex/*for EXT transfer */) )
	{
	uart_debug_2_int_value(UART_DEBUG_STATE, devIndex, dest);
		dtmfDecodingChannelFree(devIndex);
		return PBX_STATE_BUSY;
	}
	
	if(IS_CALL_DEVICE(dest) )
	{
		CALL_MY_PEER(devIndex) = dest;
		CALL_MY_PEER(dest ) = devIndex;
		CALL_EVENT_EMIT(dest, PBX_SIGNAL_RINGING);
	}
		
	return PBX_STATE_CONTINUE;
}


INT8U	ext_dialing_state_onhook(INT8U devIndex)
{
	if(IS_CALL_DEVICE(CALL_MY_PEER(devIndex) ) )/* IP device has been in RINGING state */
	{
		CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
	}
	
	dtmfDecodingChannelFree(devIndex);
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(EXT_FLASH_IS_ON(devIndex))
		{/* made waiter into busy state */
			CALL_FLASH_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}
		
		return PBX_STATE_IDLE;
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* for PSTN device, delay for onhook check */
		return PBX_STATE_BUSY;
	}
	return PBX_STATE_IDLE;
}

INT8U	ext_dialing_state_timeout(INT8U devIndex)
{/* also can be timeout event from Waiter ONHOOK */
	INT8U data channel;
	
	if(IS_SPECIAL_SERVICE_FIRST())
	{
		if( (channel = dtmfDecodingChannelFind(devIndex)) != PBX_VALUE_INVALIDATE)
		{
			if((decodingChannels[channel].digits.length>0 ) &&
				call_dialing_check_dest(devIndex, &(decodingChannels[channel].digits) == PHONE_SPECIAL_FIRST) )
			{
				CALL_EVENT_EMIT(devIndex, PBX_EVENT_DIGIT);
				return PBX_STATE_CONTINUE;
			}
		}
	}

	dtmfDecodingChannelFree(devIndex);
	return PBX_STATE_BUSY;
}


void ext_dialing_state_enter(INT8U devIndex)
{
	CALL_TIMER_START(devIndex, DIALING_TIMER_LENGTH);
	
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_TONE_DIALING);

	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
	}
	else if(IS_EXT_DEVICE(devIndex))
	{
		if( extConfigs.delayOutTime[devIndex]> 0)
		{
			callDevices[devIndex].timeout = extConfigs.delayOutTime[devIndex];
		}
	}
	else if(IS_IP_DEVICE(devIndex) && IS_EXT_DEVICE(CALL_MY_PEER(devIndex) ) )
	{/* for IP-->EXT, make statemachine continue */
		CALL_EVENT_EMIT(devIndex, PBX_EVENT_DIGIT);
	}
	/* for IP-->PSTN, wait digits from UART */
	
	return;
}

