/*
* $Id: sysInfoLocale.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>

#include "cgiLib.h"

#include "cgi_sysinfo.h"

#define	WEB_KEYWORD_LOCALE				"ADMIN_LOCALE"

#define	WEB_KEYWORD_LOCALE_CHINA		"zh_CN.GB2312"
#define	WEB_KEYWORD_LOCALE_JAPAN		"JA"
#define	WEB_KEYWORD_LOCALE_ENGLISH		"C"				/* English Locale */

static llist  	languageCfgs;

static char *__language_status_list( )
{
	char buf[4096];
	int 	length = 0;
	char	*currentLocale;
	char	localeStr[128];
	
	length += CGI_SPRINTF(buf,length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></tr>\n", 
		gettext("Current Lanaguage") );

	currentLocale = GET_VALUE(&languageCfgs, WEB_KEYWORD_LOCALE);
	if(!strcasecmp(WEB_KEYWORD_LOCALE_CHINA, currentLocale) )
		sprintf(localeStr, "%s", gettext(CGI_STR_CHINESE) );
	else if(!strcasecmp(WEB_KEYWORD_LOCALE_JAPAN, currentLocale) )
		sprintf(localeStr, "%s", gettext(CGI_STR_JAPANESE) );
	else
		sprintf(localeStr, "%s", gettext(CGI_STR_ENGLISH) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" >%s</td></TR>\n", localeStr );

	return strdup(buf);
}

#define	FORM_NAME		"localeForm"

static char *__language_add_buttons()
{
	char 	buf[4096];
	int 		length = 0;
	char		msg[256];

	length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_SYSTEM_INFO);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_SYSINFO_OP_LOCALE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n",
		gettext("Please Select the Language used") );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\">\n\t<select name=\"%s\"  maxlength=\"30\">\n", WEB_KEYWORD_LOCALE );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", WEB_KEYWORD_LOCALE_CHINA, 
		(!strcasecmp(WEB_KEYWORD_LOCALE_CHINA, GET_VALUE(&languageCfgs, WEB_KEYWORD_LOCALE)))?"selected":"" , 
		gettext(CGI_STR_CHINESE));
	length += CGI_SPRINTF(buf, length, "\t\t<option value=\"%s\" %s>%s</option>\n", WEB_KEYWORD_LOCALE_JAPAN, 
		(!strcasecmp(WEB_KEYWORD_LOCALE_JAPAN, GET_VALUE(&languageCfgs, WEB_KEYWORD_LOCALE)))?"selected":"" , 
		gettext(CGI_STR_JAPANESE));
	length += CGI_SPRINTF(buf, length, "\t\t<option value=\"%s\" %s>%s</option>\n", WEB_KEYWORD_LOCALE_ENGLISH, 
		(!strcasecmp(WEB_KEYWORD_LOCALE_ENGLISH, GET_VALUE(&languageCfgs, WEB_KEYWORD_LOCALE)))?"selected":"" , 
		gettext(CGI_STR_ENGLISH));
	length += CGI_SPRINTF(buf,length, "</select></TD></TR>\r\n");

	sprintf(msg, "%s, %s", gettext("In order to activate the configuration"), gettext("please reboot") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">\r\n");
	length += CGI_SPRINTF(buf,length, cgi_submit_button_with_alert( gettext(CGI_STR_SAVE) , msg, FORM_NAME) );
//	length += CGI_SPRINTF(buf,length, "</TD>\r\n");

//	length += CGI_SPRINTF(buf,length, "<TD  align=\"center\">\r\n");
	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_LOCALE ) );

	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n");

	return strdup(buf);
}


static int __language_init()
{
	if (readconfig( SYS_CFG_LOCALE, ITEM_EQU, NULL, &languageCfgs) != 0)
	{
		cgi_error_page(NULL, gettext("Locale Operation Failed"), gettext( "No Configuration File for Locale" ) );
		return -1;
	}
	
	return 0;
}

int __language_modify_config( SYS_INFO *info)
{
	char *locale = GET_VALUE(&info->cgiVariables, WEB_KEYWORD_LOCALE);

	list_replace_value_by_key(&languageCfgs, WEB_KEYWORD_LOCALE, locale);

	writeconfig(SYS_CFG_LOCALE, ITEM_EQU, NULL, &languageCfgs);

	cgi_refresh_page(1, WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_LOCALE, gettext(CGI_STR_PLEASE_WAIT), gettext("Language setup is saveing!") );

	return 0;
}

int cgiSysInfo_Locale(SYS_INFO *info)
{
	char *cmd=NULL;

	if(__language_init() )
	{
		return 1;
	}

	cmd = GET_CGI_ACTION( &info->cgiVariables );
	
	if( !cmd || !strlen(cmd) || strcasecmp(cmd,CGI_ACTION_ACTIVATE))
	{
		cgi_info_page(gettext("Language Configuration"), __language_status_list(), __language_add_buttons()  );
		return 0;
	}
	
	else if(!strcasecmp(cmd,CGI_ACTION_ACTIVATE))
	{
		__language_modify_config(info );
		return 0;
	}
	else
	{
		cgidebug("Language command '%s' is not found\n", cmd );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}

