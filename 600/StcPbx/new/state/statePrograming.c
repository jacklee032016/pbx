/*
* $Id: statePrograming.c,v 1.2 2007/08/10 20:15:51 lizhijie Exp $
*/

/*
* this state are used by both PBX program and IP Record mode
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"


INT8U	ext_programing_state_digits(INT8U devIndex)
{
	INT8U	data channel;

	if(PBX_IP_RECORD_MODE_IS_ON() )
	{
		return PBX_STATE_CONTINUE;
	}

	if( (channel = dtmfDecodingChannelFind(devIndex)) == PBX_VALUE_INVALIDATE)
		return PBX_STATE_BUSY;

#if 0
	if(decodingChannels[channel].digits.length <= 3)
	{
		return PBX_STATE_CONTINUE;
	}
#endif

	return pbx_program_enter_handle(devIndex, &(decodingChannels[channel].digits) );
}

INT8U	ext_programing_state_onhook(INT8U devIndex)
{
	INT8U	data channel;

	if( PBX_IP_RECORD_MODE_IS_ON() ||CALL_MY_PEER(devIndex) != PBX_VALUE_INVALIDATE )
	{
		if( IS_IP_DEVICE(CALL_MY_PEER(devIndex) ) )
		{
			DISCONNECT(devIndex, CALL_MY_PEER(devIndex) );
			PBX_COLUMN_FREE_IP(CALL_MY_PEER(devIndex) );
		}

		return PBX_STATE_IDLE;
	}
	
	if( (channel = dtmfDecodingChannelFind(devIndex)) != PBX_VALUE_INVALIDATE)
	{
		dtmfDecodingChannelFree(devIndex);
	}

	DISCONNECT(devIndex, CHANNEL_COL_MUSIC);
	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	
	return PBX_STATE_IDLE;
}

INT8U	ext_programing_state_timeout(INT8U devIndex)
{/* goto BUSY state to playing busy tone */
#if 1
	INT8U	data channel;

	if(PBX_IP_RECORD_MODE_IS_ON())
	{
		return PBX_STATE_CONTINUE;
	}
	
	if( (channel = dtmfDecodingChannelFind(devIndex)) != PBX_VALUE_INVALIDATE)
	{
		dtmfDecodingChannelFree(devIndex);
	}


	DISCONNECT(devIndex, CHANNEL_COL_MUSIC);

	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	
	return PBX_STATE_BUSY;
#else
	return PBX_STATE_CONTINUE;
#endif
}


void ext_programing_state_enter(INT8U devIndex)
{
	if(PBX_IP_RECORD_MODE_IS_OFF() )
	{
		CALL_TIMER_START(devIndex, DIALING_TIMER_LENGTH);
	}
	
	return;
}


INT8U	ext_rering_state_onhook(INT8U devIndex)
{
//	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	devIndex = devIndex;
	PBX_PROGRAM_MODE_CANCEL();
	return PBX_STATE_IDLE;
}


