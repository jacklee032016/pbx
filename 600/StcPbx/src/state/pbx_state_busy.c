/*
* $Id: pbx_state_busy.c,v 1.10 2007/07/25 17:56:26 lizhijie Exp $
*/

/*
* In BUSY state, OnHook, timeout event are process
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U	ext_busy_state_onhook(INT8U devIndex)
{
	pbx_if_ringback_tone_play(devIndex, FALSE);
	pbx_if_start_music(devIndex, FALSE);

	pbx_program_exit_handle(devIndex);

	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if( EXT_FLASH_IS_ON(devIndex) )
		{
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_SIGNAL_CANCEL);
		}
	}
		
	return STATE_IDLE;
}

INT8U	ext_busy_state_timeout(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		deviceStatus[devIndex].timeoutCounter++;
		if(deviceStatus[devIndex].timeoutCounter != busysound_time)
		{
			if( IS_ODD(deviceStatus[devIndex].timeoutCounter ) )
			{
				pbx_if_ringback_tone_play(devIndex, FALSE);
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].busy_off_time;
			}
			else
			{
				pbx_if_ringback_tone_play(devIndex, TRUE);
				deviceStatus[devIndex].timeout = phoneParams[baseConfigs.signalType].busy_on_time;
			}

			return STATE_CONTINUE;
		}

#if 0
	is_zhanghaob(j);
	status[j][0]=12;
	if(status[j][2]!=0xff)		//060308ÐÞ¸Ä
	{
		send_guaji(status[j][2]);
		status[j][2]=0xff;
	}
#endif

		pbx_if_ringback_tone_play(devIndex, FALSE);
		pbx_if_start_music(devIndex, TRUE);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex))
	{
		PSTN_SET_FREE(devIndex);
		return STATE_IDLE;
	}
	
	return STATE_CONTINUE;
}


void ext_busy_state_enter(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		pbx_if_ringback_tone_play(devIndex, TRUE);
		DEVICE_TIMER_START(devIndex, phoneParams[baseConfigs.signalType].busy_on_time);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		pbx_line_free_channel(devIndex);
	}
	else if(IS_IP_DEVICE(devIndex) )
	{
		ip_call_tx_busy_signal(devIndex, deviceStatus[devIndex].peerId);

		DEVICE_TIMER_START(devIndex, 1*PBX_100MS);
	}
}

/* for OFFHOOK event of Flasher when it is in state of DIALING, TRYING and BUSY */
INT8U	ext_offhook_state_flash(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			if(deviceStatus[devIndex].status == STATE_DIALING)
			{
				return STATE_CONTINUE;
			}

			decodingchannel_allocate(devIndex);
			if(deviceStatus[devIndex].peerId != PBX_VALUE_INVALIDATE)
			{
				deviceStatus[devIndex].peerId = PBX_VALUE_INVALIDATE;
			}
			
			return STATE_DIALING;
		}
		else if(EXT_FLASH_IS_ON(devIndex) )
		{
			DEVICE_EVENT_EMIT(deviceStatus[devIndex].flashPeerId, PBX_EVENT_OFFHOOK);
			
			if(deviceStatus[deviceStatus[devIndex].peerId].status == STATE_RINGING)
			{/* when the third party is in state of RINGING, stop ringing */
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
			}

			if(deviceStatus[devIndex].status == STATE_DIALING)
			{
				decodingchannel_free( devIndex);
			}
			
			return STATE_IN_CALL;
		}
	}

	if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_CONSOLE_MODE() )
	{
		pstnDidRinging(devIndex);
	}
	
	return STATE_CONTINUE;
}

