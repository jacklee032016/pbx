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

static int __as_93lc66_write_cmd(AS_93LC66_COMMAND  *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_93LC66);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_93LC66_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
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
	return ;

}


void as_read_93lc66()
{
	AS_93LC66_COMMAND cmd;
	cmd.type = AS_93LC66_READ;
	__as_93lc66_write_cmd(&cmd);
	return ;
}
