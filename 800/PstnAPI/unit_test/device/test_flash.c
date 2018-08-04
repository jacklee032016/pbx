/*
 * $Log: test_flash.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.3  2006/03/11 10:18:34  lizhijie
 * dd
 *
 * Revision 1.2  2006/03/11 10:18:15  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: test_flash.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
* test for ON-HOOK signal which duration below 700 ms
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

	printf( AS_VERSION_INFO("Flash Detect Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: flash <FXS device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Usage: Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: difference of flash both OnHook\n");

	printf("FLASH: Please off hook the phone connect to this FXS port firstly\n");
	as_lib_wait_offhook(fd);
	printf("FLASH: Phone is off-hook\nflash :Please on-hook it as quick as possible. If a real ON-HOOK signal detected, program quit\n");

	while(1)
	{
		res = as_lib_event_get( fd);
		if( res ==AS_EVENT_WINKFLASH)
		{
			printf("\r\n************WINFLASH event checked\n" );
		}
		else if( res == AS_EVENT_ONHOOK) 
		{
			printf("\r\n************ONHOOK event checked\nProgram quit\n" );
			break;
		}	
	}

	printf("FLASH: test succeed\n");
	
 	close(fd);
	return 0;
}

