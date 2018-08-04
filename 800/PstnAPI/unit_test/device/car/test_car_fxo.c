/*
 * $Author: lizhijie $
 * $Id: test_car_fxo.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
* test for CAR(Data Receiving terminal active signal) RX in FXO Device
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

#include "as_dev_ctl.h"

int main(int argc, char *argv[])
{
	int fd;
	int res;

	printf( AS_VERSION_INFO("FXO CAR Test"));
	
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: car_fxo <FXO device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: test FXO signal of CAR \n");

	printf("FXO CAR : Please Call to this FXO port with CallerID enabled...\n");
	
	do
	{
		res = as_lib_event_get( fd);

		if(res )
			as_error_msg( "Event '%d' received\r\n", res );
		if(res == AS_EVENT_CAR)
			as_error_msg( "Event 'CAR' received in FXO Port\r\n");
		else if(res==AS_EVENT_RINGEROFF)
		{
			as_error_msg( "ERROR : Caller ID is not enabled by the Switch you are using\r\n");
			break;
		}

	}while(1);

	printf("FXO CAR: test ended\n");
 	close(fd);
	return 0;
}

