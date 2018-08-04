 /*
 * $Log: as_uart.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.3  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.2  2005/06/03 03:42:05  lizhijie
 * debug the timeout operation of 'select', refer to comments
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_uart.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/

#include "as_uart.h"

#include <stdio.h>              /* perror, printf, puts, fprintf, fputs */
#include <fcntl.h>              /* open */
#include <sys/signal.h>
#include <string.h>             /* bzero, memcpy */
#include <limits.h>             /* CHAR_MAX */
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/* 
 * Decription for TIMEOUT_SEC(buflen,baud);
 * baud bits per second, buflen bytes to send.
 * buflen*20 (20 means sending an octect-bit data by use of the maxim bits 20)
 * eg. 9600bps baudrate, buflen=1024B, then TIMEOUT_SEC = 1024*20/9600+1 = 3 
 * don't change the two lines below unless you do know what you are doing.
*/

static void __uart_set_stopbit (struct termios *t_new,  char stopbit)
{
	switch(stopbit)
	{
		case 'A':
			t_new->c_cflag &= ~CSTOPB; /* 1 stop bit */
			break;
		case  'B':
			t_new->c_cflag &= ~CSTOPB; /* 1.5 stop bits */
			break;
		case 'C':
			t_new->c_cflag |= CSTOPB;  /* 2 stop bits */
			break;
		default:
			t_new->c_cflag &= ~CSTOPB; /* 1 stop bit */
			break;
	}
}

static void __uart_set_paritycheck (struct termios *t_new, char parity)
{
	switch (parity) 
	{
		case 'N':			/* no parity check */
			t_new->c_cflag &= ~PARENB;
			break;
		case 'E':			/* even */
			t_new->c_cflag |= PARENB;
			t_new->c_cflag &= ~PARODD;
			break;
		case 'O':			/* odd */
			t_new->c_cflag |= PARENB;
			t_new->c_cflag |= ~PARODD;
			break;
		default:			/* no parity check */
			t_new->c_cflag &= ~PARENB;
			break;
	}
}

static void __uart_set_port_attribute (uart_t *uart, struct 	termios *t_new )
{
	memset(t_new, 0, sizeof (struct termios) );
	cfmakeraw( t_new );

	 /* set baudrate */
	t_new->c_cflag = uart->baudrate;
	
	if(uart->type == HANDSHANK_SOFTWARE)
	/* software hand-shank */
		t_new->c_cflag |= CLOCAL | CREAD ; 
	else 	/* hardware hand-shank */
		t_new->c_cflag |= CLOCAL | CREAD | CRTSCTS; 

	 /* set databits */
	t_new->c_cflag &= ~CSIZE;
	t_new->c_cflag |= uart->databits; 

	/*set paritycheck*/
	__uart_set_paritycheck (t_new, uart->parity);

	/* set stopbits */
	__uart_set_stopbit (t_new, uart->stopbits);

	t_new->c_oflag = 0;
	t_new->c_lflag |= 0;
	t_new->c_oflag &= ~OPOST;
	t_new->c_cc[VTIME] = 1;        /* unit: 1/10 second. */
	t_new->c_cc[VMIN] = 1; /* minimal characters for reading */
}

/**/
int uart_open (uart_t *uart )
{
	int res;
	struct 	termios termios_new;

	uart->fd = open ( uart->port, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if ( uart->fd < 0) 
	{
		fprintf (stderr, "cannot open port %s, error is %s\r\n",  uart->port, strerror(errno) );
		return AS_FAIL ;
	}

	tcgetattr (uart->fd, &( uart->termios_old) ); 

	__uart_set_port_attribute (uart, &termios_new );

	tcflush ( uart->fd, TCIFLUSH);

	res = tcsetattr (uart->fd, TCSANOW, &termios_new);
	if(res < 0)
	{
		fprintf (stderr, "cannot set UART port %s attribute, error is %s\r\n",  uart->port, strerror(errno) );
		close(uart->fd);
		return AS_FAIL;
	}

	return AS_OK;
}

/* close serial port by use of file descriptor fd */
void uart_close( uart_t *uart)
{
	tcsetattr (uart->fd, TCSADRAIN, &( uart->termios_old ) ); 			  /* flush output data before close and restore old attribute */
	close (uart->fd);
}

int  uart_read(uart_t *uart, void *buf, int length)
{
	int   	res = 0;
	FD_ZERO (&(uart->fs_read) );
	FD_SET ( uart->fd, &(uart->fs_read) );
//	uart->tv_timeout.tv_sec = 0;//TIMEOUT_SEC (datalength, GetBaudrate ())  ;
//	uart->tv_timeout.tv_usec = TIMEOUT_USEC;

	res = select ( uart->fd + 1, &(uart->fs_read), NULL, NULL, &( uart->tv_timeout) );
/*	if (res )  change this line as following, Li zhijie, 2005.06.03 */
	if (res>0 ) 
	{
		return (read( uart->fd, buf, length));
	}
	else if(res == 0 )
	{
//		printf("Timeout when read serial port in 'SELECT'\r\n");
		;
	}
	else  if(res <0 )
	{
		printf("Failed when read serial port in 'SELECT', error is %s\r\n", strerror(errno) );
	}
		
	return res;
}

/* 
 * Write datalength bytes in buffer given by unsigned char  *data,
 * return value: bytes written
 * Nonblock mode
*/
int  uart_write(uart_t *uart, void  *data, int length)
{
	int  retval=1, len = 0, total_len = 0;

	FD_ZERO (&(uart->fs_write) );
	FD_SET ( uart->fd, &( uart->fs_write) );
//	uart->tv_timeout.tv_sec = 0;//TIMEOUT_SEC (datalength, GetBaudrate ());
//	uart->tv_timeout.tv_usec = TIMEOUT_USEC;

	for (total_len = 0; total_len < length;) 
	{

		retval = select (uart->fd + 1, NULL, &( uart->fs_write), NULL, &( uart->tv_timeout) );
/*		if (retval)		changed this line as following, Li zhijie,2005.05.03 */
		if (retval>0)
		{
			len=write( uart->fd, (void *)(data + total_len), length - total_len);
			if (len > 0) 
			{
				total_len += len;
			}
		}
		else if(retval == 0)
		{
			printf("Timeout when send out in serial port\r\n");
			tcflush ( uart->fd, TCOFLUSH);     /* flush all output data */
			break;
		}
		else if( retval <0)
		{
			printf("Failed when 'SELECT', error is %s\r\n", strerror(errno));
		}
	}

	return (total_len);
}

