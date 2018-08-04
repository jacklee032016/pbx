/*
 * $Author: lizhijie $
 * $Id: test_ntt_detect.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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
	int fd2;
	int res;
	char *file = {"9876543210.ntt.data"};
	char phonenumber[32];
	as_dsp_t	*dsp;

	if(argc < 2 )
		fd2 = open( file, O_RDWR);
	else
		fd2 = open( argv[1], O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		fprintf(stderr, "Usage: ntt_detect <filename> \n");
		exit(1);
	}
	
	printf( AS_VERSION_INFO("DSP Software FSK(Modem) CallerId of NTT specs Test"));
	printf("NTT_DETECT: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");

	printf("NTT FSK(Modem) detect\r\n");
	res = as_dsp_fsk_decode_fd(dsp,  fd2, phonenumber, NULL);
	
	as_dsp_destroy(dsp);
	close(fd2);

	printf("NTT_DETECT: End of output FSK data!res =%d phonenumber ='%s' \r\n", res, phonenumber );
	return 0;
}


