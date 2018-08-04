#include "as_lcd.h"
#include "aslib.h"

#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

//extern char devices[32] = {0};

#define HEXDIFFDECIMAL  6
#define MAXLINELENGTH   21
#define LCD_S_DEVICE   "/dev/ttyS1"


#define TEXT_START 	0xee
#define TIME_START 	0xdd
#define LCD_END    	0xff

#define MIN_LINE       0x01
#define MAX_LINE	0x06

#define MIN_ROW       0x01
#define MAX_ROW	0x15

#define SPACE		0x20

//#define 

static uart_t  serial_lcd = 
{
	name	:	"LCD UART Comm",
	port		:	LCD_S_DEVICE,
	type		:	HANDSHANK_SOFTWARE,
	baudrate	:	B115200,
	databits	:	CS8,
	stopbits	:	'A',
	parity	:	'N',

	fd		:	-1,
	
	open	:	uart_open,
	close	:	uart_close,
	read		:	NULL,
	write	:	uart_write
};

int as_lcd_printf(unsigned int x, unsigned int y, const char * frmt,...)
{
	//printf("x: %d; y: %d\n", x, y);
	int errorno = -1;
	if ((x < MIN_LINE || x > MAX_LINE) || (y < MIN_ROW|| y > MAX_ROW))
		return errorno; // invaild position

	if (frmt == NULL)
		return errorno;
	
	va_list ap;
	va_start(ap, frmt);
	unsigned char text[MAXLINELENGTH+1] = {SPACE};
	vsnprintf(text, MAXLINELENGTH+1, frmt, ap);
	va_end(ap);

	printf("---to the pc client: %s\n", text);

	unsigned char buffer[MAXLINELENGTH+6+1] = {0};
	buffer[0] = buffer[1] = TEXT_START;
	buffer[2] = y;  //  ROW 
	buffer[3] = x;  // LINE

	int n = 0;
	
	while (n < MAXLINELENGTH+1-y && text[n] != 0)
	{
		buffer[n+4] = text[n];
		//printf("%d,", n);
		++n;
	}
	//printf("\n");

	buffer[n+4] = buffer[n+5] = LCD_END;
	return lcd_textout(buffer);
	
}


int  as_lcd_time_update()
{
	struct timeval tv;
	struct tm *ptm;

	int errorno = gettimeofday(&tv, NULL);
	if (errorno == -1)return errorno;
	ptm = localtime(&tv.tv_sec);


	int second = ptm->tm_sec;
	int minute = ptm->tm_min;
	int hour = ptm->tm_hour;
	int day = ptm->tm_mday;
	int month = ptm->tm_mon+1;
	int year = ptm->tm_year % 100;

	printf("current time: %d-%d-%d %d:%d:%d\n", year, month,day, hour, minute, second);


	char timebuf[11] = {TIME_START, TIME_START};
	timebuf[2] = BCD_fromat(year);
	timebuf[3] = BCD_fromat(month);
	timebuf[4] = BCD_fromat(day);
	timebuf[5] = BCD_fromat(hour);
	timebuf[6] = BCD_fromat(minute);
	timebuf[7] = BCD_fromat(second);

	int i = 2;
	for (i = 2; i < 8; ++i)
		timebuf[8] += timebuf[i];
	
	timebuf[9] = timebuf[10] = LCD_END;

	return lcd_textout(timebuf);

}

unsigned char BCD_fromat(int decimal)
{
	// max size only one byte
	if (decimal > 0xff) return -1;

	int bin1 = decimal / 10;
	int hex =  bin1*HEXDIFFDECIMAL + decimal;

	//unsigned char v = hex;
		
	return hex;
}


int lcd_textout(const unsigned char* buffer)
{


	//printf("%s\n",buffer);
	uart_t *uart2lcd = &serial_lcd;
	int errorno;
	errorno = (uart2lcd->open)(uart2lcd);
	if (errorno == -1)
	{
		return errorno;
	}
	errorno = (uart2lcd->write)(uart2lcd, (void *)(buffer), strlen(buffer));
	//printf("--writeing text size: %d\t --writed text size: %d\n",strlen(buffer), errorno);
	(uart2lcd->close)(uart2lcd);
	
	return errorno;

	#if 0
	
	errorno = (uart2lcd->open)(uart2lcd);
	if (errorno == -1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Open uart %s for LCD print failed!", DEVICE);
		printf("Open uart error.");
		return errorno;
	}

	//printf("Start...\n");
	unsigned char charbf[27]; //
	charbf[0] = 0xee;
	charbf[1]= 0xee;
	charbf[2] = 0x1;
	charbf[3] = 0x1; //
	unsigned int j = 4;
	for (j = 4; j < 25; ++j)
		charbf[j] = 0x38;
	charbf[25] = 0xff;
	charbf[26] = 0xff;
	
	#if 0
	unsigned char buffer[MAXLINELENGTH+6] = {0};
	buffer[0] = buffer[1] = 0xdd;
	buffer[25] = buffer[26] = 0xff;
	buffer[2] = y;  // vectory
	buffer[3] = x;  // horization
	#endif
	errorno = (uart2lcd->write)(uart2lcd, (void *)(charbf), sizeof(charbf));
	printf("--write text size: %d\n", errorno);

	(uart2lcd->close)(uart2lcd);

	return errorno;

	#endif

#if 0
		unsigned char tbuf[11]; // = {0xdd, 0xdd};
		tbuf[0] = 0xdd;
		tbuf[1] = 0xdd;
		tbuf[2] = 0x5;
		tbuf[3] = 0x8;
		tbuf[4] = 0x30;
		tbuf[5] = 0x1;
		tbuf[6] = 0x1;
		tbuf[7] = 0x1;
		tbuf[8] = 0x40; 
		tbuf[9] = 0xff;
		tbuf[10]= 0xff;
		
		errorno = (uart2lcd->open)(uart2lcd);
		if (errorno == -1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"Open uart %s for LCD print failed!", DEVICE);
			printf("Open uart error.");
			return errorno;
		}
		errorno = (uart2lcd->write)(uart2lcd, (void *)(tbuf), strlen(tbuf));
		printf("--write time size: %d\n", errorno);
		(uart2lcd->close)(uart2lcd);

	
	return errorno;
#endif 
}

#if 0
int lcd_uart_open(uart_t* thisuart)
{
	return 0;
}
#endif

