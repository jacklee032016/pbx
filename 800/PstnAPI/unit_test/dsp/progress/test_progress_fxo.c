/*
* $Id: test_progress_fxo.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
* $Author: lizhijie $
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "as_thread.h"
#include "assist_lib.h"
#include "as_version.h"

/*  
 * Call Progress Detect test on FXO Port(FXO play as caller) 
*/

int main(int argc, char *argv[])
{
	int res;
	int fdo;
	int total = 0;
	char data[AS_DRIVER_BUFFER_LENGTH];
	as_dsp_t *dsp;
	char dialingStr[32];
	pstn_standard_t standard;

	printf( AS_VERSION_INFO("DSP Software Call Progress Tone(DT,RBT,SDT, BT) Detect Test on FXO Device"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: progress_fxo <astel FXO device> [Call NO] [Standard '0'-CHINA '1'-JAPAN]\n\tFXO play as Caller and check Call Progress Tone send by PBX\n");
		exit(1);
	}
	if(argc<3)
		sprintf(dialingStr, "812");
	else
		sprintf(dialingStr, "%s", argv[2] );
	if(argc<4)
		standard = PSTN_STANDARD_JAPAN;
	else
	{
		if(atoi(argv[3]) == PSTN_STANDARD_CHINA)
			standard = PSTN_STANDARD_CHINA;
		else if(atoi(argv[3]) == PSTN_STANDARD_JAPAN)
			standard = PSTN_STANDARD_JAPAN;
		else
			standard = PSTN_STANDARD_JAPAN;
	}

	fdo = open(argv[1], O_RDWR);
	if (fdo < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, standard, 20);
	assert(dsp);

	printf("PROGRESS DETECT : FXO device as callee\r\n\tPROGRESS DETECT : STEP 1: reset FXO device\r\n");
	as_lib_onhook(fdo);
	usleep(400*1000);

	printf("\n\tPROGRESS DETECT : STEP 2 : Begin to off-hook operation...\n");
	res = as_lib_offhook(fdo);

	printf("\n\tPROGRESS DETECT : STEP 3 : Check DT(Dialing Tone)...\n");

	while(1)
	{
		res = read(fdo, data, AS_DRIVER_BUFFER_LENGTH);
		total += res;
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("PROGRESS DETECT fail : EOF or this file is not a device file for PSTN firmware, %d bytes have been read\n", total);
			break;
		}

		res = as_dsp_progress_detect(dsp, data, AS_DRIVER_BUFFER_LENGTH);
		if(res == AS_PROGRESS_DIALTONE )
		{
			printf("\tCall Progress Dial Tone Detected after %d ms\n", total/8 );
			break;
		}
		else if(res != AS_PROGRESS_UNKNOWN)
		{
			printf("\tCall Progress Invalidate :%d\n ", res);
			exit(1);
		}
	};

	
	printf("\n\tPROGRESS DETECT : STEP 4 : Begin to dial Calling Number '%s'...\n", dialingStr );
	as_dsp_play_dtmf( dsp,  fdo, dialingStr);

	total = 0;
	printf("\n\tPROGRESS DETECT : STEP 5 : Check other Call Progress Tone...\r\n" );
	while(1)
	{
		res = read(fdo, data, AS_DRIVER_BUFFER_LENGTH);
		total += res;
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("PROGRESS DETECT fail : EOF or this file is not a device file for PSTN firmware, %d bytes have been read\n", total);
			break;
		}

		res = as_dsp_progress_detect_busy(dsp, data, AS_DRIVER_BUFFER_LENGTH);
		if(res == AS_PROGRESS_DIALTONE )
		{
			printf("\n\t\tCall Progress Tone Detected 'DT(Dial Tone)' after %d ms\n", total/8 );
		}
		else if(res == AS_PROGRESS_RINGBACK )
		{
			printf("\n\t\tCall Progress Tone Detected 'RBT(RingBack Tone)' after %d ms\n", total/8 );
		}
		else if(res == AS_PROGRESS_2DIALTONE )
		{
			printf("\n\t\tCall Progress Tone Detected 'SDT(Second Dial Tone)' after %d ms\n", total/8 );
		}
		else if(res == AS_PROGRESS_TALKING)
		{
			printf("\n\t\tCall Progress Tone Detected 'Talking' after %d ms\n", total/8 );
		}
		else if(res == AS_PROGRESS_BUSYTONE)
		{
			printf("\n\t\tCall Progress Tone Detected 'BT(Busy Tone)' after %d ms\n", total/8 );
			break;
		}
		else if(res != AS_PROGRESS_UNKNOWN)
		{
			printf("\n\t\tCall Progress Invalidate :%d\n ", res);
			exit(1);
		}
	};

	printf("\n\n\tPROGRESS DETECT : Successfully!\r\n" );
	
	as_lib_onhook(fdo);
	as_dsp_destroy( dsp);
	close(fdo);
	return 0;
}


