/*
* $Id: lcdUartTx.c,v 1.5 2007/09/26 19:48:34 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

static void __lcdUartTxRequest(INT8U op, INT8U service, INT8U length )
{
	INT8U data	lastInIndex = LCD_UART_CMD_END;

	if(length<0 || length>LCD_UART_REQUEST_DATA_LENGTH)
		length = 0;//CMD_VALUE_MAX;
//	else
//		length = length-1;
	lcdUartRequest[lcdUart.requestInIndex].op = op;
		
	lcdUartRequest[lcdUart.requestInIndex].service = service;

	lcdUartRequest[lcdUart.requestInIndex].sequence = lcdUart.requestSeq++;
	lcdUartRequest[lcdUart.requestInIndex].loadData[length] = LCD_UART_CMD_END;
	
	if(lcdUart.requestSeq==LCD_UART_CMD_END)
	{
		lcdUart.requestSeq = 0;
	}

	lastInIndex = lcdUart.requestInIndex;

	lcdUart.requestInIndex++;
	if(lcdUart.requestInIndex== LCD_UART_BUFFER_COUNT)
	{
		lcdUart.requestInIndex = 0;
	}

	if(lcdUart.requestInIndex == lcdUart.requestOutIndex)
	{/*block my self : main loop */
		lcdUart.requestInIndex = LCD_UART_CMD_END;
	}

	if(lcdUart.requestOutIndex == LCD_UART_CMD_END)
	{/*wakeup ISR lastly */
		lcdUart.requestOutIndex = lastInIndex;
		SBUF = lcdUartRequest[lcdUart.requestOutIndex].op;
		lcdUart.requestBuffersPosition = 1;
	}
}


void lcdUartRequestState(INT8U service)
{
	if(IS_UART_NOT_SENDABLE() )
		return;
	
	lcdUartRequest[lcdUart.requestInIndex].state = 0;
	__lcdUartTxRequest(LCD_UART_CMD_LOOKUP, service, 0);
}

void lcdUartRequestUpdateState(INT8U service)
{
	if(IS_UART_NOT_SENDABLE() )
		return;
	
	lcdUartRequest[lcdUart.requestInIndex].state = stcLcd.services[service].state;
	__lcdUartTxRequest(LCD_UART_CMD_CONFIG, service, 0);
}

void lcdUartRequestUpdateIpAddress(INT8U service)
{
	lcd_ip_address_t xdata *address = NULL;
	INT8U data i;
	
	if(IS_UART_NOT_SENDABLE() )
		return;
	
	GET_IP_ADDRESS_PTR(address);
	if(address == NULL)
		return;
	
	for(i=0; i < LCD_IP_ADRRESS_LENGTH; i++)
	{
		lcdUartRequest[lcdUart.requestInIndex].loadData[i] = address->value[i];
	}

	__lcdUartTxRequest(LCD_UART_CMD_STRING, service, LCD_IP_ADRRESS_LENGTH );
}

INT8U lcdUartSendVerInfo(void)
{
//	if(IS_UART_NOT_SENDABLE() )
//		return LCD_STATE_CONTINUE;
	
	x_str_copy( lcdUartRequest[lcdUart.requestInIndex].loadData, (INT8U *)&verInfo, 13);//sizeof(baseConfigs.verInfo) );

	__lcdUartTxRequest(LCD_UART_CMD_GET_VER_INFO, LCD_SERVICE_VERSION, 13 );

#if WITH_VERSION_STATE
	if(stcLcd.currentState == LCD_STATE_VERSION)
	{
		return LCD_STATE_INITING;
	}
	else
		return LCD_STATE_CONTINUE;
#else
	return LCD_STATE_CONTINUE;
#endif
}


