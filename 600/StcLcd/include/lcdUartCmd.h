/*
* $Id: lcdUartCmd.h,v 1.11 2007/09/26 19:47:32 lizhijie Exp $
*/

#ifndef	__LCD_UART_CMD_H__
#define	__LCD_UART_CMD_H__

#ifndef 	INT8U
#define	INT8U	unsigned char
#endif

typedef enum
{
	LCD_SERVICE_WEB 		= 0,
	LCD_SERVICE_FILE		= 1,
	LCD_SERVICE_MAIL		= 2,
	LCD_SERVICE_PRINT		= 3,
	LCD_SERVICE_DHCP		= 4,
	LCD_SERVICE_LAN		= 5,
	LCD_SERVICE_WAN		= 6,
	LCD_SERVICE_NET_MODE	= 7,
	LCD_SERVICE_TIME		= 8,
	LCD_SERVICE_VERSION	= 9			/* added 2007.09.26 */
}LCD_SERVICE_TYPE_T ;

#define	LCD_SERVICE_MAX_NUMBER						LCD_SERVICE_VERSION

typedef	enum
{
	LCD_SERVICE_STATE_DISABLE = 0,		/* state is disabled, which arrow is spare */
	LCD_SERVICE_STATE_ENABLE	= 1,		/* state is enabled, which arrow is filled */
	LCD_SERVICE_STATE_NULL	= 2		/* null, eg. not show this arrow */
}LCD_SERVICE_STATE_T;

typedef	enum
{
	LCD_NET_MODE_PPPOE 	= 0,
	LCD_NET_MODE_DHCP	= 1,
	LCD_NET_MODE_STATIC	= 2
}LCD_NET_MODE_T;

#define	LCD_NET_MODE_MAX								(LCD_NET_MODE_PPPOE+1)

#define		LCD_UART_DRAW_MODE_LINEWRAP			0
#define		LCD_UART_DRAW_MODE_NOWRAP				1

#define		LCD_STATE_COUNT							(LCD_SERVICE_STATE_NULL+1)

#define		LCD_UART_REQUEST_DATA_LENGTH			29
#define		LCD_UART_RESPONSE_DATA_LENGTH			24

/* from STC--> IP */
struct _LCD_UART_REQUEST
{
	INT8U				op;
//	INT8U				op2;
	INT8U				service;
	INT8U				state;		/* used only when update IP access mode and */

	INT8U				sequence;

	INT8U				loadData[LCD_UART_REQUEST_DATA_LENGTH];
}
#ifdef	 __C51__
;
#else
__attribute__ ((packed));
#endif

typedef	struct _LCD_UART_REQUEST	LCD_UART_REQUEST;


struct _LCD_UART_RESPONSE
{
	INT8U					op;

#ifdef	 __C51__
	LCD_SERVICE_TYPE_T		service;
	LCD_SERVICE_STATE_T	state;
#else
	INT8U					service;
	INT8U					state;
#endif

#if 0	
	INT8U					row;
	INT8U					column;
#endif

	INT8U					length;

	INT8U					drawMode;	/* line wrap or not line wrap */
	INT8U					sequence;	/*  */

	INT8U					loadData[LCD_UART_RESPONSE_DATA_LENGTH];
}
#ifdef	 __C51__
;
#else
__attribute__ ((packed));
#endif

typedef		struct _LCD_UART_RESPONSE	LCD_UART_RESPONSE;

#define		LCD_UART_CMD_STRING							0XAA
#define		LCD_UART_CMD_LOOKUP							0XAB
#define		LCD_UART_CMD_CONFIG							0XAC
#define		LCD_UART_CMD_GET_VER_INFO						0XAD	/* added 2007.09.26 */
#define		LCD_UART_CMD_END								0XFF

#define		LCD_CMD_SPACE									0X20


#define		LCD_DEFAULT_NAME_WEB				"   Web     Server "
#define		LCD_DEFAULT_NAME_FILE				"   File    Server"
#define		LCD_DEFAULT_NAME_MAIL				"   Mail    Server"
#define		LCD_DEFAULT_NAME_PRINTER			" Printer Subsystem"
#define		LCD_DEFAULT_NAME_DHCP				"   DHCP    Server"
#define		LCD_DEFAULT_NAME_CAMERA				"  Camera Subsystem"
#define		LCD_DEFAULT_NAME_LAN					"  LAN IP"
#define		LCD_DEFAULT_NAME_WAN				"  WAN IP"
#define		LCD_DEFAULT_NAME_PROTOCOL			" Protocol"


#define		LCD_VER_MAJOR_0						'0'
#define		LCD_VER_MAJOR_1						'1'
#define		LCD_VER_MINOR_0						'1'
#define		LCD_VER_MINOR_1						'1'

#if 0
typedef	struct
{
	INT8U					major1;
	INT8U					major2;
	
	INT8U					dot;
	
	INT8U					minor1;
	INT8U					minor2;

	INT8U					space;
	INT8U					dateBuild[12];
}lcd_ver_t;
#endif


#endif

