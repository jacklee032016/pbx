/*
* $Id: cgiAlertMain.c,v 1.4 2007/03/27 20:22:57 lizhijie Exp $
*/

#include "_alert.h"

static ALERT_INFO			info;

static int __alert_init(ALERT_INFO *_info)
{
	pid_t 	pid;
	cgi_html_header();
	list_create(&_info->cgiVariables);
	read_cgi_input(&_info->cgiVariables, NULL, NULL);

	if(strcasecmp(GET_CGI_OP(&_info->cgiVariables), ALERT_OP_ACTIVATE ))
	{
		pid = cgi_get_program_pid_with_param(SYS_PROC_PBX, NULL);
		if(pid > 0)
		{
			cgi_info_page( gettext(CGI_STR_WARN), gettext(CGI_STR_ALERT_VOICE), cgi_alert_stop_pbx_status() );
			return -1;
		}
	}
	
	list_create(&_info->cfgsRecord);
	if (readconfig(RECORD_CONFIG_FILE, ITEM_TRI, NULL, &_info->cfgsRecord) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&_info->cfgsUart);
	if (readconfig(PXB_UARTD_CONF, ITEM_DBL, NULL, &_info->cfgsUart) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	as_msg_queue_init( 0);

	return 0;
}

int main(int argc, char *argv[])
{
	char 		*cmd=NULL;
	ALERT_INFO	*_info = &info;
	
	if(__alert_init(_info) )
	{
		return 1;
	}

	cmd = GET_CGI_OP( &_info->cgiVariables );
	if( !cmd || !strlen(cmd) )
	{
		return cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext("Alert Voice Management"), cgi_alert_get_alert_files(_info));
	}
	else if(!strcasecmp(cmd, ALERT_OP_RECORD ) )
	{
		_info->isRecord = RECORD_ACTION_RECORD;
		return cgi_alert_record(_info);
	}
	else	 if(!strcasecmp(cmd, ALERT_OP_PLAY) )
	{
		_info->isRecord = RECORD_ACTION_PLAY;
		return cgi_alert_play(_info);
	}
	else	 if(!strcasecmp(cmd, ALERT_OP_PARAMETER) )
	{
		return cgi_alert_setup(_info);
	}
	else	 if(!strcasecmp(cmd, ALERT_OP_DELETE) || !strcasecmp(cmd, ALERT_OP_ACTIVATE)  )
	{
		return cgi_alert_file_management(_info);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


