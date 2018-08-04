/*
* $Id: cfgUartTx.c,v 1.3 2007/09/13 20:13:18 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include <REGSTCRD+.H>
#include "declarations.h"

void uart_tx_cmd(INT8U cmd, INT8U length )
{
	INT8U data	lastInIndex = PBX_VALUE_INVALIDATE;
	if(length<0 || length>CMD_VALUE_MAX)
		length = 0;//CMD_VALUE_MAX;
//	else
//		length = length-1;
	
	uartTxCmds[uart.responseInIndex].cmd = cmd;
	uartTxCmds[uart.responseInIndex].id = uart.cmdSeq++;
	if(uart.cmdSeq==PBX_VALUE_INVALIDATE)
	{
		uart.cmdSeq = 0;
	}
	uartTxCmds[uart.responseInIndex].value[length] = PBX_VALUE_INVALIDATE;

	lastInIndex = uart.responseInIndex;

	uart.responseInIndex++;
	if(uart.responseInIndex==UART_TX_BUFFER_COUNT)
	{
		uart.responseInIndex = 0;
	}

	if(uart.responseInIndex == uart.responseOutIndex)
	{/*block my self : main loop */
		uart.responseInIndex = PBX_VALUE_INVALIDATE;
	}

	if(uart.responseOutIndex==PBX_VALUE_INVALIDATE)
	{/*wakeup ISR lastly */
		uart.responseBuffersPosition = 1;
		uart.responseOutIndex = lastInIndex;
		SBUF = uartTxCmds[uart.responseOutIndex].cmd;
	}
	
}

#if WITH_OPTIMIZE_SIZE
void uart_tx_int_value(INT8U cmd, INT8U value)
{
	uartTxCmds[uart.responseInIndex].value[0] = value;
	uart_tx_cmd(cmd, 1);
}

void uart_tx_2_int_value(INT8U cmd, INT8U value1, INT8U value2)
{
	uartTxCmds[uart.responseInIndex].value[0] = value1;
	uartTxCmds[uart.responseInIndex].value[1] = value2;
	uart_tx_cmd(cmd, 2);
}
#endif

void __uart_tx_index(INT8U count)
{
//	if(ipGetConfigItemIndex==PBX_VALUE_INVALIDATE)
//		ipGetConfigItemIndex = 0;
	
	uartTxCmds[uart.responseInIndex].value[0] = uart.ipGetConfigItemIndex;

	uart.ipGetConfigItemIndex++;
	if((uart.ipGetConfigItemIndex == count) )
	{
		uart.ipGetConfigItemIndex = 0;//PBX_VALUE_INVALIDATE;
	}
}

void	uart_tx_index_item(INT8U cmd, INT8U value, INT8U count)
{
	__uart_tx_index(count);
	if(value==PBX_VALUE_INVALIDATE)
	{
		uart_tx_cmd(cmd, 1);
		return;
	}
	
	uartTxCmds[uart.responseInIndex].value[1] = value;
	uart_tx_cmd(cmd, 2);
}


void uart_tx_index_digits_item(INT8U cmd, phone_number_t xdata *pbxDigit, INT8U count)
{
	__uart_tx_index(count);
	
	pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value+1, pbxDigit->value, pbxDigit->length );
	uart_tx_cmd(cmd, pbxDigit->length+1 );
}


void uart_tx_index_call_transfer_item(INT8U cmd,INT8U type, phone_digits_t xdata *digits)
{
	INT8U data pos = 1;
	if(type==PBX_VALUE_INVALIDATE)
	{
		__uart_tx_index(STC_BLACKLIST_COUNT);
	}
	else
	{
		__uart_tx_index(PBX_FXS_COUNT);
		uartTxCmds[uart.responseInIndex].value[pos] = type;
		pos++;
	}

	pbx_x_str_copy((INT8U *)&(uartTxCmds[uart.responseInIndex].value[pos]), (INT8U *)digits->value, IS_ODD(digits->length)? (digits->length/2+1):(digits->length/2) );
	uart_tx_cmd(cmd, IS_ODD(digits->length)? (digits->length/2+1 + pos):(digits->length/2 + pos) );
}


/* after RX 0x49, it is called by timer_task */
void	net_service_config(void)
{
//	INT8U data value;
	
	if(IS_UART_NOT_SENDABLE())
	{
		return;
	}

	switch(uart.ipGetConfigIndex)
	{
		case 0:
			UART_TX_CFG_BEGIN();
			break;
		case 1:
			UART_TX_TRACK_MODE();
			break;
		case 2:
			UART_TX_NODIAL_ACTION();
			break;
		case 3:
			UART_TX_LINE_2_LINE();
			break;
		case 4:
			UART_TX_DIFFER_RINGING();
			break;
		case 5:
			UART_TX_CALLER_ID_MODE();
			break;
		case 6:
			UART_TX_GROUP_CALL();
			break;
		case 7:
			UART_TX_SPECIAL_SERVICE_FIRST();
			break;
		case 8:
			uart_tx_int_value(CMD_CFG_PSTN_PREFIX, baseConfigs.pstnPrefix);
			break;
		case 9:
			uart_tx_int_value(CMD_CFG_SIGNAL_TYPE, baseConfigs.signalType );
			break;
			
		case 10:	
			pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value, baseConfigs.ipPrefix.value, baseConfigs.ipPrefix.length );
			uart_tx_cmd(CMD_CFG_IP_PREFIX, baseConfigs.ipPrefix.length);
			break;
		case 11:
			pbx_x_str_copy(uartTxCmds[uart.responseInIndex].value, (INT8U xdata *)&baseConfigs.nightServiceTime, sizeof(night_service_time_t));
			uart_tx_cmd(CMD_CFG_NIGHT_SERVICE_TIME, sizeof(night_service_time_t));
		case 12:	
			pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value, baseConfigs.passwordSystem.value, baseConfigs.passwordSystem.length);
			uart_tx_cmd(CMD_CFG_SYS_PASSWD, baseConfigs.passwordSystem.length);
			break;
		case 13:	
			pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value, baseConfigs.passwordLine2Line.value, baseConfigs.passwordLine2Line.length);
			uart_tx_cmd(CMD_CFG_LINE2LINE_PASSWD, baseConfigs.passwordLine2Line.length );
			break;
		case 14:	
#if 0			
			pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value, baseConfigs.currentTime, STC_SYSTIME_LENGTH*2);//sizeof(baseConfigs.currentTime) );
			uart_tx_cmd(CMD_CFG_SYS_DATE, STC_SYSTIME_LENGTH*2);//sizeof(baseConfigs.currentTime)*2 );
#else
			uart_tx_2_int_value(CMD_CFG_SYS_DATE, pbxHwStatus.sysTime.hour, pbxHwStatus.sysTime.minute);//sizeof(baseConfigs.currentTime)*2 );
#endif
			break;

		case 15:	
//			pbxdigit_2_ipdigit( uartTxCmds[responseInIndex].value, baseConfigs.verInfo, STC_VERSION_LENGTH*2);//sizeof(baseConfigs.verInfo) );
			pbx_x_str_copy( uartTxCmds[uart.responseInIndex].value, (INT8U *)&verInfo, 13);//sizeof(baseConfigs.verInfo) );
			uart_tx_cmd(CMD_CFG_PBX_IP_VERSION, 13);//sizeof(baseConfigs.verInfo)*2 );
			break;


		/* PSTN Line (TRACK) */			
		case 16:/* */
			uart_tx_index_item(CMD_CFG_PSTN_FLAG, GET_BIT(baseConfigs.trunkStatusFlags, uart.ipGetConfigItemIndex), PBX_FXO_COUNT);
			break;
		case 17:
			uart_tx_index_item(CMD_CFG_PSTN_GROUP, baseConfigs.trunkBindingExtGroup[uart.ipGetConfigItemIndex], PBX_FXO_COUNT);
			break;
		case 18:
			uart_tx_index_item(CMD_CFG_PSTN_CONSOLE, baseConfigs.trunkVirtualConsoles[uart.ipGetConfigItemIndex],PBX_FXO_COUNT);
			break;
		case 19:
			uart_tx_index_digits_item(CMD_CFG_ACCOUNT_PASSWD, (&baseConfigs.accounts[uart.ipGetConfigItemIndex].passwd), PBX_ACCOUNT_NUMBER);
			break;
		case 20:
			uart_tx_index_item(CMD_CFG_ACCOUNT_AUTHOR, PBX_GET_LOW_NIBBLE(baseConfigs.accounts[uart.ipGetConfigItemIndex].authority), PBX_ACCOUNT_NUMBER);
			break;
		case 21:
			uart_tx_index_digits_item(CMD_CFG_SPECIAL_SERVICE, baseConfigs.specialServices+uart.ipGetConfigItemIndex, PBX_SPECIAL_SERVICE_COUNT);
			break;


		case 22:
			uart_tx_index_digits_item(CMD_CFG_EXT_PHONENUMER, extConfigs.phoneNumbers+uart.ipGetConfigItemIndex, PBX_FXS_COUNT);
			break;
		case 23:
			uart_tx_index_item(CMD_CFG_EXT_AUTHORITY, extConfigs.authorities[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 24:
			uart_tx_index_item(CMD_CFG_EXT_CALLER_ID, GET_BIT(extConfigs.isCallerIdOnFlags, uart.ipGetConfigItemIndex), PBX_FXS_COUNT);
			break;
		case 25:
			uart_tx_index_item(CMD_CFG_EXT_TIMEOUT, extConfigs.callLengths[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 26:	
			uart_tx_index_item(CMD_CFG_EXT_NODISTURB, GET_BIT(extConfigs.noDisturbFlags, uart.ipGetConfigItemIndex), PBX_FXS_COUNT);
			break;
		case 27:
			uart_tx_index_item(CMD_CFG_EXT_CALLWAIT, GET_BIT(extConfigs.callWaitFlags, uart.ipGetConfigItemIndex), PBX_FXS_COUNT);
			break;
		case 28:
			uart_tx_index_item(CMD_CFG_EXT_DELAY_2_LINE, extConfigs.delayOutTime[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 29:
			uart_tx_index_item(CMD_CFG_EXT_DIALOUT_MODE, extConfigs.dialOutMethod[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 30:	
			uart_tx_index_item(CMD_CFG_EXT_LEFT_TIME, extConfigs.leftTranferTimes[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 31:	
			uart_tx_index_item(CMD_CFG_EXT_GROUP, extConfigs.groups[uart.ipGetConfigItemIndex]&0x7F, PBX_FXS_COUNT);
			break;
			
		case 32:	
			uart_tx_index_item(CMD_CFG_EXT_BUSY_EXT, extConfigs.busyBranchs[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 33:
			uart_tx_index_item(CMD_CFG_EXT_SECRECT_EXT, extConfigs.secretBranchs[uart.ipGetConfigItemIndex], PBX_FXS_COUNT);
			break;
		case 34:
			uart_tx_index_call_transfer_item(CMD_CFG_EXT_TRANSFER_NIGHT, extTransfers.nightTransfers[uart.ipGetConfigItemIndex].type, &extTransfers.nightTransfers[uart.ipGetConfigItemIndex].digits);
			break;
		case 35:
			uart_tx_index_call_transfer_item(CMD_CFG_EXT_TRANSFER_LEFT, extTransfers.leftTransfers[uart.ipGetConfigItemIndex].type, &extTransfers.leftTransfers[uart.ipGetConfigItemIndex].digits);
			break;
		case 36:
			uart_tx_index_call_transfer_item(CMD_CFG_EXT_TRANSFER_UNCONDITIONAL, extTransfers.unconditionalTransfers[uart.ipGetConfigItemIndex].type, &extTransfers.unconditionalTransfers[uart.ipGetConfigItemIndex].digits);
			break;
			
		case 37:
			uart_tx_index_call_transfer_item(CMD_CFG_EXT_TRANSFER_DENY, PBX_VALUE_INVALIDATE, baseConfigs.blackList +uart.ipGetConfigItemIndex);
			break;
			
		case 38:	
			pbxdigit_2_ipdigit( uartTxCmds[uart.responseInIndex].value, baseConfigs.consolePhoneNumber.value, baseConfigs.consolePhoneNumber.length );
			uart_tx_cmd(CMD_CFG_CONSOLE_PHONENUMBER, baseConfigs.consolePhoneNumber.length);
			break;

		case 39:	
			uart_tx_int_value(CMD_CFG_CONSOLE_EXT_INDEX, baseConfigs.extIndexOfConsole);
			break;

#if WITH_EXT_DIFFER_RINGING
		case 40:	
			uart_tx_index_item(CMD_CFG_EXT_DIFFER_RINGING,GET_BIT(extConfigs.isDifferRingFlags, uart.ipGetConfigItemIndex), PBX_FXS_COUNT);
			break;
#endif

		default:
			UART_TX_CFG_END();
			uart.ipGetConfigIndex = PBX_VALUE_INVALIDATE;
			break;

	}

	if( (uart.ipGetConfigIndex!= PBX_VALUE_INVALIDATE) 
		&&(uart.ipGetConfigItemIndex==0 ) )
		uart.ipGetConfigIndex++;

}


