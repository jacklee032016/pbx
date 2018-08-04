/*
 * $Log: test_dtmf_callerId.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.3  2006/04/30 06:15:39  wangwei
 * no message
 *
 * Revision 1.2  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: test_dtmf_callerId.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "as_dev_ctl.h"
#include "as_dsp.h"
#include "as_version.h"

AS_DIAL_OPERATION dtmf_dial_str ;

char str[9]="5432198";

int __event_get(int fd)
{
	/* Avoid the silly zt_getevent which ignores a bunch of events */
	int j =-1;
	
	if (ioctl(fd, AS_CTL_GETEVENT, &j) == -1) 
		return -1;
	return j;
}

void wait_dial_complete(int fd)
{
	int res;
	
	do
	{
		res = __event_get( fd);
		if(res)
			printf("event=%d\r\n", res );
	}while(res != AS_EVENT_DIALCOMPLETE );
	printf("Dial complete : %d\r\n" , res);
}


int main(int argc, char *argv[])
{
	int fd;
	int res;
	int hook;

	printf( AS_VERSION_INFO("Test DSP software DTMF ID to send PHONE"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: test_dtmf <astel device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	hook = AS_START;
	res = ioctl(fd, AS_CTL_HOOK, &hook);

	dtmf_dial_str.op = AS_DIAL_OP_REPLACE;

	//sleep(3);
	sprintf(dtmf_dial_str.dialstr, "%s" , "818" );
	res = ioctl(fd,  AS_CTL_SET_DTMF_STR, &dtmf_dial_str);

	sleep(5);
	wait_dial_complete( fd);

	printf("DTMF_CALLERID:  Test ended successfully!\n");
	
	close(fd);
	return 0;
}
