/*
* $Id: servicesMiscMain.c,v 1.7 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"

SERVICE_INFO	_servicesInfo;

static int __servciesMisc_init()
{
	cgi_html_header();
	list_create(&_servicesInfo.cgiVariables);
	read_cgi_input(&_servicesInfo.cgiVariables, NULL, NULL);

	return 0;
}

int main()
{
	char 	*cmd=NULL;

	if(__servciesMisc_init() )
	{
		cgi_error_page(NULL, gettext("Services Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE(&_servicesInfo.cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, SERVICES_MISC_OP_CONFIG ) )
	{
		return servicesMisc_Config(&_servicesInfo);
	}
#if 0	
	if( !strcasecmp(cmd, SERVICES_MISC_OP_DHCP ) )
	{
		return servicesMisc_Dhcp(&_servicesInfo);
	}
#endif	
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_FTP) )
	{
		return servicesMisc_Ftp(&_servicesInfo);
	}
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_UPNP) )
	{
		return servicesMisc_Upnp(&_servicesInfo);
	}
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_RIP) )
	{
		return servicesMisc_Rip(&_servicesInfo);
	}
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_SYSLOG) )
	{
		return servicesMisc_Syslogd(&_servicesInfo);
	}
#if 0	
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_VPNSERVER) )
	{
		return servicesMisc_VpnServer(&_servicesInfo);
	}
	else	 if(!strcasecmp(cmd, SERVICES_MISC_OP_DHCP) 
		||!strcasecmp(cmd, SERVICES_MISC_OP_DHCP)
 		||!strcasecmp(cmd, SERVICES_MISC_OP_DHCP) )
	{
		return servicesMisc_dhcp(&servicesMiscCgiVariables, cmd);
	}
	else if(!strcasecmp(cmd, SERVICES_MISC_OP_DHCP) || !strcasecmp(cmd, SERVICES_MISC_OP_DHCP)  )
	{
		return cgi_future_page(cmd, gettext("") );
	}
#endif	
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


