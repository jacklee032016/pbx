/* 
 * $Id: as_test_pcm_channels.c,v 1.6 2007/06/21 06:57:35 lizhijie Exp $
 * Test program for all PCM channels (Time Slot) between IXP422 and PBX 
 * as_test_pcm_channel.c : 
 * Li Zhijie, 2005.05.23
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "aslib.h"
#include "as_test.h"

typedef struct
{
	int chan_no;

	int success;
	int failed;
}pcm_chan_status;

typedef struct
{
	int chan_num;
	pcm_chan_status status[8];
}pcm_chan_test_result;

pcm_chan_test_result result = 
{
	chan_num	:	8
};

static int __pcm_wait_pbx_response( unsigned char lastDigit)
{
	PBX_COMMAND *response;
	int chan_no;
	unsigned char dtmfDigit;

	AS_MSG_LOG(AS_LOG_INFO, "After SELFTEST request, waiting reply from PBX.....");
	response = as_cmd_selftest_from_uart();
	if(NULL == response)
	{
		AS_MSG_LOG(AS_LOG_ERR, "No response are received");
		return -AS_TEST_PBX_NO_RESPONSE;
	}
	
	if(response->op == PCM_PBX_BUSY)
	{
		AS_MSG_LOG(AS_LOG_ERR,"PBX is busy, selftest can not be continue");
		free(response);
		return -AS_TEST_PBX_BUSY_HERE;
	}
	
	if(response->op != PCM_CHAN_TEST )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Not validate command for test : op is %x", response->op );
		free(response);
		return -AS_TEST_PBX_SOFTWARE_ERROR;
	}
	chan_no = response->value[0];
		
	if(chan_no<0 || (chan_no >= result.chan_num && chan_no != 0X55 ) )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Channel %d is not exist, ought to be 0~%d", chan_no, result.chan_num );
		free(response);
		return -AS_TEST_PBX_SOFTWARE_ERROR;
	}

	dtmfDigit = response->value[1];
	if( dtmfDigit != lastDigit )
	{/* check this command is the first command send by PBX */
		AS_MSG_LOG(AS_LOG_ERR, "Error DTMF digit is received in PBX : received 0x%x(ought to be 0x%x)", dtmfDigit, lastDigit);
	}
	free(response);

	return chan_no;
}

/* sucess : return the channel_no need to test */
static int __pcm_test_init(void *data)
{
	int res;
	PBX_COMMAND cmd;
	as_msg_queue_init( 0);

	cmd.op = PCM_SELFTEST_REQUEST;
	cmd.id = 1;
	cmd.value[0] = 0xFF;

	res = as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SELFTEST);

	/* when the first command return by PBX, the dtmf digit is 0x55 */
	return __pcm_wait_pbx_response(0x55);
}

static void __pcm_test_destory(void *data)
{
	AS_MSG_LOG(AS_LOG_INFO, "SelfTest Ended");		
}

/* return the next channel need to test, or 0x55 for end of PCM test */
static int __pcm_test_one_step(int chan_no)
{
	int fd;
	char dev_name[128];
	static char count = 0;
	unsigned char ch;
	unsigned char dtmfDigit[32];

	sprintf(dev_name, "/dev/astel/%d", chan_no );
	fd = open(dev_name, O_RDWR);
	if (fd < 0) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Unable to open %s: %s", dev_name, strerror(errno));
		return -AS_TEST_PCM_DRIVER_NOT_INITTED;
	}
	
	count = (count+1)%10;	
	ch = 0x30 + count;
	sprintf(dtmfDigit, "%c", ch);
	as_lib_dial( fd , dtmfDigit, 100 );
	
	close(fd);

	return __pcm_wait_pbx_response( ch );
}

/* PCM channels between PBX and IXP is test */
int as_test_pcm_channels( void *data)
{
	int chan_no;
	
	chan_no = __pcm_test_init( &result );
	if(chan_no < 0 || chan_no == 0x55)
	{
		__pcm_test_destory( &result );
		return -1;
	}
	
	while( chan_no >= 0 && chan_no != 0x55 )
	{
		chan_no = __pcm_test_one_step(chan_no);
	};

	__pcm_test_destory( &result);
	return chan_no;
}

