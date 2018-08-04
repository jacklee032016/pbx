#include "as_misc_dev.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static int __as_onoffhook_write_cmd(AS_ONOFFHOOK_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_ONOFFHOOK);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_ONOFFHOOK_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	else
	{
		close(fd);
		return res;
	}
	
}

int as_get_onoffhook_states(void)
{
	AS_ONOFFHOOK_COMMAND cmd;
	cmd.type = AS_ONOFFHOOK_STATE;
	return __as_onoffhook_write_cmd(&cmd);
}




