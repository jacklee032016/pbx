/*
 * $Log: test_ring.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.2  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: test_ring.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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
	printf( AS_VERSION_INFO(" Ring the phone with DTMF Caller ID Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: ring <FXS device> [CallerId]\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: ring the phone with DTMF Caller ID \n");

	if(argc==3)
	{
		printf("RING: phone NO is %s\n",argv[2]);
		as_ring_with_dtmf_caller_id( fd, argv[2]);
	}
	else
	{
		printf("RING: phone NO is %s\n","8765432190");
		as_ring_with_dtmf_caller_id( fd, "8765432190");
	}
	
	sleep(10);
	as_stop_ring( fd);
	
	//printf("Now ring the phone\n");
	//as_ring_on_hook(fd );
	//sleep(10);
	//as_stop_ring( fd);

	printf("RING: test succeed\n");

	close(fd);
	return 0;
}

