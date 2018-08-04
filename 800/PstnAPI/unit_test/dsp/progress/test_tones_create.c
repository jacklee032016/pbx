/*
 * $Id: test_tones_create.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 * $Author: lizhijie $
*/

/*
*  create Call Progress Tone file which are used to test our DSP algorithm
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
	int tone_id;
	int duration;
	as_dsp_t	*dsp;
	char *name[4] = {"DT(Dial Tone)", "RBT(Ringback Tone)", "SDT(Second DT)", "BT(Busy Tone)" };
	
	printf( AS_VERSION_INFO("DSP Software Create Tone Data File"));
	if (argc < 4 ) 
	{
		fprintf(stderr, "Usage: create_tone <file> [tone_id] [duration(ms)]\n");
		fprintf(stderr, "\ttone_id :\n\t\t1 : DT; \n\t\t2 : RBT;\n\t\t3 : SDT\n\t\t4 : BT\n");
		fprintf(stderr, "\tduration :\tin unit of ms\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	tone_id = atoi(argv[2]);
	if(tone_id<1 || tone_id>3)
	{
		printf("parameter 'tone_id' is not validate value\n");
		return 1;
	}

	duration = atoi(argv[3]);
	if(duration < 4000)
	{
		printf("WARNING : duration is too small\n");
	}

	printf("Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");
	
	printf("add %s tone with duration of %dms into file %s...\n", name[tone_id-1], duration, argv[1]);
	switch(tone_id)
	{
		case 1:
			as_dsp_tone_play_dial(dsp, fd, duration );
			break;
		case 2:
			as_dsp_tone_play_ringback(dsp, fd, duration );
			break;
		case 3:
			as_dsp_tone_play_second_dial_tone(dsp, fd, duration );
			break;
		case 4:
			as_dsp_tone_play_busy(dsp, fd, duration );
			break;
		default:
			printf("Error\n");
	}
	printf("\t\t\t\t\tEnd!\n");
	as_dsp_destroy(dsp);
	close(fd);

	printf("DSP Create Tone File successfully!\n");
	return 0;
}

