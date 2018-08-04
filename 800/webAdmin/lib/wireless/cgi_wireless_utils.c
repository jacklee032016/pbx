/*
* $Id: cgi_wireless_utils.c,v 1.2 2006/12/19 14:33:35 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgi_syscfg.h"
#include "cgi_wireless.h"

WIFI_INFO *cgi_wireless_init(wireless_type_t type)
{
	WIFI_INFO	*info;
	char			msg[256];
	int			isError = 0;

	info = (WIFI_INFO *)malloc( sizeof(WIFI_INFO) );
	if(!info)
		return NULL;
	memset(info, 0 ,sizeof(WIFI_INFO) );

	info->type = type;
	
	cgi_html_header();
	read_cgi_input(&info->cgiVariables, NULL, NULL);

#if 0
	info->isPlugged = cgi_check_pci_device("Ethernet", "168c");
	if(! info->isPlugged )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Wifi Hardware is found") );
		return NULL;
	}

	if(type == CGI_WIRELESS_WIFI)
	{
		sprintf(info->name, "%s", WIRELESS_NAME_WIFI );
		info->socketFd = iw_sockets_open();
		if(wifi_get_info(info->socketFd, DEFAULT_WIFI_DEVICE_NAME, &info->wifiInfo ) < 0 )
		{
			isError = 1;
//			return NULL;
		}
	}
#endif

	if(isError )
	{
		sprintf(msg, "'%s' %s", gettext(info->name), gettext("Drivers is not load now") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
	}
#if 0
	info->netInfo = cgi_get_network_info(1);

#endif
	if (readconfig(WIFI_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}

	if (readconfig(WIFI_CONF_NETWORK, ITEM_EQU, NULL, &info->nicCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}

	return info;
}

int cgi_wireless_save_config(WIFI_INFO *info)
{
	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);

	if(writeconfig(WIFI_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}
	return 0;
}

int cgi_mesh_save_config(WIFI_INFO *info)
{
	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);

	if(writeconfig(WIFI_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}
	return 0;
}


