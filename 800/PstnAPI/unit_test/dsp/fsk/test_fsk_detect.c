/*
 * $Author: lizhijie $
 * $Id: test_fsk_detect.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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

/*
* Decode FSK Called ID in MDMF format which is not used in Japan, just only in China and other countris.
*/

int main(int argc, char *argv[])
{
//	int fd;
	int fd2;
	int res;
	char *file = {"fsk_4123456.data"};
	char phonenumber[32];
	char name[32];
	as_dsp_t	*dsp;
#if 0	
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fsk_detect <filename> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	
#endif
	if(argc < 2 )
		fd2 = open( file, O_RDWR);
	else
		fd2 = open( argv[1], O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		exit(1);
	}
	
	printf( AS_VERSION_INFO("DSP Software FSK(Modem) CallerId Test"));
	printf("FSK_DETECT: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_CHINA , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");

	printf("FSK detect\r\n");
	res = as_dsp_fsk_decode_fd(dsp,  fd2, phonenumber,  name);
	
	as_dsp_destroy(dsp);
//	close(fd);
	close(fd2);

	printf("FSK_DETECT: End of output FSK data!res =%d phonenumber ='%s' name = '%s'\r\n", res, phonenumber, name );
	return 0;
}


