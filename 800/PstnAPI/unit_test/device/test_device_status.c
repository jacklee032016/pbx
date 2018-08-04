/*
 * $Log: test_device_status.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.4  2006/03/23 05:23:33  wangwei
 * no message
 *
 * Revision 1.3  2006/03/20 06:46:37  lizhijie
 * add and debug some. refer to ChangeLog
 *
 * Revision 1.2  2006/03/20 06:39:02  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: test_device_status.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>


#include "as_dev_ctl.h"
#include "assist_lib.h"
#include "as_version.h"
int main(int argc, char *argv[])
{
	int dev_num;
	struct as_channel_state *chan_state;
	int i;

	printf( AS_VERSION_INFO("Read Device status Test"));
	printf("INTENT: read SLIC/CODE status , include total, place, type\n");
	
	dev_num = as_get_device_num();
	printf("STATUS: SLIC/DAA total:%d\n", dev_num);

	for(i=0;i<dev_num;i++)
	{
		chan_state = as_get_dev_state( i);
		if(!chan_state )
		{
			printf("ERROR: Device status error, check your hardware and driver carefully\n");
			exit(1);
		}

		printf("Device No :%d\n", chan_state->channel_no);
		
		if(chan_state->type==AS_CHANNEL_TYPE_FXS)
			printf("Device type is FXS(SLIC)\n");
		else if(chan_state->type==AS_CHANNEL_TYPE_FXO)			
			printf("Device type is FXO(DAA)\n");
		
		if( chan_state->available==AS_TXSTATE_ONHOOK )
			printf("Device is ON-HOOK\n");
		else if( chan_state->available == AS_TXSTATE_OFFHOOK)
			printf("Device is OFF-HOOK\n");
		else
			printf("Device state is invalidate, check your hardware and driver carefully\n");	
		
		free(chan_state);

	}

	printf("STATUS: Test succeed\n");
	return 0;
}

