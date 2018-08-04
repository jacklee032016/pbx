/*
* $Id: quickWizardWan.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "quickWizard.h"

void __cgi_redirect_to_quick_lan()
{
	return ;
}

int quickWizardWan(QUICK_INFO *info)
{
	char		*protocol;
	
	protocol = GET_VALUE(&info->cgiVariables, NIC_KEYWORD_PROTOCOL );
	if(!protocol || !strlen(protocol) )
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("No Protocol is defined for WAN") );
		return 0;
	}

	if(!strcmp(protocol, NIC_KEYWORD_PROTOCOL_PPPOE) )
	{
		char target[128];
		/* redirect to quick LAN configuration*/
		sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN );
		cgi_refresh_page(3, target, gettext(CGI_STR_PLEASE_WAIT), gettext("PPPOE configuration is activating.....") );

		cgi_syscfg_wan_activate_pppoe(&info->cgiVariables );
	}
	else if(!strcmp(protocol,NIC_KEYWORD_PROTOCOL_STATIC) )
	{
		cgi_syscfg_write_nic_config( &info->cgiVariables, SYSCFG_TYPE_WIZARD );
	
//		__cgi_redirect_to_quick_lan();
	}
	else
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("Protocol is not validate for WAN") );
	
	return 0;
}

