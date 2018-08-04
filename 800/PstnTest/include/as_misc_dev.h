/*
 * $Log: as_misc_dev.h,v $
 * Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.2  2006/04/27 05:42:06  wangwei
 * no message
 *
 * Revision 1.1  2006/03/30 07:26:02  wangwei
 * no message
 *
 * Revision 1.4  2006/03/13 10:08:37  wangwei
 * no message
 *
 * Revision 1.3  2006/02/24 02:47:24  wangwei
 * no message
 *
 * Revision 1.2  2006/01/17 12:10:35  lizhijie
 * dd
 *
 * Revision 1.1.1.1  2006/01/10 06:01:21  lizhijie
 * kernel and driver for DUX project
 *
 * Revision 1.4  2005/11/30 07:21:29  wangwei
 * 增加烧录93LC66部分程序
 *
 * Revision 1.3  2005/09/07 10:02:51  wangwei
 * no message
 *
 * Revision 1.2  2005/06/15 02:56:52  wangwei
 * 增加了蜂鸣器相关函数的申明
 *
 * Revision 1.1  2005/06/07 09:16:41  lizhijie
 * add into CVS
 *
 * $Id: as_misc_dev.h,v 1.1.1.1 2006/11/30 16:23:06 lizhijie Exp $
 * assist misc device defination is declared here
*/
#ifndef  __AS_MISC_DEV_H__
#define __AS_MISC_DEV_H__

#if  __KERNEL__

#include <linux/proc_fs.h>

#define AS_NAME_LENGTH			64

typedef struct
{
	char name[AS_NAME_LENGTH];		/* used as proc file name, such as watchdog */
	
	struct proc_dir_entry *as_proc_entry;

	int (*read_handler)(char *page, char **start, off_t off, int count, int *eof, void *data);
	int (*write_handler)(struct file *file, const char *buf, unsigned long count, void *data);
	
}as_misc_dev;
#endif

#define  AS_MISC_PROC_DIR_NAME	"as_misc"

/********WATCHDOG*********/

#define  AS_MISC_PROC_WATCHDOG		"watchdog"
typedef enum
{
	AS_WATCHDOG_ENABLE = 1,
	AS_WATCHDOG_UPDATE,
	AS_WATCHDOG_DISABLE
}AS_WATCHDOG_CMD_TYPE;
typedef struct 
{
	AS_WATCHDOG_CMD_TYPE type;
	unsigned int value;		
}AS_WATCHDOG_COMMAND;
#define AS_IXP_WATCHDOG_DEFAULT_COUNT 0XFFFFFFFE

/************BEEP***************/

#define  AS_MISC_PROC_BEEP				"beep"
typedef enum
{
	AS_BEEP_ENABLE = 1,
	AS_BEEP_DISABLE
}AS_BEEP_CMD_TYPE;
typedef struct 
{
	AS_BEEP_CMD_TYPE type;
	unsigned int value;		
}AS_BEEP_COMMAND;

/************SWITCH**************/

#define  AS_MISC_PROC_SWITCH			"switch"
typedef enum
{
	AS_SWITCH_ENABLE	= 1,
	AS_SWITCH_DISABLE
}AS_SWITCH_CMD_TYPE;
typedef struct 
{
	AS_SWITCH_CMD_TYPE type;
	unsigned int value;		
}AS_SWITCH_COMMAND;

/************BUTTON***************/

#define  AS_MISC_PROC_BUTTON			"button"
typedef enum
{
	AS_BUTTON_STATE = 1
}AS_BUTTON_CMD_TYPE;
typedef struct 
{
	AS_BUTTON_CMD_TYPE	 type;
	unsigned int value;		
}AS_BUTTON_COMMAND;

/************ONOFFHOOK**************/

#define AS_MISC_PROC_ONOFFHOOK		"onoffhook"
typedef enum
{
	AS_ONOFFHOOK_STATE = 1
}AS_ONOFFHOOK_CMD_TYPE;
typedef struct 
{
	AS_ONOFFHOOK_CMD_TYPE type;
	unsigned int value;		
}AS_ONOFFHOOK_COMMAND;

/**************EEPROM************/

#define AS_MISC_PROC_93LC66			"eeprom"
typedef enum
{
	AS_93LC66_WRITE = 1,
	AS_93LC66_READ
}AS_93LC66_CMD_TYPE;
typedef struct 
{
	AS_93LC66_CMD_TYPE type;
	unsigned int value;		
}AS_93LC66_COMMAND;

/**************LED************/

#define AS_MISC_PROC_138				"led"
typedef enum
{
	AS_138Y0_ENABLE = 1,
	AS_138Y1_ENABLE,
	AS_138Y2_ENABLE,
	AS_138Y3_ENABLE,
	AS_138Y4_ENABLE,
	AS_138Y5_ENABLE,
	AS_138Y6_ENABLE,
	AS_138Y7_ENABLE
}AS_138_CMD_TYPE;
typedef struct 
{
	AS_138_CMD_TYPE type;
	unsigned int value;		
}AS_138_COMMAND;
/**********GPIO CTL*********/
#define AS_MISC_PROC_GPIO_CTL			"gpioctl"
typedef enum
{
	AS_GPIO_CTL_LOW = 0,
	AS_GPIO_CTL_HIGH,
	AS_GPIO_CTL_IN,
	AS_GPIO_CTL_OUT
}AS_GPIO_CTL_CMD_TYPE;
typedef struct 
{
	AS_GPIO_CTL_CMD_TYPE type;
	int gpio_no;		
}AS_GPIO_CTL_COMMAND;

#endif

