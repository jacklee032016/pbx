/*
 * $Author: lizhijie $
 * $Log: test_rx_dtmf_4_fxs.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.3  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.2  2006/03/21 02:13:42  lizhijie
 * add functions called by vpbx
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Revision: 1.1.1.1 $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_version.h"

int rx_dtmf_4_fxs(int fd)
{
	int signal;
	if (ioctl(fd, AS_CTL_GET_DTMF_DETECT, &signal) == -1) 
	{
		printf("IOCTL error\r\n");
	}
	else
	{
//#if AS_EVENT_DEBUG			
		printf("DTMF signal is '%c' \r\n", (unsigned char)signal);
//#endif
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	int i = 0;
	
	printf( AS_VERSION_INFO("DTMF Caller ID Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: rx_dtmf <FXS device>\n Press <Ctrl>+C to exit\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: distinguish from phone of FXS sent DTMF signal\n");

	printf("RX_DTMF: Off Hook the phone connect to this FXS port, and press any push button.\nRX_DTMF: Then you can you the phone number just pressed\nRX_DTMF: Program will quit after you have press 20 times\n");

	while(1)
	{
		if(as_dtmf_is_ready(fd ) )
		{
			as_get_dtmf_4_fxs( fd);
			i++;
			if(i>20)
				break;
		}
	}

	printf("RX_DTMF: test succeed\n");
	
	close(fd);
	return 0;
}

