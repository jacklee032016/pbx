/*
* $Id: syscfg_misc.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "sysconf.h"

int sysconfig_reboot(SYSCFG_INFO *info)
{
	char button[2048];
	int 	length = 0;
	char target[256];
	char	*action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION);

	if( !action || !strlen(action) )
	{
		length += sprintf(button + length, "<TR><TD align=\"center\">%s</TD></TR><TR><TD><p></TD></TR>\n", gettext(CGI_STR_ARE_YOU_SURE) );
		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_SYSCONFIG, CGI_KEYWORD_OPERATOR, 
			CGI_SYSCFG_OP_REBOOT, CGI_ACTION, CGI_ACTION_ADD );
		length += sprintf(button + length, "<TR><TD><p></TD></TR><TR><TD align=\"center\">%s</TD></TR>\n" ,
			cgi_button(gettext(CGI_STR_REBOOT), target) );

		cgi_info_page(gettext(CGI_STR_REBOOT), "", button);
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_ADD) )
	{
		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_SYSCONFIG, CGI_KEYWORD_OPERATOR, 
			CGI_SYSCFG_OP_REBOOT, CGI_ACTION, CGI_ACTION_ACTIVATE );
		length += sprintf(button+ length, "%s,",gettext("Reboot will cost 1 minuts or more") );
		length += sprintf(button+ length, "%s!",gettext("it is determined by the services you selected"));
		cgi_refresh_page(60, target, gettext("Rebooting....."), button );
		fflush(stdout);
		
		usleep(800);
		system("reboot");
		return 0;
	}
	else if( !strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{
		length += sprintf(button+ length, "%s<br>",gettext("Welcome return your our Web Admin!") );
		length += sprintf(button+ length, "%s", gettext("If your have change configuration, please check it carefully") );
		cgi_info_page(gettext(CGI_STR_REBOOT), gettext("Reboot OK now!"), button );
		return 0;
	}

	cgi_error_page(NULL, CGI_STR_ERROR, gettext("Not supportted command when reboot") );

	return 0;
}

