/*
* $Id: _hwCheck.h,v 1.2 2007/06/22 20:31:00 lizhijie Exp $
*/
#ifndef	___HW_CHECK_H___
#define	___HW_CHECK_H___

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <unistd.h>
#include "cgi_lib.h"
#include "cgi_llist.h"
#include "as_version.h"
#include "as_msg_queue.h"
#include "aslib.h"

#include "uart_cmd.h"

#include "cgi_pbxHwCheck.h"

typedef	struct
{
	llist			cgiVariables;
	llist			cfgs;

	int			extsCount;
}HWCHECK_INFO;

#define	PBX_EXT_INDEX		"BranchIndex"		/* extension phone index */


int	cgiPbxSendCheckCmd(HWCHECK_INFO *info, int type, int index, int index2);
int cgiPbxReadFromUart(void);

int	cgiPbxHwCheckPcm(HWCHECK_INFO *info);
int	cgiPbxHwCheckRinger(HWCHECK_INFO *info);
int	cgiPbxHwCheck(HWCHECK_INFO *info);
int	cgiPbxHwCheckRTC(HWCHECK_INFO *info);
int	cgiPbxHwCheckNvRam(HWCHECK_INFO *info);

int	cgiPbxHwCheckDtmf(HWCHECK_INFO *info);

int  pcm_test_one_step(int chan_no);

#endif

