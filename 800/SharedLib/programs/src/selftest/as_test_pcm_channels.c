/* 
 * $Log: as_test_pcm_channels.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.5  2005/06/24 07:30:33  lijie
 * *** empty log message ***
 *
 * Revision 1.4  2005/06/21 02:01:41  lijie
 * *** empty log message ***
 *
 * Revision 1.3  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.2  2005/06/03 04:01:19  lizhijie
 * add defination for NPE test
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_test_pcm_channels.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
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

#define  PCM_SELFTEST_REQUEST		0x18

#define  PCM_CHAN_TEST			0x98
#define  PCM_PBX_BUSY			0x99


static int __pcm_wait_pbx_response( unsigned char lastDigit)
{
	PBX_COMMAND *response;
	int chan_no;
	unsigned char dtmfDigit;

	response = as_cmd_from_uart();
	if(NULL == response)
	{
		fprintf(stderr, "No response are received\r\n");
		return -AS_TEST_PBX_NO_RESPONSE;
	}
	if(response->op == PCM_PBX_BUSY)
	{
		fprintf(stderr, "PBX is busy, selftest can not be continue\r\n");
		free(response);
		return -AS_TEST_PBX_BUSY_HERE;
	}
	
	if(response->op != PCM_CHAN_TEST )
	{
		fprintf(stderr, "Not validate command for test : op is %x\r\n", response->op );
		free(response);
		return -AS_TEST_PBX_SOFTWARE_ERROR;
	}
	chan_no = response->value[0];
		
	if(chan_no<0 || (chan_no >= result.chan_num && chan_no != 0X55 ) )
	{
		fprintf(stderr, "Channel %d is not exist, ought to be 0~%d\n", chan_no, result.chan_num );
		free(response);
		return AS_TEST_PBX_SOFTWARE_ERROR;
	}

	dtmfDigit = response->value[1];
	if( dtmfDigit != lastDigit )
	{/* check this command is the first command send by PBX */
		fprintf(stderr, "Error DTMF digit is received in PBX : received 0x%x(ought to be 0x%x)\r\n", dtmfDigit, lastDigit);
	}
	free(response);

	return chan_no;
}


/* sucess : return the channel_no need to test */
static int __pcm_test_init(void *data)
{
	int res;
	PBX_COMMAND cmd;

	/* timeout for 5 seconds */
	res = as_cmd_uart_init(3, 0);
	if(res < 0 )
	{
		fprintf(stderr, "UART port open failed\r\n");
		return -AS_TEST_UART_NOT_FOUND;
	}

	cmd.op = PCM_SELFTEST_REQUEST;
	cmd.id = 1;
	cmd.value[0] = 0xFF;
	
	as_cmd_to_uart( &cmd);

	/* when the first command return by PBX, the dtmf digit is 0x55 */
	return __pcm_wait_pbx_response(0x55);
}

static void __pcm_test_destory(void *data)
{
	as_cmd_uart_destroy();
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
		fprintf(stderr, "Unable to open %s: %s\n", dev_name, strerror(errno));
		return -AS_TEST_PCM_DRIVER_NOT_INITTED;
	}
	count = (count+1)%10;	
	ch = 0x48 + count;
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
	if(chan_no <0 || chan_no==0x55)
	{
		__pcm_test_destory( &result );
		return chan_no;
	}
	
	while( chan_no>= 0 || chan_no != 0x55 )
	{
		chan_no = __pcm_test_one_step(chan_no);
	};

	__pcm_test_destory( &result);
	return chan_no;
}
