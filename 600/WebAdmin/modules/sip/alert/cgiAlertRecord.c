/*
* $Id: cgiAlertRecord.c,v 1.4 2007/03/27 20:22:57 lizhijie Exp $
*/

#include "_alert.h"

int	_cgi_alert_check_alert_running(ALERT_INFO *info)
{
	pid_t 	pid;

	pid = cgi_get_program_pid_with_param(SYS_PROC_RECORD, NULL);
	if(pid > 0)
	{
		cgi_info_page( gettext(CGI_STR_WARN), "", cgi_alert_record_exit_status(info) );
		return -1;
	}
	
	list_replace_value_by_key( &info->cfgsRecord, RECORD_DEVICE_NUMBER, "-1" );
	if(writeconfig(RECORD_CONFIG_FILE, ITEM_TRI, NULL, &info->cfgsRecord) != 0)
	{
		cgi_info_page(gettext(CGI_STR_WARN), "",  "write config file failed");
		return -1;
	}
	return 0;
}

int	cgi_alert_record(ALERT_INFO *info)
{
	char		*action;

	int	chan_no = atoi(GET_VALUE(&info->cfgsRecord, RECORD_DEVICE_NUMBER) );
	action = GET_CGI_ACTION( &info->cgiVariables);
	if(!action || !strlen(action) )
	{
		if(_cgi_alert_check_alert_running(info)<0)
			return -1;
		system("/usr/bin/alert >> /dev/null &");
		cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_RECORD), cgi_alert_waiting_offhook_status(info) );
		return 0;
	}
	else if(!strcasecmp(action, ALERT_ACTION_WAIT_OFFHOOK)  )
	{
		if( chan_no < 0 || chan_no > 8 )
		{
			return cgi_error_page(NULL, gettext(CGI_STR_WARN), gettext("No Telephone is Off-Hooked, Please retry!"));
		}
		else
		{
			return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_RECORD), cgi_alert_record_begin_status(info));
		}
	}
	else if(!strcasecmp(action, ALERT_ACTION_WORK_BEGIN)  )
	{
		if( chan_no < 0 || chan_no > 8 )
		{
			return cgi_error_page(NULL, gettext(CGI_STR_WARN), gettext("No Telephone is Off-Hooked, Please retry!"));
		}
		else
		{
			RECORD_EVENT_WORKS_BEGIN();
			return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_RECORD), cgi_alert_record_end_status(info));
		}
	}
	else if(!strcasecmp(action, ALERT_ACTION_WORK_END)  )
	{
		RECORD_EVENT_WORKS_END();
		return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_RECORD), gettext(CGI_STR_THANKS));
	}
	else
	{
		cgi_invalidate_page();
	}
	
	return 0;
}


int	cgi_alert_play(ALERT_INFO *info)
{
	char		*action;

	action = GET_CGI_ACTION( &info->cgiVariables);
	if(!action || !strlen(action) )
	{
		char	cmd[256];
		if(_cgi_alert_check_alert_running(info)<0)
		{
			return -1;
		}	
		sprintf(cmd, "/usr/bin/alert -p -f %s >> /dev/null &", GET_VALUE(&info->cgiVariables, ALERT_ACTION_FILE) );
		system(cmd);
		cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_PLAY), cgi_alert_waiting_offhook_status(info) );
		return 0;
	}
	else if(!strcasecmp(action, ALERT_ACTION_WAIT_OFFHOOK)  )
	{
		int	chan_no = atoi(GET_VALUE(&info->cfgsRecord, RECORD_DEVICE_NUMBER) );
		if( chan_no < 0 || chan_no > 8 )
		{
			return cgi_error_page(NULL, gettext(CGI_STR_WARN), gettext("No Telephone is Off-Hooked, Please retry!"));
		}
		else
		{
			return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_PLAY), cgi_alert_record_begin_status(info));
		}
	}
	else if(!strcasecmp(action, ALERT_ACTION_WORK_BEGIN)  )
	{
		RECORD_EVENT_WORKS_BEGIN();
		return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_PLAY), cgi_alert_record_end_status(info));
	}
	else if(!strcasecmp(action, ALERT_ACTION_WORK_END)  )
	{
		RECORD_EVENT_WORKS_END();
		return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext(CGI_STR_PLAY), gettext(CGI_STR_THANKS));
	}
	else
	{
		cgi_invalidate_page();
	}
	
	return 0;
}


int	cgi_alert_file_management(ALERT_INFO *info)
{
	
	char		*op = GET_CGI_OP(&info->cgiVariables);
	if( !strcasecmp(op, ALERT_OP_DELETE) )
	{/* delete*/
		char		fileName[256];
		sprintf(fileName, "rm -rf %s%s", GET_VALUE(&info->cfgsRecord, RECORD_FILE_HOME), GET_VALUE(&info->cgiVariables, ALERT_ACTION_FILE));
		system(fileName);
	}
	else
	{/* activate greeting of SIP UA */
		llist	sipCfg;
		list_create(&sipCfg);

		if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &sipCfg) != 0)
		{
			return -1;
		}
		
		list_replace_value_by_key(&sipCfg, SIPUA_GREETING_FILE, GET_VALUE(&info->cgiVariables, ALERT_ACTION_FILE) );
		writeconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &sipCfg);

		list_clear(&sipCfg);
	}

	cgi_refresh_page(2, WEB_URL_ALERT, gettext(CGI_STR_WAIT), gettext("Update Alert Voice") );
	
	return 0;
}

