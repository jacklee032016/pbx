/*
* $Id: sipUa_record.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"

#define	RECORD_DEVICE_TYPE				"Device_Type"
#define	RECORD_DEVICE_NUMBER				"Device_Fd"
#define	RECORD_FILE_PATH					"File_Path"
#define	RECORD_TIME_LENGTH				"Time_Length"
#define	RECORD_START_TIPTONE				"Start_tipTone"
#define	RECORD_END_TIPTONE				"End_tipTone"
#define	RECORD_ACTION_TYPE				"Action_Type"
#define	RECORD_WAIT_TIME					"Wait_Time"

#define	RECORD_ACTION_TYPE_RECORD		"play"
#define	RECORD_ACTION_TYPE_PLAY			"recorder"

static llist  	recordCfgs;

#define	PXB_RECORD_CONF					"/etc/taper.cfg"


static char *__sipUa_record_status()
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_VOICE);

	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_DEVICE_TYPE, GET_VALUE(&recordCfgs,RECORD_DEVICE_TYPE ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_DEVICE_NUMBER, GET_VALUE(&recordCfgs,RECORD_DEVICE_NUMBER ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_FILE_PATH, GET_VALUE(&recordCfgs,RECORD_FILE_PATH ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_TIME_LENGTH, GET_VALUE(&recordCfgs,RECORD_TIME_LENGTH ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_START_TIPTONE, GET_VALUE(&recordCfgs,RECORD_START_TIPTONE ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_END_TIPTONE, GET_VALUE(&recordCfgs,RECORD_END_TIPTONE ));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		RECORD_WAIT_TIME, GET_VALUE(&recordCfgs,RECORD_WAIT_TIME ));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\r\n", gettext("Action") );

	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
//		CGI_ACTION );
		RECORD_ACTION_TYPE );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
		RECORD_ACTION_TYPE_RECORD,  gettext(CGI_STR_RECORD));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
		RECORD_ACTION_TYPE_PLAY,  gettext(CGI_STR_PLAY));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n",cgi_submit_button( gettext("Action") ) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n" , cgi_help_button(CGI_HELP_MSG_RECORD) );

	return strdup(buf);

}


int	sipUa_record(llist *cgiVariables)
{
	pid_t 	pid;
	char		*action;

	list_create(&recordCfgs);

	if (readconfig(PXB_RECORD_CONF, ITEM_TRI, NULL, &recordCfgs) != 0)
	{
		return -1;
	}

	pid=(pid_t)cgi_get_pid_by_name("recorder");
	if(pid>0)
	{
		char		msg[256];
		sprintf(msg,"%s,%s", gettext("Record Program is running now"), gettext("Please waiting"));
		cgi_error_page(NULL, gettext(CGI_STR_WARN), msg );
//		system("killall recorder");
		return -1;
	}

	action = GET_VALUE(cgiVariables, RECORD_ACTION_TYPE );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Alert Voice Record"), gettext("Please select action which you want to"), __sipUa_record_status() );
		return 0;
	}
	else if(!strcasecmp(action, RECORD_ACTION_TYPE_PLAY) || !strcasecmp(action, RECORD_ACTION_TYPE_RECORD) )
	{
		int timeout = atoi(GET_VALUE(&recordCfgs, RECORD_TIME_LENGTH) );
		char		target[256];
		sprintf(target, "%s?%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_VOICE );
		cgidebug("refresh time=%d\n", timeout);
		cgi_refresh_page(timeout, target, gettext(GET_VALUE(&recordCfgs, RECORD_ACTION_TYPE)), gettext(CGI_STR_PLEASE_WAIT) );

		system("/usr/bin/recorder /etc/pbx/record.conf >> /dev/null &");
	}

	return 0;
}

