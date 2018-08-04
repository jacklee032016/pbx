/*
* $Id: devExt.c,v 1.7 2007/09/13 20:13:49 lizhijie Exp $
* Tone Interface based on Hardware Architecture
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

#define	IS_RINGER_EXTRENAL(extIndex)	\
	((IS_PSTN_LINE_DEVICE(CALL_MY_PEER(extIndex))) ||(IS_IP_DEVICE(CALL_MY_PEER(extIndex))) )

#define	IS_DIFFER_RINGING()	\
	(boolDifferRinging == TRUE)

/* can be used by IP device as timer out machanism */
void	_extTonePlay(INT8U extIndex, INT8U toneId)
{
	pbx_tone	code		*tone;
	INT8U	data		index;

	if(toneId == TONE_TYPE_NONE)
	{
		devToneRingbackPlay(extIndex, FALSE);
		extDevices.timeoutCounter[extIndex] = 0;
		extDevices.timeout[extIndex] = 0;
		return;
	}
	
	tone =  &phoneParams[baseConfigs.signalType].tones[toneId];
	
	index =  (extDevices.timeoutCounter[extIndex])%tone->stepCount;
	/* IS_ODD, stop it */
	devToneRingbackPlay(extIndex, (IS_ODD(index))?FALSE:TRUE);

	(extDevices.timeoutCounter[extIndex])++;
	extDevices.timeout[extIndex] = tone->steps[index];

}


/* can be used by PSTN and IP device as Timeout Mechanism */
void	_extRingerPlay(INT8U extIndex, INT8U ringerId)
{
	pbx_tone	code		*ringer;
	INT8U	data		index;

	if(ringerId == RINGER_TYPE_NONE)
	{
		extDevices.timeoutCounter[extIndex] = 0;
		extDevices.timeout[extIndex] = 0;
		extDevices.status[extIndex] = EXT_STATE_IDLE;
		PBX_CLEAR_FLAGS(extDevices.relayCtrl, extIndex);
		EXT_CTRL_RINGING();
		return;
	}
	
	ringer = &phoneParams[baseConfigs.signalType].ringers[ringerId];
	
	index =  extDevices.timeoutCounter[extIndex]%ringer->stepCount;
	
	if(!IS_ODD(extDevices.timeoutCounter[extIndex]) )
	{/* IS_ODD, stop it */
		PBX_SET_FLAGS(extDevices.relayCtrl, extIndex);
		EXT_CTRL_RINGING();
		extDevices.status[extIndex] = EXT_STATE_RINGER_ON;

		if(index == 0)
		{
			CALL_EVENT_EMIT(extIndex, PBX_EVENT_RINGER_ON);
		}
	}
	else
	{
		if(extDevices.status[extIndex] == EXT_STATE_RINGER_ON)
		{
			PBX_CLEAR_FLAGS(extDevices.relayCtrl, extIndex);
			EXT_CTRL_RINGING();
			PBX_SET_FLAGS(extDevices.relayCtrl, extIndex);
			extDevices.status[extIndex] = EXT_STATE_RINGEROFF_PENDING;
			extDevices.timeout[extIndex] = 1;	/* delay about 50 ms */

			return;
		}
		else
		{/* state of RINGEROFF_PENDING */
			PBX_CLEAR_FLAGS(extDevices.relayCtrl, extIndex);
			EXT_CTRL_RINGING();
			extDevices.status[extIndex] = EXT_STATE_RINGER_OFF;

			if((ringerId == RINGER_TYPE_INTERNAL && index == 3) ||
				(ringerId == RINGER_TYPE_EXTERNAL && index == 1) )
			{
				CALL_EVENT_EMIT(extIndex, PBX_EVENT_RINGER_OFF);
			}
		}
	}

	(extDevices.timeoutCounter[extIndex])++;
	extDevices.timeout[extIndex] = ringer->steps[index];

	return ;
}


void	check_ext_hook_event(void)
{
	INT8U	data extStates, q;
	INT8U	data m=0x01;

	q = pbx_read_hw_status(HW_STATUS_EXT_HOOK);
	extStates = (q^extDevices.hookStatus)&(~extDevices.relayCtrl);
	if(extStates == 0)
	{/* No hook status has been changed this time, except which is ringing now */
		return;
	}
	extDevices.hookStatus = q|extDevices.relayCtrl;
	
	for(q=0;q<PBX_FXS_COUNT;q++)
	{
		if( ((extStates&m) != 0 ) )
		{/* hook status of this EXT has changed now */
			if( ((extDevices.hookStatus&m)==0 )&&((extDevices.relayCtrl&m)== 0) )
			{/* from High to Low */
				if(EXT_FLASH_TIMER_IS_ON(q) )
				{
					EXT_FLASH_TIMER_CANCEL(q);
					CALL_EVENT_EMIT(q, PBX_EVENT_FLASH);
				}
				else
				{
					CALL_EVENT_EMIT(q, PBX_EVENT_OFFHOOK);
				}
				extDevices.status[q] = EXT_STATE_OFFHOOK;
			}
			else if( (extDevices.relayCtrl&m)== 0 )
			{/* Hook status of this EXT : from Low level to High level */
				{
					if(! EXT_FLASH_TIMER_IS_ON(q) )
					{
						EXT_FLASH_TIMER_SETUP(q);
					}
				}
			}
		}
		m=m<<1;
	}	
	
}


void extTimerTask50ms(void)
{
	INT8U data	i;
	
	for(i=0; i< PBX_FXS_COUNT; i++)
	{
		if( extDevices.timeout[i] >0 )
		{
			(extDevices.timeout[i]) --;
			if( extDevices.timeout[i] <= 0 )
			{
				DEVICE_EVENT_EMIT(i, DEV_EVENT_TIMEOUT);
			}
		}
		
		if( extDevices.hookTimer[i] >0 )
		{
			(extDevices.hookTimer[i])--;
			if( extDevices.hookTimer[i] <= 0 )
			{/* This is not a FLASH event when in state of IN_CALL */
				CALL_EVENT_EMIT(i, PBX_EVENT_ONHOOK);
				extDevices.status[i] = EXT_STATE_IDLE;
				EXT_FLASH_TIMER_CANCEL(i);
			}
		}
	}

	check_ext_hook_event();
}

void _extDevTimeout(INT8U extIndex)
{
	INT8U data toneId = TONE_TYPE_NONE;
	INT8U data ringerId = RINGER_TYPE_NONE;
	
	switch(callDevices[extIndex].status)
	{
		case PBX_STATE_DIALING:
			if(LOCAL_IS_JAPAN() )
				toneId = TONE_TYPE_PDT;
			else
				toneId = TONE_TYPE_DIAL;
			break;
		case PBX_STATE_BUSY:
			toneId = TONE_TYPE_BUSY;
			break;
		case PBX_STATE_TRYING:
			toneId = TONE_TYPE_RINGBACK;
			break;
		case PBX_STATE_WAIT:
			toneId = TONE_TYPE_CALLWAIT;
			break;
		case PBX_STATE_IN_CALL:
			/* uncommented this line for CALLWAIT tone, 2007.09.13 lizhijie */
			toneId = TONE_TYPE_CALLWAIT;
			break;

		case PBX_STATE_CAR:
			ringerId = RINGER_TYPE_CAR;
			break;
		case PBX_STATE_RINGING:
			if(IS_RINGER_EXTRENAL(extIndex) || !IS_DIFFER_RINGING() )
				ringerId = RINGER_TYPE_EXTERNAL;
			else
				ringerId = RINGER_TYPE_INTERNAL;
			break;
		case PBX_STATE_PBX_RERING:
			ringerId = RINGER_TYPE_INTERNAL;
			break;
		default :
			break;
	}
	
	if(toneId != TONE_TYPE_NONE)
	{
		_extTonePlay(extIndex, toneId);
	}

	if(ringerId != RINGER_TYPE_NONE)
	{
		_extRingerPlay(extIndex, ringerId);
	}
}

void _extGetCallerDigits(INT8U devIndex)
{
	INT8U data channel;
	if(IS_EXT_DEVICE(CALL_MY_PEER(devIndex) ))
	{
		dtmfEncodeChannelAllocate(devIndex, 
			extConfigs.phoneNumbers[CALL_MY_PEER(devIndex)].value, 
			extConfigs.phoneNumbers[CALL_MY_PEER(devIndex)].length );
	}
	else if(IS_CALL_DEVICE(CALL_MY_PEER(devIndex) ) )
	{
		channel = CallerIdChannelFind(CALL_MY_PEER(devIndex) );
		if(channel != PBX_VALUE_INVALIDATE)
		{
			dtmfEncodeChannelAllocate(devIndex, 
				callerIdData[channel].digits.value, 
				callerIdData[channel].digits.length);
			if(!IS_PSTN_LINE_DEVICE(CALL_MY_PEER(devIndex)))
			{
				CallerIdChannelFree(CALL_MY_PEER(devIndex));
			}
		}
	}
}

void	devExtEventHandler(INT8U devIndex, dev_event_t event)
{
	INT8U  data toneId;
	if(!IS_EXT_DEVICE(devIndex) )
		return;// FALSE;
	
	switch(event)
	{
		case DEV_EVENT_TONE_DIALING:
			if(LOCAL_IS_JAPAN() )
				toneId = TONE_TYPE_PDT;
			else
				toneId = TONE_TYPE_DIAL;
			_extTonePlay(devIndex, toneId);
			break;
		case DEV_EVENT_TONE_BUSY:
			_extTonePlay(devIndex, TONE_TYPE_BUSY);
			break;
		case DEV_EVENT_TONE_RINGBACK:
			_extTonePlay(devIndex, TONE_TYPE_RINGBACK);
			break;
		case DEV_EVENT_TONE_CALLWAIT:
			_extTonePlay(devIndex, TONE_TYPE_CALLWAIT);
			break;
		case DEV_EVENT_MUSIC:
			devMusicPlay(devIndex, TRUE);
			break;
		case DEV_EVENT_RINGING:
			if(IS_RINGER_EXTRENAL(devIndex) || !IS_DIFFER_RINGING() )
				toneId = RINGER_TYPE_EXTERNAL;
			else
				toneId = RINGER_TYPE_INTERNAL;
			_extGetCallerDigits(devIndex);
			_extRingerPlay(devIndex, toneId);
			if(IS_CALL_DEVICE(CALL_MY_PEER(devIndex) ) )
			{/* peer is still in RINGING state */
				CALL_PEER_EVENT(devIndex, PBX_SIGNAL_RINGING);
			}
			break;
		case DEV_EVENT_CAR_RINGING:
			_extGetCallerDigits(devIndex);
			_extRingerPlay(devIndex, RINGER_TYPE_CAR);
			break;
		case DEV_EVENT_CALLER_DIGITS:
			_extGetCallerDigits(devIndex);
			break;
		case DEV_EVENT_CALLEE_DIGITS:/* not used for EXT */
			
			break;
		case DEV_EVENT_CANCEL:
			if(PBX_PROGRAM_MODE_IS_ON(devIndex))
			{
				PBX_PROGRAM_MODE_CANCEL();
			}
//			break;
		case DEV_EVENT_OK:
//			break;
		case DEV_EVENT_STOP:
			devMusicPlay(devIndex, FALSE);
			_extTonePlay(devIndex, TONE_TYPE_NONE);
			_extRingerPlay(devIndex,RINGER_TYPE_NONE );
			break;
		case DEV_EVENT_TIMEOUT:
			_extDevTimeout(devIndex);
			break;
		default:
			break;
	}
}

