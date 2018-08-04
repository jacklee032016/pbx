/*
* $Id: pbx_state_ringing.c,v 1.13 2007/07/25 17:56:26 lizhijie Exp $
*/
/*
* In RINGING state, offhook, ring on-off timeout, ringing timeout event and cancel this call signal
* are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U	ext_ringing_state_offhook(INT8U devIndex)
{/* this offhook event of ringing extension can by only detected when ringoff */

	DEVICE_TIMER_CANCEL(devIndex);
	if(IS_EXT_DEVICE(devIndex) )
	{
		pbx_if_start_ringing(devIndex, FALSE);
		callerid_free_channel(devIndex);
		
		if(PBX_PROGRAM_MODE_IS_ON(devIndex))
		{
			PBX_PROGRAM_MODE_CANCEL();
			return STATE_BUSY;
		}
	}

	if( pbx_voice_channel_connect(devIndex, deviceStatus[devIndex].peerId ))
	{
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_OK);

		return STATE_IN_CALL;
	}
	
	return STATE_BUSY;
}

/* for both ringing timeout and ring ON-OFF timeout event */
INT8U	ext_ringing_state_timeout(INT8U devIndex)
{
	INT8U data count;
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(deviceStatus[devIndex].status == STATE_RINGING)
		{
			count = backsound_time10;
		}
		else
		{/* RERING state, must be a odd number */
			count = PBX_RERING_COUNT;
		}
		
		deviceStatus[devIndex].timeoutCounter++;
		if( deviceStatus[devIndex].timeoutCounter <= count )
		{
			if( IS_ODD(deviceStatus[devIndex].timeoutCounter))
			{
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].ringer_off_time;
				pbx_if_start_ringing(devIndex, FALSE);
				
				if(deviceStatus[devIndex].timeoutCounter == 1 && GET_BIT(extConfigs.isCallerIdOnFlags,devIndex) )/* send caller ID when first ringer off */
				{
					callerid_play_start(devIndex );
				}

				if(deviceStatus[devIndex].timeoutCounter == 3 && 
					IS_PSTN_LINE_DEVICE(deviceStatus[devIndex].peerId) )
				{
					DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_FLASH);
				}
			}
			else
			{
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].ringer_on_time;
				pbx_if_start_ringing(devIndex, TRUE);

			}

		}
		else
		{
			DEVICE_EVENT_EMIT(devIndex, PBX_SIGNAL_CANCEL);
			pbx_if_start_ringing(devIndex, FALSE);

			/* this event can be only handled by the Waiter when Flasher has onhook after RINGING
			* If Flasher is not onhook after ringing, this event is ignore by Flasher which has timer itself 
			*/
			if(EXT_FLASH_IS_ON(devIndex) )
			{/*ignore timeout event send by Waiter when Waiter is ONHOOK ahead */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
			}
		}

		return  STATE_CONTINUE;
	}
	else 
		return STATE_IDLE;
}

INT8U	ext_ringing_state_cancel(INT8U devIndex)
{
	if( IS_EXT_DEVICE(devIndex) )
	{
		pbx_if_start_ringing(devIndex, FALSE);
		if(PBX_PROGRAM_MODE_IS_ON(devIndex))
		{
			PBX_PROGRAM_MODE_CANCEL();
		}
		callerid_free_channel(devIndex);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		callerid_free_channel(devIndex);
		return STATE_BUSY;
	}
	else if(IS_IP_DEVICE(devIndex) )
	{
		ip_call_tx_cancel_signal( devIndex );
	}	
	return STATE_IDLE;
}


void ext_ringing_state_enter(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
		{
			callerid_allocate_channel(devIndex, extConfigs.phoneNumbers[devIndex].value, extConfigs.phoneNumbers[devIndex].length );
		}
		
		DEVICE_TIMER_START(devIndex, phoneParams[baseConfigs.signalType].ringer_on_time);
		pbx_if_start_ringing(devIndex, TRUE);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* simulate a OFFHOOK event on PSNT Device for call to PSTN */
	/* offhook and send out calling digits */
		PSTN_SET_BUSY(devIndex);
		pbx_line_offhook(devIndex);
		if(callerid_find_channel(devIndex) == PBX_VALUE_INVALIDATE)
		{/* dial PSTN phone number with a phone */
			DEVICE_EVENT_EMIT(devIndex, PBX_EVENT_OFFHOOK);
		}
		else
		{/* this is for call transfer operation */
			callerid_play_start( devIndex);
		}
	}
	else if(IS_IP_DEVICE(devIndex) )
	{
		ip_call_tx_dialing_signal(devIndex, deviceStatus[devIndex].peerId);
	}

	return;
}

