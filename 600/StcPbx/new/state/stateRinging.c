/*
* $Id: stateRinging.c,v 1.7 2007/09/01 21:25:43 lizhijie Exp $
*/
/*
* In RINGING state, offhook, ring on-off timeout, ringing timeout event and cancel this call signal
* are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

#define	EXT_IS_LEFT_TRANSFER(devIndex)	\
	(extConfigs.leftTranferTimes[devIndex] > 0 && \
		10*extConfigs.leftTranferTimes[devIndex] < RINGBACK_TIMER_LENGTH && \
		extTransfers.leftTransfers[devIndex].type != EXT_TRANSFER_TYPE_UNKNOWN)	

INT8U	ext_ringing_state_offhook(INT8U devIndex)
{/* this offhook event of ringing extension can by only detected when ringoff */

	CALL_TIMER_CANCEL(devIndex);
	if(IS_EXT_DEVICE(devIndex) )
	{
		dtmfEncodeChannelFree(devIndex);
		
		if(PBX_PROGRAM_MODE_IS_ON(devIndex))
		{
			PBX_PROGRAM_MODE_CANCEL();
			return PBX_STATE_BUSY;
		}
	}

	if( voiceChannelConnect(devIndex, CALL_MY_PEER(devIndex) ))
	{
		CALL_PEER_EVENT(devIndex, PBX_SIGNAL_OK);
		return PBX_STATE_IN_CALL;
	}

//	uart_debug_2_int_value(UART_DEBUG_CAR, devIndex, CALL_MY_PEER(devIndex));
	return PBX_STATE_BUSY;
}

INT8U	ext_ringing_state_ringer_off(INT8U devIndex)
{
	INT8U data channel;
	channel  = dtmfEncodeChannelFind(devIndex);
	if(IS_EXT_DEVICE(devIndex) )
	{
//	uart_debug_2_int_value(UART_DEBUG_PSTN_RINGER, devIndex, channel);
		if(channel != PBX_VALUE_INVALIDATE && encodingChannels[channel].point == PBX_VALUE_INVALIDATE )
		{/* this first ringing */
			if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
			{
				dtmfEncoderPlayStart(devIndex );
			}
			
			if(PBX_CHECK_FLAGS(extConfigs.isCallerIdOnFlags,devIndex) )
			{
				dtmfEncoderPlayStart(devIndex );
			}
			else
			{
				dtmfEncodeChannelFree(devIndex);
			}
		}
		else
		{
			dtmfEncodeChannelFree(devIndex);
			
			if(IS_PSTN_LINE_DEVICE(CALL_MY_PEER(devIndex) ) &&
				PSTN_IS_DID() )
			{
				CALL_PEER_EVENT(devIndex, PBX_EVENT_FLASH);
				DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
				return PBX_STATE_IDLE;
			}
			
			if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
			{/*exit PROGRAM_MODE when CANCEL event is processed */
				DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_CANCEL);
				return PBX_STATE_IDLE;
			}
		}
	}

	return  PBX_STATE_CONTINUE;
}


/* for both ringing timeout and ring ON-OFF timeout event */
INT8U	ext_ringing_state_timeout(INT8U devIndex)
{
	INT8U data dest = PHONE_ERROR;

	if( (callDevices[devIndex].timeoutCounter > DURATION_COUNT_RINGBACK) )
	{/* this timeout for left transfer */
		if(IS_EXT_DEVICE(devIndex) )
		{
			if(extTransfers.leftTransfers[devIndex].type == EXT_TRANSFER_TYPE_IP )
				dest = PHONE_IP;
			else if(extTransfers.leftTransfers[devIndex].type == EXT_TRANSFER_TYPE_PSTN )
				dest = PHONE_PSTN;

			dest = callTransfer(dest, extTransfers.leftTransfers[devIndex].digits.value, extTransfers.leftTransfers[devIndex].digits.length);

			if(IS_CALL_DEVICE(dest) )
			{
				CALL_MY_PEER(CALL_MY_PEER(devIndex)) = dest;
				CALL_MY_PEER(dest) = CALL_MY_PEER(devIndex);
				CALL_MY_PEER(devIndex) = PBX_VALUE_INVALIDATE;
				CALL_EVENT_EMIT(dest, PBX_SIGNAL_RINGING);
			}

			return PBX_STATE_IDLE;
		}
	}

	if(callDevices[devIndex].timeoutCounter <DURATION_COUNT_RINGBACK )
	{
		CALL_TIMER_RESTART(devIndex, RINGBACK_TIMER_LENGTH);
		
		return PBX_STATE_CONTINUE;
	}
	
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	if(IS_EXT_DEVICE(devIndex) )
	{

		CALL_EVENT_EMIT(devIndex, PBX_SIGNAL_CANCEL);

		/* this event can be only handled by the Waiter when Flasher has onhook after RINGING
		* If Flasher is not onhook after ringing, this event is ignore by Flasher which has timer itself 
		*/
		if(EXT_FLASH_IS_ON(devIndex) )
		{/*ignore timeout event send by Waiter when Waiter is ONHOOK ahead */
			CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}

		return  PBX_STATE_CONTINUE;
	}

	return PBX_STATE_IDLE;
}

INT8U	ext_ringing_state_cancel(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_CANCEL);
	dtmfEncodeChannelFree(devIndex);

	return PBX_STATE_IDLE;
}


void ext_ringing_state_enter(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_RINGING);
	CALL_TIMER_START(devIndex, RINGBACK_TIMER_LENGTH);

	if(IS_EXT_DEVICE(devIndex) )
	{
		if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
		{
			dtmfEncodeChannelAllocate(devIndex, extConfigs.phoneNumbers[devIndex].value, extConfigs.phoneNumbers[devIndex].length );
		}
		else
		{
			if(EXT_IS_LEFT_TRANSFER(devIndex) )
			{
				CALL_TIMER_START(devIndex, 10*extConfigs.leftTranferTimes[devIndex]);
				callDevices[devIndex].timeoutCounter = DURATION_COUNT_RINGBACK + 1;
			}
		}
	}

	return;
}

INT8U	ext_car_state_offhook(INT8U devIndex)
{
	CALL_TIMER_CANCEL(devIndex);

	if(dtmfEncodeChannelFind(devIndex)!=PBX_VALUE_INVALIDATE )
	{
		dtmfEncoderPlayStart(devIndex );
	}
#if WITH_UART_DEBUG_CAR
	uart_debug_int_value(UART_DEBUG_CAR, 1);
#endif

	return PBX_STATE_CONTINUE;
}

INT8U	ext_car_state_timeout(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	return PBX_STATE_RINGING;
}

INT8U	ext_car_state_onhook(INT8U devIndex)
{
	CALL_TIMER_CANCEL(devIndex);

	dtmfEncodeChannelFree(devIndex);

#if WITH_UART_DEBUG_CAR
	uart_debug_int_value(UART_DEBUG_CAR, 0);
#endif
	/*notify peer begin to play ringback after CAR finished with TIMEOUT event */
	CALL_PEER_EVENT(devIndex, PBX_EVENT_TIMEOUT);
	
	return PBX_STATE_RINGING;
}

void ext_car_state_enter(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_CAR_RINGING);
	CALL_TIMER_START(devIndex, CAR_TIMER_LENGTH);

	return;
}

