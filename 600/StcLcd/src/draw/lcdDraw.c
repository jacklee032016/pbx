/*
* $Id: lcdDraw.c,v 1.8 2007/06/01 18:03:32 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*page的取值范围为0-7
colume的取值范围为0-127
pages的取值范围为1-8
columes的取值范围为1-128
*/
#if 0
void lcdDisplayData(lcd_position_t *pos, INT8U code *pvalue)
{
	INT8U data i=0,j=0;

	/* 确保字符可以完全显示的条件:所在列号+占用列数<总列数*/
	if(( (pos->beginColumn + pos->width) < MAX_COLUMNS) && 
		( (pos->beginPage + LCD_PAGES_PER_ROW) <= MAX_PAGES) )
	{
		for(i=0;i<LCD_PAGES_PER_ROW; i++)
		{	
			send_control((pos->beginPage<<3)+0x80); //set start line address   jjl change 0x40 to 80
			send_control(pos->beginPage+0xb0+i);	  //set page address
			send_control(pos->beginColumn>>4|0x10); //set column address	列的设置分两步，现设置高四位，其中保证D4为1
			send_control((pos->beginColumn&0x0f));  // 设置列的低4位，此时保证D4为0

			for(j=0;j<pos->width;j++)
			{
				send_data(*(pvalue + i*pos->width+ j) );
			}
		}
		
		/* set the line address to line0  jjl change 0x40 to 80*/
		send_control(0x80);
	}
}
#endif

/* column and width is in unit of pixel, row and height is in unit of page  */
void lcdDrawData(int row, int column, int heigh/*in pages*/, int width, INT8U code *pvalue)
{
	INT8U data i=0,j=0;

	if(( (column + width) < MAX_COLUMNS) && 	(( row + heigh) <= MAX_PAGES) )
	{
		for(i=0; i<heigh; i++)
		{	
			send_control( (row<<3)+0x80); //set start line address   jjl change 0x40 to 80
			send_control( row+0xb0+i);	  //set page address
			
			/*set column address	列的设置分两步，现设置高四位，其中保证D4为1*/
			send_control( column>>4|0x10); 
			/* 设置列的低4位，此时保证D4为0*/
			send_control( (column&0x0f));

			for(j=0;j<width;j++)
			{
//				send_data(~(*(pvalue + i*width+ j) ));
				send_data((*(pvalue + i*width+ j) ));
			}
		}
		
		/* set the line address to line0  jjl change 0x40 to 80*/
		send_control(0x80);
	}
}

void __lcdCleanAsciiRow(int row)
{
	INT8U data _indexColumn;
	
	for(_indexColumn=0; _indexColumn< LCD_DRAW_AREA_MAX_COLUMN; _indexColumn++)
	{
		lcdDrawAscii12x16Char(row, _indexColumn, ' ' );
	}
}

void lcd_draw_ip_address(INT8U service)
{
	INT8U	data	i;
	lcd_ip_address_t xdata *address;
	if(service==LCD_SERVICE_LAN)
	{
		address = &stcLcd.lanAddress;
	}
	else
	{
		address = &stcLcd.wanAddress;
	}
	
	for(i=0; i<LCD_IP_ADRRESS_LENGTH/4; i++)
	{
		lcdDrawAscii12x16Char(0, i+1, address->value[i]);
		lcdDrawAscii12x16Char(0, i+5, address->value[3+i]);
		lcdDrawAscii12x16Char(1, i+1, address->value[6+i]);
		lcdDrawAscii12x16Char(1, i+5, address->value[9+i]);
	}
	

	lcdDrawAscii12x16Char(0, 4, '.');
	lcdDrawAscii12x16Char(1, 4, '.');

//	lcdDrawAscii5x8String(0, 0, stcLcd.services[service].info.help );
}

void lcdRefreshDrawArea(void)
{
	__lcdCleanAsciiRow(0);
	__lcdCleanAsciiRow(1);
	__lcdCleanAsciiRow(2);

	if( stcLcd.currentService == LCD_SERVICE_WAN && stcLcd.currentState != LCD_STATE_INITING)
	{
		lcd_draw_ip_address( LCD_SERVICE_WAN);
	}
	else if( stcLcd.currentService == LCD_SERVICE_LAN && stcLcd.currentState != LCD_STATE_INITING)
	{
		lcd_draw_ip_address( LCD_SERVICE_LAN);
	}
	else if( stcLcd.currentService == LCD_SERVICE_NET_MODE && stcLcd.currentState == LCD_STATE_MODIFY)
	{
		lcdDrawAscii12x16String(0, 0, (INT8U *)&netModeNames[0][0]+stcLcd.services[LCD_SERVICE_NET_MODE].state*(LCD_DRAW_AREA_MAX_COLUMN+1));
	}
	else //if(stcLcd.currentService!= LCD_SERVICE_LAN && stcLcd->currentService!= LCD_SERVICE_WAN)
	{
		lcdDrawAscii12x16String(0, 0, &stcLcd.services[stcLcd.currentService].info.title[0]);

	}
	
	LCD_DRAW_ALERT_ROW();
	LCD_DRAW_STATUS_ROW();
}

void lcdRefreshScreen(void)
{
	SCREEN_CLEAN();
	if(stcLcd.currentState == LCD_STATE_IDLE )
	{
		lcdScreenProtected();
	}
	else
	{
		lcdDrawArrows();
		lcdRefreshDrawArea();
	}
}

