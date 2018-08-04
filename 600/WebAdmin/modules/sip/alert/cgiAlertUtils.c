/*
* $Id: cgiAlertUtils.c,v 1.6 2007/05/31 20:38:06 lizhijie Exp $
*/
#include <stdlib.h>
#include "_alert.h"
#include "cgi_sysinfo.h"

char *cgi_alert_stop_pbx_status(void )
{
	char 	buf[8192];
	int 		length = 0;
	char		target[128];

//	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
/* table 5 : Notes */
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>%s, <strong>'%s</strong>' %s. <strong>'%s'</strong> %s. \r\n</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), gettext("When use Alert Voice Function"), 
		gettext(CGI_STR_SIP_MY_UA), gettext("must been not running"),
		gettext(CGI_STR_UARTD) , gettext("must been running") );
	
	sprintf(target, "%s",WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_SYS_PROC);
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n",cgi_button(gettext(CGI_STR_MODIFY), target) );

	return strdup(buf);
}

char *cgi_alert_waiting_offhook_status(ALERT_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	char		targetBegin[128],targetEnd[128];
	int		timeout;
	char 	*op = GET_CGI_OP( &info->cgiVariables);

	timeout = atoi(GET_VALUE(&info->cfgsRecord, RECORD_WAIT_TIME) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
/* table 5 : Notes */
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><br>%s. %s %d %s, %s. %s. \r\n\r\n",
		gettext("Alert Voice Function has been started"), 
		gettext("Within"), timeout, 
		gettext(CGI_STR_SECONDS) ,gettext("Please Off-Hook a telephone"), 
		gettext("Otherwise you must restart the Alert Voice Function") );

	if(info->isRecord == RECORD_ACTION_RECORD)
	{
		length += CGI_SPRINTF(buf,length, "<br>%s.%s\r\n",
			gettext("You can click the 'Begin' button startup record after off-hook"),
			gettext("In order to work correctly, please delay a little after off-hook"));
	}
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n" );

	
	sprintf(targetBegin, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=", op, "&"CGI_ACTION"="ALERT_ACTION_WORK_BEGIN);
	sprintf(targetEnd, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=", op, "&"CGI_ACTION"="ALERT_ACTION_WORK_END);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">\r\n" );
	if(info->isRecord == RECORD_ACTION_RECORD)
	{
		length += CGI_SPRINTF(buf,length, "%s\r\n",cgi_button(gettext(CGI_STR_BEGIN), targetBegin) );
	}
	length += CGI_SPRINTF(buf,length, "%s\r\n",cgi_button(gettext(CGI_STR_EXIT), targetEnd) );

	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n" );

	sprintf(targetBegin, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=", op, "&"CGI_ACTION"="ALERT_ACTION_WAIT_OFFHOOK);
	length += CGI_SPRINTF(buf,length, "%s\r\n", cgi_refresh_script(targetBegin, timeout));

	return strdup(buf);
}


char *cgi_alert_record_begin_status(ALERT_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	char		targetBegin[128], targetEnd[128];
	char		*op = GET_CGI_OP(&info->cgiVariables);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");

	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	if(info->isRecord == RECORD_ACTION_RECORD)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>%s, %s!  %s!\r\n</TD></TR>\r\n",
			gettext(CGI_STR_NOTE), gettext("One Telephone has been Off-Hook"), 
			gettext("Click following Button to begin or exit record"),
			gettext("When Beep is heared, please speak") );
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>* : </strong>%s, %s.<br><strong>* : </strong>%s.\r\n</TD></TR>\r\n",
			gettext("One Telephone has been Off-Hook"), 
			gettext("Click 'Exit' Button to exit play at once"),
			gettext("After play over, 'Exit' must be clicked in order to work correctly"));
	}
	
	sprintf(targetBegin, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=",op, "&"CGI_ACTION"="ALERT_ACTION_WORK_BEGIN);
	sprintf(targetEnd, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=",op, "&"CGI_ACTION"="ALERT_ACTION_WORK_END);

	if(info->isRecord == RECORD_ACTION_RECORD)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s %s</TD></TR>\r\n",cgi_button(gettext(CGI_STR_RECORD), targetBegin), cgi_button(gettext(CGI_STR_EXIT), targetEnd ));
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n", cgi_button(gettext(CGI_STR_EXIT), targetEnd ));
	}
	
	return strdup(buf);
}

char *cgi_alert_record_end_status(ALERT_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	char		targetEnd[128];
	char		*op = GET_CGI_OP(&info->cgiVariables);
	int		timeLength = atoi(GET_VALUE(&info->cfgsRecord, RECORD_TIME_LENGTH) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>* : </strong><br>%s, %s %d %s. %s!\r\n</TD></TR>\r\n",
		gettext("When you click following Button to end recording"), 
		gettext("or recording will been ended in"), timeLength, gettext(CGI_STR_SECONDS),
		gettext("no matter how, if you not click it, the PBX may not work correctly") );
	
	sprintf(targetEnd, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=",op, "&"CGI_ACTION"="ALERT_ACTION_WORK_END);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n", cgi_button(gettext(CGI_STR_EXIT), targetEnd ));

	length += CGI_SPRINTF(buf,length, "%s\r\n", cgi_refresh_script(targetEnd, timeLength));
	
	return strdup(buf);
}

char *cgi_alert_record_exit_status(ALERT_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	char		targetEnd[128];
	char		*op = GET_CGI_OP(&info->cgiVariables);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>%s, %s!\r\n</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), 
		gettext("Alert Voice Record is running now. Please try later"),
		gettext("Or click the following button to exit it"));
	
	sprintf(targetEnd, "%s%s%s",WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"=",op, "&"CGI_ACTION"="ALERT_ACTION_WORK_END);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n", cgi_button(gettext(CGI_STR_EXIT), targetEnd ));

	return strdup(buf);
}


