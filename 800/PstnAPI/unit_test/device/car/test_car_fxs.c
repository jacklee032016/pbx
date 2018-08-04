/*
 * $Author: lizhijie $
 * $Id: test_car_fxs.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
* test for CAR(Data Receiving terminal active signal) TX in FXS Device
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "assist_lib.h"
#include "as_version.h"

int main(int argc, char *argv[])
{
	int fd;
	int res;

	printf( AS_VERSION_INFO("FXS CAR Test"));
	
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: car_fxs <FXS device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: test FXS signal of CAR \n");
	printf("FXS CAR : Please Call to this FXO port with CallerID enabled...\n");
	
	as_car_on_fxs(fd );
	do
	{
		res = as_lib_event_get( fd);
		if(res != 0)

		if(res == AS_EVENT_RINGOFFHOOK)
		{
			as_error_msg( "Phone feedback Primary Answer Signal(OFF-HOOK) event\r\nModem Signal can be send now\n");
			break;
		}	
	}while(1);

	printf("FXS CAR: test succeed!\n");
 	close(fd);
	return 0;
}

