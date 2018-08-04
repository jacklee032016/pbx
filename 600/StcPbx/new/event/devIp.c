/*
* $Id: devIp.c,v 1.6 2007/09/08 18:56:09 lizhijie Exp $
*/
/*
* For all IP-->PBX command of SIP Call Process
*/

#pragma	ot(9,speed)
//#pragma large
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

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
	
	if(IS_UART_NOT_SENDABLE()||!pbxHwStatus.columnFrees )
		return ;
	
	extLineIndex = GET_DEVICE_INDEX(value[0] );
	if(!CALL_CHECK_STATUS(extLineIndex, PBX_STATE_IDLE) )
		errFlag = TRUE;
	
	if(errFlag == FALSE)
	{
		ipChannel = findFreeIpChannel();
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
				if(PSTN_IS_DISABLE(extLineIndex) || !PSTN_IS_ONHOOK(extLineIndex) )
				{
					errFlag = TRUE;
				}
			}
			else
			{/* IP-->IP forbidden */
				errFlag = TRUE;
			}
		}
	}
	
	if(errFlag)
	{
		/* no ip channel is found, so use 0 as ip channel */
		uart_tx_2_int_value(CMD_CALL_BUSY_HERE,0, GET_PHY_DEVICE_ID(extLineIndex));
		return;
	}

	PBX_COLUMN_BUSY(ipChannel);

	uart_tx_2_int_value(CMD_CALL_CHANNEL_INDEX, ipChannel, GET_PHY_DEVICE_ID(extLineIndex));

	ipChannel = GET_IP_DEVICE_INDEX(ipChannel);
	
	/* assigned dest, so do not dialing in DIALING state */
	CALL_MY_PEER(ipChannel) = extLineIndex;
	CALL_MY_PEER(extLineIndex) = ipChannel;
	
	CALL_EVENT_EMIT(ipChannel, PBX_EVENT_OFFHOOK);
}

/* IP Send Caller Phone Number; IP Channel is only index; digit can be one or more */
static void _ip_call_rx_caller_digits_signal(INT8U xdata *value)
{
#if 0	
	INT8U	data i = 0;
#else
	INT8U	data	length = 0;
#endif
	INT8U 	data channel, devIndex;
	devIndex = GET_IP_DEVICE_INDEX(value[0]);
	channel = CallerIdChannelAllocate(devIndex);
	if(channel == PBX_VALUE_INVALIDATE)
		return;
#if 0	
	while( value[i+2]!=PBX_VALUE_INVALIDATE && i< PHONE_DIGIT_BUFFER_SIZE )
	{
		callerIdData[channel].digits.value[i]= value[i+2];
		i++;
	}

	if((value[i-1]&0x0f)==0x0f)		//ÆæÊýÎ»
		callerIdData[channel].digits.length = 2*i -1;
	else 
		callerIdData[channel].digits.length = 2*i;
#else
	length = value[2];
	COPY_DIGITS_BUFFER( (&callerIdData[channel].digits), (&value[3]), length );
#endif

	if(IS_CALL_DEVICE(CALL_MY_PEER(devIndex)) )
	{
		DEVICE_EVENT_EMIT(CALL_MY_PEER(devIndex), DEV_EVENT_CALLER_DIGITS);
	}
}


static void _ip_call_rx_callee_digits_signal(INT8U xdata *value)
{
	INT8U	data decodingIndex, length;
	
	if(callDevices[GET_IP_DEVICE_INDEX(value[0])].status == PBX_STATE_DIALING)
	{
		decodingIndex = dtmfDecodingChannelFind(GET_IP_DEVICE_INDEX(value[0]) );
		if(decodingIndex == PBX_VALUE_INVALIDATE)
		{/* decoding channel has been freed when in TRYING state */
//			CALL_EVENT_EMIT(GET_IP_DEVICE_INDEX(value[0]), PBX_EVENT_ONHOOK);
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
		CALL_EVENT_EMIT(GET_IP_DEVICE_INDEX(value[0]), PBX_EVENT_DIGIT);
	}
}

/* only IP channel index is return */
static void _ip_call_rx_cancel_signal(INT8U xdata *value)
{
/* when RX this signal, caller(IP) is in state of TRYING and callee(ext|PSTN) is in state of RINGING 
* so simulate ONHOOK event on caller(IP)
*/
	INT8U	data		ipIndex = GET_IP_DEVICE_INDEX(value[0]);
	CALL_EVENT_EMIT(ipIndex, PBX_EVENT_ONHOOK);
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
	INT8U data extLineIndex = CALL_MY_PEER(ipIndex);

	/* simulate a TIMEOUT event to ext|pstn caller which send CANCEL event to ip device */
	CALL_EVENT_EMIT(extLineIndex, PBX_EVENT_TIMEOUT);
	if(callDevices[extLineIndex].status == PBX_STATE_RINGING) 
	{
		callDevices[extLineIndex].timeoutCounter = DURATION_COUNT_RINGBACK;
	}
	else
	{/* EXT's state of DIALING, so emit a CANCEL signal on IP device  */
		CALL_EVENT_EMIT(ipIndex, PBX_SIGNAL_CANCEL);
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
	CALL_PEER_EVENT(ipIndex, PBX_SIGNAL_RINGING);
}


/* only IP channel index is return in TRYING signal */
static void _ip_call_rx_offhook_signal(INT8U xdata *value)
{
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);

	/* simulate OFF-HOOK event on callee device(IP device) */
	CALL_EVENT_EMIT(ipIndex, PBX_EVENT_OFFHOOK);
}


/* only IP channel index is return in ONHOOK signal */
static void _ip_call_rx_onhook_signal(INT8U xdata *value)
{
	INT8U data ipIndex = GET_IP_DEVICE_INDEX(value[0]);

	if(callDevices[ipIndex].status == PBX_STATE_RINGING)
	{/* simulate a timeout event on caller device(EXT) when IP is in state of RINGING, so statemachine will be continue */
		callDevices[CALL_MY_PEER(ipIndex)].timeoutCounter = DURATION_COUNT_RINGBACK;
		CALL_PEER_EVENT(ipIndex, PBX_EVENT_TIMEOUT);
	}
	else
	{/* simulate OFF-HOOK event on callee device(IP device) when in state of IN_CALL */
		CALL_EVENT_EMIT(ipIndex, PBX_EVENT_ONHOOK);
	}
	PBX_COLUMN_FREE_IP(ipIndex);
}

/************************* when IP-PBX is startup *************************/
/* 0x04 : reset all device involved IP in PBX side; when SIP UA is start, this is called */
static void _ip_call_rx_reset_all_signal(INT8U xdata *value)
{
	INT8U data j = value[0];/* just for compiler warning */
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

void _ipSendDigits(INT8U devIndex, INT8U type, INT8U xdata *digits, INT8U length)
{
	uartTxCmds[uart.responseInIndex].value[0] = GET_IP_CHANNEL_INDEX(devIndex);
	uartTxCmds[uart.responseInIndex].value[1] = GET_PHY_DEVICE_ID( CALL_MY_PEER(devIndex) );

	pbxdigit_2_ipdigit(uartTxCmds[uart.responseInIndex].value + 2, digits, length);

	uart_tx_cmd(type, length+2);
}

void	devIpEventHandler(INT8U devIndex, dev_event_t event)
{
	INT8U  data	channel, length = 0;
	if(!IS_IP_DEVICE(devIndex) )
		return;
	if(IS_UART_NOT_SENDABLE() )
		return;
	
	switch(event)
	{
		case DEV_EVENT_TONE_BUSY:
			uart_tx_2_int_value(CMD_CALL_BUSY_HERE,GET_IP_CHANNEL_INDEX(devIndex), GET_PHY_DEVICE_ID(CALL_MY_PEER(devIndex)));
			break;
		case DEV_EVENT_TONE_DIALING:
			CALL_PEER_EVENT(devIndex, PBX_SIGNAL_RINGING);
			break;
		case DEV_EVENT_TONE_RINGBACK:
			uart_tx_2_int_value(CMD_CALL_TRYING, GET_IP_CHANNEL_INDEX(devIndex), GET_PHY_DEVICE_ID(CALL_MY_PEER(devIndex)));
			break;
		case DEV_EVENT_RINGING:
			uart_tx_2_int_value(CMD_CALL_DIALING, GET_IP_CHANNEL_INDEX(devIndex), GET_PHY_DEVICE_ID(CALL_MY_PEER(devIndex)) );
			PBX_COLUMN_BUSY(GET_IP_CHANNEL_INDEX(devIndex));

#if 0
			if(IS_EXT_DEVICE(CALL_MY_PEER(devIndex) ))
			{
				_ipSendDigits(devIndex, CMD_CALL_CALLERID_DIGITS,
					extConfigs.phoneNumbers[CALL_MY_PEER(devIndex)].value, 
					extConfigs.phoneNumbers[CALL_MY_PEER(devIndex)].length );
			}
			else
			{
				channel = CallerIdChannelFind(CALL_MY_PEER(devIndex) );
				if(channel != PBX_VALUE_INVALIDATE)
				{
					_ipSendDigits(devIndex, CMD_CALL_CALLERID_DIGITS,
						callerIdData[channel].digits.value, 
						callerIdData[channel].digits.length);
				}
			}
#endif
			if( (channel = dtmfEncodeChannelFind(devIndex) ) != PBX_VALUE_INVALIDATE)
			{/* call Transfer */
				_ipSendDigits(devIndex,CMD_CALL_PHONE_DIGITS, encodingChannels[channel].digits.value, encodingChannels[channel].digits.length);
				dtmfEncodeChannelFree(devIndex);
			}

			break;
		case DEV_EVENT_OK:
			uart_tx_2_int_value(CMD_CALL_OFFHOOK, GET_IP_CHANNEL_INDEX(devIndex), GET_PHY_DEVICE_ID(CALL_MY_PEER(devIndex)) );
			break;
		case DEV_EVENT_CANCEL:
			uart_tx_2_int_value(CMD_CALL_ONHOOK, GET_IP_CHANNEL_INDEX(devIndex), GET_PHY_DEVICE_ID(CALL_MY_PEER(devIndex)) );
			PBX_COLUMN_FREE_IP(devIndex);
			break;
		case DEV_EVENT_CALLER_DIGITS:
			
			break;
		case DEV_EVENT_CALLEE_DIGITS:
			/* PSTN/EXT-->IP, phone number is dialed step by step and decodingChannel is owned by peer */
			if( (channel = dtmfDecodingChannelFind(CALL_MY_PEER(devIndex)) ) != PBX_VALUE_INVALIDATE)
			{
				_ipSendDigits(devIndex,CMD_CALL_PHONE_DIGITS, decodingChannels[channel].digits.value, decodingChannels[channel].digits.length);
				decodingChannels[channel].digits.length = 0;
			}
			break;
		case DEV_EVENT_MUSIC:
			devMusicPlay(devIndex, TRUE);
			break;
		case DEV_EVENT_STOP:
			devMusicPlay(devIndex, FALSE);
			break;
#if 0
		case DEV_EVENT_TONE_CALLWAIT:
			break;
#endif			
		default:
			break;
	}
}

