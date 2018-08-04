/*
* $Id: pbx_tone_if.c,v 1.11 2007/07/25 17:53:55 lizhijie Exp $
* Tone Interface based on Hardware Architecture
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"


#if 0
/* all phone's music is play by CD4066 */
void	extension_ringback_tone_play(INT8U devIndex, BOOL isEnable)
{
	if(isEnable)
	{
		PBX_SET_FLAGS(pbxHwStatus.ringbackStatus, devIndex);
	}
	else
	{
		PBX_CLEAR_FLAGS(pbxHwStatus.ringbackStatus, devIndex);
	}

	EXT_CTRL_RINGBACK();
}
#endif


void	pbx_if_start_ringing(INT8U x, BOOL isPlay)
{	
	if(isPlay == TRUE)
	{
		PBX_SET_FLAGS(pbxHwStatus.extRelayCtrl, x);
		EXT_CTRL_RINGING();
	}
	else
	{
		PBX_CLEAR_FLAGS(pbxHwStatus.extRelayCtrl, x);
		EXT_CTRL_RINGING();
		PBX_SET_FLAGS(pbxHwStatus.extRelayCtrl, x);
		
		EXT_RINGER_TIMER_SETUP(x, 2);	/* about 50 ms */
	}
}


void	pbx_if_start_music(INT8U x, BOOL isPlay)
{
	if( IS_EXT_DEVICE(x) )
	{
		if(isPlay)
		{
			PBX_SET_FLAGS(pbxHwStatus.musicStatus, x);
		}
		else
		{
			PBX_CLEAR_FLAGS(pbxHwStatus.musicStatus, x);
		}

		EXT_CTRL_MUSIC();
	}
	else if( IS_PSTN_LINE_DEVICE(x) )
	{
		switch_connect(x, CHANNEL_COL_MUSIC, isPlay);
	}
#if WITH_IP_VOICE_CHANNEL			
	else
	{
		switch_connect(CHANNEL_ROW_IP_R_1, GET_IP_CHANNEL_INDEX(x), isPlay );
		switch_connect(CHANNEL_ROW_IP_R_1, CHANNEL_COL_MUSIC, isPlay);
	}
#endif
}

void pbx_if_play_record(INT8U rowDev, BOOL isPlay)
{
	if(!IS_ROW_DEVICE(rowDev) )
		return;
	
	switch_connect(rowDev, CHANNEL_COL_RECORD,isPlay);
	if(isPlay == TRUE)
	{
		RECORDER_PLAY_START();
	}
	else
	{
		RECORDER_PLAY_STOP();
	}
}

void	pbx_if_ringback_tone_play(INT8U devIndex, BOOL isPlay)
{	
	if( IS_EXT_DEVICE(devIndex) )
	{
#if 1
		if(isPlay==TRUE)
		{
			PBX_SET_FLAGS(pbxHwStatus.extToneCtrl, devIndex);
		}
		else
		{
			PBX_CLEAR_FLAGS(pbxHwStatus.extToneCtrl, devIndex);
		}
		EXT_CTRL_TONE();
#else
		switch_connect(devIndex, CHANNEL_COL_TONE, isPlay);
#endif
	}
	else if( IS_PSTN_LINE_DEVICE(devIndex) )
	{
		switch_connect(devIndex, CHANNEL_COL_TONE, isPlay);
	}
#if WITH_IP_VOICE_CHANNEL			
	else
	{/* CONNECT IP col and Ringback tone col with a preallocated row(14 or 15) */
		switch_connect(CHANNEL_ROW_IP_R_1,  CHANNEL_COL_TONE, isPlay );	
		switch_connect(CHANNEL_ROW_IP_R_1,  GET_IP_CHANNEL_INDEX(devIndex), isPlay );
	}
#endif

}

