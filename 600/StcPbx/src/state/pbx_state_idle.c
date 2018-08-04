/*
* $Id: pbx_state_idle.c,v 1.24 2007/07/22 20:48:41 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

/*
* IDLE state, offhook event is handled as caller and ringing Signal is process as callee
*/

INT8U	ext_idle_state_digits(INT8U devIndex)
{
#if UART_DEBUG						
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
		ipChannel = pbx_ip_find_free_device();
		if(ipChannel == PBX_VALUE_INVALIDATE)
		{
			return STATE_BUSY;
		}

		uart_tx_int_value(CMD_RECORD_REQUEST, ipChannel);

		PBX_COLUMN_BUSY(ipChannel);
		CONNECT(devIndex, ipChannel);
		
		ipChannel = GET_IP_DEVICE_INDEX(ipChannel);
		/* assigned dest, so do not dialing in DIALING state */
		deviceStatus[devIndex].peerId = ipChannel;
		return STATE_PBX_PROGRAM;
	}

	
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		return STATE_CONTINUE;
	}

	/* ignore offhook event rx from IP side when play as stop_tingback_tone */
	if( (IS_IP_DEVICE(devIndex) )&&
		(deviceStatus[devIndex].peerId == PBX_VALUE_INVALIDATE) )
	{
			return STATE_CONTINUE;
	}
	
	if(!decodingchannel_allocate(devIndex))
	{/* For IP and EXT, enter into BUSY state to feedback Busy signal */
		return  STATE_BUSY;
	}

	return STATE_DIALING;
}

INT8U ext_idle_state_ringing(INT8U devIndex)
{
	if(PBX_PROGRAM_MODE_IS_ON(devIndex) )
	{
		return STATE_PBX_RERING;
	}
	
	if(IS_EXT_DEVICE(devIndex)||IS_PSTN_LINE_DEVICE(devIndex) )
	{
		PBX_ROW_BUSY(devIndex);
	}
	
	return STATE_RINGING;
}

INT8U ext_idle_state_car(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
//		pstnPending[GET_PSTN_NUMBER(devIndex)] = PSTN_RINGER_BLOCKED;

		DEVICE_TIMER_START(devIndex, 15*PBX_100MS);/* NTT specs P.76, about 2 seconds for CallerID rx */
		pstn_callerid_begin(devIndex );
		pbx_line_offhook(devIndex );
	}
	
	return STATE_CONTINUE;
}

INT8U ext_idle_state_ringeron(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		if(deviceStatus[devIndex].timeoutCounter == 2 ||
			deviceStatus[devIndex].timeoutCounter == 3)
		{
			if(LOCAL_IS_CHINA() )
				pstn_callerid_end(devIndex );
		}
	}
	
	return STATE_CONTINUE;
}

INT8U ext_idle_state_ringeroff(INT8U devIndex)
{
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
#if WITH_PSTN_FLOW_DEBUG						
		uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_BEGIN_CHINA_CALLER_ID, deviceStatus[devIndex].timeoutCounter );
#endif
		if(deviceStatus[devIndex].timeoutCounter == 1 ||
			deviceStatus[devIndex].timeoutCounter == 2)
		{
			if(LOCAL_IS_CHINA() )
			{
#if WITH_PSTN_FLOW_DEBUG						
				uart_tx_2_int_value(CMD_UART_DEBUG, PSTN_FLOW_BEGIN_CHINA_CALLER_ID, deviceStatus[devIndex].timeoutCounter );
#endif
				pstn_callerid_begin(devIndex );
			}
		}
		else if(deviceStatus[devIndex].timeoutCounter >= 3)
		{
			PBX_ROW_BUSY(devIndex);

			PSTN_SET_BUSY(devIndex);
			
			if(PSTN_IS_CONSOLE_MODE() )
			{
				if(pstnDidRinging(devIndex) )
					return STATE_TRYING;
			}
	//			else
			{/* it console mode and EXT is busy  or direct dial mode */
				if(decodingchannel_allocate(devIndex) == FALSE)
				{
					return STATE_CONTINUE;
				}
				
				pbx_line_offhook(devIndex );
				return STATE_DIALING;
			}
		}
	}
	
	return STATE_CONTINUE;
}


/* for PSTN callee, timeout for auto offhook */
INT8U	ext_idle_state_timeout(INT8U devIndex)
{
	if(PBX_SELFTEST_MODE_IS_ON() )
	{
#if UART_DEBUG						
		uart_tx_2_int_value(CMD_UART_DEBUG, CMD_SELFTEST_BUSY, SELFTEST_CAUSE_TIMEOUT);
#endif
		return self_test_exit_handle(devIndex, SELFTEST_CAUSE_TIMEOUT);
	}

	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		pstn_callerid_end(devIndex );

		PSTN_RINGER_NEW_STATE(GET_PSTN_NUMBER(devIndex), PSTN_RINGER_OFF_DETECT, 5*PBX_100MS);
		deviceStatus[devIndex].timeoutCounter = 0;
		pbx_line_onhook(devIndex);
		return STATE_CONTINUE;
	}

	return STATE_CONTINUE;
}

void ext_idle_state_enter(INT8U devIndex)
{
	deviceStatus[devIndex].peerId = PBX_VALUE_INVALIDATE;
	deviceStatus[devIndex].flashPeerId = PBX_VALUE_INVALIDATE;
	deviceStatus[devIndex].timeout = 0;
	deviceStatus[devIndex].timeoutCounter = 0;
	deviceStatus[devIndex].currentEvent = PBX_EVENT_UNKNOWN;
	
	if(IS_EXT_DEVICE(devIndex) )
	{
		holdId[devIndex] = PBX_VALUE_INVALIDATE;
	}
	
	if(IS_PSTN_LINE_DEVICE(devIndex) )
	{
		pstnPending[GET_PSTN_NUMBER(devIndex)] = PSTN_RINGER_DISABLE;
	}

	if(PBX_PROGRAM_MODE_IS_GET_PHONE_NUMBER(devIndex ) )
		DEVICE_EVENT_EMIT(devIndex, PBX_SIGNAL_RINGING);
	
	return ;
}

