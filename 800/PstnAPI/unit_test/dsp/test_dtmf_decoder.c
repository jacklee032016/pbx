/*
 * $Author: lizhijie $
 * $Id: test_dtmf_decoder.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "as_thread.h"
#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_version.h"

#define __DECODER_BUFFER_COUNT			500		/* buf size is 200ms */
#define __DECODER_BUFFER_SIZE				2


int		__dtmf_decode_count	=	0;
unsigned char	__decode_result[32];

int 		currentbuf =-1;
unsigned char *buf;

	as_dsp_t *dsp;
	int currentPosition = -1;


	char number[32];
	
void *_dtmf_decoder_thread_handler(void *data)
{
	int res;

	printf("DTMF Decoding Thread running...\r\n");
	while(1)
	{
		if(currentbuf>currentPosition)
		{
			currentPosition++;
//			printf("\nDecoding Thread work on %d buffer\n", currentPosition);
			res = as_dsp_dtmf_console_decode(dsp, buf+currentPosition*__DECODER_BUFFER_SIZE*AS_DRIVER_BUFFER_LENGTH, __DECODER_BUFFER_SIZE*AS_DRIVER_BUFFER_LENGTH, number, sizeof(number) );
			if(res ==1)
			{
				printf("Decoder detect '%c'\n", number[0]);
				__decode_result[__dtmf_decode_count++] = number[0];
			}
		}
		if(currentPosition == (__DECODER_BUFFER_COUNT-1))
		{
			printf("Buffer used up by DTMF Decoding thread\n");
			break;
		}	
		
	}

	sleep(1);
	as_dsp_destroy( dsp);

	return NULL;
}

as_thread_t  _decoder_thread =
{
	name	:	"DTMF Decoder",
	handler	:	_dtmf_decoder_thread_handler,
	log		: 	printf,

	private	:	NULL
};


int main(int argc, char *argv[])
{
	int fd;
	int res =0;
	int i = 0;
	int count = 0;
	
//	currentbuf = 0;


	printf( AS_VERSION_INFO("DSP Software DTMF Decode Test"));
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

	buf = (unsigned char *)malloc(__DECODER_BUFFER_COUNT*AS_DRIVER_BUFFER_LENGTH*__DECODER_BUFFER_SIZE);
	assert(buf!=0);
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);
	assert(dsp);
	as_dsp_dtmf_decoder_reset( dsp);

	as_thread_create(&_decoder_thread);
	
	printf("Please off hook your phone\n");
	as_lib_wait_offhook(fd);

	printf("Phone is OFF-HOOK, please press Push Button to test DTMF detect\n");

	while(1)
	{
		res = read(fd, buf+i*AS_DRIVER_BUFFER_LENGTH, AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("read Length is not correct : %d\r\n" , res);
			break;
		}
		i++;

		count++;
		if(count==__DECODER_BUFFER_SIZE)
		{
			count = 0;
			currentbuf ++;

#if 0
			res = as_dsp_dtmf_console_decode(dsp, buf+currentbuf*__DECODER_BUFFER_SIZE*AS_DRIVER_BUFFER_LENGTH, __DECODER_BUFFER_SIZE*AS_DRIVER_BUFFER_LENGTH, number, sizeof(number) );
			if(res ==1)
			{
				printf("Decoder detect '%c'\n", number[0]);
				__decode_result[__dtmf_decode_count++] = number[0];
			}
#endif

//			printf("Input data in %d buffer by READ thread\n", currentbuf);
			if(currentbuf== __DECODER_BUFFER_COUNT-1)
				break;
		}
		if(i*AS_DRIVER_BUFFER_LENGTH== __DECODER_BUFFER_COUNT*AS_DRIVER_BUFFER_LENGTH*__DECODER_BUFFER_SIZE)
			break;
		
	};

	printf("Buffer used up by Read Thread\n" );
	pthread_join(_decoder_thread.id, NULL);
#if 0
	do
	{
		
	}while(currentPosition < (__DECODER_BUFFER_COUNT-1) );
#endif

	printf("%d buffers are used by Decoding Thread\n", currentPosition);
	printf("RX %d DTMF Digits!Program quit, please on hook the phone\n", __dtmf_decode_count);
	printf("DTMF Decode Digits : '%s'\n", __decode_result);
	close(fd);
	return 0;
}

