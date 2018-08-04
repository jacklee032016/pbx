/*
* $Id: lcdMacros.h,v 1.7 2007/09/26 19:47:32 lizhijie Exp $
*/
#ifndef  __LCD_MACRO_H__
#define	__LCD_MACRO_H__

/*¶¨Òå²»Í¬µÄÓ²¼þ°æ±¾*/
#define		VERSC					1
#define		VERSD					2
#define		HW_VERSION			VERSD

/*¶¨ÒåÁ½ÖÖ²»Í¬µÄÃæ°åÅÅÁÐ·½Ê½*/
#define		h4v4					0x00
#define		h5v3					0x01
#define		LCD_SCREEN_LAYOUT		h4v4

#define		lcd_d0 					P1_0		/*Òº¾§ÆÁµÄÊý¾ÝÏß0*/
#define		lcd_d1 					P1_1		/*Òº¾§ÆÁµÄÊý¾ÝÏß1*/
#define		lcd_d2 					P1_2		/*Òº¾§ÆÁµÄÊý¾ÝÏß2*/
#define		lcd_d3 					P1_3		/*Òº¾§ÆÁµÄÊý¾ÝÏß3*/
#define		lcd_d4 					P1_4		/*Òº¾§ÆÁµÄÊý¾ÝÏß4*/
#define		lcd_d5 					P1_5		/*Òº¾§ÆÁµÄÊý¾ÝÏß5*/
#define		lcd_d6 					P1_6		/*Òº¾§ÆÁµÄÊý¾ÝÏß6*/
#define		lcd_d7 					P1_7		/*Òº¾§ÆÁµÄÊý¾ÝÏß7*/

#define		ButtonMenu				P2_0		/* ±³¹â°´¼ü£¬°´ÏÂÎªµÍµçÆ½*/
#define		ButtonUp				P2_1		/* ÏÂ·­°´¼ü£¬°´ÏÂÎªµÍµçÆ½*/
#define		ButtonDown				P2_2		/* ÉÏ·­°´¼ü£¬°´ÏÂÎªµÍµçÆ½*/
#define		UART_CTS				P2_3


#if(HW_VERSION == VERSD)
	#define	LcdPinFORMAT 			P3_4		/*Òº¾§ÆÁµÄÊý¾Ý¸ñÊ½£º0ÎªÃüÁî£»1ÎªÊý¾Ý¡£*/
	#define	LcdPinRESET 			P3_3		/*Òº¾§ÆÁµÄ¸´Î»¶Ë£º0Îª¸´Î»£»1ÎÞÐ§¡£*/
	#define	LcdPinCS 				P3_2		/*Òº¾§ÆÁµÄÆ¬Ñ¡Ïß£º0ÎªÆ¬Ñ¡ÓÐÐ§£»1ÎÞÐ§¡£*/
	#define	LcdPinWRITE			P3_5		/*Òº¾§ÆÁµÄÐ´ÐÅºÅ£º0ÎªÐ´£»1ÎÞÐ§¡£*/
	#define	LcdPinREAD 				P3_6		/*Òº¾§ÆÁµÄ¶ÁÐÅºÅ£º0Îª¶Á£»1ÎÞÐ§¡£*/
	#define	LcdPinBACKLIGHT 		P3_7		/*Òº¾§ÆÁµÄ±³¹âÏß£º0ÎªÃð£»1ÎªÁÁ¡£*/
#elif(HW_VERSION == VERSC)
	#define	LcdPinFORMAT			P3_2		/*Òº¾§ÆÁµÄÊý¾Ý¸ñÊ½£º0ÎªÃüÁî£»1ÎªÊý¾Ý¡£ */
	#define	LcdPinRESET				P3_3		/*Òº¾§ÆÁµÄ¸´Î»¶Ë£º0Îª¸´Î»£»1ÎÞÐ§¡£ */
	#define	LcdPinCS				P3_4		/*Òº¾§ÆÁµÄÆ¬Ñ¡Ïß£º0ÎªÆ¬Ñ¡ÓÐÐ§£»1ÎÞÐ§¡ */£
	#define	LcdPinWRITE			P3_6		/*Òº¾§ÆÁµÄÐ´ÐÅºÅ£º0ÎªÐ´£»1ÎÞÐ§¡£ */
	#define	LcdPinREAD				P3_7		/*Òº¾§ÆÁµÄ¶ÁÐÅºÅ£º0Îª¶Á£»1ÎÞÐ§¡£ */
	#define	LcdPinBACKLIGHT		P3_5		/*Òº¾§ÆÁµÄ±³¹âÏß£º0ÎªÃð£»1ÎªÁÁ¡£ */
#endif


#define		LCD_CS_SIGNAL_ENABLE					FALSE
#define		LCD_CS_SIGNAL_INVALIDATE				TRUE

#define		LCD_RESET_SIGNAL_ENABEL				FALSE
#define		LCD_RESET_SIGNAL_INVALIDATE			TRUE

#define		LCD_FORMAT_COMMAND					FALSE
#define		LCD_FORMAT_DATA						TRUE

#define		LCD_PIN_SIGNAL_ENABLE				FALSE
#define		LCD_PIN_SIGNAL_DISABLE				TRUE

/*
* STC51 definations
* timer delay = (65525-timer)*12/22,118,400
* for Timer 0 about 25ms delay, it is 0x4C00(19456)
*/

#define		STC51_TIMER_0_25MS					0x4C	/* high byte */
#define		STC51_TIMER_1_64MS					0x04	/* ????? */		

#define		STC51_PIN_ENABLED						TRUE
#define		STC51_PIN_DISABLED					FALSE

#define		LCD_UART_CTS_ENABLE					FALSE
#define		LCD_UART_CTS_DISABLE					TRUE

#define		LCD_UART_BUFFER_COUNT				2

#define		LCD_PAGES_PER_ROW					2
#define		MAX_PAGES								8
#define		MAX_COLUMNS							132
#define		LCD_MAX_ROWS							4

#define		LCD_CHAR_DEFAULT_WIDTH				12
#define		LCD_CHAR_DEFAULT_HEIGHT				16

#define		LCD_CHAR_5X8_WIDTH					5
#define		LCD_CHAR_5X8_HEIGHT					8
#define		LCD_DRAW_AREA_MAX_ASCII5X8			21
#define		LCD_DRAW_AREA_ROW_ASCII5X8			6

#define		LCD_CHAR_12X16_WIDTH					(LCD_CHAR_DEFAULT_WIDTH)
#define		LCD_CHAR_12X16_HEIGHT				(LCD_CHAR_DEFAULT_HEIGHT)
#define		LCD_DRAW_AREA_MAX_ASCII12X16		9
#define		LCD_DRAW_AREA_ROW_ASCII12X16		2



#define		LCD_DRAW_AREA_MAX_ROW				6
#define		LCD_DRAW_AREA_MAX_COLUMN			9


#define		LCD_ARROW_HIGHT						16		/* 0x10 */
#define		LCD_ARROW_RIGHT_WIDTH				20

#define		RIGHT_ARROW_START_COLUMN			108


#define		LCD_IP_ADRRESS_LENGTH				12		/* not contains '.' in IP Address */

#define		LCD_IP_ADDRESS_ADD					1
#define		LCD_IP_ADDRESS_MINUS					-1

#if (LCD_SCREEN_LAYOUT == h4v4)
	#define	LCD_ARROW_UP_WIDTH					27		/* 0x1b */
	#define	SERVICE_COUNT							8
#else
	#define	LCD_ARROW_UP_WIDTH					25		/* 0x19 */
	#define	SERVICE_COUNT							9
#endif

#define		LCD_BUTTON_CLICK_NONE				0
#define		LCD_BUTTON_CLICK_UP					1
#define		LCD_BUTTON_CLICK_DOWN				2
#define		LCD_BUTTON_CLICK_MENU				3

#define		NOP()									_nop_()				


#define		SCREEN_CLEAN()				\
	full_screen(0x00, 0, MAX_PAGES)

#define		SCREEN_BLINK()				\
	full_screen(0xFF, 0, MAX_PAGES)

#define		LCD_DRAW_ALERT_ROW()		\
	lcdDrawAscii5x8String(0, 0, stcLcd.alertInfo)

#define		LCD_DRAW_STATUS_ROW()		\
	lcdDrawAscii5x8String(1, 0, stcLcd.statusInfo )


#define		LCD_BUTTON_CLIECKED(btn)	\
		((btn)==FALSE)

#define		IS_IP_ADDRESS()	\
		( (stcLcd.currentService == LCD_SERVICE_WAN) || (stcLcd.currentService == LCD_SERVICE_LAN) )

#define	GET_IP_ADDRESS_PTR(address)	\
	do{	if(stcLcd.currentService == LCD_SERVICE_LAN){	\
				(address) = &stcLcd.lanAddress; }		\
	else if(stcLcd.currentService == LCD_SERVICE_WAN){	\
		(address) = &stcLcd.wanAddress;	}				\
}while(0)


#define		IS_PROTOCOL()	\
		((stcLcd.currentService == LCD_SERVICE_NET_MODE) )

#define	IS_UART_NOT_SENDABLE()	\
		(lcdUart.requestInIndex == LCD_UART_CMD_END)

#define	ASCII_OFFSET			0x20


#define	START_TIMER(seconds)	\
		(stcLcd.menuStateTimeout = (seconds) )

#define	START_REFRESH_TIMER(fiveSeconds)	\
		(stcLcd.refreshTimeout = (fiveSeconds) )


#endif

