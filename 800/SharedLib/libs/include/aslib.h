/*
 * $Author: lizhijie $
 * $Log: aslib.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.17  2005/11/30 09:16:19  wangwei
 * no message
 *
 * Revision 1.16  2005/10/14 05:12:11  wangwei
 * no message
 *
 * Revision 1.15  2005/09/30 09:46:27  wuaiwu
 * no message
 *
 * Revision 1.14  2005/09/28 05:35:26  wangwei
 * no message
 *
 * Revision 1.13  2005/09/16 09:22:27  wangwei
 * 增加读写序列号函数的申明
 *
 * Revision 1.12  2005/09/07 11:03:41  wangwei
 * no message
 *
 * Revision 1.11  2005/09/01 08:59:58  tangr
 * deal version
 *
 * Revision 1.10  2005/08/30 08:27:43  wuaiwu
 * no message
 *
 * Revision 1.9  2005/08/04 08:24:15  lijie
 * no message
 *
 * Revision 1.8  2005/08/04 03:15:18  lijie
 * 1.串口读bug修正
 * 2.添加发送dtmf音的接口
 *
 * Revision 1.7  2005/08/02 01:35:29  wuaiwu
 * added interfaces of syslog
 *
 * Revision 1.6  2005/07/15 07:43:04  wuaiwu
 * no message
 *
 * Revision 1.5  2005/07/08 01:36:11  lijie
 * no message
 *
 * Revision 1.4  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.3  2005/06/15 06:09:10  wangwei
 * 增加蜂鸣器的用户程序接口的函数声明
 *
 * Revision 1.2  2005/06/07 09:51:02  lizhijie
 * add watchdog function declarations
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: aslib.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/

#ifndef __ASLIB_H__
#define __ASLIB_H__
#ifdef __cplusplus
	extern "C"
	{
#endif

/*********************************************************************
 *  Following are utils function used in driver 
*********************************************************************/
void as_daemon_init();

unsigned char   as_lib_law_linear2alaw (short linear);
unsigned char  as_lib_law_linear2ulaw(short sample);
short as_lib_law_ulaw2linear(unsigned char sample);
short as_lib_law_full_ulaw2linear(unsigned char sample);
short as_lib_law_alaw2linear(unsigned char sample);
short as_lib_law_full_alaw2linear(unsigned char sample);

#define A_LAW_CODE		0
#define U_LAW_CODE		1  /* default code method of our driver */
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


#include "as_pbx_cmd.h"
int  as_cmd_to_uart(PBX_COMMAND *cmd);
PBX_COMMAND *as_cmd_from_uart(void );
/*parameter timeout value*/
int as_cmd_uart_init(int seconds, int millseconds);
void as_cmd_uart_destroy();

#include "as_msg_queue.h"
void  as_msg_queue_init( );
void as_msg_queue_destroy();

//the uart recieve sip cmd from rmsgid
PBX_COMMAND *as_cmd_uart_from_sip( );
//the uart recieve cgi cmd from rmsgid
PBX_COMMAND *as_cmd_uart_from_cgi( );

//recvie the cmd from uart then to smsgid then send to cgi
int as_cmd_uart_to_cgi( PBX_COMMAND *cmd );
//recive the cmd from uart then then to smsgid then send to sip
int as_cmd_uart_to_sip( PBX_COMMAND *cmd );

//the msg from the cgi and send to the rmsgid
int as_cmd_cgi_to_uart(PBX_COMMAND *cmd );
//the msg from the sip and send to the rmsgid
int as_cmd_sip_to_uart( PBX_COMMAND *cmd );

//sip recvie the msg from the smgid 
PBX_COMMAND *as_cmd_sip_from_uart(); 
//cgi recvie the msg from the smgid
PBX_COMMAND *as_cmd_cgi_from_uart();

#define CGI_TO_PBX_REBOOT	(2)

#include "as_taper.h"
void set_parameter(recorder_parameter_t *param);
int active_test();
int start_work(int);
#include "as_utils.h"

int as_log_init(log_stru_t lobj);


#define as_cplog(pri,frmt, args...)\
if (pri <= 7 && get_current_level() >= pri)\
log_information(pri, __FILE__, __LINE__,frmt,##args)
#ifndef   __AS_RELEASE__
#define as_debug(pri,frmt, args...)\
if (pri <= 7 && get_current_level() >= pri)\
log_information(pri, __FILE__, __LINE__,frmt,##args)
#else
#define as_debug(pri,frmt, args...)
#endif


#include "as_lcd.h"
int as_lcd_printf(unsigned int x, unsigned int y, const char* format,...);
int as_lcd_time_update();

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

 #define  PDEBUG(fmt,args...)  printf(fmt,##args)

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
int as_error_msg(char *fmt, ...);

int as_dial_init(int length);
int as_dial_dtmf(int fd, char*bufTelNumber);
int as_write_serial_no (unsigned char *serNO);
int as_read_serial_no(unsigned char *read_serial_no);

void as_switch_low(void);
void as_switch_high(void);

void as_led1_enable();
void as_led2_enable();
void as_led3_enable();
void as_led4_enable();
void as_led5_enable();
void as_led6_enable();
void as_led7_enable();

int as_get_onoffhook_states(void);



#define as_flash_options_matter_sum 7
struct options_flash
{
	int options_num;
	int options_len[as_flash_options_matter_sum];
	char *options_value[as_flash_options_matter_sum];
};

int as_write_flash (struct options_flash *opflash);
int as_read_flash(struct options_flash *opflash);
void as_write_93lc66();
void as_read_93lc66();

#ifdef __cplusplus
};
#endif

#endif

