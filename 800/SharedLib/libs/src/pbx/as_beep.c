#include "as_misc_dev.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int __as_beep_write_cmd(AS_BEEP_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_BEEP);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_BEEP_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

void as_beep_long(int times)
{
	int temp;
	AS_BEEP_COMMAND cmd;
	for(temp=0;temp<times;temp++)
	{
		cmd.type = AS_BEEP_ENABLE;
		__as_beep_write_cmd(&cmd);
		usleep(600000);
		cmd.type = AS_BEEP_DISABLE;
		__as_beep_write_cmd(&cmd);
		usleep(50000);
	}
}

void as_beep_short(int times)
{
	int temp;
	AS_BEEP_COMMAND cmd;
	for(temp=0;temp<times;temp++)
	{
		cmd.type = AS_BEEP_ENABLE;
		__as_beep_write_cmd(&cmd);
		usleep(40000);
		cmd.type = AS_BEEP_DISABLE;
		__as_beep_write_cmd(&cmd);
		usleep(50000);
	}
}
