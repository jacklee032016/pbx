/*
* $Id: lcdIsr.c,v 1.7 2007/09/27 17:48:29 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*Interrupt 4 : UART ISR */
void	lcdIsrUart(void) interrupt 4 using 3
{
	INT8U 	xdata	*buf;
	INT8U	data		lastIndex;
	
	if(RI)
	{
		if( lcdUart.responseInIndex == LCD_UART_CMD_END )
		{/* no close RI when return */
			UART_CTS = LCD_UART_CTS_DISABLE;
		}
		else
		{
			RI=0;
			
			buf = (INT8U xdata *)(&lcdUartResponse[lcdUart.responseInIndex]);
			*(buf+ lcdUart.responseBuffersPosition) = SBUF;
			
			if( (*(buf+ lcdUart.responseBuffersPosition) == LCD_UART_CMD_END) ||
				((lcdUart.responseBuffersPosition+1)== sizeof(LCD_UART_RESPONSE)) )
			{
				lcdUart.responseBuffersPosition=0;
				lastIndex = lcdUart.responseInIndex;
				
				lcdUart.responseInIndex++;
				if( (lcdUart.responseInIndex == LCD_UART_BUFFER_COUNT) )
				{
					lcdUart.responseInIndex = 0;
				}
				
				if( lcdUart.responseInIndex == lcdUart.responseOutIndex )
				{/* block myself */
					lcdUart.responseInIndex = LCD_UART_CMD_END;
					UART_CTS = LCD_UART_CTS_DISABLE;
				}
				
				if( lcdUart.responseOutIndex== LCD_UART_CMD_END)
				{/* wake up main loop to read data */
					lcdUart.responseOutIndex = lastIndex;
				}
				
			}
			else
			{
				lcdUart.responseBuffersPosition++;
			}
		}
	}
	else 

	if(TI)
	{
		TI=0;
		if( lcdUart.requestOutIndex != LCD_UART_CMD_END)
		{
			buf = (INT8U xdata *)(&lcdUartRequest[lcdUart.requestOutIndex] );

			SBUF= *(buf + lcdUart.requestBuffersPosition);
			
			if( (*(buf+lcdUart.requestBuffersPosition) == LCD_UART_CMD_END ) )
			{
				lastIndex = lcdUart.requestOutIndex;
				lcdUart.requestBuffersPosition = 0;

				if(lcdUart.requestInIndex == LCD_UART_CMD_END )
				{/*wakeup main loop as once */
					lcdUart.requestInIndex = lastIndex;
				}

				lcdUart.requestOutIndex++;
				if(lcdUart.requestOutIndex == LCD_UART_BUFFER_COUNT)
				{
					lcdUart.requestOutIndex = 0;
				}

				if(lcdUart.requestOutIndex == lcdUart.requestInIndex)
				{/* block myself */
					lcdUart.requestOutIndex = LCD_UART_CMD_END;
				}

			}
			else
			{		
				lcdUart.requestBuffersPosition++;
			}
		}
	}

	ES = STC51_PIN_ENABLED;

}

/* Interrupt No. 1 : Timer/Counter 0 */
void lcdIsrTimer0(void) interrupt 1 using 3
{
	TH0 = STC51_TIMER_0_25MS;
	TL0 = 0x00;

	lcdOptions.counter50ms ++;

	if(lcdOptions.counter50ms >= 2)
	{
		flag50Ms = TRUE;
		lcdOptions.counter50ms = 0;

		lcdOptions.counter100ms++;

		if(lcdOptions.counter100ms >= 6 )
		{
			flag100Ms = TRUE;
			lcdOptions.counter100ms = 0;
		}
		
		lcdOptions.counter500ms++;
		if(lcdOptions.counter500ms >= 10 )
		{
			flag500Ms = TRUE;
			lcdOptions.counter500ms = 0;
		}

		lcdOptions.counter1s++;

		if(lcdOptions.counter1s >= 20)
		{
			flag1s = TRUE;
			lcdOptions.counter1s = 0;

			lcdOptions.counter5s ++;
			if(lcdOptions.counter5s >= 5)
			{
				flag5s = TRUE;
				lcdOptions.counter5s = 0;
			}
		}
	}
	
}

