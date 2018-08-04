/*
 * $Log: test_fxo_calling.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.4  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.3  2006/04/30 06:15:51  wangwei
 * no message
 *
 * Revision 1.2  2006/04/29 08:16:17  lizhijie
 * add
 *
 * Revision 1.1  2006/04/29 08:14:18  lizhijie
 * no message
 *
 * Revision 1.3  2006/04/19 12:28:39  lizhijie
 * add CAR support and others
 *
 * Revision 1.2  2006/04/19 12:27:49  lizhijie
 * no message
 *
 * Revision 1.1  2006/03/27 04:05:52  lizhijie
 * no message
 *
 * $Id: test_fxo_calling.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 * $Author: lizhijie $
*/

/*
* test calling process both in initial and end a call 
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "as_version.h"
#include "assist_lib.h"

#define 	FXO_CALL_DELAY		300	/* 3000*20 ms */

int main(int argc, char *argv[])
{
	int fd;
	int res;
	char dialingStr[32];
	int count = 0;

	printf( AS_VERSION_INFO("Test FXO send DTMF to Calling"));

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_call <FXO device> [calleeId]\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	if(argc<3)
		sprintf(dialingStr, "812");
	else
		sprintf(dialingStr, "%s", argv[2] );
	
	as_dsp_t *dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);
	printf("\t\tFXO-CALLER : Begin Calling FXO with '%s'....\r\n\r\n", dialingStr );

	res = as_lib_onhook(fd);

	printf("\n\t\tFXO-CALLER : Begin to off-hook operation...\n");
	res = as_lib_offhook(fd);

	printf("\n\t\tFXO-CALLER : Begin to dial Calling Number...\n");

	usleep(400*1000);	/* 400 ms */
	printf("\n\t\tFXO-CALLER : Begin to dial Calling Number...\n");
	as_dsp_play_dtmf( dsp,  fd, dialingStr);

	printf("\n\t\tFXO-CALLER : delay and play DT about %d seconds...\n", FXO_CALL_DELAY*20/1000 );
	while(1)
	{
		char buf[160];
#if 0
		res = as_lib_event_get( fd);
		if(res > 1)
		{
			printf("event %d RX\n", res);
			break;
		}
#endif		
		as_dsp_readDialTone( dsp,  buf);
		write(fd, buf, 160);
		count++;
		if(count*20%1000 == 0)
		{
			printf("\r\t\t%d second....", count*20/1000 );
		}
		if(count >= FXO_CALL_DELAY)
			break;
	};
	
	as_dsp_destroy(dsp);
	
	printf("\n\t\tFXO-CALLER : After %d seconds delay, then hanging up automatically...\n", FXO_CALL_DELAY*20/1000 );
	res = as_lib_onhook(fd);
	close(fd);
	
	printf("\n\t\tFXO-CALLER : End of FXO Calling.\n");
	
	return 0;
}

