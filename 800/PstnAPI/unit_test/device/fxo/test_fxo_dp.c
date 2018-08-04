/*
* $Id: test_fxo_dp.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_version.h"

int main(int argc, char *argv[])
{
	int fd;
	int res;
	int type;

	printf( AS_VERSION_INFO("Test FXO DP Dialing(10/20 PPS)"));

	if (argc < 4) 
	{
		fprintf(stderr, "Usage: fxo_dp <FXO device> <Call NO> [1|2]\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	type = atoi(argv[3] );
	if(type == 1)
		type = AS_DP_10PPS;
	else
		type = AS_DP_20PPS;
	
	res = as_lib_onhook(fd);
	sleep(3);

	if(assist_dsp_fxo_send_pps(fd, argv[2], type) != 0)
		return -1;
	
	printf("Please check your switch for the number %s\n", argv[2]);

	printf("FXO_DP:  Test ended successfully!\n");
	
	return 0;
	
}

