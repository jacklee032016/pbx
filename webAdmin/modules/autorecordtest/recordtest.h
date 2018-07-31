/*
*/

#ifndef __RECORDTEST_H__
#define __RECORDTEST_H__

#include "cgiLib.h"
#include "libCmn.h"
#include "cmn_msg_queue.h"
#include "cgi_video.h"

#define	CGI_2_MSGQUEUE( cmd) \
	cmn_cmd_apps_to_mntrd(cmd, CMN_MSG_TYPE_MNRD )
	
#endif


