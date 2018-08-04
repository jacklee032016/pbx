/*
 * $Log: test_dsp_tones.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.10  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.9  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.8  2006/04/29 08:16:17  lizhijie
 * add
 *
 * Revision 1.7  2006/04/29 08:14:42  lizhijie
 * no message
 *
 * $Id: test_dsp_tones.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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
	as_dsp_t *dsp;
	
	printf( AS_VERSION_INFO("DSP Software Tone Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: dsp_tones < asisst FXS device> \n\t");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: to play tones of softwareful\n");
	printf("DSP_TONES: off hook your phone connect this port and listen to it\n");


	printf( AS_VERSION_INFO("DSP Software Tones Test"));
	printf("DSP_TONES: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");
	
	printf("DSP_TONES: Please off hook your phone\n");
	as_lib_wait_offhook(fd);
	printf("DSP_TONES: Phone is OFF-HOOK, Listen to tone play by DSP software\n");

	printf("DSP_TONES: Now play DT(Dial TONE)...\n");
	as_dsp_tone_play_dial(dsp, fd, PLAY_DURATION*1000 );
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("DSP_TONES: Now play BT(Busy TONE)...\n");
	as_dsp_tone_play_busy(dsp, fd, PLAY_DURATION*1000);
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);
	
	printf("DSP_TONES: Now play RBT(RingBack TONE)...\n");
	as_dsp_tone_play_ringback(dsp, fd, PLAY_DURATION*1000);
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("DSP_TONES: Now play SDT(Second Dial TONE)...\n");
	as_dsp_tone_play_second_dial_tone(dsp, fd, PLAY_DURATION*1000 );
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("DSP_TONES: Now play CPT(Acceptance TONE)...\n");
	as_dsp_tone_play_accept_tone(dsp, fd, PLAY_DURATION*1000);
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("DSP_TONES: Now play NFT(Notification Tone)...\n");
	as_dsp_tone_play_notify_tone(dsp, fd, PLAY_DURATION*1000);
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("DSP_TONES: Now play IIT(Incoming Identification)...\n");
	as_dsp_tone_play_incoming_id_tone(dsp, fd, PLAY_DURATION*1000);
	printf("\t\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	as_dsp_destroy(dsp);
	close(fd);

	printf("DSP_TONES: DSP Tone Test ended successfully!\n");
	return 0;
}

