/*
* $Id: lcdHwCtrl.c,v 1.2 2007/05/22 06:35:12 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*����ʾ��״̬��æ����true*/
BOOL status_read(void)
{
	INT8U data status;
	LCD_DATA = 0xff;//����P1��Ϊ�����ʽ
	{
		LcdPinFORMAT = LCD_FORMAT_COMMAND;
		LcdPinCS = LCD_PIN_SIGNAL_ENABLE;
		LcdPinWRITE = LCD_PIN_SIGNAL_DISABLE;
		LcdPinREAD = LCD_PIN_SIGNAL_ENABLE;	  /*/������֮ǰҪ��һ��α��������*/
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
	
	while (status_read());//�ȴ�����״̬
}

/*���Ϳ�������,��һ������������*/
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
	while (status_read());//�ȴ�����״̬	
}

/*ȫ�������в�����
dispȫ����������ʾ(0xff)������(0x00),
xstartΪ��ʼ�У�countΪ����*/
void full_screen(INT8U displayChar,INT8U beginPage, INT8U height)
{
	INT8U data x=0,colume=0;
	
	for(;x<height;x++)
	{
		/* set start line ,ֻ���Գ�8������Ϊÿ�β������ǰ�ҳ���е�*/
		send_control((beginPage<<3)+0x40); 
		send_control(beginPage+0xb0+x); //set page
		send_control(0x10);	  //set colume to 0x00 �����������ã�ǰ�����ø�λ����� 
		send_control(0x00);	   //���õ�λ
		
		for(colume=0x00;colume<MAX_COLUMNS;colume++)	 //����ÿ��дһ���ֽ�-8λ
		{
			while (status_read());
			send_data(displayChar);
		}
	}
}

