
/*
 * $Log: as_misc_switch.c,v $
 * Revision 1.1.1.1  2006/11/30 16:27:59  lizhijie
 * AS800 Kernel
 *
 * Revision 1.2  2006/03/30 07:41:15  wangwei
 * no message
 *
 * Revision 1.1  2006/02/24 02:39:05  wangwei
 * no message
 *
 * Revision 1.2  2005/07/01 08:02:08  wangwei
 * 更改蜂鸣器的GPIO引脚映射为0
 *
 * Revision 1.1  2005/06/15 05:49:31  wangwei
 * 增加蜂鸣器用户程序接口
 *
 * Revision 1.1  2005/06/07 09:16:21  lizhijie
 * add into CVS
 *
 * $Id: as_misc_switch.c,v 1.1.1.1 2006/11/30 16:27:59 lizhijie Exp $
 * Version info for Misc Drivers, Li Zhijie, 2005.06.07 
*/


#include <linux/config.h>
#include <linux/version.h>
#include <linux/kmod.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>

#include <linux/interrupt.h>
#include <linux/time.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <linux/timex.h>
#include <asm/hardware.h>
#include "as_misc_dev.h"

//#define SWITCH                           IXP425_GPIO_PIN_4
/*ADD for AS800-SD , 2006.03.23*/
#define SWITCH                           IXP425_GPIO_PIN_12

static void  as_ixp_switch_enable(void )
{
	gpio_line_config(SWITCH,IXP425_GPIO_OUT);	
	gpio_line_set(SWITCH,IXP425_GPIO_LOW);	
}

static void as_ixp_switch_disable(void )
{
	gpio_line_config(SWITCH,IXP425_GPIO_OUT);	
	gpio_line_set(SWITCH,IXP425_GPIO_HIGH);
}

int as_switch_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	as_misc_dev	*dev;

	if (off > 0)
		return 0;
		
	dev = (as_misc_dev *)data;
	if (!dev)
		return 0;

	MOD_INC_USE_COUNT;

	len += sprintf(page + len,"Assist buzzer\n");

	MOD_DEC_USE_COUNT;
	
	return len;
}

int as_switch_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	AS_SWITCH_COMMAND cmd;
	int len;

	MOD_INC_USE_COUNT;

	len = copy_from_user(&cmd, buf, sizeof(AS_SWITCH_COMMAND ));
	if( len )
	{
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}

	switch(cmd.type)
	{
		case AS_SWITCH_ENABLE:
			as_ixp_switch_enable();
			break;
			
		case AS_SWITCH_DISABLE:
			as_ixp_switch_disable();
			break;
				
		default:
			printk(KERN_INFO "Not validate command for Assist BEEP Driver\r\n");
			len = 0;
			break;
	}
	MOD_DEC_USE_COUNT;

	return len;
}
