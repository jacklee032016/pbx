/*
* $Id: lcdDeclarations.h,v 1.7 2007/09/26 19:47:32 lizhijie Exp $
*/

#ifndef	__LCD_DECLARATIONS_H__
#define	__LCD_DECLARATIONS_H__

#include "datatype.h"
#include <AT89X52.H>
#include	<intrins.h>
#include "lcdMacros.h"
#include "lcdUartCmd.h"
#include "lcdStateMachine.h"
#include "lcdTypes.h"

extern	INT8U		bdata		globalFlags;

extern	bit	flag50Ms;
extern	bit	flag100Ms;
extern	bit	flag500Ms;
extern	bit	flag1s;
extern	bit	flag5s;
extern	bit 	blinkOnArrow;
extern	bit 	blinkOnIp;


extern	lcd_options_t			data				lcdOptions;


extern	LCD_UART_RESPONSE	xdata			lcdUartResponse[LCD_UART_BUFFER_COUNT];
extern	LCD_UART_REQUEST	xdata			lcdUartRequest[LCD_UART_BUFFER_COUNT];


extern	LCD_DRAWER_T		xdata			stcLcd;
extern	lcd_uart_t			xdata			lcdUart;

extern	INT8U				code 			assist[5][128];
extern	lcd_service_info_t 		code				defaultServiceInfos[LCD_SERVICE_MAX_NUMBER];
extern	INT8U				code				defaultStateNames[3][LCD_DRAW_AREA_MAX_COLUMN];
extern	INT8U				code				netModeNames[3][LCD_DRAW_AREA_MAX_COLUMN];
extern	INT8U				code				verInfo[];
extern	INT8U				code				stringWaiting[];
extern	INT8U				code				stringReboot[];
extern	INT8U				code				stringWanError[];

void x_str_copy(INT8U *dest, INT8U *src, INT8U length );
void delay_ms(INT16U ms);

void lcdInit(void);
BOOL status_read(void);
void send_data(INT8U datas);
void send_control(INT8U control);
void full_screen(INT8U displayChar,INT8U beginPage, INT8U height);


void lcdDrawAscii12x16Char(INT8U row, INT8U column, INT8U digitOrChar);
void lcdDrawAscii12x16String(INT8U row, INT8U column, INT8U *digitOrChar);
void lcdDrawAscii5x8String(INT8U row, INT8U column, INT8U *digitOrChar);


void lcdDisplayData(lcd_position_t *pos, INT8U code *pvalue);
void lcdDrawData(int row, int column, int heigh/*in pages*/, int width, INT8U code *pvalue);
void lcdDrawArrow(INT8U index, INT8U serviceState);
void lcdDrawArrows(void);
void lcdRefreshDrawArea(void);
void lcdRefreshScreen(void);
void lcdScreenProtected(void);

void lcdTimerTask(void);

void	lcdUartRxResponse(void);
void lcdUartRequestState(INT8U service);
void lcdUartRequestUpdateState(INT8U service);
void lcdUartRequestUpdateIpAddress(INT8U service);

void	lcdStatemachineHandle(void);

INT8U lcdUartSendVerInfo(void);
void update_status_bar(void);

#endif

