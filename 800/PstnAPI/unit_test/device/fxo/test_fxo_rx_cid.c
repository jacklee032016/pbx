/*
 * $Id: test_fxo_rx_cid.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 * $Author: lizhijie $
*/

/*
* test callee process both in initial and end a call 
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_busy_detect.h"
#include "as_dsp.h"
#include "as_version.h"

#define 	FXO_CALL_DELAY_3_SECOND		400

int main(int argc, char *argv[])
{
#define AS_FSK_SILENCE_THRESHOLD 			200 //1024	/* energy level threshold between SILENCE and NOISE in linear mode */
	int fd, fd2;
	int res;
	char buf[AS_DRIVER_BUFFER_LENGTH];
	unsigned char 	fskdata[100000];
	int i = 0;
	int length = 0;
	int total = 0;
	int n=0;
	pstn_param_t  	*param = NULL;
	int isLocalFile = 0;

	printf( AS_VERSION_INFO("Test RX Caller Id from Device compatible with NTT specs"));

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_cid <FXO device> \n");
		exit(1);
	}
	if(argc >= 3 )
		isLocalFile = 1;

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	fd2 = open("2.data", O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open : %s\n",  strerror(errno));
		exit(1);
	}

	printf("INTENT: RX Caller Id from Device compatible with NTT specs\n");
	printf("NTT CID : Please Call to this FXO port with CallerID enabled...\n");

	as_dsp_t  *dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);

	if(!isLocalFile )
		res = as_lib_onhook(fd);
	
	as_error_msg("\n\t\tNTT-CID : \n\tSTEP 1 : Waiting CAR signal from outside...\n");

	if(!isLocalFile )
	{
		while(1)
		{
			res = as_lib_event_get( fd);
			if(res== AS_EVENT_CAR)
			{
				i++;
				printf("\tFXO CID : RX %d CAR Event\n", i);
				if(i==1)
					break;
			}
			else if(res== AS_EVENT_RINGEROFF)
			{
				as_error_msg( "ERROR : Caller ID is not enabled by the Switch you are using\r\n");
				exit(1);
			}
		};
	}	
	
	as_error_msg("\n\tSTEP 2 : Off-Hook this FXO device...\n");

	if(!isLocalFile )
		as_lib_offhook(fd);

	i = 0;
	n =0;
	as_error_msg("\n\tSTEP 3 : Read and Write/decode Modem Signal...\n");
//	usleep(200 * 1000);
	while(1)
	{
		res = read(fd, buf, AS_DRIVER_BUFFER_LENGTH);
		if( res >0 &&  as_dsp_fsk_isVAD(dsp, buf, res)  )
		{
			memcpy(	fskdata+ length, buf, res);
			length += res;
			printf("copy %d bytes FSK data( total : %d)\n", res, length );
		}
#if 0		

#if 0		
		if( res >0 )
		{
			for (i=0; i< res; i++) 
				accum += abs( XLAW( buf[i], U_LAW_CODE));//dsp->law) );
			accum /= res;
			as_error_msg("\n\t\t : accum : %d\n", accum);
			if(length>2000 && accum<AS_FSK_SILENCE_THRESHOLD )
					break;
			if(length< 900 && accum<AS_FSK_SILENCE_THRESHOLD )
					length = 0;
			if(accum >= AS_FSK_SILENCE_THRESHOLD )
			{
				as_error_msg("\n\t\t : add %d bytes VAD data(total %d) accum : %d\n", res, length, accum);
				for (i=0; i< res; i++) 
					fskdata[length ++]= buf[i];
			}
//				if(length>4000)
//					break;
		}
#else
		if( res >0 )
		{
			for (i=0; i< res; i++) 
				accum += abs( XLAW( buf[i], U_LAW_CODE));//dsp->law) );
			accum /= res;
			as_error_msg("\n\t\t : accum : %d\n", accum);
//			if(length>2000 && accum<AS_FSK_SILENCE_THRESHOLD )
//					break;
//			if(length< 900 && accum<AS_FSK_SILENCE_THRESHOLD )
//					length = 0;
			if(accum >= AS_FSK_SILENCE_THRESHOLD )
			{
				accumCount += res;
				as_error_msg("\n\t\t : add %d bytes VAD data(total %d) accum : %d\n", res, length, accum);
				if(accumCount > 3000)
					break;
			}
				for (i=0; i< res; i++) 
					fskdata[length ++]= buf[i];
				if(length>6000)
					break;
		}
#endif

#endif

		if(res != AS_DRIVER_BUFFER_LENGTH)
			break;
		n++;
		total += res;
		if(n== FXO_CALL_DELAY_3_SECOND)
		{
			break;
		}	
	};

	as_error_msg("\n\tTotal %d bytes read from device and VAD data length is %d\n", total, length);
	
	param =  as_dsp_fsk_decode(dsp, fskdata, length );

	if( param && (param->result & AS_CALL_PARAM_CALLERID) ) 
	{
		printf("\n\tCaller ID : %s\n", param->caller);
	}
	if( param && (param->result & AS_CALL_PARAM_CALLED) ) 
	{
		printf("\n\tCalled Phone Number : %s\n", param->called);
	}
	
	as_error_msg("\n\tSTEP 4 : ON-Hook this FXO device and wait RING signal...\n");

	if(!isLocalFile )
	{
		as_lib_onhook(fd);

		while(1)
		{
			res = as_lib_event_get( fd);
			if(res== AS_EVENT_RINGEROFF)
			{
				as_error_msg( "FXO CID : rx RING Signal\r\n");
				break;
			}
		};
	}
	write(fd2, fskdata, length);
	
	printf("\tFXO CID : quit normally\n");
	as_dsp_destroy(dsp);
	close(fd);
	close(fd2);
	
	printf("\nEnd of NTT rx.\n");
	
	return 0;
}

