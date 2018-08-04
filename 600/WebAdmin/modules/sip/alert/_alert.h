/*
* $Id: _alert.h,v 1.4 2007/03/27 20:22:57 lizhijie Exp $
*/
#ifndef	___ALERT_H__
#define	___ALERT_H__

#include "aslib.h"
#include "cgi_lib.h"
#include "cgi_services.h"
#include "cgi_alert.h"
#include "sipUa.h"


#define	ALERT_ACTION_WAIT_OFFHOOK		"ActionOffHook"
#define	ALERT_ACTION_WORK_BEGIN			"ActionBeginWork"
#define	ALERT_ACTION_WORK_END			"ActionEndWork"

char *cgi_alert_stop_pbx_status(void );
char *cgi_alert_waiting_offhook_status(ALERT_INFO *info);
char *cgi_alert_record_begin_status(ALERT_INFO *info);
char *cgi_alert_record_end_status(ALERT_INFO *info);
char *cgi_alert_record_exit_status(ALERT_INFO *info);

char	*cgi_alert_get_alert_files(ALERT_INFO *info);
int	cgi_alert_file_management(ALERT_INFO *info);

int	cgi_alert_record(ALERT_INFO *info);
int	cgi_alert_play(ALERT_INFO *info);
int	cgi_alert_setup(ALERT_INFO *info);

#endif

