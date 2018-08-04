/*
 * $Id: test_catch_modem_data.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 * $Author: lizhijie $
*/

/*
* test callee process both in initial and end a call 
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "as_version.h"
#include "as_dev_ctl.h"
#include "assist_lib.h"

#define 	FXO_CALL_DELAY		3000	/* ms */

int main(int argc, char *argv[])
{
	int fd, fd2;
	int res;
	char buf[160];
	int i = 0;
	int length;

	printf( AS_VERSION_INFO("RX Caller ID with NTT specs"));

	if (argc < 3) 
	{
		fprintf(stderr, "Usage: fxo_cid <FXO device> <record file>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	fd2 = open(argv[2], O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[2], strerror(errno));
		exit(1);
	}

	printf("INTENT: RX Caller Id from Device compatible with NTT specs\n");
	printf("NTT CID : Please Call to this FXO port with CallerID enabled...\n");

	as_dsp_t *dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);

	res = as_lib_onhook(fd);
	as_error_msg("\n\t\tNTT-CID : \n\tSTEP 1 : Waiting CAR signal from outside...\n");
	
	while(1)
	{
		res = as_lib_event_get( fd);
		if(res== AS_EVENT_CAR)
		{
			i++;
			printf("\tFXO CID : RX %d CAR Event\n", i);
			if(i==1)
				break;
		}
		else if(res== AS_EVENT_RINGEROFF)
		{
			as_error_msg( "ERROR : Caller ID is not enabled by the Switch you are using\r\n");
			exit(1);
		}
	};
	
	as_error_msg("\n\tSTEP 2 : Off-Hook this FXO device...\n");
	as_lib_offhook(fd);

	i = 0;
	as_error_msg("\n\tSTEP 3 : Read and Write/decode Modem Signal...\n");
	while(1)
	{
		length = read(fd, buf, 160);
		length = write(fd2, buf, 160);
		i++;
			
		if(i==250)	/* about n*20 ms */
			break;
	};

	as_error_msg("\n\tSTEP 4 : ON-Hook this FXO device and wait RING signal...\n");
	as_lib_onhook(fd);

	while(1)
	{
		res = as_lib_event_get( fd);
		if(res== AS_EVENT_RINGEROFF)
		{
			as_error_msg( "FXO CID : rx RING Signal\r\n");
			break;
		}
	};

	printf("\tFXO CID : quit normally\n");
	as_dsp_destroy(dsp);
	
	close(fd);
	close(fd2);
	
	printf("\nEnd of NTT rx.\n");
	
	return 0;
}

