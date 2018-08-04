/*
* $Id: lcdInit.c,v 1.8 2007/09/26 19:48:16 lizhijie Exp $
*/

#pragma	ot(9,speed)
#pragma small

#include "lcdDeclarations.h"

/*LCD的初始化程序，全屏显示，500ms后清空屏幕*/
void __lcd_init(void)
{
	while (status_read());
	send_control(0xe2);
	while (status_read());
	send_control(0xa0);//set adc to normal
	send_control(0xc8);//set common output to normal direction
	send_control(0xa2);//set lcd bias to 1/9 bias

	send_control(0x2c);//set power control to use internal power supply only ////////
	delay_ms(100);
	send_control(0x2e);//set power control to use internal power supply only////////
	delay_ms(10);
	send_control(0x2f);//set power control to use internal power supply only ///////
	delay_ms(10);

	send_control(0x24);//Select internal resistance ratio of the regulator resistor	
	send_control(0x81);//set Electronic volume mode on
	send_control(0x2f);//set Electronic volume resistor to largest

	send_control(0xf8);//set booster ratio mode on
	send_control(0x00);//set booster ratio to 3x
	
	send_control(0xa6);//set the lcd display normal
	//display all panel
	SCREEN_BLINK();
	send_control(0xaf);//set display on
	delay_ms(100);	   //delay 200ms

	SCREEN_CLEAN();
	send_control(0xaf);//set display on
	delay_ms(1000);	   //delay 200ms
	//after write the display Ram once, must set the line address to line0
	send_control(0x40);//set the line address to line0

	delay_ms(100);
}


void lcdInit(void)
{
	INT8U data i;
/*
	LcdPinRESET = LCD_PIN_SIGNAL_ENABLE;
	delay_ms(100);
	LcdPinRESET = LCD_PIN_SIGNAL_DISABLE;
*/

//	AUXR &= 0xfd;
	//LcdPinBACKLIGHT = 0;//backlight off
	// delay_ms(100);
	// LcdPinRESET = 0;//复位lcd
	//串口初始化程序
	ES = 0; // 禁止串口中断
	SCON = 0x50; /* 0101,0000 8 位数据位， 无奇偶校验*/

	T2CON = 0x34;		/* 0011，0100，由T2 作为波特率发生器*/
	RCAP2H = 0xff;		/* 时钟22.1184MHz, Low is 0xFA : 115200 波特率*/
	RCAP2L = 0xfa;
	
	LcdPinRESET = LCD_PIN_SIGNAL_ENABLE;
	
	/*/定时器初始化程序*/
	TMOD = 0x11;/* Timer 0 and 1 work in mode 1 : 16 bit counter */
	TR0 = STC51_PIN_DISABLED;    /*定时器停止计数 ，关闭计数器*/
	TR1 = STC51_PIN_DISABLED;
	
	TH0 = STC51_TIMER_0_25MS;
	TL0 = 0x00;
	ET1 = STC51_PIN_DISABLED;/* timer 1 Disabled */
	
	LcdPinRESET = LCD_PIN_SIGNAL_DISABLE;


	__lcd_init();
	//delay_ms(10);
	//LcdPinBACKLIGHT = 1;//backlight on
	EA = STC51_PIN_ENABLED;
	ES = STC51_PIN_ENABLED;
	ET0 = STC51_PIN_ENABLED;
	EA = 1;
	PS=1;			/* 串口中断级别优先*/

	UART_CTS = LCD_UART_CTS_ENABLE;
	
	lcdUart.requestSeq = 0;

	lcdUart.responseInIndex = 0;
	lcdUart.responseOutIndex = LCD_UART_CMD_END;
	lcdUart.responseBuffersPosition = 0;
	lcdUart.requestInIndex = 0;
	lcdUart.requestOutIndex = LCD_UART_CMD_END;
	lcdUart.requestBuffersPosition = 0;

	lcdOptions.counter50ms 	= 0;
	lcdOptions.counter100ms 	= 0;
	lcdOptions.counter500ms 	= 0;
	lcdOptions.counter1s 		= 0;
	lcdOptions.counter5s 		= 0;

	flag50Ms	= FALSE;
	flag100Ms	= FALSE;
	flag500Ms	= FALSE;
	flag1s 		= FALSE;
	flag5s		= FALSE;


	stcLcd.currentState = LCD_STATE_FIRST;

	stcLcd.currentEvent = LCD_EVENT_NONE;
	
	stcLcd.services[LCD_SERVICE_NET_MODE].state = LCD_SERVICE_STATE_DISABLE;
	stcLcd.currentService = LCD_SERVICE_NET_MODE;
	for(i=0; i< LCD_SERVICE_MAX_NUMBER; i++)
	{
		x_str_copy( (INT8U *)&stcLcd.services[i].info, (INT8U *)&defaultServiceInfos[i], sizeof(lcd_service_info_t) );
		stcLcd.services[i].state = LCD_SERVICE_STATE_NULL;
	}
	
	stcLcd.wanAddress.index = 0;
	stcLcd.lanAddress.index = 0;
	for(i=0; i< LCD_IP_ADRRESS_LENGTH;i++)
	{
		stcLcd.wanAddress.value[i] = '0';
		stcLcd.lanAddress.value[i] = '1';
	}
	
	stcLcd.button.clickCount = 0;
	stcLcd.button.state = LCD_BUTTON_CLICK_NONE;
	stcLcd.screenIndex = 0;

	stcLcd.statusInfo = (INT8U *)&verInfo;

	stcLcd.alertInfo = &stcLcd.services[stcLcd.currentService].info.help[0];
	stcLcd.timeInfo[0] = 0;
	stcLcd.flagAlertBar = TRUE;
	stcLcd.flagStatusBar = TRUE;
	START_REFRESH_TIMER(6);
	
	blinkOnArrow = TRUE;
	blinkOnIp = TRUE;
/*
	显示WELCOME
	for(i=0;i<593;i+=8)
	{
		for(j=0;j<4;j++)
			lcdDisplayData(0x02+j,0x00,0x01,0x80,&welcome[j][i]);
		delay_ms(300); //jjl change 350 to 
	}
*/   

	delay_ms(50);	  
	SCREEN_CLEAN();
	
	lcdUartRequestState(stcLcd.currentService);
	/* display Logon */
	lcdScreenProtected();
	
	delay_ms(1000);
	SCREEN_CLEAN();;
}

