/*
* $Id: stateWait.c,v 1.5 2007/09/13 20:32:06 lizhijie Exp $
*/
/*
* In INCALL state, onhook, timeout event and cancel signal(Onhook by peer) are process.
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

INT8U getMyHolder(INT8U devIndex)
{
	INT8U data i;
	
	for(i=0; i< PBX_FXS_COUNT; i++)
	{
		if(extDevices.holdId[i] == devIndex)
			return i;
	}

	return PBX_FXS_COUNT+2;
}


INT8U	ext_wait_state_onhook(INT8U devIndex)
{/* I(Waiter) am on hook before the second call  */
	INT8U data holder;

	holder = getMyHolder(devIndex);
	
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);

	if(IS_CONFERENCE_MEMBER(devIndex) )
	{
		CONFERENCE_DEL_MEMBER(devIndex);
		if(CONFERENCE_HAS_NO_MEMBER(CALL_MY_PEER(devIndex) ) )
		{
//			if(CALL_CHECK_STATUS(CALL_MY_PEER(devIndex), PBX_STATE_IN_CALL) )
			if(CONFERENCE_VOICE_IS_ON(CALL_MY_PEER(devIndex)) )
			{
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
			}
		}
	}
	else if(IS_EXT_DEVICE(holder) )
	{/*I am the beginer of CallWait/CallHold */
		DEVICE_EVENT_EMIT(holder, DEV_EVENT_STOP);
	}
	else if(EXT_IS_HOLDER(CALL_MY_PEER(devIndex) ) )
	{/* I am the primitive peer of the Call Hold */
		/* clean flashPeer to made the flash event has no effect after I am onhook */
		CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = PBX_VALUE_INVALIDATE;

	}
	else
	{
		/* send timeout event to the Flasher no matter what state flasher is */

		/* cancel call transfer status, so Flasher and Waiter can distinct whether send msg back to Waiter */
		CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = PBX_VALUE_INVALIDATE;

		callDevices[CALL_MY_PEER(devIndex)].timeoutCounter = DURATION_COUNT_RINGBACK+1;
		CALL_PEER_EVENT(devIndex, PBX_EVENT_TIMEOUT);
	}
	
	return PBX_STATE_IDLE;
}

INT8U	ext_wait_state_offhook(INT8U devIndex)
{/* this is a flash event */
	INT8U data holder;

	holder = getMyHolder(devIndex);

	if(IS_EXT_DEVICE(holder) )
	{/*I am the beginer of CallWait/CallHold */
		DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	}

	/* normally peer of waiter is in row of switch because which has emit flash event */
	if( voiceChannelConnect(CALL_MY_PEER(devIndex), devIndex ))
	{
		devMusicPlay(devIndex, FALSE);
		devMusicPlay(CALL_MY_PEER(devIndex), FALSE);
		
		CALL_PEER_EVENT(devIndex, PBX_SIGNAL_OK);
		return PBX_STATE_IN_CALL;
	}
	
	CALL_PEER_EVENT(devIndex, PBX_EVENT_TIMEOUT);
	return PBX_STATE_BUSY;
}

INT8U	ext_wait_state_ok(INT8U devIndex)
{
	/* The third is offhook now, voice channel is connect by the third party */
	devMusicPlay(devIndex, FALSE);
	return PBX_STATE_IN_CALL;
}


INT8U	ext_wait_state_cancel(INT8U devIndex)
{
	INT8U data holder;
	/* The third is timeout when ringing or the Flasher is onhook before the second call is finished */

	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);

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
			DEVICE_EVENT_EMIT(holder, DEV_EVENT_STOP);
		}
	}
	
	return PBX_STATE_BUSY;
}


INT8U	ext_wait_state_timeout(INT8U devIndex)
{
	INT8U data holder;

	holder = getMyHolder(devIndex);
	if(!IS_EXT_DEVICE(holder) )
		return PBX_STATE_CONTINUE;

/* no response from holder about 40 seconds, then play busy tone to Waiter */
		
//	CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
	return PBX_STATE_BUSY;
}

void ext_wait_state_enter(INT8U devIndex)
{
	INT8U data holder;

	holder = getMyHolder(devIndex);
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_MUSIC);
//	devMusicPlay(devIndex, TRUE);
	
	if(IS_EXT_DEVICE(holder) )
	{/* this is for CallWait/CallHold */
		/* start timer for HST */
		CALL_TIMER_START(devIndex,  TIMEOUT_NEVER);
		/* play HST on holder */
//		devToneRingbackPlay(holder, TRUE);
		DEVICE_EVENT_EMIT(holder, DEV_EVENT_TONE_CALLWAIT);
	}
	else
	{/* no timer is needed which is implemented by the Flasher or the third party */
		voiceChannelBroken(devIndex, CALL_MY_PEER(devIndex) );
	}
	return;
}

