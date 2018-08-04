/*
 * $Log: test_tones.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.3  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.2  2006/03/13 02:50:20  lizhijie
 * add tone defination for IIT,SDT,NFT,CPT and play mechanism for tone
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: test_tones.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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


#define PLAY_DURATION			3 	/* second */
#define GAP_DURATION			1 	/* second */

int main(int argc, char *argv[])
{
	int fd;
	printf( AS_VERSION_INFO("DSP Software Tones Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: tones < asisst FXS device> \n\toff hook your phone connect this port and listen to it\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: hardware to play DT, BT, RBT, SDT, IIT, CPT, NFT signal\n");

	printf( AS_VERSION_INFO("Firmware Tones Test"));
	printf("TONES: Please off hook the phone connect to this FXS port ....\n");
	as_lib_wait_offhook(fd);

	/* 1 */
	printf("TONES: Now play DT (Dial TONE)...\n");
	as_tone_play_dial( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 2 */
	printf("TONES: Now play BT (Busy TONE)...\n");
	as_tone_play_busy( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 3 */
	printf("TONES: Now play RBT(RingBack TONE)...\n");
	as_tone_play_ringback( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 4 */
	printf("TONES: Now play SDT(Second Dial TONE)...\n");
	as_tone_play_dialrecall( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 5 */
	printf("TONES: Now play IIT(Incoming Identification TONE)...\n");
	as_tone_play_income_id_tone( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 6 */
	printf("TONES: Now play CPT(Acceptance TONE)...\n");
	as_tone_play_accept_tone( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 7 */
	printf("TONES: Now play NFT(Notification TONE)...\n");
	as_tone_play_notify_tone( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 8 */
	printf("TONES: Now play CallWait TONE...\n");
	as_tone_play_callwait( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 9 */
	printf("TONES: Now play Record TONE...\n");
	as_tone_play_record( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 10 */
	printf("TONES: Now play Info TONE...\n");
	as_tone_play_info( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	/* 11 */
	printf("TONES: Now play Stutter TONE...\n");
	as_tone_play_stutter( fd);
	sleep(PLAY_DURATION);
	as_tone_play_stop( fd);
	printf("\t\t\t\tEnd!\n");
	sleep(GAP_DURATION);

	printf("TONES: test succeed\n");

	close(fd);
	return 0;
}

