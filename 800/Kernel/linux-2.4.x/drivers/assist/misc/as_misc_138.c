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


#define SN138_A             		IXP425_GPIO_PIN_1
#define SN138_B				 IXP425_GPIO_PIN_2
#define SN138_C				 IXP425_GPIO_PIN_3
#define SN138_CS			IXP425_GPIO_PIN_0


static void init_138_abc(void)
{
	gpio_line_config(SN138_CS,IXP425_GPIO_OUT);
	gpio_line_set(SN138_CS,IXP425_GPIO_LOW);
	gpio_line_config(SN138_A,IXP425_GPIO_OUT);
	gpio_line_config(SN138_B,IXP425_GPIO_OUT);
	gpio_line_config(SN138_C,IXP425_GPIO_OUT);
	return;
}

static void disable_138(void)
{
	gpio_line_set(SN138_CS,IXP425_GPIO_HIGH);
	return;
}


static void  as_138y0_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_LOW);
	gpio_line_set(SN138_B,IXP425_GPIO_LOW);
	gpio_line_set(SN138_C,IXP425_GPIO_LOW);
	disable_138();
	return;
}

static void  as_138y1_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_B,IXP425_GPIO_LOW);
	gpio_line_set(SN138_C,IXP425_GPIO_LOW);	
	disable_138();
	return;
}

static void  as_138y2_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_LOW);
	gpio_line_set(SN138_B,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_C,IXP425_GPIO_LOW);		
	disable_138();
	return;
}

static void  as_138y3_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_B,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_C,IXP425_GPIO_LOW);		
	disable_138();
	return;
}

static void  as_138y4_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_LOW);
	gpio_line_set(SN138_B,IXP425_GPIO_LOW);
	gpio_line_set(SN138_C,IXP425_GPIO_HIGH);		
	disable_138();
	return;
}

static void  as_138y5_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_B,IXP425_GPIO_LOW);
	gpio_line_set(SN138_C,IXP425_GPIO_HIGH);		
	disable_138();
	return;
}

static void  as_138y6_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_LOW);
	gpio_line_set(SN138_B,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_C,IXP425_GPIO_HIGH);	
	disable_138();
	return;
}

static void  as_138y7_enable(void )
{
	init_138_abc();
	gpio_line_set(SN138_A,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_B,IXP425_GPIO_HIGH);
	gpio_line_set(SN138_C,IXP425_GPIO_HIGH);		
	disable_138();
	return;
}


int as_138_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	as_misc_dev	*dev;

	if (off > 0)
		return 0;
		
	dev = (as_misc_dev *)data;
	if (!dev)
		return 0;

	MOD_INC_USE_COUNT;

	len += sprintf(page + len,"Assist 138\n");

	MOD_DEC_USE_COUNT;
	
	return len;
}

int as_138_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	AS_138_COMMAND cmd;
	int len;

	MOD_INC_USE_COUNT;

	len = copy_from_user(&cmd, buf, sizeof(AS_138_COMMAND ));
	if( len )
	{
		MOD_DEC_USE_COUNT;
		return -EFAULT;
	}

	switch(cmd.type)
	{
		case AS_138Y0_ENABLE:
			as_138y0_enable();
			break;
			
		case AS_138Y1_ENABLE:
			as_138y1_enable();
			break;

		case AS_138Y2_ENABLE:
			as_138y2_enable();
			break;

		case AS_138Y3_ENABLE:
			as_138y3_enable();
			break;

		case AS_138Y4_ENABLE:
			as_138y4_enable();
			break;

		case AS_138Y5_ENABLE:
			as_138y5_enable();
			break;

		case AS_138Y6_ENABLE:
			as_138y6_enable();
			break;

		case AS_138Y7_ENABLE:
			as_138y7_enable();
			break;
		
		default:
			printk(KERN_INFO "Not validate command for Assist 138 Driver\r\n");
			len = 0;
			break;
	}
	MOD_DEC_USE_COUNT;

	return len;
}

