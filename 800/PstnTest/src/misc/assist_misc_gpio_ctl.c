#include "as_misc_dev.h"
#include "assist_lib.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define GPIO_PIN_BEEP 				0 
#define GPIO_PIN_LED_STATUS 		1
#define GPIO_PIN_LED_LINE   			2
#define GPIO_PIN_LED_VOIP			3
#define GPIO_PIN_LED_PPPOE1		4
#define GPIO_PIN_LED_PPPOE2		5
#define GPIO_PIN_FACTORY_BUTTON 	7
#define GPIO_PIN_CHAN_CTL 			12
#define GPIO_PIN_HOOK_CHECK 		14

static int as_gpio_write_cmd(AS_GPIO_CTL_COMMAND  *cmd,char *proc_name )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , proc_name);

	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_GPIO_CTL_COMMAND));
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return res;
}



int as_beep(int times,beep_type type)
{
	int temp,beep_time;
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;
	cmd.gpio_no = GPIO_PIN_BEEP;

	if((type != BEEP_LONG)&&(type != BEEP_SHORT))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	if(type == BEEP_LONG)
		beep_time = 600000;
	else if(type == BEEP_SHORT)
		beep_time = 40000;
	else
		return -1;

	for(temp=0;temp<times;temp++)
	{
		cmd.type = AS_GPIO_CTL_HIGH;
		as_gpio_write_cmd(&cmd,proc_name);
		usleep(beep_time);
		cmd.type = AS_GPIO_CTL_LOW;
		as_gpio_write_cmd(&cmd,proc_name);
		usleep(50000);
	}

	return 0;
}


int as_led_status(led_status_type type)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((type != LED_STATUS_ON)&&(type != LED_STATUS_OFF))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_LED_STATUS;
	cmd.type = type;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

int as_led_line(led_LINE_type status)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((status != LED_LINE_ON)&&(status != LED_LINE_OFF))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_LED_LINE;
	cmd.type = status;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

int as_led_voip(led_LINE_type status)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((status != LED_LINE_ON)&&(status != LED_LINE_OFF))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_LED_VOIP;
	cmd.type = status;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

int as_led_pppoe1(led_LINE_type status)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((status != LED_LINE_ON)&&(status != LED_LINE_OFF))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_LED_PPPOE1;
	cmd.type = status;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

int as_led_pppoe2(led_LINE_type status)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((status != LED_LINE_ON)&&(status != LED_LINE_OFF))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_LED_PPPOE2;
	cmd.type = status;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

factory_button_status as_factory_button_states(void)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;
	
	cmd.gpio_no = GPIO_PIN_FACTORY_BUTTON;
	cmd.type = AS_GPIO_CTL_IN;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}


int as_delay_chan_ctl(delay_switch_chan_tpye status)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;

	if((status != DELAY_SWITCH_2_FXS)&&(status != DELAY_SWITCH_2_LINE))
	{
		printf("GPIO CTL ERROR: para error\n");
		return -1;
	}
	
	cmd.gpio_no = GPIO_PIN_CHAN_CTL;
	cmd.type = status;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}

phone_hook_status as_hook_check(void)
{
	AS_GPIO_CTL_COMMAND cmd;
	unsigned char *proc_name = AS_MISC_PROC_GPIO_CTL;
	
	cmd.gpio_no = GPIO_PIN_HOOK_CHECK;
	cmd.type = AS_GPIO_CTL_IN;
	
	return as_gpio_write_cmd(&cmd,proc_name);
}



