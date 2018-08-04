/*
 * $Log: test_event.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/04/30 06:15:39  wangwei
 * no message
 *
 * Revision 1.4  2006/03/27 04:07:38  lizhijie
 * add FXO test programs
 *
 * Revision 1.3  2006/03/27 04:06:05  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/25 11:27:28  lizhijie
 * add FXO line status detect
 *
 * Revision 1.1  2006/03/25 11:26:47  lizhijie
 * no message
 *
 * $Id: test_event.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
* test for Event send by PSTN firmware
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
	int event;

	printf( AS_VERSION_INFO("SLIC/DAA Device Event Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: event <FXS/FXO device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Usage: Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: Display Event send by firmware\n");


	while(1)
	{
		event = as_lib_event_get( fd);
		switch( event)
		{
			case AS_EVENT_NONE:
			{/* no event happened in firmware, then this event is emitted*/
//				printf("\r\n************AS_EVENT_NONE event checked\n" );
				continue;
			}
			case AS_EVENT_WINKFLASH:
			{
				printf("\r\n************WINFLASH event checked\n" );
				continue;
			}
			case AS_EVENT_ONHOOK:
			{
				printf("\r\n************ONHOOK event checked\n" );
				continue;
			}
			case AS_EVENT_RINGOFFHOOK:
			{
				printf("\r\n************AS_EVENT_RINGOFFHOOK event checked\n" );
				continue;
			}
			case AS_EVENT_ALARM:
			{
				printf("\r\n************AS_EVENT_ALARM event checked\n" );
				continue;
			}
			case AS_EVENT_NOALARM:
			{
				printf("\r\n************AS_EVENT_NOALARM event checked\n" );
				continue;
			}
			case AS_EVENT_DIALCOMPLETE:
			{
				printf("\r\n************AS_EVENT_DIALCOMPLETE event checked\n" );
				continue;
			}
			case AS_EVENT_RINGERON:
			{
				printf("\r\n************AS_EVENT_RINGERON event checked\n" );
				continue;
			}
			case AS_EVENT_RINGEROFF:
			{
				printf("\r\n************AS_EVENT_RINGEROFF event checked\n" );
				continue;
			}
			case AS_EVENT_HOOKCOMPLETE:
			{
				printf("\r\n************AS_EVENT_HOOKCOMPLETE event checked\n" );
				continue;
			}
			case AS_EVENT_BITSCHANGED:
			{
				printf("\r\n************AS_EVENT_BITSCHANGED event checked\n" );
				continue;
			}
			case AS_EVENT_PULSE_START:
			{
				printf("\r\n************AS_EVENT_PULSE_START event checked\n" );
				continue;
			}
			case AS_EVENT_TIMER_EXPIRED:
			{
				printf("\r\n************AS_EVENT_TIMER_EXPIRED event checked\n" );
				continue;
			}
			case 17://AS_EVENT_POLARITY:
			{
				printf("\r\n************AS_EVENT_POLARITY event checked\n" );
				continue;
			}
			default:
			{
				printf("\r\n************unknown event checked %d\n" ,event );
				continue;
			}
				
		}
	}

	printf("EVENT: test succeed\n");
	
 	close(fd);
	return 0;
}

