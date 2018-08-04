/*
 * $Author: lizhijie $
 * $Id: test_fsk_file.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 * $Revision: 1.1.1.1 $
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
	int fd, fd2;
	int len;
	char *file = {"fsk_4123456.data"};
//	as_dsp_t	*dsp;

	char buf[1024];
	if (argc < 2)
	{
		fprintf(stderr, "Usage: dsp_fsk_test_file <asstel device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	if(argc<3)
		fd2 = open(file, O_RDWR);
	else
		fd2 = open(argv[2], O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		exit(1);
	}

	printf( AS_VERSION_INFO("DSP Software FSK(Modem) CallerId Test"));
	printf("FSK_FILE: Assist DSP engine is initting...\n");
	/* init DSP Library */
//	dsp = as_dsp_init_no_cfg(U_LAW_CODE, 20);	/* u law, 20ms audio data is kept in dsp engine */
//	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");

	as_ring_on_hook( fd);

	as_lib_wait_ringer_off( fd);
	
	sleep(2);
	printf("FSK_FILE: Begin output FSK data...\r\n");
	while(1)
	{
		len = read(fd2, buf, 160 );
		if(len != 160)
			break;
		len = write(fd, buf, len);
		if(len != 160)
			break;
	};

//	as_dsp_destroy(dsp);
	close(fd);
	close(fd2);

	printf("FSK_FILE: End of output FSK data!\r\n");
	return 0;
}

