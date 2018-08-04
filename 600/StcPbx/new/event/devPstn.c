/*
* $Id: devPstn.c,v 1.12 2007/09/13 20:13:49 lizhijie Exp $
* PSTN Line device operations
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

void	_pstnTonePlay(INT8U devIndex, INT8U toneId)
{
	pbx_tone	code		*tone;
	INT8U	data		index;

	if(toneId == TONE_TYPE_NONE)
	{
		devToneRingbackPlay(devIndex, FALSE);
		pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = 0;
#if WITH_PSTN_HW_RINGER_DETECT
		pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#else
		pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#endif
		return;
	}
	
	tone =  &phoneParams[baseConfigs.signalType].tones[toneId];
	
#if WITH_PSTN_HW_RINGER_DETECT
	index =  (pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)])%tone->stepCount;
#else
	index =  (pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)])%tone->stepCount;
#endif
	/* IS_ODD, stop it */
	devToneRingbackPlay(devIndex, (IS_ODD(index))?FALSE:TRUE);

	pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = tone->steps[index];
#if WITH_PSTN_HW_RINGER_DETECT
	(pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)])++;
#else
	(pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)])++;
#endif

}


void	pstnDeviceOffhook(INT8U devIndex)
{
	PBX_SET_FLAGS(pstnDevices.relayCtrl, GET_PSTN_NUMBER(devIndex));
	PSTN_CTRL_HOOK();

	pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] = PSTN_HW_STATE_OFFHOOK;
}

void	pstnDeviceOnhook(INT8U devIndex)
{
	PBX_CLEAR_FLAGS(pstnDevices.relayCtrl, GET_PSTN_NUMBER(devIndex) );
	PSTN_CTRL_HOOK();

	pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] = PSTN_HW_STATE_ONHOOK_PENDING;
	pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = 3*PBX_50MS;
#if WITH_PSTN_HW_RINGER_DETECT
	pstnDevices.ringerOffCounter[GET_PSTN_NUMBER(devIndex)] = 0;
	pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#else
	pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#endif

}


/* return newly allocated line Index or 0xFF is failed, x is source device */
INT8U pstnDeviceAllocate(void )
{
	INT8U data i;

	for(i=PBX_FXS_COUNT; i< PBX_ROW_DEVICE_COUNT; i++)
	{
		if( PSTN_IS_ENABLE(i) && PSTN_IS_ONHOOK(i) )
		{
			pstnDeviceOffhook(i);
			return i;
		}
	}
	
	return PBX_VALUE_INVALIDATE;
}

void pstnDeviceFree(INT8U lineIndex )
{
	PSTN_SET_ONHOOK(lineIndex);
}

void _pstnBusyToneDetect(INT8U index, INT8U isOff)
{
#define	BUST_DETECT_BIT		7
	INT8U	data 	bytecnt;
	INT8U	data 	lastLevel ;	
		
	bytecnt = pstnDevices.cutbuf[index];
	lastLevel =  GET_BIT(bytecnt, BUST_DETECT_BIT);

#if WITH_UART_DEBUG_PSTN_BUSY_DETECT
	if(isOff == FALSE)
	{/* low level indicate that busy tone is playing */
//		uart_debug_2_int_value(UART_DEBUG_BUSY_DETCET, index, 1);
	}
	else
	{
//		uart_debug_2_int_value(UART_DEBUG_BUSY_DETCET, index, 0);
	}
#endif

	if(isOff == lastLevel )
	{/* with same level in this 50ms */
		if( (bytecnt&0x7F) > 20 )
		{/* no level change for about more than xxxxms, no busy tone : recount it */
			bytecnt = SET_BIT(isOff, BUST_DETECT_BIT);
		}
		else
		{/* count continued */
		}
		bytecnt++;/* bytecnt low nibble *50ms = voltage not change time */
	}
	else
	{
//			bytecnt ^= 0x10;		/* XOR */
#if WITH_UART_DEBUG_PSTN_BUSY_DETECT
		uart_debug_4_int_value(UART_DEBUG_BUSY_DETCET, index, (bytecnt&0x7F), GET_BIT(bytecnt, BUST_DETECT_BIT), isOff );
#endif
		if( ((bytecnt & 0x0f) > 6/*PBX_50MS*/) && ((bytecnt & 0x0f) < 12 ) )
		{/* level change within 400~550 ms, so no busy tone */
			pstnDevices.busyNum[index]++;
		}
		else
		{
			pstnDevices.busyNum[index]=0;
		}
		
		bytecnt = SET_BIT(isOff, BUST_DETECT_BIT);	/*save current level */
	}
		
	if(pstnDevices.busyNum[index] >= 8)	/* 4 period of busy tone, about 4 seconds */
	{
		pstnDevices.busyNum[index]=0;
		bytecnt = 0x00;
		/* notify other part */
		pstnDevices.busyDetected |= SET_BIT(0x01, index);
	}
	
	pstnDevices.cutbuf[index] = bytecnt;

#if 0		
	if (!isOff )
	{
		pstnDevices.vadTimeCount[index]=(unsigned int )5*60*20; /* 5 minutes */
	}
	else
	{
		if (pstnDevices.vadTimeCount[index]>0)
		{
			pstnDevices.vadTimeCount[index]--;
			if (pstnDevices.vadTimeCount[index] <= 0)
				pstnDevices.busyDetected |= SET_BIT(0x01,index);
		
		}
		else
		{
			pstnDevices.vadTimeCount[i]=0;
		}
	}
#endif
}

#if WITH_PSTN_HW_RINGER_DETECT
#if 0
void	_pstnHwRingerTest(INT8U index, BOOL isRinging)
{
	static INT8U data last[4];
	static INT8U data lastOn[4], lastOff[4];
	
	if(last[index] != isRinging)
	{
#if WITH_UART_DEBUG_PSTN_RINGER
		uart_debug_4_int_value(UART_DEBUG_PSTN_RINGER,index, last[index], 
			pstnDevices.ringerOnCounter[index]-lastOn[index] , pstnDevices.ringerOffCounter[index]-lastOff[index]);
#endif
		lastOn[index] = pstnDevices.ringerOnCounter[index];
		lastOff[index] = pstnDevices.ringerOffCounter[index] ;
		last[index] = isRinging;
	}
	if(isRinging)
	{/* low level :ringer is ON now */
		pstnDevices.ringerOnCounter[index] = pstnDevices.ringerOnCounter[index]+1;
		if(pstnDevices.ringerOnCounter[index]== 255)
			pstnDevices.ringerOnCounter[index] = 0;
	}
	else 
	{
		pstnDevices.ringerOffCounter[index] = pstnDevices.ringerOffCounter[index]+1;
		if( pstnDevices.ringerOffCounter[index] == 255)
			pstnDevices.ringerOffCounter[index] = 0;
	}

#if 0
	if(pstnDevices.vadTimeCount[index] > 0)
	{
		(pstnDevices.vadTimeCount[index])--;
	}
	else
	{
//		if(pstnDevices.vadTimeCount[index] == 0)
		{
			pstnDevices.vadTimeCount[index] = 20;
			
#if WITH_UART_DEBUG_PSTN_RINGER
			uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, PSTN_FLOW_BEGIN_RINGER_ON, 
				pstnDevices.ringerOnCounter[index] , pstnDevices.ringerOffCounter[index]);
#endif
		}
	}
#endif

}
#endif

void	_pstnCheckRinger(INT8U index, BOOL isRinging)
{
	INT8U	data		devIndex = index+PBX_FXS_COUNT;
	
	if(isRinging)
	{/* low level :ringer is ON now */
		if(pstnDevices.hwStatus[index] == PSTN_HW_STATE_ONHOOK ||
			pstnDevices.hwStatus[index] == PSTN_HW_STATE_RINGER_OFF )
		{/* ringer begin */
			if(pstnDevices.hwStatus[index] == PSTN_HW_STATE_ONHOOK)
			{
				callDevices[devIndex].timeoutCounter = 0;
				pstnDevices.ringerOffCounter[index] = 0;
				pstnDevices.ringerOnCounter[index] = 0;
				/* not send first RINGER_ON event */
				CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_ON);
			}
			
			else //if(hwStatus[GET_PSTN_NUMBER(q)] == PSTN_RINGER_OFF)
			{
				CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_ON);
#if WITH_UART_DEBUG_PSTN_RINGER
				uart_debug_4_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_ON,
					pstnDevices.ringerOnCounter[index] , pstnDevices.ringerOffCounter[index]);
#endif
#if 0
				if( (pstnDevices.ringerOnCounter[index] >= phoneParams[baseConfigs.signalType].ringer_on_time -3 ) &&
					(pstnDevices.ringerOnCounter[index] <=  phoneParams[baseConfigs.signalType].ringer_on_time +3 ) )
				{
					callDevices[devIndex].timeoutCounter++;
				}
#endif
			}
			pstnDevices.hwStatus[index] = PSTN_HW_STATE_RINGER_ON;
			pstnDevices.ringerOnCounter[index] = 1;
			
		}
		
		(pstnDevices.ringerOnCounter[index])++;
	}
	else 
	{
		if(pstnDevices.hwStatus[index] == PSTN_HW_STATE_RINGER_ON )
		{/* ringer end */
			if(LOCAL_IS_JAPAN() )
			{
				if(/* (pstnDevices.ringerOffCounter[index] >= TIMEOUT_CAR_LENGTH-2 ) &&
					(pstnDevices.ringerOffCounter[index] <=  TIMEOUT_CAR_LENGTH+1 ) &&*/
					(pstnDevices.ringerOnCounter[index] >= TIMEOUT_CAR_LENGTH-2 ) &&
					(pstnDevices.ringerOnCounter[index] <=  TIMEOUT_CAR_LENGTH+2 ) )
				{
					CALL_EVENT_EMIT(devIndex, PBX_EVENT_CAR);
#if WITH_UART_DEBUG_PSTN_RINGER
					uart_debug_4_int_value(UART_DEBUG_PSTN_RINGER,index, PSTN_FLOW_BEGIN_CAR, pstnDevices.ringerOnCounter[index], pstnDevices.ringerOffCounter[index] );
#endif
				}
				else
				{
/*				
			if( (pstnDevices.ringerOnCounter[index] >= phoneParams[baseConfigs.signalType].ringer_on_time -3 ) &&
				(pstnDevices.ringerOnCounter[index] <=  phoneParams[baseConfigs.signalType].ringer_on_time +3 ) )
*/				
					CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_OFF);
#if WITH_UART_DEBUG_PSTN_RINGER
					uart_debug_4_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_OFF, pstnDevices.ringerOnCounter[index], pstnDevices.ringerOffCounter[index] );
#endif
				}
			}
			else
			{
				CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_OFF);
			}
			
		
			pstnDevices.hwStatus[index] = PSTN_HW_STATE_RINGER_OFF;
			pstnDevices.ringerOffCounter[index] = 1;
		}
		
		(pstnDevices.ringerOffCounter[index])++;

		if(pstnDevices.hwStatus[index] != PSTN_HW_STATE_ONHOOK)
		{
			if(pstnDevices.ringerOffCounter[index] > (phoneParams[baseConfigs.signalType].ringer_off_time + 12) )
			{
				CALL_EVENT_EMIT(devIndex, PBX_EVENT_ONHOOK);/* Send to Call In TRYING state */
				pstnDevices.hwStatus[index] = PSTN_HW_STATE_ONHOOK;
#if WITH_UART_DEBUG_PSTN_RINGER
				uart_debug_4_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_TIMEOUT, pstnDevices.ringerOnCounter[index], pstnDevices.ringerOffCounter[index] );
#endif
			}
		}

	}
	
}

#else
void	_pstnCheckRinger(INT8U index, BOOL isRinging)
{
	INT8U	data		devIndex = index+PBX_FXS_COUNT;

	switch( pstnDevices.hwStatus[index])
	{
		case PSTN_HW_STATE_ONHOOK:
		{
			if(isRinging)
			{
				(pstnDevices.ringerCounter[index])++;
				if(pstnDevices.ringerCounter[index]>= 2)
				{
					/* this is the first ringer on event, not count it */
					CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_ON);

#if WITH_UART_DEBUG_PSTN_RINGER
					uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER,index, PSTN_FLOW_BEGIN_RINGER_ON, callDevices[devIndex].timeoutCounter );//pstnDevices.ringerCounter[index] );
#endif
					if(LOCAL_IS_JAPAN() && PSTN_CALLID_IS_ON() )
					{/* CAR */
						PSTN_HW_NEW_STATE(index, PSTN_HW_STATE_RINGER_ON_PENDING, TIMEOUT_CAR_LENGTH*4);
					}
					else
					{
						PSTN_HW_NEW_STATE(index, PSTN_HW_STATE_RINGER_OFF_PENDING, (phoneParams[baseConfigs.signalType].ringer_on_time+PBX_100MS) );
					}
				}
			}
			else
			{
				pstnDevices.ringerCounter[index] = 0;
			}
			break;
		}
		case PSTN_HW_STATE_RINGER_OFF_DETECT:
		{
			if(isRinging)
			{
				(pstnDevices.ringerCounter[index])++;
				if(pstnDevices.ringerCounter[index]>= 2)
				{
					callDevices[devIndex].timeoutCounter++;
					CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_ON);
					PSTN_HW_NEW_STATE(index, PSTN_HW_STATE_RINGER_OFF_PENDING, (phoneParams[baseConfigs.signalType].ringer_on_time+PBX_100MS));
#if WITH_UART_DEBUG_PSTN_RINGER
					uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_ON, callDevices[devIndex].timeoutCounter );
#endif
					break;
				}
			}
			else
			{
				pstnDevices.ringerCounter[index] = 0;
			}
			break;
		}
		default:
			break;
	}

}

#endif

void _pstnDevTimeout(INT8U index)
{
	INT8U	data		devIndex = index+PBX_FXS_COUNT;
	
	switch(pstnDevices.hwStatus[index] )
	{
		case PSTN_HW_STATE_ONHOOK_PENDING:
			pstnDevices.hwStatus[index] = PSTN_HW_STATE_ONHOOK;
			CALL_EVENT_EMIT(devIndex, PBX_EVENT_TIMEOUT);
			break;
#if WITH_PSTN_HW_RINGER_DETECT
#else
		case PSTN_HW_STATE_RINGER_ON_PENDING:
		{
			CALL_EVENT_EMIT(devIndex, PBX_EVENT_CAR);
			PSTN_HW_NEW_STATE(index, PSTN_HW_STATE_RINGER_BLOCKED, (phoneParams[baseConfigs.signalType].ringer_off_time +5*PBX_100MS) );
#if WITH_UART_DEBUG_PSTN_RINGER
			uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_BEGIN_CAR, callDevices[devIndex].timeoutCounter );
#endif
			break;
		}
		case PSTN_HW_STATE_RINGER_OFF_PENDING:
		{
			CALL_EVENT_EMIT(devIndex, PBX_EVENT_RINGER_OFF);
			PSTN_HW_NEW_STATE(index, PSTN_HW_STATE_RINGER_OFF_DETECT, (phoneParams[baseConfigs.signalType].ringer_off_time +5*PBX_100MS) );
#if WITH_UART_DEBUG_PSTN_RINGER
			uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_OFF, callDevices[devIndex].timeoutCounter);
#endif
			break;
		}
		case PSTN_HW_STATE_RINGER_OFF_DETECT:
		{
			PSTN_HW_NEW_STATE(index,PSTN_HW_STATE_ONHOOK, 0);
			CALL_EVENT_EMIT(devIndex, PBX_EVENT_TIMEOUT);
#if WITH_UART_DEBUG_PSTN_RINGER
			uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER, index, PSTN_FLOW_RINGER_TIMEOUT, pstnDevices.ringerCounter[index] );
#endif
			break;
		}
#endif
		case PSTN_HW_STATE_OFFHOOK:
			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_TRYING) )
			{
				_pstnTonePlay(devIndex, TONE_TYPE_RINGBACK);
			}
			else if(CALL_CHECK_STATUS(devIndex, PBX_STATE_DIALING) )
			{
#if 0
#if WITH_PSTN_HW_RINGER_DETECT
				if( pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] == 0)
				{
					(pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)])++;
				}
#else
				if( pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] == 0)
				{
					(pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)])++;
				}
#endif
				else
#endif

				{
					devRecordPlay(devIndex, TRUE);
					pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = RECORD_TIMER_LENGTH*2;	/* about 10 seconds */
#if WITH_PSTN_HW_RINGER_DETECT
					pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#else
					pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#endif
				}
			}
			
			break;
		default:
			break;
	}
}

void pstnTimerTask50ms(void)
{
	INT8U	data linesStates, index;
	BOOL	isRinging;

	linesStates = pbx_read_hw_status(HW_STATUS_PSTN_HOOK_BUSY);

	for(index=0;index<PBX_FXO_COUNT;index++)
	{
		if(PSTN_IS_DISABLE(index+PBX_FXS_COUNT))
			continue;

		if(pstnDevices.timeout[index] > 0)
		{
			(pstnDevices.timeout[index]) --;
			if(pstnDevices.timeout[index] <= 0)
			{
				DEVICE_EVENT_EMIT(index+PBX_FXS_COUNT, DEV_EVENT_TIMEOUT );
			}
		}
		
		if(PSTN_IS_OFFHOOK(index+PBX_FXS_COUNT) ) //PBX_CHECK_FLAGS(pstnDevices.relayCtrl, index) )
		{/* when it is offhook */
		
			_pstnBusyToneDetect(index, GET_BIT(linesStates, index) );

			/* after busy detected */
			if( PBX_CHECK_FLAGS(pstnDevices.busyDetected, index) )
//			&& PBX_CHECK_FLAGS(pstnDevices.relayCtrl, index))
			 {
			 	pstnDeviceOnhook(index+PBX_FXS_COUNT);
				CALL_EVENT_EMIT(index+PBX_FXS_COUNT, PBX_EVENT_ONHOOK);
				
				PBX_CLEAR_FLAGS(pstnDevices.busyDetected, index);
#if WITH_UART_DEBUG_PSTN_BUSY_DETECT
				uart_debug_2_int_value(UART_DEBUG_BUSY_DETCET, index+PBX_FXS_COUNT, PBX_EVENT_ONHOOK);
#endif
			 }
		}
		else 
		{/* when it is not offhook */
			isRinging = (!PBX_CHECK_FLAGS(linesStates, (index+PBX_FXO_COUNT) ) );

#if 0//TEST
#if WITH_PSTN_HW_RINGER_DETECT
//			if(index == 0)
			{
				_pstnHwRingerTest(index, isRinging);
			}
#else
			else
//			if(index == 0)
			{
//				_pstnHwRingerTest(index, isRinging);
			}
//			else

#endif
#endif
			{
				_pstnCheckRinger(index, isRinging);
			}
		}
	}

	pstnDevices.busyDetected = 0;
}

void	devPstnEventHandler(INT8U devIndex, dev_event_t event)
{
	INT8U data 	channel;
	if(!IS_PSTN_LINE_DEVICE(devIndex) )
		return ;
	
	switch(event)
	{
		case DEV_EVENT_TONE_DIALING:
			/* play record alert voice here */
			devRecordPlay(devIndex, TRUE);
			pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = RECORD_TIMER_LENGTH*2;	/* about 10 seconds */
#if WITH_PSTN_HW_RINGER_DETECT
			pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#else
			pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#endif

			break;
		case DEV_EVENT_MUSIC:
			devMusicPlay(devIndex, TRUE);
			break;
		case DEV_EVENT_TONE_BUSY:
			if(PSTN_IS_OFFHOOK(devIndex))
				pstnDeviceOnhook(devIndex);
			else
				pstnDeviceFree(devIndex);
			break;
		case DEV_EVENT_TONE_RINGBACK:
			_pstnTonePlay(devIndex, TONE_TYPE_RINGBACK);
			break;
		case DEV_EVENT_TONE_CALLWAIT:
			break;
		case DEV_EVENT_RINGING:
			/* EXT|PSTN-->PSTN, has been offhook, but IP-->PSTN, should be offhook here */
			pstnDeviceOffhook(devIndex);
			/* offhook and send out calling digits */
			if(dtmfEncodeChannelFind(devIndex) == PBX_VALUE_INVALIDATE)
			{/* dial PSTN phone number with a phone */
				if(IS_ROW_DEVICE(CALL_MY_PEER(devIndex)) )
				{
					CALL_EVENT_EMIT(devIndex, PBX_EVENT_OFFHOOK);
				}
			}
			else
			{/* this is for call transfer operation */
				dtmfEncoderPlayStart( devIndex);
			}			
			break;
		case DEV_EVENT_OK:/* Call from PSTN side */
			pstnDeviceOffhook(devIndex );
			CallerIdChannelFree(devIndex);
			devMusicPlay(devIndex, FALSE);
			_pstnTonePlay(devIndex, TONE_TYPE_NONE );
			break;
		case DEV_EVENT_STOP:
			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_DIALING) )
			{
				devRecordPlay(devIndex, FALSE);
				pstnDevices.timeout[GET_PSTN_NUMBER(devIndex)] = 0;
#if WITH_PSTN_HW_RINGER_DETECT
				pstnDevices.ringerOnCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#else
				pstnDevices.ringerCounter[GET_PSTN_NUMBER(devIndex)] = 0;
#endif
				break;
			}
			devMusicPlay(devIndex, FALSE);
/* no op is needed when in state of TRYING 
			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_TRYING) )
				(devIndex, FALSE);
*/			
		case DEV_EVENT_CANCEL:
//			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_IN_CALL) )
				pstnDeviceOnhook( devIndex);
#if 1			
			if(CALL_CHECK_STATUS(devIndex, PBX_STATE_RINGING) )
				dtmfEncodeChannelFree(devIndex);
#endif			
			break;
		case DEV_EVENT_CALLER_DIGITS:/* not used for PSTN line */
			break;
		case DEV_EVENT_CALLEE_DIGITS:
			channel = dtmfDecodingChannelFind(CALL_MY_PEER(devIndex) );
			if(channel != PBX_VALUE_INVALIDATE)
			{/* IP-->PSTN */
				dtmfEncodeChannelAllocate(devIndex, 
					decodingChannels[channel].digits.value, 
					decodingChannels[channel].digits.length );
				dtmfDecodingChannelFree(CALL_MY_PEER(devIndex));

				dtmfEncoderPlayStart(devIndex);
			}
			break;
		case DEV_EVENT_TIMEOUT:
			_pstnDevTimeout(GET_PSTN_NUMBER(devIndex) );
			break;
		default:
			break;
	}
}


