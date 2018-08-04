/*
* $Id: pbx_state_trying.c,v 1.16 2007/07/25 17:56:26 lizhijie Exp $
*/

/*
* In TRYING state, timeout, onhook and OK signal(callee has been offhook) are process
* PEER is busy is not used: When Dialing out, retrieve whether dest is busy

* process DIGIT event in this state for IP Call
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


INT8U	ext_trying_state_timeout(INT8U devIndex)
{
	deviceStatus[devIndex].timeoutCounter ++;
#if 0	
	if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_CONSOLE_MODE() )
	{
		if( deviceStatus[devIndex].timeoutCounter < 12) /*12*5 = 60seconds */
		{
			if(pstnPending[GET_PSTN_NUMBER(devIndex)] == PSTN_RINGER_PENDING)
			{/* still heartbeat when timeout 5 seconds */
				pstnPending[GET_PSTN_NUMBER(devIndex)] = PSTN_RINGER_OFF;
				DEVICE_TIMER_START(devIndex,  50*PBX_100MS);/* about 5 second, more 4 seconds which is max period of space time*/

				return STATE_CONTINUE;
			}
		}
	}
	else
#endif		
	if(IS_EXT_DEVICE(devIndex) || IS_IP_DEVICE(devIndex) ||
		(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_DIRECT_MODE()) )
	{/* when PSTN is not DID mode, ringback tone is play by myself */
		if( deviceStatus[devIndex].timeoutCounter < RINGBACK_LOOP_COUNT)
		{
		
			if( IS_ODD(deviceStatus[devIndex].timeoutCounter))
			{
				pbx_if_ringback_tone_play(devIndex, FALSE);
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].ringback_off_timer;
			}
			else
			{
				pbx_if_ringback_tone_play(devIndex, TRUE);
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].ringback_on_timer;
			}
			
			return STATE_CONTINUE;
		}
		
	}

	pbx_if_ringback_tone_play(devIndex, TRUE);
	DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
	return STATE_BUSY;
}

INT8U	ext_trying_state_onhook(INT8U devIndex)
{
	pbx_if_ringback_tone_play(devIndex, FALSE);

	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{/* for PSTN, it is that busy tone is detected */
		return STATE_BUSY;
	}

	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(EXT_FLASH_IS_ON(devIndex) )
		{
			/* assign id of the third party to the Waiter */
			deviceStatus[deviceStatus[devIndex].flashPeerId].peerId = deviceStatus[devIndex].peerId;
			/* assign id of Waiter to the third part */
			deviceStatus[deviceStatus[devIndex].peerId].peerId = deviceStatus[devIndex].flashPeerId;

			/* optional, assign my id(flasher ID) to flashPeerId of the third party */
#if 0		
			deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = devIndex;
#endif		
		}
		else
		{
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
		}
	}
	else
	{
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
	}
	
	return STATE_IDLE;
}

INT8U	ext_trying_state_OK(INT8U devIndex)
{
	/* voice channel is connected in RINGING state */
	pbx_if_ringback_tone_play(devIndex, FALSE);
	if(IS_IP_DEVICE(devIndex))
	{
		ip_call_tx_offhook_signal( devIndex);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )//&& PSTN_IS_CONSOLE_MODE() )
	{
		pbx_line_offhook(devIndex );
	}
	return STATE_IN_CALL;
}

INT8U	ext_trying_state_flash(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_CONSOLE_MODE() )
	{
		pstnDidRinging(devIndex);
	}

	return STATE_CONTINUE;
}


void ext_trying_state_enter(INT8U devIndex)
{
	DEVICE_TIMER_START(devIndex,  phoneParams[baseConfigs.signalType].ringback_on_timer);

	if(IS_ROW_DEVICE(devIndex) )
	{
		pbx_if_ringback_tone_play(devIndex, TRUE);

		if(IS_PSTN_LINE_DEVICE(devIndex) )
		{
			if(PSTN_IS_CONSOLE_MODE() )
			{
				DEVICE_TIMER_CANCEL(devIndex);/* about 5 second, more 4 seconds which is max period of space time*/
			}
		}
	}
	else if(IS_IP_DEVICE(devIndex) )
	{
		ip_call_tx_trying_signal(devIndex, deviceStatus[devIndex].peerId );
	}

	return;
}

