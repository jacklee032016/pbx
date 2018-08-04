/*
 * $Id: as_pbx_cmd.c,v 1.9 2007/04/25 09:12:52 lizhijie Exp $
*/
#include "as_uart.h"
#include "aslib.h"

#include <stdlib.h>
#include <assert.h>


uart_t  serial_pbx = 
{
	name	:	"PBX UART Comm",
#if  __ARM_IXP__
	port		:	UART_TTYS0,
#else
	port		:	UART_TTYS1,
#endif

#if  !__USE_SOFTWARE__
	type		:	HANDSHANK_HARDWARE,
#else	
	type		:	HANDSHANK_SOFTWARE,
#endif

	baudrate	:	B9600,
	databits	:	CS8,
	stopbits	:	'A',
	parity	:	'N',

	fd		:	-1,
	
	open	:	uart_open,
	close	:	uart_close,
	read		:	uart_read,
	write	:	uart_write
};


/* send command to PBX through UART */
int  as_cmd_to_uart(char *data, void *_th)
{
	as_thread_t *th = (as_thread_t *)_th;
	uart_t *uart = &serial_pbx;
	int	length;
	int	res;

	PBX_MSG_LENGTH(data, length);
	length += 1;
	
	res = (uart->write)(uart, data, length);
	if(res != length)
	{
		AS_MSG_LOG(AS_LOG_ERR, "%s : write Op 0x%x to UART port : Wrote =%d  should be %d",(th)?th->name:"Unknown", *data, res, length);
	}

#if DEBUG_RAW_RW
	{
		int 	i;
		
		AS_MSG_LOG(AS_LOG_EMERG, "Data send to UART" );
		for(i=0; i< res; i++)
		{
			AS_MSG_LOG(AS_LOG_EMERG, "0x%x", *(data+i));
		}
		AS_MSG_LOG(AS_LOG_EMERG, "\n" );
	}
#endif

	return res;
}


/* receive command from PBX through UART */
int as_cmd_from_uart(char *buf, int size)
{
	uart_t *uart = &(serial_pbx);
	return (uart->read)(uart, buf, size );
}

/* parameter : timeout value, 0 : forever  */
int as_cmd_uart_init(int seconds, int millseconds)
{
	int res;
	uart_t *uart = &serial_pbx; 

	uart->tv_timeout.tv_sec = seconds;
	uart->tv_timeout.tv_usec = millseconds/1000;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,  "%s" , uart->name  );
	res = ( uart->open)( uart );
	if( AS_FAIL== res)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Error in open PBX Communication Serial Port : %s", uart->name );
	}	
	return res;
}

void as_cmd_uart_destroy(void)
{
	uart_t *uart = &serial_pbx;
	(uart->close)(uart);
}


