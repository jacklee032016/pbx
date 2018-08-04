/*
* $Id: cgiAlertSetup.c,v 1.3 2007/03/27 20:22:57 lizhijie Exp $
*/

#include "_alert.h"

char	*cgi_alert_get_alert_files(ALERT_INFO *info)
{
	llist		files;
	node	*current;
	int		res = 0;
	char 	buf[8192*5];
	int 		length = 0;
	char		targetPlay[256], targetDelete[256], targetActivate[256];
	llist		sipCfg;
	char		*currentGreetingFile;
	
	list_create(&sipCfg);
	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &sipCfg) != 0)
	{
		return gettext(CGI_STR_ERROR);
	}
		
	currentGreetingFile = GET_VALUE(&sipCfg, SIPUA_GREETING_FILE);

	list_create(&files);
	res = cgi_get_files_in_dir(GET_VALUE(&info->cfgsRecord, RECORD_FILE_HOME), &files );
	if(res > 0)
	{
		length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s(%s)</strong></TD><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
			gettext("File Name"), gettext("File Length"),gettext(CGI_STR_MSECONDS), gettext(CGI_STR_MANAGEMENT) );

		current = files.head;
		while (current != 0) 
		{
			int	isNotDefault = strcasecmp(current->entry.name, SIPUA_DEFAULT_GREETING_FILE);
			int	isNotUsed = strcasecmp(current->entry.name, currentGreetingFile);
			
			sprintf(targetPlay, 		"%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_PLAY"&"ALERT_ACTION_FILE"=", current->entry.name);
			sprintf(targetDelete, 	"%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_DELETE"&"ALERT_ACTION_FILE"=", current->entry.name);
			sprintf(targetActivate, 	"%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_ACTIVATE"&"ALERT_ACTION_FILE"=", current->entry.name);
			length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\">%s</TD><TD align=\"center\">%d</TD><TD align=\"center\">%s %s %s</TD></TR>", 
				current->entry.name, atoi(current->entry.value)/8, 
				cgi_button(gettext(CGI_STR_PLAY), targetPlay),
				(isNotUsed)?cgi_button(gettext(CGI_STR_ACTIVATE), targetActivate):"",
				(isNotDefault && isNotUsed)?cgi_button(gettext(CGI_STR_DELETE), targetDelete):"" );
			current = current->next;
		}

//		length += CGI_SPRINTF(buf,length, "</table>\n");

	}

	list_clear( &files);
	list_clear(&sipCfg);

	return strdup(buf);
	
}

static char *__cgi_alert_config_status(ALERT_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_ALERT );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, ALERT_OP_PARAMETER);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE);

/* table1 : Recording Parameters */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Recording Parameters") );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("File Name"), RECORD_FILE_NAME, GET_VALUE(&info->cfgsRecord, RECORD_FILE_NAME) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("File Length"), RECORD_TIME_LENGTH, GET_VALUE(&info->cfgsRecord,RECORD_TIME_LENGTH), gettext(CGI_STR_SECONDS) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("OffHook Timeout"), RECORD_WAIT_TIME, GET_VALUE(&info->cfgsRecord,RECORD_WAIT_TIME), gettext(CGI_STR_SECONDS) );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* TABLE 2 : Button info */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t</TD></TR></FORM>\r\n" );

/* table 3 : Notes */
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* %s : </strong>%s. %s\r\n",
		gettext(CGI_STR_NOTE), gettext("File Name"), 
		gettext("define a new file name, then a new file is created after recording"), 
		gettext("Otherwise override the existed file with same name") );
	length += CGI_SPRINTF(buf,length, "<br><strong>* %s : </strong>%s. </TD></TR>\r\n",
		gettext("File Length"),
		gettext("One second used about 8 KB in Size") );

	return strdup(buf);

}


int	cgi_alert_setup(ALERT_INFO *info)
{
	char		*action;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext(CGI_STR_ALERT_VOICE), gettext("Recording Parameters Configuration"), __cgi_alert_config_status(info) );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_SAVE)  )
	{
		char *target = WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_PARAMETER;

		list_replace_with_new_list(&info->cfgsRecord, &info->cgiVariables );
		if(writeconfig(RECORD_CONFIG_FILE, ITEM_TRI, NULL, &info->cfgsRecord) != 0)
		{
			cgi_info_page(gettext(CGI_STR_WARN), "",  "write config file failed");
			return -1;
		}
		return cgi_refresh_page(2, target, gettext(CGI_STR_WAIT), gettext("Write Configuration"));
	}

	return 0;
}


