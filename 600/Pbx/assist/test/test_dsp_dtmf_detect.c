/*
 * $Id: test_dsp_dtmf_detect.c,v 1.2 2007/09/08 18:21:40 lizhijie Exp $
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#include "assist_dsp.h"
#include "pbx_globals.h"

#define AS_TMP_COUNT		7

//__DECODE_LENGTH			102*

int main(int argc, char *argv[])
{
	int fd;
#if 0	
	int res;
	char phonenum[64];
	char buf[160*AS_TMP_COUNT];
	int i;
	char tmp[160];
#endif	
	int length =1;

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: dsp_dtmf_detect <astel device> \r\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\r\n", argv[1], strerror(errno));
		exit(1);
	}

	assist_dsp_t *dsp = assist_dsp_init(U_LAW_CODE, 1.0, 1.0, 160*AS_TMP_COUNT);
	while( length > 0)
	{
#if 0	
		int length = 160*4;
		res = as_dsp_dtmf_detect(fd, U_LAW_CODE, length , phonenum, 64);
		if(res>0)
		{
			printf("CallerID ='%s' in buffer with length of '%d'\n", phonenum, length);                               //check event
		}	
#endif

#if 0
		for(i=0; i<AS_TMP_COUNT; i++)
		{
			length = read(fd, tmp, 160);
			memcpy(buf + i*160, tmp, length );
		}
		
		res = (dsp->ops->dtmf_decode)( dsp, buf, 160*AS_TMP_COUNT , phonenum, 64);
		if(res>0)
		{
			printf("CallerID ='%s' in buffer with length of '%d'\r\n", phonenum, 160*AS_TMP_COUNT);
		}	
#endif		
		
	}

	assist_dsp_destory( dsp);

	printf( "Close!!!\r\n");
	return 0;
}

