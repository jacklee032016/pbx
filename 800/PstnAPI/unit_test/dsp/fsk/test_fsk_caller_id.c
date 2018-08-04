/*
 * $Author: lizhijie $
 * $Log: test_fsk_caller_id.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.9  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.8  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.7  2006/04/29 08:16:17  lizhijie
 * add
 *
 * Revision 1.6  2006/04/29 08:15:04  lizhijie
 * no message
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
#include <assert.h>

#include "assist_lib.h"
#include "as_version.h"

int main(int argc, char *argv[])
{
	int fd;
	int res;
	char phonenumber[32];
	char username[32];
	as_dsp_t *dsp;

	memset(phonenumber, 0, 32);
	memset(username, 0, 32);

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fsk_gen <filename> [phonenumber] [username]\nBoth device file and normal file can used here\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	
	if(argc < 3 )
		sprintf(phonenumber,"%s", "87654321");
	else
		sprintf(phonenumber, "%s", argv[2] );

	if(argc < 4 )
		sprintf(username, "%s" ,"LiZhijie");
	else
		sprintf(username, "%s" , argv[3]);
	
	printf( AS_VERSION_INFO("DSP Software FSK(Modem) CallerId Test"));
	printf("FSK_GEN: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_CHINA , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");
	

	printf("Now begin ringing with phone number '%s', user name '%s'...\n", phonenumber, username);
	res = as_dsp_fsk_ring_with_callerid(dsp, fd, phonenumber, username );
	printf("\t\t\t\t\tEnd!\n");

	printf("FSK caller ID ended, result is %d\r\n", res);

	as_dsp_destroy(dsp);
	close(fd);
	return 0;
}

