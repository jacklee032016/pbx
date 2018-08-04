/*
 * $Author: lizhijie $
 * $Log: test_busy_detect.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.7  2006/05/30 03:04:27  lizhijie
 * optimzed DTMF decoder and add DSP configuration interface
 *
 * Revision 1.6  2006/05/25 03:19:41  lizhijie
 * add pstn_standard
 *
 * Revision 1.5  2006/04/30 06:16:01  wangwei
 * no message
 *
 * Revision 1.4  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.3  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.2  2006/03/20 06:39:43  lizhijie
 * no message
 *
 * Revision 1.1  2006/03/17 07:57:37  lizhijie
 * no message
 *
 * $Revision: 1.1.1.1 $
*/

/*
*  Detect BT(Busy Tone) from a normal disk file of BT
*  Display BT detect result and the time need to finish this test 
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
	int total = 0;
	char data[AS_DRIVER_BUFFER_LENGTH];
	char file[32];
	as_dsp_t *dsp;

	printf( AS_VERSION_INFO("DSP Software BT(Busy Tone) Detect Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: busy <filename> \n\t---Detect Busy Tone from a normal file or device file(Default : busy.tone.u)\n\n\n");
	}

	if(argc < 2 )
		sprintf(file, "%s", "busy.tone.u" );
	else
		sprintf(file, "%s", argv[1] );

	fd = open( file, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		exit(1);
	}
	dsp = as_dsp_init_no_cfg(U_LAW_CODE,PSTN_STANDARD_JAPAN, 20);
	assert(dsp);
	
	while(1)
	{
		res = read(fd, data, AS_DRIVER_BUFFER_LENGTH);
		total += res;
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("BT(Busy Tone) Detect fail : EOF or this file is not a device file for PSTN firmware, %d bytes have been read\n", total);
			break;
		}

		if(as_dsp_busydetect(dsp, data, AS_DRIVER_BUFFER_LENGTH) )
		{
			printf("BT(Busy Tone) Detected on %s after %d ms\n", file, total/8 );
			break;
		}
	};

	as_dsp_destroy( dsp);
 
	printf("BUSY_DETECT:    Test ended successfully!\n");  
	
	close(fd);
	return 0;
}

