/*
 * $Author: lizhijie $
 * $Id: test_dsp_dtmf_gen.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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
#define PLAY_DURATION			5 	/* second */
#define GAP_DURATION			1 	/* second */

int main(int argc, char *argv[])
{
	int fd;
	int delay = 300;
	as_dsp_t *dsp;
	int i;

	printf( AS_VERSION_INFO("To send DSP Software DTMF Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: dsp_dtmf_gen < asisst FXS device> [Call Number] [ring-off to send DTMF time] \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: to genareting DTMF signal to PHONE\n");
	
	

	if(argc == 4)
		delay = atoi(argv[3]);

	printf("DSP_DTMF_GEN: Initialize assist DSP library for SLIC/DAA!Please waiting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
//	printf("Please off hook your phone\n");
	as_lib_wait_offhook(fd);
//	printf("Phone is OFF-HOOK, Listen to tone play by DSP software\n");

	as_ring_on_hook(fd );

//	as_lib_wait_ringer_off(fd);
	usleep(delay);
	if(argc > 2)
	{
		printf("DSP_DTMF_GEN: RingOff, Now send CallerID...%s\n",argv[2]);
		as_dsp_play_dtmf(dsp, fd, argv[2] );
	}
	else
	{
		printf("DSP_DTMF_GEN: RingOff, Now send CallerID...84567890\n");
		as_dsp_play_dtmf(dsp, fd, "84567890" );
	}
	sleep(GAP_DURATION*5);

	for(i=0;i<10;i++)
	{
		as_dsp_play_dtmf(dsp, fd, "84567890" );
		sleep(GAP_DURATION);
	}	
	as_dsp_destroy(dsp);
	close(fd);

	printf("DSP_DTMF_GEN: DSP Tone Test ended successfully!\n");
	return 0;
}

