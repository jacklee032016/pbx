/*
* $Id: lcdd_main.c,v 1.11 2007/09/26 20:05:24 lizhijie Exp $
*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "aslib.h"
#include "cgi_services.h"
#include "as_version.h"

#include "lcdd.h"

#define	AS_LCD_DAEMON_NAME				"LCDMonitor"

#define	LCDD_NAME							"LCD Daemon"

log_stru_t  lcdDaemonLog = 
{
	name		:	LCDD_NAME,
	lstyle		:	USE_FILE,
//	lstyle		:	USE_CONSOLE,
	llevel		: 	AS_LOG_DEBUG,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_LCDD,
	isDaemonized	:	1
};


int stc_lcd_usage()
{
	printf("\t-h : help;\r\n\t-s : syslogd;\r\n\t-d level : debug level\r\n\t\t5 : Dialed Digits\r\n\t\t6 : MSG/CMD\r\n");
	exit(1);
}


lcdd_t	lcdd;


static as_thread_t readUartThread =
{
	id		:	-1,
	name	:	"Read LCD Request",
	priority	:	3,
	handler	:	lcdd_read_thread,
	log		:	printf,
	priv		:	&lcdd
};

static as_thread_t servicesMonitorThread =
{
	id		:	-1,
	name	:	"Services Monitor",
	priority	:	3,
	handler	:	lcdd_monitor_thread,
	log		:	printf,
	priv		:	&lcdd
};

uart_t  serial_Lcd = 
{
	name	:	"LCD UART Comm",
#if  __ARM_IXP__
	port		:	USB_SERIAL_DEVICE,
#else
	port		:	UART_TTYS1,
#endif

#if  0//!__USE_SOFTWARE__
	type		:	HANDSHANK_HARDWARE,
#else	
	type		:	HANDSHANK_SOFTWARE,
#endif

	baudrate	:	B115200,
	databits	:	CS8,
	stopbits	:	'A',
	parity	:	'N',

	fd		:	-1,
	
	open	:	uart_open,
	close	:	uart_close,
	read		:	uart_read,
	write	:	uart_write
};


int lcd_threads_init(lcdd_t *lcdd)
{
	int res;
	uart_t *uart = &serial_Lcd; 
	
	uart->tv_timeout.tv_sec = 2;
	uart->tv_timeout.tv_usec = 25000/1000;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,  "%s" , uart->name  );
	res = ( uart->open)( uart );
	if( AS_FAIL== res)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Error in open PBX Communication Serial Port : %s", uart->name );
		return -1;
	}

	lcdd->uart = uart;

#if 0
	if(lcdd_get_ver_info(lcdd) )
		return 1;
#endif

	initPrintLcd( lcdd);
	lcdd->currentService = LCD_SERVICE_NET_MODE;

	as_thread_create( &readUartThread);
	as_thread_create( &servicesMonitorThread );/* must init it firstly*/

	as_thread_join( &servicesMonitorThread);
	as_thread_join( &readUartThread);

	return 0;
}

void lcd_threads_destory(lcdd_t *lcdd)
{
	(lcdd->uart->close)(lcdd->uart);
}


int main(int argc, char* argv[])
{
	int 	c;
	int	isLogSys = 0;
	int	level = AS_LOG_NOTICE;

	memset(&lcdd, 0 , sizeof(lcdd_t) );

	as_ver_debug(AS_VERSION_INFO(AS_LCD_DAEMON_NAME));
	as_ver_log(AS_LCD_DAEMON_NAME,AS_VERSION_INFO(AS_LCD_DAEMON_NAME));
	while (1)
	{
		c = getopt (argc, argv, "hd:vs");
		if (c == -1)
			break;

		switch (c)
		{
			case 'v':
				as_ver_log(AS_LCD_DAEMON_NAME,AS_VERSION_INFO(AS_LCD_DAEMON_NAME));
				exit(0);
				break;
			case 'h':
				stc_lcd_usage();
				break;
			case 'd':
				level = atoi(optarg);
				break;
			case 's':
				isLogSys = 1;
				break;
			default:
				stc_lcd_usage();
				break;
		}
	}	

	if(level <= AS_LOG_DEBUG && level>= AS_LOG_EMERG)
		lcdDaemonLog.llevel = level;

	if(isLogSys)
		lcdDaemonLog.lstyle = USE_SYSLOG;

	if(as_log_init( &lcdDaemonLog) < 0)
	{
		printf("%s Log File %s Init Failed.\n", lcdDaemonLog.name, lcdDaemonLog.logFileName);
	}

	if(lcd_threads_init(&lcdd))
		return 0;
	
	lcd_threads_destory(&lcdd);
	
	return 0;
}

