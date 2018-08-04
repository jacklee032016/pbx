/*
* $Id: stateTrying.c,v 1.7 2007/09/03 21:09:26 lizhijie Exp $
*/

/*
* In TRYING state, timeout, onhook and OK signal(callee has been offhook) are process
* PEER is busy is not used: When Dialing out, retrieve whether dest is busy

* process DIGIT event in this state for IP Call
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


INT8U	ext_trying_state_timeout(INT8U devIndex)
{
	if( (callDevices[devIndex].timeoutCounter > DURATION_COUNT_RINGBACK) )
	{/* this timeout for CAR signal */
//		if(CAR_IS_SUPPORT(CALL_MY_PEER(devIndex)) )
//		{
		DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_TONE_RINGBACK);
		CALL_TIMER_START(devIndex,  RINGBACK_TIMER_LENGTH);

		return PBX_STATE_CONTINUE;
	}

	if(callDevices[devIndex].timeoutCounter <DURATION_COUNT_RINGBACK )
	{
		CALL_TIMER_RESTART(devIndex, RINGBACK_TIMER_LENGTH);
		return PBX_STATE_CONTINUE;
	}
	
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
	CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
	return PBX_STATE_BUSY;
}

INT8U	ext_trying_state_onhook(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);

	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if(EXT_FLASH_IS_ON(devIndex) )
		{
			/* assign id of the third party to the Waiter */
			CALL_MY_PEER(CALL_MY_FLASH(devIndex)) = CALL_MY_PEER(devIndex);
			/* assign id of Waiter to the third part */
			CALL_MY_PEER(CALL_MY_PEER(devIndex)) = CALL_MY_FLASH(devIndex);

			/* optional, assign my id(flasher ID) to flashPeerId of the third party */
#if 0		
			CALL_MY_FLASH(CALL_MY_PEER(devIndex)) = devIndex;
#endif		
		}
		else
		{
			CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}
	}
	else
	{
		CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
	}
	
	return PBX_STATE_IDLE;
}

/* for both DIALING and TRYING state of caller */
INT8U	ext_calling_state_OK(INT8U devIndex)
{
	/* voice channel is connected in RINGING state */
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_OK);
	return PBX_STATE_IN_CALL;
}

INT8U	ext_trying_state_flash(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_DIRECT_MODE() )
	{
		pstnDidRinging(devIndex);
	}

	return PBX_STATE_CONTINUE;
}


void ext_trying_state_enter(INT8U devIndex)
{
	if(CAR_IS_SUPPORT(CALL_MY_PEER(devIndex)) )
	{/* not play ringback tone now */
		CALL_TIMER_START(devIndex, CAR_TIMER_LENGTH);
		callDevices[devIndex].timeoutCounter = DURATION_COUNT_RINGBACK + 1;
	}
	else
	{
		if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_DID(devIndex) )
		{/* it is determined by PSTN Caller */
			CALL_TIMER_CANCEL(devIndex);
		}
		else
		{
			DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_TONE_RINGBACK);
			CALL_TIMER_START(devIndex,  RINGBACK_TIMER_LENGTH);
		}
	}

#if 0
	if(IS_ROW_DEVICE(devIndex) )
	{
		if(IS_PSTN_LINE_DEVICE(devIndex) )
		{
			if(PSTN_IS_DIRECT_MODE() )
			{
				CALL_TIMER_CANCEL(devIndex);/* about 5 second, more 4 seconds which is max period of space time*/
			}
		}
	}
#endif

	return;
}

