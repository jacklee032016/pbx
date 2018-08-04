/*
 * $Author: lizhijie $
 * $Log: as_uart.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.2  2005/08/30 08:27:43  wuaiwu
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_uart.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
 * as_uart.h
 * Common Header file for UART operation, Li Zhijie, 2005.05.23 
*/

#ifndef  __AS_UART_H__
#define __AS_UART_H__

#include <termios.h>            /* tcgetattr, tcsetattr */
#include <sys/types.h>
#include <unistd.h>             /* read, write, close */

#include "as_global.h"

#if  __ARM_IXP__
#define SERIAL_CONSOLE		1		/* Console is ttyS1 in IXP422 */
#define SERIAL_PBX			0		/* ttyS0 in IXP422 */
#else
#define SERIAL_CONSOLE		1
#define SERIAL_PBX			0
#endif


#define  UART_TTYS0			"/dev/ttyS0"
#define  UART_TTYS1			"/dev/ttyS1"
#define  UART_TTYS2			"/dev/ttyS2"

 
#define TIMEOUT_SEC(buflen,baud) 	(buflen*20/baud+2)
#define TIMEOUT_USEC 0

/*for debug*/
#define  UART_DEBUG 	1

 #if   UART_DEBUG
	#define  UARTDEBUG(fmt,args...)	printf(fmt,##args)
 #else    
	#define  UARTDEBUG(fmt,args...)    
 #endif

typedef enum
{
	HANDSHANK_HARDWARE = 0,
	HANDSHANK_SOFTWARE = 1	
}AS_UART_HANDSHANK_TYPE;

struct  AS_UART;

struct AS_UART
 {
 	char name[64];

	char port[64];
	
	AS_UART_HANDSHANK_TYPE 	type;

/*B0/B50/B75/B110/B134/B150/B200/B300/B600/B1200/B2400/B9600/B19200/B38400/B57600/B115200 */	
	int baudrate;
/* CS8/CS7/CS6/CS5 */
	int databits;
	char stopbits;				/* 'A' : 1 stop bit;	 'B' : 1.5 stop bits ;		'C' : 2 stop bits*/
	char parity;				/* 'N' : no check;	 'E' : even check; 		'O' : odd check  */
	
	int    fd;

	struct 	termios termios_old;
	fd_set	fs_read, fs_write;
	struct	timeval tv_timeout;

	int (* open)(struct AS_UART *uart);
	void (* close)(struct AS_UART *uart);
	int (* read)(struct AS_UART *uart, void *buf, int length);
	int (* write)(struct AS_UART *uart, void *data, int length);
 };

typedef  struct AS_UART		uart_t;

int uart_open (uart_t*);
int  uart_write(uart_t*, void*, int);
void uart_close( uart_t*);
#endif

