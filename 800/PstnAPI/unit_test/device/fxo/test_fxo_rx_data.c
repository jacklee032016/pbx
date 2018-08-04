/*
 * $Id: test_fxo_rx_data.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
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

#define 	FXO_CALL_DELAY		3000	/* ms */

static int li_as_dsp_busy_maybe( busy_t *dsp )
{
	int accum = 0;
	int x;
	int res = 0;
	
	if ( dsp->bufp!= AS_DRIVER_BUFFER_LENGTH)
		return 0;
	for (x=0;x<AS_DRIVER_BUFFER_LENGTH; x++) 
		accum += abs(dsp->buf[x]);
	accum /= AS_DRIVER_BUFFER_LENGTH;

 	if (accum > 30000 ) 
	{ 
		dsp->totalSilence += AS_DRIVER_BUFFER_LENGTH/8;  /* 20 ms */

		if (dsp->totalNoise) 
		{/* from NOISE to SILENCE, so record the NOISE level in last duration */
			memmove(dsp->historicNoise, dsp->historicNoise + 1, (AS_BUSY_HISTORY - 1)*sizeof(dsp->historicNoise[0]));
			dsp->historicNoise[AS_BUSY_HISTORY - 1] = dsp->totalNoise;
			dsp->busyMaybe = 1;
			
			dsp->busyCount ++;
		}
		dsp->totalNoise = 0;
		res = 1;
	} 
	else 
	{ 
		dsp->totalNoise += AS_DRIVER_BUFFER_LENGTH/8;

		if (dsp->totalSilence) 
		{/* from SILENCE to NOISE, record silence level in last duration */
			int currentSilence = dsp->historicSilence[AS_BUSY_HISTORY - 1];
			int lastSilence = dsp->historicSilence[AS_BUSY_HISTORY - 2];

			memmove(dsp->historicSilence,	dsp->historicSilence + 1, (AS_BUSY_HISTORY - 1)*sizeof(dsp->historicSilence[0]));
			dsp->historicSilence[AS_BUSY_HISTORY - 1] = dsp->totalSilence;
			
			/* check if the previous sample differs only by AS_BUSY_PERCENT from the one before it */
			if (currentSilence < lastSilence) 
			{
				if (currentSilence + currentSilence/AS_BUSY_PERCENT >= lastSilence)
					dsp->busyMaybe = 1;
				else 
					dsp->busyMaybe = 0;
			} 
			else 
			{
				if (currentSilence - currentSilence/AS_BUSY_PERCENT <= lastSilence)
					dsp->busyMaybe = 1;
				else 
					dsp->busyMaybe = 0;
			}
					
		}
		dsp->totalSilence = 0;
	}
	
	return res;
}

static int li_as_busydetect(busy_t *dsp, unsigned char *buff,int length)
{
	int x;

	for (x=0;x< length; x++) 
	{
		dsp->buf[dsp->bufp] = XLAW( buff[x], dsp->law );
		dsp->bufp++;
	}	
	return 0;
}

int main(int argc, char *argv[])
{
	int fd, fd2;
	int res;
	char buf[160], name[32];
	char silencebuf[160];
	int i = 0;
	int length;
	int n=0;

	printf( AS_VERSION_INFO("Test RX Caller Id from Device compatible with NTT specs"));

	memset(buf,0,sizeof(buf));
	memset(silencebuf,0,sizeof(silencebuf));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_cid <FXO device> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	system("rm save.data");
	system("touch save.data");
	fd2 = open("save.data", O_RDWR);
	if (fd2 < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: RX Caller Id from Device compatible with NTT specs\n");
	printf("NTT CID : Please Call to this FXO port with CallerID enabled...\n");

	as_dsp_t  *dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN , 20);
	busy_t  *dp=as_dsp_new_busydetect(dsp);

	res = as_lib_onhook(fd);
	as_error_msg("\n\t\tNTT-CID : \n\tSTEP 1 : Waiting CAR signal from outside...\n");
	
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
	
	as_error_msg("\n\tSTEP 2 : Off-Hook this FXO device...\n");
	as_lib_offhook(fd);

	i = 0;
	as_error_msg("\n\tSTEP 3 : Read and Write/decode Modem Signal...\n");
	while(1)
	{
		length = read(fd, silencebuf, 160);
		li_as_busydetect(dp,silencebuf, 160);
		n=li_as_dsp_busy_maybe(dp);
		if(1!=n)
		{	
			memset(silencebuf,0,sizeof(silencebuf));
			dp->bufp = 0;
		i++;
			if(i==150)
				break;
			continue;
		}	
		write(fd2,silencebuf,160);
		dp->bufp = 0;
	};
	lseek(fd2,0,SEEK_SET);
	as_dsp_fsk_decode_fd(dsp,fd2,buf, name);
	printf("---------%s\n",buf);
	as_error_msg("\n\tSTEP 4 : ON-Hook this FXO device and wait RING signal...\n");
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

	printf("\tFXO CID : quit normally\n");
	as_dsp_destroy(dsp);
	
	close(fd);
	close(fd2);
	
	printf("\nEnd of NTT rx.\n");
	
	return 0;
}

