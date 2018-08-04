/*
* $Id: uart_calls_rx.c,v 1.12 2007/07/07 09:32:46 lizhijie Exp $
*/
/*
* For all IP-->PBX command of SIP Call Process
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"

#define	GET_DEVICE_INDEX( phyIndex)	\
	((phyIndex) < PBX_FXS_COUNT)?(phyIndex):(((phyIndex)&0x0F) +PBX_FXS_COUNT)

/*************************     as Callee : 5 signals *******************/

/* allocate a IP channel and associated it with EXT or PSTN
*  only physical channel index(value[0]) in this signal(IP channel has nor known until now )
*/
static void _ip_call_rx_dialing_signal(INT8U xdata *value)
{
	INT8U 	data 	ipChannel;
	INT8U	data		extLineIndex ;
	BOOL			errFlag = FALSE;
	
	if(IS_UART_NOT_SENDABLE()||!columnFrees )
		return ;
	
	extLineIndex = GET_DEVICE_INDEX( value[0] );

	ipChannel = pbx_ip_find_free_device();
	if(ipChannel == PBX_VALUE_INVALIDATE)
	{
		errFlag = TRUE;
	}
	else
	{
		if( IS_EXT_DEVICE(extLineIndex) )
		{/* IP-->EXT : 0 ~7 */
		}
		else if( IS_PSTN_LINE_DEVICE(extLineIndex) )
		{/* IP-->PSTN : in format of '0x8*', eg. 0x80~0x83  */
			/* check PSTN Line flags to determine whether it can be used now */
			if( (!PSTN_IS_FREE(extLineIndex) ) || (PSTN_IS_DISABLE(extLineIndex) ) )
			{
				errFlag = TRUE;
			}
			else
			{
				PSTN_SET_BUSY(extLineIndex);
				pbx_row_changed( extLineIndex);
				PBX_ROW_BUSY(extLineIndex);
			}
		}
		else
		{/* IP-->IP forbidden */
			errFlag = TRUE;
		}
	}
	
	if(errFlag)
	{
#if 0
		uart_tx_int_value(CMD_CALL_BUSY_HERE, value[0]);
#else
		/* no ip channel is found, so use 0 as ip channel */
		ip_call_tx_busy_signal(STATE_IP_OFFSET, extLineIndex);
#endif
		return;
	}

	PBX_COLUMN_BUSY(ipChannel);

#if 0
	uart_tx_2_int_value(CMD_CALL_TRYING, ipChannel, value[0]);
#else
//	ip_call_tx_trying_signal( ipChannel,  extLineIndex);
	uart_tx_2_int_value(CMD_CALL_CHANNEL_INDEX, ipChannel, GET_PHY_DEVICE_ID(extLineIndex));
#endif

	ipChannel = GET_IP_DEVICE_INDEX(ipChannel);
	
	/* assigned dest, so do not dialing in DIALING state */
	deviceStatus[ipChannel].peerId = extLineIndex;
	DEVICE_EVENT_EMIT(ipChannel, PBX_EVENT_OFFHOOK);

}

/* IP Send Caller Phone Number; IP Channel is only index; digit can be one or more */
static void _ip_call_rx_caller_digits_signal(INT8U xdata *value)
{
#if 0
	INT8U	data i,j;
	
	for(i=0;i<4;i++)
	{
		if(callerIdData[i].owner == PBX_VALUE_INVALIDATE)/* no physical index is found: this is free now */
		{
			callerIdData[i].owner = GET_IP_DEVICE_INDEX(value[0]);
			j=0;

			while( value[j+1]!=PBX_VALUE_INVALIDATE && j< PHONE_DIGIT_BUFFER_SIZE )
			{
				callerIdData[i].digits.value[j]= value[j+1];
				j++;
			}

			if((value[j-1]&0x0f)==0x0f)		//ÆæÊýÎ»
				j = 2*j -1;
			else 
				j = 2*j;

			callerIdData[i].digits.length = j;/* phone number length */
			break;
		}
	}
#endif	
}


static void _ip_call_rx_callee_digits_signal(INT8U xdata *value)
{
	INT8U	data decodingIndex, length;
	
	if(deviceStatus[GET_IP_DEVICE_INDEX(value[0])].status == STATE_DIALING)
	{
		decodingIndex = decodingchannel_find(GET_IP_DEVICE_INDEX(value[0]) );
		if(decodingIndex == PBX_VALUE_INVALIDATE)
		{/* decoding channel has been freed when in TRYING state */
//			DEVICE_EVENT_EMIT(GET_IP_DEVICE_INDEX(value[0]), PBX_EVENT_ONHOOK);
			return;
		}

	/* value[1] is PSTN index coded in format of IP side; value[2] is digit length  */
#if 1
		length = value[2];
		COPY_DIGITS_BUFFER( (&decodingChannels[decodingIndex].digits), (&value[3]), length );
#else
		decodingChannels[decodingIndex].digits.value[0] = 0x56;
		decodingChannels[decodingIndex].digits.value[1] = 0x3F;
		decodingChannels[decodingIndex].digits.length = 3;
#endif		
		DEVICE_EVENT_EMIT(GET_IP_DEVICE_INDEX(value[0]), PBX_EVENT_DIGIT);
	}
}

/* only IP channel index is return */
static void _ip_call_rx_cancel_signal(INT8U xdata *value)
{
/* when RX this signal, caller(IP) is in state of TRYING and callee(ext|PSTN) is in state of RINGING 
* so simulate ONHOOK event on caller(IP)
*/
	INT8U	data		ipIndex = GET_IP_DEVICE_INDEX(value[0]);
	DEVICE_EVENT_EMIT(ipIndex, PBX_EVENT_ONHOOK);
	/* added this line for IP cancel this call which from IP */
	PBX_COLUMN_FREE_IP(ipIndex);
}


/*************************** as Caller : 3 signals **********************/
/* only IP channel index is return in BUSY signal */
static void _ip_call_rx_busy_signal(INT8U xdata *value)
{
/* when RX this signal, caller(ext|PSTN) is in TRYING 
* and callee(IP) are in state of RINGING */
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);
	INT8U data extLineIndex = deviceStatus[ipIndex].peerId;

	/* simulate a TIMEOUT event to ext|pstn caller which send CANCEL event to ip device */
	DEVICE_EVENT_EMIT(extLineIndex, PBX_EVENT_TIMEOUT);
	if(deviceStatus[extLineIndex].status == STATE_RINGING) 
	{
		deviceStatus[extLineIndex].timeoutCounter = RINGBACK_LOOP_COUNT;
	}
	else
	{/* EXT's state of DIALING, so emit a CANCEL signal on IP device  */
		DEVICE_EVENT_EMIT(ipIndex, PBX_SIGNAL_CANCEL);
	}
	
	PBX_COLUMN_FREE_IP(ipIndex);
}

/* only IP channel index is return in TRYING signal */
static void _ip_call_rx_trying_signal(INT8U xdata *value)
{
/* when RX this signal, caller(ext|PSTN) is in DIALING 
* and callee(IP) are in state of RINGING, so emit a RINGING event to DIALING state
*/
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);

	/* make EXT|Line device response to RINGING event to goto TRYING state */
	DEVICE_EVENT_EMIT( deviceStatus[ipIndex].peerId, PBX_SIGNAL_RINGING);
}


/* only IP channel index is return in TRYING signal */
static void _ip_call_rx_offhook_signal(INT8U xdata *value)
{
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);

	/* simulate OFF-HOOK event on callee device(IP device) */
	DEVICE_EVENT_EMIT(ipIndex, PBX_EVENT_OFFHOOK);
}


/* only IP channel index is return in ONHOOK signal */
static void _ip_call_rx_onhook_signal(INT8U xdata *value)
{
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);

	if(deviceStatus[ipIndex].status == STATE_RINGING)
	{/* simulate a timeout event on caller device(EXT) when IP is in state of RINGING, so statemachine will be continue */
		deviceStatus[deviceStatus[ipIndex].peerId].timeoutCounter = RINGBACK_LOOP_COUNT;
		DEVICE_EVENT_EMIT(deviceStatus[ipIndex].peerId, PBX_EVENT_TIMEOUT);
	}
	else
	{/* simulate OFF-HOOK event on callee device(IP device) when in state of IN_CALL */
		DEVICE_EVENT_EMIT(ipIndex, PBX_EVENT_ONHOOK);
	}
	PBX_COLUMN_FREE_IP(ipIndex);
}

/************************* when IP-PBX is startup *************************/
/* 0x04 : reset all device involved IP in PBX side; when SIP UA is start, this is called */
static void _ip_call_rx_reset_all_signal(INT8U xdata *value)
{
	INT8U data j = value[0];/* just for compiler warning */
#if 0	
	for(j= STATE_IP_OFFSET; j<20;j++)
	{
		if(status[j][0]==STATE_IP_SEND_ONHOOK_2)//108)
		{
			PBX_COLUMN_FREE_IP(j);
			status[j][0] = STATE_IP_IDLE;
		}
		else if(status[j][0]==STATE_IP_SEND_ONHOOK)//112)
			status[j][0] = STATE_IP_IDLE;
		else if(status[j][0]==STATE_IP_PCM_SELFTEST)//113)
		{
			decodingchannel_free(j);
			status[j][0] = STATE_IP_IDLE;
			columnFrees=0xff;
		}
		else
			ip_pbx_play_busytone(j-STATE_IP_OFFSET);
	}
#endif	
}


uart_call_rx_t		code		call_rx_handlers[]=
{
	/* when play as callee */
	{
		CMD_CALL_DIALING,
		_ip_call_rx_dialing_signal,
	},
	{
		CMD_CALL_PHONE_DIGITS,
		_ip_call_rx_callee_digits_signal,
	},
	{
		CMD_CALL_CALLERID_DIGITS,
		_ip_call_rx_caller_digits_signal,
	},
	{
		CMD_CALL_CANCEL,
		_ip_call_rx_cancel_signal,
	},

	/* play as caller */	
	{
		CMD_CALL_BUSY_HERE,
		_ip_call_rx_busy_signal,
	},
	{
		CMD_CALL_TRYING,
		_ip_call_rx_trying_signal,
	},
	{
		CMD_CALL_OFFHOOK,
		_ip_call_rx_offhook_signal,
	},
	{
		CMD_CALL_ONHOOK,
		_ip_call_rx_onhook_signal,
	},
	
	/* when IP is startuping */
	{
		CMD_CALL_RESET_ALL,
		_ip_call_rx_reset_all_signal,
	},

	/* other for selftest and recorder */	
#if 0
	{
		case CMD_SELFTEST_REQUEST://0x18:
			return ip_pbx_self_test_begin();
	},
#endif
};

	
BOOL	uart_call_rx_handle(uart_cmd_t xdata *value)
{
	INT8U	data i;
	
	for(i=0; i< sizeof(call_rx_handlers)/sizeof(uart_call_rx_t); i++)
	{
		if(call_rx_handlers[i].cmd == value->cmd )
		{
			(call_rx_handlers[i].handle)(value->value);
			return TRUE;
		}
	}

	return FALSE;
}

