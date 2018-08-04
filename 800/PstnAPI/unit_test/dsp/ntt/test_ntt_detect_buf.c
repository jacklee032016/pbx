/*
 * $Author: lizhijie $
 * $Id: test_ntt_detect_buf.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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

#include "as_dsp.h"

int main(int argc, char *argv[])
{
#define AS_FSK_SILENCE_THRESHOLD 			1000 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */
	int fd;
	int res, total = 0;
	char *file = {"0000000014.ntt.data"};
	char 			buf[AS_DRIVER_BUFFER_LENGTH];
	unsigned char 	fskdata[32000];
	as_dsp_t			*dsp;
	pstn_param_t  	*param = NULL;
	int i =0;
	int accum = 0;
	int length = 0;
	
	if(argc < 2 )
		fd = open( file, O_RDWR);
	else
		fd = open( argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
		fprintf(stderr, "Usage: ntt_dem <filename> \n");
		exit(1);
	}
	
	printf( AS_VERSION_INFO("DSP Software FSK(Modem) CallerId of NTT specs Test"));
	printf("NTT_DETECT: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");
	printf("NTT FSK(Modem) detect\r\n");
	while(1)
	{
		res = read(fd, buf, AS_DRIVER_BUFFER_LENGTH);
		total += res;
		printf("read %d bytes from file\n", total);
		
		if( res >0 )
		{
			for (i=0; i< res; i++) 
				accum += abs( XLAW( buf[i], U_LAW_CODE));//dsp->law) );
			accum /= res;
			if(accum >= AS_FSK_SILENCE_THRESHOLD )
			{
				for (i=0; i< res; i++) 
					fskdata[length ++]= buf[i];
			}
		}	
		total += accum;
		
		if(res < AS_DRIVER_BUFFER_LENGTH || length> sizeof(fskdata) )
		{
			break;
		}
	}

	param =  as_dsp_fsk_decode(dsp, fskdata, length );
		
	printf("Demodulate result : \n");
	if( param && (param->result & AS_CALL_PARAM_CALLERID) ) 
	{
		printf("\tCaller ID : %s\n", param->caller);
	}
	if( param && (param->result & AS_CALL_PARAM_CALLED) ) 
	{
		printf("\tCalled Phone Number : %s\n", param->called);
	}
	
	if( !param || (param->result == 0) ) 
	{
		printf("\tNothing detect in FSK modem signal\n");
	}
	
	as_dsp_destroy(dsp);
	close(fd);

	return 0;
}


