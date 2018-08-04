/*
 * $Author: lizhijie $
 * $Log: busy_tone_gen.c,v $
 * Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
 * PSTN test library
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.1  2004/12/20 03:21:47  lizhijie
 * add test for FSK generate and detect for test into CVS
 *
 * $Revision: 1.1.1.1 $
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "assist_lib.h"


int main(int argc, char *argv[])
{
	int fd;
	int res;
//	int law = AS_LAW_DEFAULT;
//	int hook;
	as_dsp_gen_engine *dsp;
	
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: busy_tone_gen <file> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	dsp = as_dsp_init_dsp(fd, A_LAW_CODE);

	printf("Busy tone\r\n");
	res = as_dsp_play_tone_busy(dsp, fd );
	
	printf("DSP play tone ended, result is %d\r\n", res);
	close(fd);
	free(dsp);
	return 0;
}

