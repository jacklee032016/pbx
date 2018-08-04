/*
 * $Id: test_fxo_rx_chinese_cid.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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


int main(int argc, char *argv[])
{
#define 	FXO_CALL_DELAY_4_SECOND		210
	int fd;
	int res;
	char buf[AS_DRIVER_BUFFER_LENGTH];
	unsigned char 	fskdata[50000];
	int i = 0;
	int length = 0;
	int total = 0;
	int n=0;
	pstn_param_t  	*param = NULL;
	int isLocalFile = 0;

	printf( AS_VERSION_INFO("Test RX Caller Id from Device compatible with Chinese specs"));

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_ccid <FXO device> \n");
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

	printf("INTENT: RX Caller Id from Device compatible with Chinese specs\n");
	printf("China CID : Please Call to this FXO port with CallerID enabled...\n");

	as_dsp_t  *dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_CHINA , 20);

	if(!isLocalFile )
		res = as_lib_onhook(fd);
	as_error_msg("\n\t\tChina-CID : \n\tSTEP 1 : Waiting CAR signal from outside...\n");

	if(!isLocalFile)
	{
		while(1)
		{
			res = as_lib_event_get( fd);
			if(res== AS_EVENT_RINGEROFF)
			{
				i++;
				printf("\tFXO CID : RX %d first RINGEROFF Event\n", i);
				if(i==1)
					break;
			}
		};
	}
	as_error_msg("\n\tSTEP 2 : Off-Hook this FXO device...\n");

	i = 0;
	n =0;
	as_error_msg("\n\tSTEP 3 : Read and Write/decode Modem Signal...\n");
//	usleep(300 * 1000);
	while(1)
	{
		res = read(fd, buf, AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
			break;
		
		if( res >0 &&  as_dsp_fsk_isVAD(dsp, buf, res)  )
		{
			memcpy(	fskdata+i, buf, res);
			i += res;
		}
		
		n++;
		total += res;
		
		if(!isLocalFile)
		{
			if(n== FXO_CALL_DELAY_4_SECOND)
			{
				break;
			}	
			res = as_lib_event_get( fd);
			if(res== AS_EVENT_RINGEROFF)
			{
				as_error_msg( "FXO CID : rx RING Signal\r\n");
				break;
			}
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
		printf("\n\tUser Name : %s\n", param->called);
	}
	if( param && (param->result & AS_CALL_PARAM_DATE) ) 
	{
		printf("\n\tDate Info : %c%c:%c%c %c%c:%c%c\n", 
			param->date[0], param->date[1],param->date[2],param->date[3],
			param->date[4],param->date[5],param->date[6],param->date[7]);
	}
	
	as_error_msg("\n\tSTEP 4 : ON-Hook this FXO device and wait RING signal...\n");

	if(!isLocalFile)
		as_lib_onhook(fd);

	printf("\tFXO CID : quit normally\n");
	as_dsp_destroy(dsp);
	close(fd);
	
	printf("\nEnd of China CallerID\n");
	
	return 0;
}

