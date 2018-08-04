#include "as_misc_dev.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int __as_switch_write_cmd(AS_SWITCH_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_SWITCH);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_SWITCH_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}

void as_switch_low(void)
{

	AS_SWITCH_COMMAND cmd;
	cmd.type = AS_SWITCH_ENABLE;/*low*/
	__as_switch_write_cmd(&cmd);

}

void as_switch_high(void)
{
	AS_SWITCH_COMMAND cmd;
	cmd.type = AS_SWITCH_DISABLE;
	__as_switch_write_cmd(&cmd);

}

