/*
* $Id: wifiMain.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_wifi.h"

int main()
{
	char 		*cmd=NULL;
	WIFI_INFO	*info = cgi_wireless_init(CGI_WIRELESS_WIFI);

	if( !info )
		return 1;

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGI_WIRELESS_OP_MODE) )
	{
		return cgi_wireless_mode(info);
	}
	if(!strcasecmp(cmd, CGI_WIFI_OP_NIC)||!strcasecmp(cmd, CGI_WIFI_OP_NIC_DHCP) )
	{
		return cgi_lan_setup(&info->cgiVariables, CGI_LAN_WIFI,CGI_LAN_WHERE_WIFI);
//		return wifi_Network(_wifiInfo);
	}
	if(!strcasecmp(cmd, CGI_WIFI_OP_STATUS) )
	{
		return wifi_Info(info);
	}
	if( !strcasecmp(cmd, CGI_WIFI_OP_ACL)  )
	{
		return wifi_Acl(info);
	}
	if( !strcasecmp(cmd, CGI_WIFI_OP_AUTHEN)  )
	{
		return wifi_Authen(info);
	}
	if( !strcasecmp(cmd, CGI_WIFI_OP_STATION)  )
	{
		return wifi_Stations(info);
	}
#if 1	
	if(!strcasecmp(cmd, CGI_WIFI_OP_SCAN) )
	{
		return cgi_future_page(cmd, gettext("WiFi") );
	}
#endif	
	if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_FLOOD)||!strcasecmp(cmd, CGI_WIFI_OP_AODV_TIMER) ||
		!strcasecmp(cmd, CGI_WIFI_OP_AODV_ROUTE) ||!strcasecmp(cmd, CGI_WIFI_OP_AODV_NEIGH) )
	{
		return wifi_Aodv_Info(info);
	}
	if(!strcasecmp(cmd, CGI_WIFI_OP_AODV_CTRL) )
	{
		return wifi_Aodv_Ctrl(info);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


