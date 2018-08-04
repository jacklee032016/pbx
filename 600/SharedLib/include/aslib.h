/*
 * $Id: aslib.h,v 1.26 2007/09/02 15:52:49 lizhijie Exp $
*/

#ifndef __ASLIB_H__
#define __ASLIB_H__
#ifdef __cplusplus
	extern "C"
	{
#endif

#define	DEBUG_RAW_RW						1

/* implemented System V Message Queue as Blocked */
#define	WITH_MSG_QUEUE_NOWAIT			0

#define	AS_LOG_FILE_UARTD					"/tmp/log/uartd.log"
#define	AS_LOG_FILE_TAPER					"/tmp/log/taper.log"
#define	AS_LOG_FILE_RECORD				"/tmp/log/record.log"
#define	AS_LOG_FILE_RENEW				"/usr/etc/renew.log"
#define	AS_LOG_FILE_SELFTEST				"/usr/etc/selftest.log"	/* This File is not used */

/* following LOG File are used by programs in WebAdmin modules */
#define	AS_LOG_FILE_CGIPBX				"/tmp/log/cgipbx.log"
#define	AS_LOG_FILE_LCDD					"/tmp/log/lcdd.log"
#define	AS_LOG_FILE_SET_DEFAULT			"/tmp/log/default.log"
#define	AS_LOG_FILE_RESTORE_TFTP			"/tmp/log/restore_tftp.log"
#define	AS_LOG_FILE_RESTORE_DEFAULT		"/tmp/log/restore_default.log"
#define	AS_LOG_FILE_WTD					"/tmp/log/wtd.log"

#define	AS_LOG_FILE_PCM_DEVICE			"/tmp/log/pcm.log"

#define	AS_LOG_FILE_STC_ISP				"/tmp/log/isp.log"
/* added this item in 2007.05.28 */
#define	AS_LOG_FILE_PEANUT				"/tmp/log/peanut.log"

unsigned char   as_lib_law_linear2alaw (short linear);
unsigned char  as_lib_law_linear2ulaw(short sample);
short as_lib_law_ulaw2linear(unsigned char sample);
short as_lib_law_full_ulaw2linear(unsigned char sample);
short as_lib_law_alaw2linear(unsigned char sample);
short as_lib_law_full_alaw2linear(unsigned char sample);

#define	A_LAW_CODE		0
#define	U_LAW_CODE		1  /* default code method of our driver */
#define	LINEAR2XLAW(s,law) ((law == U_LAW_CODE) ? as_lib_law_linear2ulaw(s) : as_lib_law_linear2alaw(s)) 
#define	XLAW(c,law) ((law ==  U_LAW_CODE) ? as_lib_law_ulaw2linear(c): as_lib_law_alaw2linear(c) )
#define	FULLXLAW(c,law) ((law ==  U_LAW_CODE) ? as_lib_law_full_ulaw2linear(c): as_lib_law_full_alaw2linear(c))


/* dial a digit string 
 digitstring : digit string to dial in ascii 
 length : length of every tone(char) in ms,default is 100 ms
 */
int as_lib_dial(int fd , char *digitstring ,int length );

#include "as_uart.h"
int uart_open (uart_t *uart );
void uart_close( uart_t *uart);
int  uart_read(uart_t *uart, void *buf, int length);
int  uart_write(uart_t *uart, void  *data, int length);


#include "as_thread.h"
int as_thread_create(as_thread_t *thread);
int as_thread_join(as_thread_t *thread);

#include "as_msg_queue.h"
void  as_msg_queue_init(int isUartd );
void as_msg_queue_destroy();

/* msg from the apps(sip,cgi,record,selftest) to uartd */
int as_cmd_apps_to_uart(PBX_COMMAND *cmd, AS_MESSAGE_TYPE type);
int as_cmd_uart_to_apps( PBX_COMMAND *cmd, AS_MESSAGE_TYPE type);

PBX_COMMAND *as_cmd_sip_from_uart(); 
PBX_COMMAND *as_cmd_cgi_from_uart();
PBX_COMMAND *as_cmd_selftest_from_uart();
PBX_COMMAND *as_cmd_record_from_uart();

#define CGI_TO_PBX_REBOOT	(2)

#include "as_record.h"
#include "as_utils.h"
#include "as_timer.h"

int as_log_init(log_stru_t *lobj);

#define	AS_SHARED_DEBUG		0

#if AS_SHARED_DEBUG
	#define	TRACE()	AS_MSG_LOG(AS_LOG_WARNING,"%s %s %d", __FILE__, __FUNCTION__, __LINE__)
#else
	#define	TRACE()	do{}while(0)
#endif

#ifndef   __AS_RELEASE__
#define	AS_MSG_LOG(level, format ,...) \
	do{ 					\
		if ( level <= AS_LOG_DEBUG && get_current_level() >= level) \
			log_information(level, __FILE__, __LINE__, format, ##__VA_ARGS__);	\
	}while(0)

#else
#define	AS_MSG_LOG(level, format ,...) \
	do{ 					\
		if ( level <= AS_LOG_DEBUG && get_current_level() >= level) \
			log_information(level, format, ##__VA_ARGS__);	\
	}while(0)
#endif


#ifndef   __AS_RELEASE__
#define	AS_MSG_DEBUG(level, format ,...) \
	do{ 					\
		if ( level <= AS_LOG_DEBUG && get_current_level() >= level) \
			log_information(level, __FILE__, __LINE__, format, ##__VA_ARGS__);	\
	}while(0)
#else
#define	AS_MSG_DEBUG(level, format ,...) 
#endif


#if WITH_OLD_LCD
#include "as_lcd.h"
int as_lcd_printf(unsigned int x, unsigned int y, const char* format,...);
int as_lcd_time_update();
#else
#define as_lcd_printf( x,  y,  args__...)      
#define as_lcd_time_update()

#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define KB(x)						((x) / 1024)
#define PERCENTAGE(x,total)		(((x) * 100) / (total))


int safe_open (const char *pathname,int flags);
void safe_rewind (int fd, int offset, const char *filename);
void safe_read (int fd,const char *filename,void *buf, size_t count, int verbose);


#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

/* time for warm-reset, when both set to 0, it set to maxium: about 64 seconds */
int as_watchdog_enable(int seconds, int millseconds);
int as_watchdog_disable();
int as_watchdog_update();
void as_beep_long(int times);
void as_beep_short(int times);
int as_get_button_states(void);
int as_ver_log( char *modulename, char *info);
int as_ver_opt (int argc, char **argv, char *info);
int as_ver_debug( char *verInfo);

int as_dial_init(int length);
int as_dial_dtmf(int fd, char*bufTelNumber);
int as_write_serial_no (unsigned char *serNO);
int as_read_serial_no(unsigned char *read_serial_no, int length);

#define FLASH_SERIAL_OFFSET 			131072L		/* offset is 128 K bytes */
#define FLASH_SERIAL_DEVICE			"/dev/mtd0"	

/* cmd-line flags */
#define FLAG_NONE			0x00
#define FLAG_VERBOSE		0x01
#define FLAG_HELP			0x02
#define FLAG_FILENAME		0x04
#define FLAG_DEVICE			0x08

/* 6 blocks and 32K per block, total 192 K for bootloader partition in FLASH mtd 
* First 3 blocks are occupied by bootloader
*/
#define LOCK_START 			0
#define LOCK_SECTORS 		3


/* size of read/write buffer */
#define BUFSIZE 				(10 * 1024)
#define SERIAL_BUFSIZE 		20


#define	AS_FLASH_MAGIC_START			0xAB
#define	AS_FLASH_MAGIC_END			0xCD
#define	AS_FLASH_MAGIC_LENGTH		2

#define	AS_FLASH_ITEM_LENGTH			32
typedef	struct
{
	char		magicStart[AS_FLASH_MAGIC_LENGTH];
	
	char		serialNo[AS_FLASH_ITEM_LENGTH];
	char		userName[AS_FLASH_ITEM_LENGTH];
	char		passwd[AS_FLASH_ITEM_LENGTH];

	char		wanMac[AS_FLASH_ITEM_LENGTH];
	char		wanIp[AS_FLASH_ITEM_LENGTH];
	char		wanMask[AS_FLASH_ITEM_LENGTH];

	char		lanMac[AS_FLASH_ITEM_LENGTH];
	char		lanIp[AS_FLASH_ITEM_LENGTH];
	char		lanMask[AS_FLASH_ITEM_LENGTH];

	char		magicEnd[AS_FLASH_MAGIC_LENGTH];
}AS_FLASH_INFO_T;

extern	AS_FLASH_INFO_T		factoryDefault;

int	as_default_write(AS_FLASH_INFO_T *defaultInfo);
int	as_default_read(AS_FLASH_INFO_T *defaultInfo);
void as_default_info_debug(AS_FLASH_INFO_T *info);

void as_write_93lc66();
void as_read_93lc66();


#define	STC_ISP_FIRST_BYTE		0x5A
#define	STC_ISP_SECOND_BYTE		0xA5
#define	STC_ISP_END_BYTE			0x16

int as_cmd_uart_init(int seconds, int millseconds);
void as_cmd_uart_destroy(void);

#ifdef __cplusplus
};
#endif

#endif

