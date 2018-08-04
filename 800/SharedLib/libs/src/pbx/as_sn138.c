#include "as_misc_dev.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


static int __as_sn138_write_cmd(AS_138_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_138);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_138_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

static void as_sn138y0_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y0_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y1_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y1_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y2_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y2_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y3_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y3_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y4_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y4_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y5_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y5_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y6_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y6_ENABLE;
	__as_sn138_write_cmd(&cmd);
}

static void as_sn138y7_active()
{
	AS_138_COMMAND cmd;
	cmd.type = AS_138Y7_ENABLE;
	__as_sn138_write_cmd(&cmd);
}
/*******************************/
void as_led1_enable()
{
	as_sn138y1_active();
	as_sn138y0_active();
}

void as_led2_enable()
{
	as_sn138y2_active();
	as_sn138y0_active();
}

void as_led3_enable()
{
	as_sn138y3_active();
	as_sn138y0_active();
}

void as_led4_enable()
{
	as_sn138y4_active();
	as_sn138y0_active();
}

void as_led5_enable()
{
	as_sn138y5_active();
	as_sn138y0_active();
}

void as_led6_enable()
{
	as_sn138y6_active();
	as_sn138y0_active();
}

void as_led7_enable()
{
	as_sn138y7_active();
	as_sn138y0_active();
}

