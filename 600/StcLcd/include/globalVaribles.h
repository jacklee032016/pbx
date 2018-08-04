/*
* $Id: globalVaribles.h,v 1.4 2007/09/26 19:47:32 lizhijie Exp $
*/

#define		FLAGS_BIT_50MS			0
#define		FLAGS_BIT_100MS			1
#define		FLAGS_BIT_500MS			2
#define		FLAGS_BIT_1S				3
#define		FLAGS_BIT_5S				4
#define		FLAGS_BIT_BLINK_ARROW	5
#define		FLAGS_BIT_BLINK_IP			6
	
INT8U		bdata		globalFlags;

sbit	flag50Ms		=	globalFlags^FLAGS_BIT_50MS;
sbit	flag100Ms		=	globalFlags^FLAGS_BIT_100MS;
sbit	flag500Ms		=	globalFlags^FLAGS_BIT_500MS;
sbit	flag1s			=	globalFlags^FLAGS_BIT_1S;
sbit	flag5s			=	globalFlags^FLAGS_BIT_5S;

sbit 	blinkOnArrow 	= 	globalFlags^FLAGS_BIT_BLINK_ARROW;
sbit 	blinkOnIp 		= 	globalFlags^FLAGS_BIT_BLINK_IP;

lcd_options_t			data				lcdOptions;


LCD_UART_RESPONSE	xdata			lcdUartResponse[LCD_UART_BUFFER_COUNT];
LCD_UART_REQUEST	xdata			lcdUartRequest[LCD_UART_BUFFER_COUNT];


LCD_DRAWER_T		xdata			stcLcd;
lcd_uart_t			xdata			lcdUart;


