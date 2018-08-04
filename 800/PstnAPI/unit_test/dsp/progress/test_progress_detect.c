/*
 * $Author: lizhijie $
 * $Log: test_progress_detect.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.5  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.4  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.3  2006/04/30 06:16:20  wangwei
 * no message
 *
 * Revision 1.2  2006/04/20 05:17:24  lizhijie
 * add Call Progress Tone Detect
 *
 * Revision 1.1  2006/04/20 05:14:50  lizhijie
 * Call Progress Tone Detect Test
 *
 * $Id: test_progress_detect.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
*  Detect BT(Busy Tone) from a normal disk file of BT
*  Display BT detect result and the time need to finish this test 
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "assist_lib.h"
#include "as_version.h"

int main(int argc, char *argv[])
{
	int fd;
	int res;
	int total = 0;
	char data[AS_DRIVER_BUFFER_LENGTH];
	char file[32];
	as_dsp_t *dsp;

	printf( AS_VERSION_INFO("DSP Software Call Progress Tone(DT,RBT,SDT, BT) Detect Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: progress <filename> \n\t---Detect Call Progress Tone from a normal file or device file(Default : dial.tone.u)\n\n\n");
	}

	if(argc < 2 )
		sprintf(file, "%s", "dial.tone.u" );
	else
		sprintf(file, "%s", argv[1] );

	fd = open( file, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		exit(1);
	}
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);
	assert(dsp);
	
	while(1)
	{
		res = read(fd, data, AS_DRIVER_BUFFER_LENGTH);
		total += res;
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("BT(Busy Tone) Detect fail : EOF or this file is not a device file for PSTN firmware, %d bytes have been read\n", total);
			break;
		}

		res = as_dsp_progress_detect(dsp, data, AS_DRIVER_BUFFER_LENGTH);
		if(res)
		{
			printf("Call Progress Tone Detected on %s after %d ms\n", file, total/8 );
			break;
		}
	};

	switch(res)
	{
		case AS_PROGRESS_DIALTONE:
		{
			printf("\t'DT(Dial Tone)' detected\n");
			break;
		}
		case AS_PROGRESS_RINGBACK:
		{
			printf("\t'RBT(RingBack Tone)' detected\n");
			break;
		}
		case AS_PROGRESS_2DIALTONE:
		{
			printf("\t'SDT(Second Dial Tone)' detected\n");
			break;
		}
		case AS_PROGRESS_BUSYTONE:
		{
			printf("\t'BT(Busy Tone)' detected\n");
			break;
		}
		case AS_PROGRESS_TALKING:
		{
			printf("\t'TALKING' detected\n");
			break;
		}
		default:
		{
			printf("\tUnknown detected\n");
			break;
		}
	}

 	printf("PROGRESS_DETECT:  Test ended successfully!\n");  
	as_dsp_destroy( dsp);
	close(fd);
	return 0;
}

