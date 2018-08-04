/*
* $Id: lcdTypes.h,v 1.9 2007/09/28 08:33:43 lizhijie Exp $
*/
#ifndef	__LCD_TYPES_H__
#define	__LCD_TYPES_H__


typedef	struct	
{
	INT8U	beginPage;

	INT8U	beginColumn;
	INT8U	width;
}lcd_position_t;


typedef	struct
{
	INT8U		code			*arrow[LCD_STATE_COUNT+1];
}lcd_arrow_t;


typedef	struct
{
	lcd_position_t				position;
	lcd_arrow_t		code		*arrows;
}lcd_service_arrow_t;


typedef	struct
{
	INT8U	index;	/* which digit is modifying now, from 0~11 */
	
	INT8U	value[LCD_IP_ADRRESS_LENGTH];
	
}lcd_ip_address_t;


#define		LCD_DATA		P1

typedef	struct
{
	INT8U	requestSeq;
	
	INT8U	requestInIndex;				/* used by Main Loop */
	INT8U	requestOutIndex;			/* used by ISR */
	INT8U	requestBuffersPosition;

	INT8U	responseInIndex;			/* used by ISR */
	INT8U	responseOutIndex;			/* used by Main Loop */
	INT8U	responseBuffersPosition;
}lcd_uart_t;

typedef	struct
{
	INT8U		state;
	INT8U		clickCount;

	INT8U		tickCounts;				/* 50ms /tick */
}lcd_button_t;

typedef	struct
{
	INT8U		title[LCD_DRAW_AREA_MAX_COLUMN*LCD_DRAW_AREA_MAX_ROW/2];
	INT8U		help[LCD_DRAW_AREA_MAX_ASCII5X8];
}lcd_service_info_t;

typedef	struct
{
#if 0
	INT8U		index;
	INT8U		arrowType;
#endif

	INT8S		state;
	INT8U		drawMode;			/* auto line feed ot not */

#if 0
	/* drawing text and coordination */
	INT8U		row;
	INT8U		column;
	INT8U		value[LCD_DRAW_AREA_MAX_COLUMN*LCD_DRAW_AREA_MAX_ROW/2];
	INT8U		help[LCD_DRAW_AREA_MAX_ASCII5X8];
#endif

	lcd_service_info_t		info;
}lcd_service_t;


typedef	struct
{
	/* state Machine handlers */
	LCD_STATE_T		currentState;
	LCD_EVENT_T		currentEvent;
	INT8U			menuStateTimeout;		/* in Unit of second */
	INT8U			refreshTimeout;			/* for screen refresh, in unit of of 5s */
	
	INT8S			currentService;
	lcd_service_t		services[LCD_SERVICE_MAX_NUMBER];
	
//	INT8U			areaBuf[LCD_DRAW_AREA_MAX_ROW/2][LCD_DRAW_AREA_MAX_COLUMN];

	lcd_ip_address_t	wanAddress;
	lcd_ip_address_t	lanAddress;

	lcd_button_t		button;

	INT8U			screenIndex;

	INT8U			*alertInfo;
	INT8U			*statusInfo;
	INT8U			flagStatusBar;
	INT8U			flagAlertBar;

	INT8U			timeInfo[LCD_DRAW_AREA_MAX_ASCII5X8];
}LCD_DRAWER_T;


typedef	struct
{
	INT8U 				counter50ms;
	INT8U 				counter100ms;
	INT8U 				counter500ms;
	INT8U 				counter1s;
	INT8U 				counter5s;
}lcd_options_t;


#endif

