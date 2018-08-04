/*
* $Id: lcdUartRx.c,v 1.8 2007/09/27 17:48:29 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

void	lcdUartRxResponse(void)
{
	INT8U data length = 0;
	lcd_service_t xdata *service;
	
	if(lcdUart.responseOutIndex != LCD_UART_CMD_END )
	{
#if 0	
		if(lcdUartResponse[lcdUart.responseOutIndex].service == LCD_SERVICE_VERSION)
		{
			stcLcd.currentService = lcdUartResponse[lcdUart.responseOutIndex].service;
			stcLcd.currentEvent = LCD_EVENT_UART_VERSION;
		}
		else 	
#endif			
		if(lcdUartResponse[lcdUart.responseOutIndex].op == LCD_UART_CMD_STRING &&
			stcLcd.currentState != LCD_STATE_MODIFY )
		{

			for(length=0; length<LCD_UART_RESPONSE_DATA_LENGTH;length++)
			{
				if(lcdUartResponse[lcdUart.responseOutIndex].loadData[length] == LCD_UART_CMD_END)
					break;
			}
			
			if(lcdUartResponse[lcdUart.responseOutIndex].service>= LCD_SERVICE_WEB &&
				lcdUartResponse[lcdUart.responseOutIndex].service < LCD_SERVICE_TIME)// MAX_NUMBER )
			{
				service = &stcLcd.services[lcdUartResponse[lcdUart.responseOutIndex].service];

				service->drawMode = lcdUartResponse[lcdUart.responseOutIndex].drawMode;
				service->state = lcdUartResponse[lcdUart.responseOutIndex].state;
#if 0
				service->row = lcdUartResponse[lcdUart.responseOutIndex].row;
				service->column = lcdUartResponse[lcdUart.responseOutIndex].column;
#endif

				
				if(lcdUartResponse[lcdUart.responseOutIndex].service == LCD_SERVICE_LAN)
				{
					x_str_copy(stcLcd.lanAddress.value, lcdUartResponse[lcdUart.responseOutIndex].loadData,  length);
					stcLcd.lanAddress.value[length] = '\0';
				}
				else if(lcdUartResponse[lcdUart.responseOutIndex].service == LCD_SERVICE_WAN)
				{
					x_str_copy(stcLcd.wanAddress.value, lcdUartResponse[lcdUart.responseOutIndex].loadData,  length);
					stcLcd.wanAddress.value[length] = '\0';
				}
				else
				{
					x_str_copy(service->info.title, lcdUartResponse[lcdUart.responseOutIndex].loadData,  length);
					service->info.title[length] = '\0';
				}

				if(stcLcd.currentState == LCD_STATE_FIRST)
				{
					stcLcd.currentService = lcdUartResponse[lcdUart.responseOutIndex].service;
				}
				
				stcLcd.currentEvent = LCD_EVENT_UART_NOTIFY;
			}
			else if(lcdUartResponse[lcdUart.responseOutIndex].service == LCD_SERVICE_TIME )
			{
				length = (length>sizeof(stcLcd.timeInfo))? sizeof(stcLcd.timeInfo) : length;
				x_str_copy(stcLcd.timeInfo, lcdUartResponse[lcdUart.responseOutIndex].loadData,  length);
				stcLcd.timeInfo[length] = '\0';
				update_status_bar();
			}

			lcdUartResponse[lcdUart.responseOutIndex].op = LCD_UART_CMD_END;
		}

		length = lcdUart.responseOutIndex;
		
		lcdUart.responseOutIndex++;
		if(lcdUart.responseOutIndex == LCD_UART_BUFFER_COUNT)
			lcdUart.responseOutIndex = 0;
		
		if(lcdUart.responseInIndex == lcdUart.responseOutIndex)
		{/*block myself(main loop) read */
			lcdUart.responseOutIndex = LCD_UART_CMD_END;
		}
		
		if(lcdUart.responseInIndex == LCD_UART_CMD_END)
		{/* if ISR is blocked, wake it up and assign index of just used  */
			lcdUart.responseInIndex = length;
			UART_CTS = LCD_UART_CTS_ENABLE;
		}
	}
}


