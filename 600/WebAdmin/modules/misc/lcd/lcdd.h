/*
* $Id: lcdd.h,v 1.10 2007/09/27 17:57:03 lizhijie Exp $
*/

#ifndef	__AS_LCDD_H__
#define	__AS_LCDD_H__

#include <sys/types.h>
#include <termios.h>

#include "aslib.h"
#include "lcdUartCmd.h"

#define	LCDD_DEBUG					0

#define	USB_SERIAL_DEVICE				"/dev/ttyUSB0"

#define 	LCD_CMD_NAME_WWWSERVER		"www"
#define 	LCD_CMD_NAME_FILESERVER		"smbd"
#define 	LCD_CMD_NAME_MAILSERVER		"master"
#define 	LCD_CMD_NAME_PRINTSERVER	"cupsd"
#define 	LCD_CMD_NAME_DHCPSERVER		"udhcpd"
#define 	LCD_CMD_NAME_CAMERA			"camserv"

#define USBCONVERT_REV_BUFF_MAX		1024


enum
{
	LCD_CMD_WRAP = 0,
	LCD_CMD_NO_WRAP = 1,
	LCD_CMD_CLEAR = 2
};

typedef	struct
{
	unsigned short	header;
	unsigned char		length;
	
	unsigned char		service;		/**/
	unsigned char		status;
	unsigned	char		x;			/* 1~9 */
	unsigned	char		y;			/* 1~3 */

	unsigned char		lcdCmd;		/**/
	
}frame_to_lcd;


#define	LCD_NAME_LENGTH				18*10
typedef		struct
{
	LCD_SERVICE_TYPE_T		type;
	int						state;
	
	char						*command;		/* used when find pid in /proc file system */
	char						*startScript;
	char						name[LCD_NAME_LENGTH];			/* display on LCD screen */
}lcd_item_t;


typedef	struct	
{
	int					currentService;

	LCD_UART_REQUEST	request;

	int					inIndex;
	int					outIndex;
	unsigned	char			buf[USBCONVERT_REV_BUFF_MAX];
	
	uart_t				*uart;
}lcdd_t;

LCD_UART_REQUEST *lcdd_read_command(lcdd_t *lcdd);
int lcdd_write_msg(lcdd_t *lcdd, lcd_item_t *item );
int lcdd_write_reboot_cmd(lcdd_t *lcdd, lcd_item_t *item );


void initPrintLcd(lcdd_t *lcdd);

int lcd_threads_init();
void lcd_threads_destory(lcdd_t *lcdd);
void *lcdd_read_thread(void *data);
void *lcdd_monitor_thread(void *data);

int lcdd_get_ver_info(lcdd_t *lcdd );
void lcdd_write_ver_info(lcdd_t *lcdd);


#endif

