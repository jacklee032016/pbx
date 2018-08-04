/*
* $Id: as_button.c,v 1.4 2007/03/16 20:39:41 lizhijie Exp $
*/

#include "as_misc_dev.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "aslib.h"

static int __as_button_write_cmd(AS_BEEP_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_BUTTON);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Unable to open %s: %s", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_BEEP_COMMAND) );
	if(res < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Write %s failed : %s", str, strerror(errno));
		close(fd);
		return -1;
	}
	else
	{
		close(fd);
		return res;
	}
	
}

int as_get_button_states(void)
{
	AS_BEEP_COMMAND cmd;
	cmd.type = AS_BUTTON_STATE;
	return __as_button_write_cmd(&cmd);
}

