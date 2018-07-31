/*
* $Id$
*/

#include "_video.h"

#define	CGI_2_MSGQUEUE( cmd) \
	cmn_cmd_apps_to_mntrd(cmd, CMN_MSG_TYPE_MNRD )



int	videoActionButton(VIDEO_INFO *info, int msgOpType)
{
//	char *action;
	int	res = 0;
	CMN_MESSAGE msg;
	CMN_COMMAND *cmd = &msg.cmd;
	
	cmd->op = msgOpType;

	if(cmd->op == CMN_CMD2SERVER_IMG_BRIGHTNESS)
	{
		cgidebug("Brightness Ctrl\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_CONTRAST)
	{
		cgidebug("Contrast Ctrl\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_SATURATION)
	{
		cgidebug("Saturation Ctrl\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_SHARPNESS)
	{
		cgidebug("Sharpness Ctrl\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_HUE)
	{
		cgidebug("Hue Ctrl\r\n");
	}
	
	else if(cmd->op == CMN_CMD2SERVER_VIDEO_QUALITY)
	{
		cgidebug("Quality\r\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_FLICKER)
	{
		cgidebug("Flicker\r\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_RESOLUTION)
	{
		cgidebug("Resolution\r\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_FRAMERATE)
	{
		cgidebug("Framerate\r\n");
	}
	else if(cmd->op == CMN_CMD2SERVER_IMG_HUE)
	{
		cgidebug("Hue\r\n");
	}
	else
	{
		CMN_MSG_LOG(CMN_LOG_DEBUG, "Unknow msg type '0x%f'", cmd->op);
	}

//	cmn_msg_queue_init(FALSE);
	cmn_msg_queue_init(TRUE);

	res = CGI_2_MSGQUEUE( cmd);
	cgidebug("msg send to mntrd\r\n");

/*	
	// client can not destroy msg queue
	cmn_msg_queue_destroy();
*/
	return 0;
}

