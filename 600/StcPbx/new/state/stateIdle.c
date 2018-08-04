/*
* $Id: stateIdle.c,v 1.5 2007/09/01 21:25:43 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

/*
* IDLE state, offhook event is handled as caller and ringing Signal is process as callee
*/

INT8U	ext_idle_state_digits(INT8U devIndex)
{
#if WITH_UART_DEBUG_SELFTEST
	uart_debug_int_value(UART_DEBUG_SELFTEST, devIndex);//, 0x90);
#endif
	return self_test_exit_handle(devIndex, SELFTEST_CAUSE_ERROR_DIGIT);
}

/* return new state after event has been processed; failed : return 0xff */
INT8U ext_idle_state_offhook(INT8U devIndex)
{
	INT8U 	data 	ipChannel;
	
	if(IS_EXT_DEVICE(devIndex) && PBX_IP_RECORD_MODE_IS_ON() )
	{/* when PBX enter into IP recording mode */
		ipChannel = findFreeIpChannel();
		if(ipChannel == PBX_VALUE_INVALIDATE)
		{
			return PBX_STATE_BUSY;
		}

		uart_tx_int_value(CMD_RECORD_REQUEST, ipChannel);

		PBX_COLUMN_BUSY(ipChannel);
		CONNECT(devIndex, ipChannel);
		
		ipChannel = GET_IP_DEVICE_INDEX(ipChannel);
		/* assigned dest, so do not dialing in DIALING state */
		CALL_MY_PEER(devIndex) = ipChannel;
		return PBX_STATE_PROGRAM;
	}

	
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		return PBX_STATE_CONTINUE;
	}

	/* ignore offhook event rx from IP side when play as stop_tingback_tone */
	if( (IS_IP_DEVICE(devIndex) )&&
		(CALL_MY_PEER(devIndex) == PBX_VALUE_INVALIDATE) )
	{
			return PBX_STATE_CONTINUE;
	}
	
	if(!dtmfDecodingChannelAllocate(devIndex))
	{/* For IP and EXT, enter into BUSY state to feedback Busy signal */
		return  PBX_STATE_BUSY;
	}

	return PBX_STATE_DIALING;
}

INT8U ext_idle_state_ringing(INT8U devIndex)
{
	if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
	{
		return PBX_STATE_PBX_RERING;
	}
	
	if(IS_EXT_DEVICE(devIndex)||IS_PSTN_LINE_DEVICE(devIndex) )
	{
		PBX_ROW_BUSY(devIndex);
		if(CAR_IS_SUPPORT(devIndex) )
		{
			return PBX_STATE_CAR;
		}
	}
	
	return PBX_STATE_RINGING;
}

INT8U ext_idle_state_car(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )//&& LOCAL_IS_JAPAN() )
	{
		CallerIdChannelAllocate(devIndex );
		if(LOCAL_IS_JAPAN())
		{
			CALL_TIMER_START(devIndex, CAR_PENDING_TIMER_LENGTH);/* NTT specs P.76, about 2 seconds for CallerID rx */
			pstnDeviceOffhook(devIndex );
		}
	}
	
	return PBX_STATE_CONTINUE;
}

INT8U ext_idle_state_ringeron(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
#if 0	
		if(callDevices[devIndex].timeoutCounter == 2 ||
			callDevices[devIndex].timeoutCounter == 3)
		{
			if(LOCAL_IS_CHINA() )
		}
#else
//		pstn_callerid_end(devIndex );
		CallerIdChannelAllocate(devIndex );
#endif		
	}
	
	return PBX_STATE_CONTINUE;
}

INT8U ext_idle_state_ringeroff(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
#if 0
#if WITH_UART_DEBUG_PSTN_RINGER
		uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER,GET_PSTN_NUMBER(devIndex), PSTN_FLOW_BEGIN_CHINA_CALLER_ID, callDevices[devIndex].timeoutCounter );
#endif
		if(callDevices[devIndex].timeoutCounter == 1 ||
			callDevices[devIndex].timeoutCounter == 2)
		{
			if(LOCAL_IS_CHINA() )
			{
#if WITH_UART_DEBUG_PSTN_RINGER
				uart_debug_3_int_value(UART_DEBUG_PSTN_RINGER,GET_PSTN_NUMBER(devIndex), PSTN_FLOW_BEGIN_CHINA_CALLER_ID, callDevices[devIndex].timeoutCounter );
#endif
				CallerIdChannelAllocate(devIndex );
			}
		}
		else
#endif
		callDevices[devIndex].timeoutCounter++;
			if(callDevices[devIndex].timeoutCounter >= 2)
		{
			pstn_callerid_end(devIndex );

			if(PSTN_IS_DIRECT_MODE() )
			{
				if(pstnDidRinging(devIndex) )
					return PBX_STATE_TRYING;
			}
	//			else
			{/* it console mode and EXT is busy  or direct dial mode */
				if(dtmfDecodingChannelAllocate(devIndex) == FALSE)
				{
					return PBX_STATE_CONTINUE;
				}
				
				pstnDeviceOffhook(devIndex );
				return PBX_STATE_DIALING;
			}
		}
	}
	
	return PBX_STATE_CONTINUE;
}


/* for PSTN callee, timeout for auto offhook */
INT8U	ext_idle_state_timeout(INT8U devIndex)
{
	if(PBX_SELFTEST_MODE_IS_ON() )
	{
#if WITH_UART_DEBUG						
		uart_debug_int_value(CMD_SELFTEST_BUSY, SELFTEST_CAUSE_TIMEOUT);
#endif
		return self_test_exit_handle(devIndex, SELFTEST_CAUSE_TIMEOUT);
	}

	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		pstn_callerid_end(devIndex );
#if WITH_PSTN_HW_RINGER_DETECT
#else
		PSTN_HW_NEW_STATE(GET_PSTN_NUMBER(devIndex), PSTN_HW_STATE_RINGER_OFF_DETECT, 5*PBX_100MS);
#endif
		callDevices[devIndex].timeoutCounter = 0;
		pstnDeviceOnhook(devIndex);
		return PBX_STATE_CONTINUE;
	}

	return PBX_STATE_CONTINUE;
}

void ext_idle_state_enter(INT8U devIndex)
{
	CALL_MY_PEER(devIndex) = PBX_VALUE_INVALIDATE;
	CALL_MY_FLASH(devIndex) = PBX_VALUE_INVALIDATE;
	CALL_TIMER_START(devIndex,  TIMEOUT_NEVER);

	callDevices[devIndex].currentEvent = PBX_EVENT_UNKNOWN;
	
	if(IS_EXT_DEVICE(devIndex) )
	{
		extDevices.holdId[devIndex] = PBX_VALUE_INVALIDATE;
	}
	
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		pstnDevices.hwStatus[GET_PSTN_NUMBER(devIndex)] = PSTN_HW_STATE_ONHOOK;
	}

	if(PBX_PROGRAM_MODE_IS_GET_PHONE_NUMBER(devIndex ) )
		CALL_EVENT_EMIT(devIndex, PBX_SIGNAL_RINGING);
	
	return ;
}

