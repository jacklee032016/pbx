/*
* $Id: lcdHwCtrl.c,v 1.2 2007/05/22 06:35:12 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*读显示屏状态，忙返回true*/
BOOL status_read(void)
{
	INT8U data status;
	LCD_DATA = 0xff;//设置P1口为输入格式
	{
		LcdPinFORMAT = LCD_FORMAT_COMMAND;
		LcdPinCS = LCD_PIN_SIGNAL_ENABLE;
		LcdPinWRITE = LCD_PIN_SIGNAL_DISABLE;
		LcdPinREAD = LCD_PIN_SIGNAL_ENABLE;	  /*/读数据之前要加一条伪读操作。*/
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		LcdPinREAD = LCD_PIN_SIGNAL_ENABLE;
	}
	
	status = LCD_DATA;
	NOP();
	{
		LcdPinCS = LCD_PIN_SIGNAL_DISABLE;
		LcdPinFORMAT = LCD_FORMAT_DATA;
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		LcdPinWRITE = LCD_PIN_SIGNAL_DISABLE;
	}
	
	return(status&0x90);
	//if(status&0x90!=0)return(true);	
	//else return (false);
}

/* draw a column which height is one page, eg. 8 bits */
void send_data(INT8U datas)
{
	{
		LcdPinCS = LCD_PIN_SIGNAL_ENABLE;			//display data write
		NOP();
		LcdPinFORMAT = LCD_FORMAT_DATA;
		NOP();
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		NOP();
		LcdPinWRITE = LCD_PIN_SIGNAL_ENABLE;
		NOP();
	}
	LCD_DATA = datas;
	NOP();
	{
		NOP();
		LcdPinCS = LCD_PIN_SIGNAL_DISABLE;		   //stop display data write
		NOP();
		LcdPinFORMAT = LCD_FORMAT_DATA;
		NOP();
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		NOP();
		LcdPinWRITE = LCD_PIN_SIGNAL_DISABLE;
	}
	
	while (status_read());//等待空闲状态
}

/*发送控制命令,第一个参数是命令*/
void send_control(INT8U control)
{
	{
		LcdPinCS = LCD_PIN_SIGNAL_ENABLE;			  //
		NOP();
		LcdPinFORMAT = LCD_FORMAT_COMMAND;
		NOP();
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		NOP();
		LcdPinWRITE = LCD_PIN_SIGNAL_ENABLE;
		NOP();
	}
	LCD_DATA = control;
	NOP();
	{
		NOP();
		LcdPinCS = LCD_PIN_SIGNAL_DISABLE;
		NOP();
		LcdPinFORMAT = LCD_FORMAT_DATA;
		NOP();
		LcdPinREAD = LCD_PIN_SIGNAL_DISABLE;
		NOP();
		LcdPinWRITE = LCD_PIN_SIGNAL_DISABLE;
	}
	while (status_read());//等待空闲状态	
}

/*全屏或整行操作，
disp全屏或整行显示(0xff)或清屏(0x00),
xstart为起始行，count为行数*/
void full_screen(INT8U displayChar,INT8U beginPage, INT8U height)
{
	INT8U data x=0,colume=0;
	
	for(;x<height;x++)
	{
		/* set start line ,只所以乘8，是因为每次操作都是按页进行的*/
		send_control((beginPage<<3)+0x40); 
		send_control(beginPage+0xb0+x); //set page
		send_control(0x10);	  //set colume to 0x00 ，分两次设置，前次设置高位，后次 
		send_control(0x00);	   //设置低位
		
		for(colume=0x00;colume<MAX_COLUMNS;colume++)	 //纵向每次写一个字节-8位
		{
			while (status_read());
			send_data(displayChar);
		}
	}
}

