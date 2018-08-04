/*
* $Id: stateBusy.c,v 1.5 2007/09/08 18:56:50 lizhijie Exp $
*/

/*
* In BUSY state, OnHook, timeout event are process
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

INT8U	ext_busy_state_onhook(INT8U devIndex)
{
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);

	pbx_program_exit_handle(devIndex);

	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			pbx_conference_master_onhook(devIndex);
		}
		else if( EXT_FLASH_IS_ON(devIndex) )
		{
			CALL_FLASH_EVENT(devIndex, PBX_SIGNAL_CANCEL);
		}
	}
		
	return PBX_STATE_IDLE;
}

INT8U	ext_busy_state_timeout(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_MUSIC);
	}
	else if(IS_PSTN_LINE_DEVICE(devIndex))
	{
		pstnDeviceFree(devIndex);
		return PBX_STATE_IDLE;
	}
	
	return PBX_STATE_CONTINUE;
}


void ext_busy_state_enter(INT8U devIndex)
{
	CALL_TIMER_START(devIndex, BUSY_TIMER_LENGTH );
	DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_TONE_BUSY);
	if(IS_IP_DEVICE(devIndex) )
	{
		CALL_TIMER_START(devIndex, 1*PBX_100MS);
	}
}

/* for OFFHOOK event of Flasher when it is in state of DIALING, TRYING and BUSY */
INT8U	ext_offhook_state_flash(INT8U devIndex)
{
	if(IS_EXT_DEVICE(devIndex) )
	{
		if(IS_CONFERENCE_MASTER(devIndex) )
		{
			if( CALL_CHECK_STATUS(devIndex, PBX_STATE_DIALING) )
			{
				return PBX_STATE_CONTINUE;
			}

			dtmfDecodingChannelAllocate(devIndex);
			if(CALL_MY_PEER(devIndex) != PBX_VALUE_INVALIDATE)
			{
				CALL_MY_PEER(devIndex) = PBX_VALUE_INVALIDATE;
			}
			
			return PBX_STATE_DIALING;
		}
		else if(EXT_FLASH_IS_ON(devIndex) )
		{
			DEVICE_EVENT_EMIT(devIndex, DEV_EVENT_STOP);
			
			CALL_FLASH_EVENT(devIndex, PBX_EVENT_OFFHOOK);
			
			if(CALL_CHECK_STATUS(CALL_MY_PEER(devIndex), PBX_STATE_RINGING) )
			{/* when the third party is in state of RINGING, stop ringing */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_CANCEL);
			}

			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_DIALING) )
			{
				dtmfDecodingChannelFree( devIndex);
			}

			EXT_FLASH_CANCEL(devIndex);
			return PBX_STATE_IN_CALL;
		}
	}

	if(IS_PSTN_LINE_DEVICE(devIndex) && PSTN_IS_DIRECT_MODE() )
	{
		pstnDidRinging(devIndex);
	}
	
	return PBX_STATE_CONTINUE;
}

