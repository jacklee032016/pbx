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

static int gpio_defind[]=
{
	IXP425_GPIO_PIN_0,
	IXP425_GPIO_PIN_1,
	IXP425_GPIO_PIN_2,
	IXP425_GPIO_PIN_3,
	IXP425_GPIO_PIN_4,
	IXP425_GPIO_PIN_5,
	IXP425_GPIO_PIN_6,
	IXP425_GPIO_PIN_7,
	IXP425_GPIO_PIN_8,
	IXP425_GPIO_PIN_9,
	IXP425_GPIO_PIN_10,
	IXP425_GPIO_PIN_11,
	IXP425_GPIO_PIN_12,
	IXP425_GPIO_PIN_13,
	IXP425_GPIO_PIN_14,
	IXP425_GPIO_PIN_15
};

static int as_gpio_check(AS_GPIO_CTL_CMD_TYPE type,int no)
{
	switch(no)
	{
		case IXP425_GPIO_PIN_0:
		case IXP425_GPIO_PIN_1:
		case IXP425_GPIO_PIN_2:
		case IXP425_GPIO_PIN_7:
		case IXP425_GPIO_PIN_12:
		case IXP425_GPIO_PIN_14:
		case IXP425_GPIO_PIN_3:
		case IXP425_GPIO_PIN_4:
		case IXP425_GPIO_PIN_5:
			return 0;
			
		case IXP425_GPIO_PIN_6:
		case IXP425_GPIO_PIN_8:
		case IXP425_GPIO_PIN_9:
		case IXP425_GPIO_PIN_10:
		case IXP425_GPIO_PIN_11:
		case IXP425_GPIO_PIN_13:
		case IXP425_GPIO_PIN_15:
			printk("GPIO ERROR: lawless GPIO Option\n");
			return -1;
			
		default:
			printk("GPIO ERROR: Error GPIO Numer\n");
			return -1;
	}
	return 0;
}

static int as_gpio_ctl(AS_GPIO_CTL_CMD_TYPE type,int no)
{
	int res;
	
	res = as_gpio_check(type,no);
	if(res < 0)
		return res;

	switch(type)
	{
		case AS_GPIO_CTL_LOW:
		case AS_GPIO_CTL_HIGH:
			gpio_line_config(no,IXP425_GPIO_OUT);
			gpio_line_set(no,type);
			return 0;
			
		case AS_GPIO_CTL_IN:
			gpio_line_config(no,IXP425_GPIO_IN);
			gpio_line_get(no, &res);
			return res;

		case AS_GPIO_CTL_OUT:
			gpio_line_config(no,IXP425_GPIO_OUT);
			break;
			
		default:
			printk("GPIO ERROR: Error GPIO Option Type\n");
			return -1;		
	}

	return 0;
}

int as_gpio_ctl_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	as_misc_dev	*dev;

	if (off > 0)
		return 0;
		
	dev = (as_misc_dev *)data;
	if (!dev)
		return 0;

	MOD_INC_USE_COUNT;

	len += sprintf(page + len,"Assist GPIO Option\n");

	MOD_DEC_USE_COUNT;
	
	return len;
}

int as_gpio_ctl_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	AS_GPIO_CTL_COMMAND cmd;
	int len =0;

	MOD_INC_USE_COUNT;

	len = copy_from_user(&cmd, buf, sizeof(AS_GPIO_CTL_COMMAND ));
	if( len )
	{
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}

	len = as_gpio_ctl(cmd.type,gpio_defind[cmd.gpio_no]);
	
	if(len < 0)
	printk(KERN_INFO "Not validate command for Assist GPIO Option\r\n");
	
	MOD_DEC_USE_COUNT;

	return len;
}
