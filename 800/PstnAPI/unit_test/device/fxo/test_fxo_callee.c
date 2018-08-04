/*
 * $Id: test_fxo_callee.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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
	int fd;
	int res;
	char buf[160];
	int i = 0;
	int length;

	printf( AS_VERSION_INFO("Test Busy Tones"));

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_callee <FXO device>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	as_dsp_t *dsp = as_dsp_init_no_cfg(U_LAW_CODE,PSTN_STANDARD_JAPAN, 20);

	res = as_lib_onhook(fd);
	printf("\n\t\tFXO-Callee : Waiting Ringing from outside...\n");

	while(1)
	{
		res = as_lib_event_get( fd);
		if(res== AS_EVENT_RINGERON)
		{
//			printf("\tFXO-Callee : RX RingerON Event\n");
		}
		else if(res== AS_EVENT_RINGEROFF)
		{
			i++;
			printf("\tFXO-Callee : RX %d RingerOFF Event\n" , i);
			if(i==2)
				break;
		}
		else if(res== AS_EVENT_ONHOOK)
		{
//			printf("\tCallee : RX ON-HOOK Event\n");
		}
		else if(res== AS_EVENT_RINGOFFHOOK)
		{
//			printf("\tCallee : RX OFF-HOOK Event\n");
		}
		else if(res != AS_EVENT_NONE )
		{
//			printf("\tCallee : event %d RX\n", res);
		}	
	};

	printf("\tFXO-Callee : After the second ringing, then hang off......\n" );
	res = as_lib_offhook(fd);
	printf("\tFXO-Callee : After hang off, waiting remote hangup......\n" );
	while(1)
	{
		res = as_lib_event_get( fd);
		if(res== AS_EVENT_RINGERON)
		{
//			printf("\tCallee : RX RingerON Event\n");
		}
		else if(res== AS_EVENT_RINGEROFF)
		{
//			printf("\tCallee : RX RingerOFF Event\n");
		}
		else if(res== AS_EVENT_ONHOOK)
		{
			printf("\tFXO-Callee : RX ONHOOK Event\n");
			break;
		}
		else if(res== AS_EVENT_RINGOFFHOOK)
		{
//			printf("\tCallee : RX OFFHOOK Event\n");
		}
		else if(res !=AS_EVENT_NONE )
		{
//			printf("\tFXO-Callee : event %d RX\n", res);
		}
		length = read(fd, buf, 160);
		if(length != 160)
		{
			printf("\tFXO-Callee : Data read error : %d\n", length);
			break;
		}
		else if( as_dsp_busydetect( dsp, buf, length) )
		{
			printf("\tFXO-Callee : Busy detect, remote on-hook\n");
			break;
		}

	};

	printf("\tFXO-Callee : Remote hangup, quit normally\n");
	as_dsp_destroy(dsp);
	
	res = as_lib_onhook(fd);
	close(fd);
	
	printf("\nEnd of FXO Callee.\n");
	
	return 0;
}

