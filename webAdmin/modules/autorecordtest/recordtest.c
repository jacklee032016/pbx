/*
*/

#include "recordtest.h"

int main(int argc, char *argv[])
{

	int	res = 0;
	CMN_MESSAGE msg;
	CMN_COMMAND *cmd = &msg.cmd;
	cmd->op = CMN_CMD2SERVER_VIDEO_RECORD;	

	cgidebug("value is %s\r\n",argv[1]);
	cgitrace;

	sprintf((char *)cmd->value,"%s",argv[1]);
	
	cgidebug("value is %s\r\n",cmd->value);
	cgitrace;
	cgidebug("msg send to mntrd\r\n");
	cgitrace;
	
	cmn_msg_queue_init(TRUE);
	res = CGI_2_MSGQUEUE( cmd);
	
	cgidebug("msg send to mntrd\r\n");
	cgitrace;
	
	return 0;
} 


