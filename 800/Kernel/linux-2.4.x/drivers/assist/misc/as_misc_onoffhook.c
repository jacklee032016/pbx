
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


//#define ONOFFHOOK                           IXP425_GPIO_PIN_7
/*add for AS800-SD , 2006.03.23*/
#define ONOFFHOOK                           IXP425_GPIO_PIN_14

static int as_get_gpio_value(void )
{
	int tmp_v;
	int *tmp_value;
	tmp_value=&tmp_v;
	gpio_line_config(ONOFFHOOK,IXP425_GPIO_IN);
	gpio_line_get(ONOFFHOOK, tmp_value);
	return tmp_v;
}


int as_onoffhook_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	as_misc_dev	*dev;

	if (off > 0)
		return 0;
		
	dev = (as_misc_dev *)data;
	if (!dev)
		return 0;

	MOD_INC_USE_COUNT;

	len += sprintf(page + len,"Assist reposition botton\n");

	MOD_DEC_USE_COUNT;
	
	return len;
}





int as_onoffhook_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	AS_ONOFFHOOK_COMMAND cmd;
	int len;

	MOD_INC_USE_COUNT;

	len = copy_from_user(&cmd, buf, sizeof(AS_ONOFFHOOK_COMMAND ));
	if( len )
	{
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}

	switch(cmd.type)
	{
		case AS_ONOFFHOOK_STATE:
			len=as_get_gpio_value();
			break;
			
		default:
			printk(KERN_INFO "Not validate command for Assist button Driver\r\n");
			len = 0;
			break;
	}
	MOD_DEC_USE_COUNT;

	return len;
}

