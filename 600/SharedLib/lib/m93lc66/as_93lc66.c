/*
* $Id: as_93lc66.c,v 1.3 2007/03/16 20:39:41 lizhijie Exp $
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

static int __as_93lc66_write_cmd(AS_93LC66_COMMAND  *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_93LC66);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Unable to open %s: %s", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_93LC66_COMMAND) );
	if(res < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Write %s failed : %s", str, strerror(errno));
		close(fd);
		return -1;
	}
	
	close(fd);
	return 0;
}

void as_write_93lc66(void)
{
	AS_93LC66_COMMAND cmd;
	cmd.type = AS_93LC66_WRITE;
	__as_93lc66_write_cmd(&cmd);
}

void as_read_93lc66()
{
	AS_93LC66_COMMAND cmd;
	cmd.type = AS_93LC66_READ;
	__as_93lc66_write_cmd(&cmd);
}

