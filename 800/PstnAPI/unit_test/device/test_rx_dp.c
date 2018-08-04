/*
 * $Author: lizhijie $
 * $Log: test_rx_dp.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/04/30 06:15:39  wangwei
 * no message
 *
 * Revision 1.3  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.2  2006/03/11 10:18:34  lizhijie
 * dd
 *
 * Revision 1.1  2006/03/11 10:18:15  lizhijie
 * no message
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

int rx_dial_pulse_signal(int fd)
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
	int signal;
	
	printf( AS_VERSION_INFO("Recevice DP Test"));
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: rx_dp <FXS device>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	printf("INTENT: distinguish from phone of FXS sent DP10 signal\n");

	printf("RX_DP: Please off hook the phone connect to this FXS port firstly\n");
	as_lib_wait_offhook(fd);
	printf("RX_DP: Phone is off-hook\nRX_DP: Please dialing with DP(Dial Pulse) Phone\n");

	while(1)
	{
		if((signal =as_get_dp_signal(fd ) )> 0 )
		{
			if(signal>= 0)
				printf(" \treceive DIAL PULSE sigal : '%c'\n" , (unsigned char )signal );
			
			i++;
			if(i>20)
				break;
		}
	}

	printf("RX_DP: test succeed\n");
	
	close(fd);
	return 0;
}

