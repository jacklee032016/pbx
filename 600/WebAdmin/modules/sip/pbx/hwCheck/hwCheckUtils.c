/*
* $Id: hwCheckUtils.c,v 1.3 2007/09/04 20:46:17 lizhijie Exp $
*/
#include "_hwCheck.h"

int	cgiPbxSendCheckCmd(HWCHECK_INFO *info, int type, int index, int index2)
{
	PBX_COMMAND cmd;


	cmd.op = CMD_SELFTEST_REQUEST;
	cmd.value[0] = type;
	cmd.value[1] = index;
	cmd.value[2] = index2;
	cmd.value[3] = PBX_VALUE_INVALIDATE;
	as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_SELFTEST);

	return 0;
}

int cgiPbxReadFromUart(void)
{
	PBX_COMMAND *cmdbuf;
	int	count = 0;

	char		statusMsg[1024];
	int		length = 0;

	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", "Read Odd msg left on queue" );
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);
	cmdbuf = as_cmd_selftest_from_uart();
	while(cmdbuf)
	{
		count++;
		length += CGI_SPRINTF(statusMsg, length, "<p>%d %s.....",count, "MSG" );
		CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

		free(cmdbuf);
		cmdbuf = NULL;
		cmdbuf = as_cmd_selftest_from_uart();
	}
	
	return 0;
}

int  pcm_test_one_step(int chan_no)
{
	int fd;
	char dev_name[128];
	static char count = 0;
	unsigned char ch;
	unsigned char dtmfDigit[32];

	sprintf(dev_name, "/dev/astel/%d", (chan_no) );
	fd = open(dev_name, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s", dev_name, strerror(errno));
		return -1;
	}
	
	count = (count+1)%10;	
	ch = 0x48 + count;
	sprintf(dtmfDigit, "%c", ch);
	as_lib_dial( fd , "12", 100 );
	
	close(fd);

	return 0;
}

