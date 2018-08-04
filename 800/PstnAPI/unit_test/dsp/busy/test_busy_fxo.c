/*
* $Id: test_busy_fxo.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
* $Author: lizhijie $
* $Log: test_busy_fxo.c,v $
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
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "as_thread.h"
#include "assist_lib.h"
#include "as_version.h"

/*  
 * BT detect test on FXO Port(FXO play as callee) 
*/

int main(int argc, char *argv[])
{
	int res;
	int fdo;
	unsigned char buf[AS_DRIVER_BUFFER_LENGTH];
	as_dsp_t *dsp;

	printf( AS_VERSION_INFO("DSP Software BT(Busy Tone) Detect on FXO Device Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: busy_fxo <astel FXO device>\n\tFXO play as Callee and check BT send by PBX\n");
		exit(1);
	}

	fdo = open(argv[1], O_RDWR);
	if (fdo < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	dsp = as_dsp_init_no_cfg(U_LAW_CODE,PSTN_STANDARD_JAPAN, 20);
	assert(dsp);

	printf("BT Detect : FXO device as callee\r\nBT Detect : STEP 1: reset FXO device\r\n");
	as_lib_onhook(fdo);

	printf("BT Detect : STEP 2: Waiting RINGOFF event.....\r\n");
	as_lib_wait_offhook(fdo);
	printf("\tBT Detect : FXO Device RX first Ring\r\n" );
	as_lib_wait_offhook(fdo);
	printf("\tBT Detect : FXO Device RX second Ring\r\n" );

	printf("BT Detect : STEP 3: set the FXO device into OFFHOOK status\r\n");
	as_lib_offhook( fdo);

	printf("BT Detect : STEP 4 : Please hang the phone making this call, then check result of BT detect\r\n" ); 
	while(1)
	{
		res = read(fdo,  buf,  AS_DRIVER_BUFFER_LENGTH);
		if(res != AS_DRIVER_BUFFER_LENGTH)
		{
			printf("BT Detect : BT(Busy Tone) Detect fail : this file is not a device file for PSTN firmware, %d bytes have been read\n", res);
			break;
		}

		if(as_dsp_busydetect(dsp, buf, AS_DRIVER_BUFFER_LENGTH) )
		{
			printf("BT Detect : BT(Busy Tone) Detected on %s, then on-hook FXO Device\n", argv[1] );
			as_lib_onhook(fdo);
			break;
		}
	};

	as_dsp_destroy( dsp);

	printf("BUSY_FXO: Test ended successfully!\n");  
	
	close(fdo);
	return 0;
}


