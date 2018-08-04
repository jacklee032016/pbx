/*
* $Id: pbx_state_wait.c,v 1.4 2007/07/25 17:56:26 lizhijie Exp $
*/
/*
* In INCALL state, onhook, timeout event and cancel signal(Onhook by peer) are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

INT8U getMyHolder(INT8U devIndex)
{
	INT8U data i;
	
	for(i=0; i< PBX_FXS_COUNT; i++)
	{
		if(holdId[i] == devIndex)
			return i;
	}

	return PBX_FXS_COUNT+2;
}


INT8U	ext_wait_state_onhook(INT8U devIndex)
{/* I(Waiter) am on hook before the second call  */
	INT8U data holder;

	holder = getMyHolder(devIndex);
	
	pbx_if_start_music(devIndex, FALSE);

	if(IS_CONFERENCE_MEMBER(devIndex) )
	{
		CONFERENCE_DEL_MEMBER(devIndex);
		if(CONFERENCE_HAS_NO_MEMBER(deviceStatus[devIndex].peerId) )
		{
//			if(deviceStatus[deviceStatus[devIndex].peerId].status == STATE_IN_CALL)
			if(CONFERENCE_VOICE_IS_ON(deviceStatus[devIndex].peerId) )
			{
				DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);
			}
		}
	}
	else if(IS_EXT_DEVICE(holder) )
	{/*I am the beginer of CallWait/CallHold */
		pbx_if_ringback_tone_play(holder, FALSE);
	}
	else if(EXT_IS_HOLDER(deviceStatus[devIndex].peerId) )
	{/* I am the primitive peer of the Call Hold */
		/* clean flashPeer to made the flash event has no effect after I am onhook */
		deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = PBX_VALUE_INVALIDATE;

	}
	else
	{
		/* send timeout event to the Flasher no matter what state flasher is */

		/* cancel call transfer status, so Flasher and Waiter can distinct whether send msg back to Waiter */
		deviceStatus[deviceStatus[devIndex].peerId].flashPeerId = PBX_VALUE_INVALIDATE;
		
		deviceStatus[deviceStatus[devIndex].peerId].timeoutCounter = 0XFE;/* max value */
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_TIMEOUT);
	}
	
	return STATE_IDLE;
}

INT8U	ext_wait_state_offhook(INT8U devIndex)
{/* this is a flash event */
	INT8U data holder;
	pbx_if_start_music(devIndex, FALSE);

	holder = getMyHolder(devIndex);

	if(IS_EXT_DEVICE(holder) )
	{/*I am the beginer of CallWait/CallHold */
		pbx_if_ringback_tone_play(holder, FALSE);
	}

	/* normally peer of waiter is in row of switch because which has emit flash event */
	if( pbx_voice_channel_connect(deviceStatus[devIndex].peerId, devIndex ))
	{
		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_OK);

//		pbx_if_start_music(deviceStatus[devIndex].peerId, TRUE);
//		pbx_if_start_music(devIndex, TRUE);
		return STATE_IN_CALL;
	}
	
	deviceStatus[deviceStatus[devIndex].peerId].timeoutCounter = 0XFE;/* max value */
	DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_EVENT_TIMEOUT);
	return STATE_BUSY;
}

INT8U	ext_wait_state_ok(INT8U devIndex)
{
	/* The third is offhook now, voice channel is connect by the third party */
	pbx_if_start_music(devIndex, FALSE);
	return STATE_IN_CALL;
}


INT8U	ext_wait_state_cancel(INT8U devIndex)
{
	INT8U data holder;
	/* The third is timeout when ringing or the Flasher is onhook before the second call is finished */
	pbx_if_start_music(devIndex, FALSE);

	if(IS_CONFERENCE_MEMBER(devIndex) )
	{/* just because of master onhook when i am in state of wait */
		CONFERENCE_DEL_MEMBER(devIndex);
	}
	else 
	{
		holder = getMyHolder(devIndex);

		/* following cases are happened when CallHolder is ONHOOK ahead */
		if(IS_EXT_DEVICE(holder) )
		{/*I am the beginer of CallWait/CallHold */
			pbx_if_ringback_tone_play(holder, FALSE);
		}
	}
	
	return STATE_BUSY;
}


INT8U	ext_wait_state_timeout(INT8U devIndex)
{
	INT8U data holder;

	holder = getMyHolder(devIndex);
	if(!IS_EXT_DEVICE(holder) )
		return STATE_CONTINUE;
	
	pbx_if_ringback_tone_play(holder, FALSE);
	
	deviceStatus[devIndex].timeoutCounter ++;
	if( deviceStatus[devIndex].timeoutCounter < HST_TOTAL_LENGTH)
	{
		if( (deviceStatus[devIndex].timeoutCounter%HST_DURATION == 0) ||
			(deviceStatus[devIndex].timeoutCounter%HST_DURATION == 2) )
		{
			pbx_if_ringback_tone_play(devIndex, TRUE);
		}
		
		deviceStatus[devIndex].timeout = HOLD_SERVICE_TONE_TIME;
		return STATE_CONTINUE;
	}
	else
	{/* no response from holder about 40 seconds, then play busy tone to Waiter */
		
//		DEVICE_EVENT_EMIT(deviceStatus[devIndex].peerId, PBX_SIGNAL_CANCEL);

		return STATE_BUSY;
	}
}

void ext_wait_state_enter(INT8U devIndex)
{
	INT8U data holder;

	holder = getMyHolder(devIndex);
	pbx_if_start_music(devIndex, TRUE);
	
	if(IS_EXT_DEVICE(holder) )
	{/* this is for CallWait/CallHold */
		/* start timer for HST */
		DEVICE_TIMER_START(devIndex,  HOLD_SERVICE_TONE_TIME);
		/* play HST on holder */
		pbx_if_ringback_tone_play(holder, TRUE);
	}
	else
	{/* no timer is needed which is implemented by the Flasher or the third party */
		pbx_voice_channel_broken(devIndex, deviceStatus[devIndex].peerId);
	}
	return;
}

