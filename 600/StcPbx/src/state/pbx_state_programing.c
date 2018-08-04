/*
* $Id: pbx_state_programing.c,v 1.10 2007/07/25 17:56:26 lizhijie Exp $
*/

/*
* this state are used by both PBX program and IP Record mode
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


INT8U	ext_programing_state_digits(INT8U devIndex)
{
	INT8U	data channel;

	if(PBX_IP_RECORD_MODE_IS_ON() )
	{
		return STATE_CONTINUE;
	}

	if( (channel = decodingchannel_find(devIndex)) == PBX_VALUE_INVALIDATE)
		return STATE_BUSY;

#if 0
	if(decodingChannels[channel].digits.length <= 3)
	{
		return STATE_CONTINUE;
	}
#endif

	return pbx_program_enter_handle(devIndex, &(decodingChannels[channel].digits) );
}

INT8U	ext_programing_state_onhook(INT8U devIndex)
{
	INT8U	data channel;

	if( PBX_IP_RECORD_MODE_IS_ON() ||deviceStatus[devIndex].peerId != PBX_VALUE_INVALIDATE )
	{
		if( IS_IP_DEVICE(deviceStatus[devIndex].peerId) )
		{
			DISCONNECT(devIndex, deviceStatus[devIndex].peerId);
			PBX_COLUMN_FREE_IP(deviceStatus[devIndex].peerId);
		}

		return STATE_IDLE;
	}
	
	if( (channel = decodingchannel_find(devIndex)) != PBX_VALUE_INVALIDATE)
	{
		decodingchannel_free(devIndex);
	}

	DISCONNECT(devIndex, CHANNEL_COL_MUSIC);
	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	
	return STATE_IDLE;
}

INT8U	ext_programing_state_timeout(INT8U devIndex)
{/* goto BUSY state to playing busy tone */
#if 1
	INT8U	data channel;

	if(PBX_IP_RECORD_MODE_IS_ON())
	{
		return STATE_CONTINUE;
	}
	
	if( (channel = decodingchannel_find(devIndex)) != PBX_VALUE_INVALIDATE)
	{
		decodingchannel_free(devIndex);
	}


	DISCONNECT(devIndex, CHANNEL_COL_MUSIC);

	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	
	return STATE_BUSY;
#else
	return STATE_CONTINUE;
#endif
}


void ext_programing_state_enter(INT8U devIndex)
{
	if(PBX_IP_RECORD_MODE_IS_OFF() )
	{
		DEVICE_TIMER_START(devIndex, DIALING_TIMER_LENGTH);
	}
	
	return;
}


INT8U	ext_rering_state_onhook(INT8U devIndex)
{
//	pbx_program_exit_handle(devIndex);//, &(decodingChannels[channel].digits) );
	devIndex = devIndex;
	PBX_PROGRAM_MODE_CANCEL();
	return STATE_IDLE;
}


