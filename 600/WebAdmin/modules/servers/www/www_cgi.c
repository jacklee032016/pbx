/*
* $Id: www_cgi.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>

#include "cgi_lib.h"

#include "cgi_services.h"
#include "cgi_syscfg.h"

typedef	struct
{
	llist			wwwCfgs;
	llist			cgiVariables;
	
	network_info 	*netInfo;
}WWW_INFO;

static WWW_INFO	_wwwInfo;

static char *__www_status_list( WWW_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	char		target[256];
	interface_info	*wan = &info->netInfo->wanInfo;
	
	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><TD align=\"center\">%s</TD></tr>", 
		gettext(CGI_STR_STATUS), cgi_service_get_state_name(SERVICE_NAME_WWW), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	sprintf(target, "%s?%s=%s", WEB_URL_SYSCONFIG, CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_HOSTNAME);
	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td>",
		gettext("Domain Name"), cgi_domain_name_read(), cgi_button(gettext(CGI_STR_MODIFY), target) );

	sprintf(target, "%s?%s=%s", WEB_URL_SYSCONFIG, CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN);// wan->name  );
	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td>",
		gettext("WAN IP Address"), wan->addr , cgi_button(gettext(CGI_STR_MODIFY), target ) );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td>%s</td>",
		gettext(CGI_STR_DEVICE), GET_VALUE(&info->wwwCfgs, BOA_KEYWORD_LISTEN), "");
//		gettext("WWW IP Address"), GET_VALUE(&info->wwwCfgs, BOA_KEYWORD_LISTEN), "");
	return strdup(buf);
}

static char *__www_add_buttons()
{
	char buf[4096];
	int length = 0;
	
	length += CGI_SPRINTF(buf,length, "<TR>\r\n");
#if 0	
	char	target[256];
	sprintf(target,"<TD align=\"center\">%s?%s=%s", WEB_URL_WWW_DETECT, CGI_KEYWORD_OPERATOR, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf,length, cgi_button(	gettext(CGI_STR_ACTIVATE ) , target ) );
	length += CGI_SPRINTF(buf,length, "</TD>\r\n");
#endif

	length += CGI_SPRINTF(buf,length, "<TD colspan=2 align=\"center\">\r\n");
	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_WWW ) );

	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s:</strong><p>%s,%s,%s!<br>%s.%s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE),gettext("Although WWW service is started"),gettext("but if no IP Address is displayed in this page"),
		gettext("then the WWW pages can not be browsed in Internet"), gettext("This is because of your WAN Configuration is not correct"),
		gettext("Please check your WAN configuration carefully when in this case"));

	return strdup(buf);
}


static int __www_init(WWW_INFO *info)
{
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);
	
	list_create(&info->wwwCfgs);

	if (readconfig( WEB_CFG_WWW, ITEM_DBL, NULL, &info->wwwCfgs) != 0)
	{
		return -1;
	}
	
	info->netInfo = cgi_get_network_info(1);

	return 0;
}

int __www_modify_config( WWW_INFO *info)
{

	list_replace_value_by_key(&info->wwwCfgs, BOA_KEYWORD_LISTEN, info->netInfo->wanInfo.addr );

	writeconfig(WEB_CFG_WWW, ITEM_DBL, NULL, &info->wwwCfgs);

	cgi_refresh_page(3, WEB_URL_WWW_DETECT, gettext(CGI_STR_PLEASE_WAIT), gettext("WWW will restart with new configuration") );
#if 0
	if(cgi_service_get_state(SERVICE_NAME_WWW) == SERVICE_STATE_RUNNING)
	{
		cgi_service_stop(SERVICE_NAME_WWW);
		sleep(2);
	}
	cgi_service_start(SERVICE_NAME_WWW);
#else
	if(cgi_service_get_state(SERVICE_NAME_WWW) != SERVICE_STATE_RUNNING)
		cgi_service_restart(SERVICE_NAME_WWW);
#endif
	return 0;
}

int main()
{
	char *cmd=NULL;
	WWW_INFO	*info = &_wwwInfo;

	if(__www_init( info) )
	{
		cgi_error_page(NULL, gettext("WWW Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE( &info->cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) )
	{
		cgi_info_page(gettext("WWW Configuration"), __www_status_list(info), __www_add_buttons()  );
		return 0;
	}
	else if(!strcasecmp(cmd, CGI_ACTION_ACTIVATE))
	{
		__www_modify_config(info);
		return 0;
	}
	else
	{
		cgidebug("WWW command '%s' is not found\n", cmd );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}

