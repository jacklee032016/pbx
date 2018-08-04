/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Id: test_fsk_ntt_callerid_buf.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
 *
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
	as_dsp_t *dsp;
	unsigned char *data;
	int length;
	int i, count;
	int total = 0;

	memset(phonenumber, 0, 32);

	if (argc < 2) 
	{
		fprintf(stderr, "Usage: ntt_cid <filename> [phonenumber]\nBoth device file and normal file can used here\n");
		fprintf(stderr, "\n\tThis test can only be used with NTT telephone\n");
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
	
	printf( AS_VERSION_INFO("DSP Software FSK(Modem) NTT CallerId Test"));
	printf("Modem: Assist DSP engine is initting...\n");
	/* init DSP Library */
	dsp = as_dsp_init_no_cfg(U_LAW_CODE, PSTN_STANDARD_JAPAN, 20);	/* u law, 20ms audio data is kept in dsp engine */
	assert(dsp);
	printf("\t\t\t\t\tEnd!\n");
	printf("\n\tNTT-CID : STEP 0 : Begin to calculating FSK data...\n");
	data = as_dsp_fsk_encode(dsp, phonenumber,NULL,  &length );
	printf("\n\t\tFSK buffer length is %d\n", length );
#if 0	
	if(( (length/AS_DRIVER_BUFFER_LENGTH)*AS_DRIVER_BUFFER_LENGTH ) != length)
		printf("\t\t\tERROR : buffer length is error\n");
#endif

	printf("\n\tNTT-CID : STEP 1 : Begin to send CAR Signal to phone...\n");
	as_car_on_fxs(fd );
	
	printf("\n\tNTT-CID : STEP 2 : Waiting Primary Answer Signal feedback from phone...\n");
	do
	{
		res = as_lib_event_get( fd);
		if(res == AS_EVENT_RINGOFFHOOK)
		{
			as_error_msg( "\t\tEvent 'OFF-HOOK' received\r\n" );
			break;
		}
	}while(1);
	
	usleep(2000*1000);  /* sleep at least 1/2 sec */
	printf("\n\tNTT-CID : STEP 3 : Send Caller ID(Modem Signal) to phone...\n");

	count = length /AS_DRIVER_BUFFER_LENGTH;
	for( i=0; i< count; i++ )
	{
		res = write(fd, data +i*AS_DRIVER_BUFFER_LENGTH, AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("file write failure, length is %d\n", res);
			exit(1);
		}
		total += res;
	}
	res = length - count*AS_DRIVER_BUFFER_LENGTH;
	if(res >0)
	{
		res = write(fd, data+i*AS_DRIVER_BUFFER_LENGTH, res);
		total += res;
	}	
	
	printf("\n\t\ttotal %d bytes write to file\n" ,total);
	printf("\n\tNTT-CID : STEP 4 : waiting Incoming Successful Signal feedback from phone...\n");
	do
	{
		res = as_lib_event_get( fd);
		if((res == AS_EVENT_ONHOOK ) )
		{
			as_error_msg( "\t\tEvent 'ON-HOOK' received\r\n" );
			break;
		}
			
	}while(1);

	printf("\n\tNTT-CID : STEP 5 : send Call Signal(IR)...\n");
	as_ring_on_hook(fd );

	printf("Modem caller ID ended, result is %d\r\n", res);

	as_dsp_destroy(dsp);
	close(fd);
	return 0;
}

