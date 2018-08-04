/*
* $Id: uart_calls_tx.c,v 1.7 2007/05/14 08:31:12 lizhijie Exp $
* send Call command by uart
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "function_declarations.h"
#include "_uart_handlers.h"

/******************************** as Caller  *************************/
/* allocate channel for IP call and send out offhook+callee phone number
* param : x, caller index; y : callee phone number: length|oxBB(IP ID)|digits
* return newly allocated channle for this IP call */
void ip_call_tx_dialing_signal(INT8U ipIndex, INT8U extLineIndex)//, INT8U xdata *y/*docoded phone number*/)
{
/** it is always OK after IP dest is found 
	if(IS_UART_NOT_SENDABLE() )
		return PBX_VALUE_INVALIDATE;
*/
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_PBX_IP_DIALING,GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex) );
#else
	uart_tx_2_int_value(CMD_CALL_DIALING,GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex) );
#endif
	
	PBX_COLUMN_BUSY(GET_IP_CHANNEL_INDEX(ipIndex));
}

void	ip_call_tx_caller_digits_signal(INT8U  extLineIndex)
{
	extLineIndex = extLineIndex;
}

/*  */
INT8U ip_call_tx_callee_digits_signal(INT8U extLineIndex, phone_digits_t xdata *digits)
{
	if(IS_UART_NOT_SENDABLE() )
		return PBX_VALUE_INVALIDATE;
	
	uartTxCmds[uartTxInIndex].value[0] = GET_IP_CHANNEL_INDEX(deviceStatus[extLineIndex].peerId);
	uartTxCmds[uartTxInIndex].value[1] = GET_PHY_DEVICE_ID(extLineIndex);

	pbxdigit_2_ipdigit(uartTxCmds[uartTxInIndex].value + 2, digits->value, digits->length);

#if  	OLD_SIGNAL
	uart_tx_cmd(CMD_CALL_PBX_IP_PHONENUMBER, digits->length+2);
#else
	uart_tx_cmd(CMD_CALL_PHONE_DIGITS, digits->length+2);
#endif
	digits->length = 0;

	return 0;
}

void	ip_call_tx_cancel_signal(INT8U ipIndex)
{/* IP need 2 params */
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_CANCEL, GET_IP_CHANNEL_INDEX(ipIndex),GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId) );
#else
//	uart_tx_int_value(CMD_CALL_CANCEL, GET_IP_CHANNEL_INDEX(ipIndex) );
	uart_tx_2_int_value(CMD_CALL_ONHOOK, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId) );
#endif

	PBX_COLUMN_FREE_IP(ipIndex);
}


/******************************** as Callee  *************************/
/* when BUSY signal is send out, ip channel is not allocated, so only one param(device Index) is used in UART_CMD */
void ip_call_tx_busy_signal(INT8U ipIndex, INT8U extLineIndex)
{/* IP need 2 param although first param has no meaning */
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_PBX_IP_I_AM_BUSY,GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex));
#else
	uart_tx_2_int_value(CMD_CALL_BUSY_HERE,GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex));
#endif
}

void ip_call_tx_trying_signal(INT8U ipIndex, INT8U extLineIndex)
{/* feed newly allocated ip channel to IP side, so 2 params are used in this CMD */
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_PBX_IP_CHANNEL_INDEX, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex));
#else
	uart_tx_2_int_value(CMD_CALL_TRYING, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(extLineIndex));
#endif
}


BOOL ip_call_tx_offhook_signal(INT8U ipIndex)
{
	if((IS_ROW_DEVICE(ipIndex) ) || (ipIndex>=PBX_DEVICE_COUNT))
		return FALSE;
	if(IS_UART_NOT_SENDABLE() )
		return FALSE;
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_PBX_IP_OFFHOOK, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId));
#else
	uart_tx_2_int_value(CMD_CALL_OFFHOOK, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId) );
//	uart_tx_int_value(CMD_CALL_OFFHOOK, GET_IP_CHANNEL_INDEX(ipIndex) );
#endif

	return TRUE;	
}

/******************************** End a Call  *************************/
BOOL ip_call_tx_onhook_signal(INT8U ipIndex)
{
	if((IS_ROW_DEVICE(ipIndex)) || (ipIndex>=PBX_DEVICE_COUNT))
		return FALSE;
	if(IS_UART_NOT_SENDABLE() )
		return FALSE;
#if  	OLD_SIGNAL
	uart_tx_2_int_value(CMD_CALL_PBX_IP_ONHOOK, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId) );
#else
	uart_tx_2_int_value(CMD_CALL_ONHOOK, GET_IP_CHANNEL_INDEX(ipIndex), GET_PHY_DEVICE_ID(deviceStatus[ipIndex].peerId) );
//	uart_tx_int_value(CMD_CALL_ONHOOK, GET_IP_CHANNEL_INDEX(ipIndex) );
#endif
	PBX_COLUMN_FREE_IP(ipIndex);
	return TRUE;
}

