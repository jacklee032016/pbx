/*
* $Id: lcdTimerTask.c,v 1.8 2007/09/27 17:48:29 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

#define	LCD_TICK_COUNT	2

void __lcdCheckButton(void)
{
	BOOL isReleased = FALSE;
	
	if(stcLcd.button.state == LCD_BUTTON_CLICK_NONE)
	{
		if(ButtonUp== FALSE)/* clicked */
		{
			stcLcd.button.state = LCD_BUTTON_CLICK_UP;
		}
		
		if( ButtonDown == FALSE )
		{
			stcLcd.button.state = LCD_BUTTON_CLICK_DOWN;
		}
		
		if( ButtonMenu == FALSE)
		{
			stcLcd.button.state = LCD_BUTTON_CLICK_MENU;
		} 
		
		if(stcLcd.button.state != LCD_BUTTON_CLICK_NONE)
		{
			stcLcd.button.tickCounts = 0;
		}
	}
	else
	{
		stcLcd.button.tickCounts ++;
		
		if(stcLcd.button.state == LCD_BUTTON_CLICK_MENU )
		{
			if(!LCD_BUTTON_CLIECKED(ButtonMenu) )
			{
				if( stcLcd.button.tickCounts >= LCD_TICK_COUNT &&
					stcLcd.button.tickCounts < 15)/*  */
				{
					stcLcd.currentEvent = LCD_EVENT_BTN_MENU_NEXT;
					isReleased = TRUE;
				}
				else if(stcLcd.button.tickCounts >= 15 )
				{
					stcLcd.currentEvent = LCD_EVENT_BTN_MENU;
					isReleased = TRUE;
				}
			}
		}
		else if(stcLcd.button.state == LCD_BUTTON_CLICK_UP )
		{
			if(!LCD_BUTTON_CLIECKED(ButtonUp) )
			{
				if(stcLcd.button.tickCounts >= LCD_TICK_COUNT )
				{
					stcLcd.currentEvent = LCD_EVENT_BTN_UP;
				}

				isReleased = TRUE;
			}
		}
		else if(stcLcd.button.state == LCD_BUTTON_CLICK_DOWN)
		{
			if(!LCD_BUTTON_CLIECKED(ButtonDown) )
			{
				if(stcLcd.button.tickCounts >= LCD_TICK_COUNT)
				{
					stcLcd.currentEvent = LCD_EVENT_BTN_DOWN;
				}

				isReleased = TRUE;
			}
		}

		if(isReleased == TRUE)
		{
			stcLcd.button.tickCounts = 0;
			stcLcd.button.state = LCD_BUTTON_CLICK_NONE;
			
			stcLcd.menuStateTimeout = 0;
		}

	}
}

void __lcdBlinkIp(void)
{
	INT8U data row, column;
	lcd_ip_address_t xdata *address = NULL;
	
	if(stcLcd.currentState==LCD_STATE_MODIFY)
	{
		if(stcLcd.currentService == LCD_SERVICE_WAN)
		{
			address = &stcLcd.wanAddress;
		}
		else if(stcLcd.currentService == LCD_SERVICE_LAN)
		{
			address = &stcLcd.lanAddress;
		}

		if(address != NULL)
		{
			row = address->index/3;
			if( row >= 2 )
				row = 1;
			else
				row = 0;
			
			column = address->index%6+1;
			if(column>3)
				column ++;
		}

	}
	
	if(blinkOnIp)
	{
		if(address != NULL)
			lcdDrawAscii12x16Char(row, column, address->value[address->index]);
	}
	else
	{
		if(address != NULL)
			lcdDrawAscii12x16Char(row, column, ' ');
	}

	blinkOnIp = !blinkOnIp;
}

void lcdTimerTask(void)
{

	if(flag50Ms == TRUE)
	{
		flag50Ms = FALSE;

		__lcdCheckButton();
	}

	if(flag100Ms == TRUE)
	{
		flag100Ms = FALSE;

		if(stcLcd.currentState == LCD_STATE_MODIFY)
		{
			__lcdBlinkIp();
		}
	}

	if(flag500Ms == TRUE)
	{
		flag500Ms = FALSE;
		if(stcLcd.currentState != LCD_STATE_IDLE)
		{
			if(blinkOnArrow)
			{
				lcdDrawArrow(stcLcd.currentService, stcLcd.services[stcLcd.currentService].state);
			}
			else
			{
				lcdDrawArrow(stcLcd.currentService, LCD_SERVICE_STATE_NULL+1);
			}

			blinkOnArrow = !blinkOnArrow;
		}
		
	}
	
	if(flag1s == TRUE)
	{/* blink IP Address Position and the selected Service's Arrow */
		flag1s = FALSE;
		if(stcLcd.menuStateTimeout > 0 )
		{
			stcLcd.menuStateTimeout--;
			if(stcLcd.menuStateTimeout <= 0)
			{
				stcLcd.menuStateTimeout = 0;
				stcLcd.currentEvent = LCD_EVENT_BTN_TIMEOUT;
			}
		}

	}

	if(flag5s == TRUE)
	{
		flag5s = FALSE;

		if(stcLcd.currentState == LCD_STATE_IDLE)
		{
			lcdRefreshScreen();
		}
		
		if(stcLcd.refreshTimeout > 0 )
		{
			stcLcd.refreshTimeout--;
			if(stcLcd.refreshTimeout <= 0)
			{
				stcLcd.refreshTimeout = 0;
				stcLcd.currentEvent = LCD_EVENT_REFRESH_TIMEOUT;
			}
		}
	}
}

