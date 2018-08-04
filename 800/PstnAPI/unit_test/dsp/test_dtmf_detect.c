/*
 * $Author: lizhijie $
 * $Id: test_dtmf_detect.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_version.h"

#define TEST_RX_TOTAL		5

int main(int argc, char *argv[])
{
	int fd;
	int res =0;
	int i = 0;
	char buf[AS_DRIVER_BUFFER_LENGTH];
	char dial_str[32];
	as_dsp_t *dsp;

	int count = 0;

	printf( AS_VERSION_INFO("DSP Software DTMF Detect Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: dtmf_dsp_detect  <assist device1>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 10);
	assert(dsp);
	printf("Please off hook your phone\n");
	as_lib_wait_offhook(fd);

	printf("Phone is OFF-HOOK, please press Push Button to test DTMF detect\n");

	while(1)
	{
		res = read(fd, buf, AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("read Length is not correct : %d\r\n" , res);
			break;
		}

		count++;
		if(count==5)
		{
			count = 0;
			res = as_dsp_dtmf_detect(dsp, buf, AS_DRIVER_BUFFER_LENGTH, dial_str, 32);
			if(res>0 )
			{
				printf("DTMF Digit Detect : '%s'\n", dial_str);
				i += res;
				if(i >TEST_RX_TOTAL)
					break;
			}
		}
	};

	as_dsp_destroy( dsp);
	close(fd);
	printf("RX %d DTMF Digits!Program quit, please on hook the phone\n", TEST_RX_TOTAL);
	return 0;
}

