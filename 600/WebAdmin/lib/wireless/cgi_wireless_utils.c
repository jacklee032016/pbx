/*
* $Id: cgi_wireless_utils.c,v 1.2 2006/12/19 14:33:35 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_wireless.h"

WIFI_INFO *cgi_wireless_init(wireless_type_t type)
{
	WIFI_INFO	*info;
	char			msg[256];
	int			isError = 0;
#if WITH_MESH	
	int			nicCount = 0;
#endif

	info = (WIFI_INFO *)malloc( sizeof(WIFI_INFO) );
	if(!info)
		return NULL;
	memset(info, 0 ,sizeof(WIFI_INFO) );

	info->type = type;
	
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

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
#if WITH_MESH	
	else
	{
		FILE *fp ;
		char		line[1024];
		sprintf(info->name, "%s", WIRELESS_NAME_MESH);
		info->socketFd = meshu_device_open(0);
		if(info->socketFd < 0 )
		{
			isError = 1;
		}

		list_create(&info->meshDevs );
		if( (fp=fopen(MESH_PROC_DEVICES, "r"))==NULL)
		{/* no Ops is needed now */
			;
		}
		else
		{
			while(fgets(line,1024,fp))
			{
				int i = 0;
				char *name;
				char *k = strchr(line, ':');
				if(!k)
					continue;
				*k = '\0';
				while(line[i]==' '|| line[i]=='\t' || line[i]=='\n' || line[i]=='\r' )//|| line[i]=='=')
					i++;
				name = line+i;
				list_append_raw(&info->meshDevs, name , NULL, NULL, NULL);

				if(strcasecmp(name, "portal") )
				{
					int fd ;
					nicCount++;
					fd = meshu_device_open(nicCount);
					meshu_get_info(fd, &info->meshInfo[nicCount-1] );
					MESH_DEVICE_CLOSE(fd);
				}
			}
			fclose( fp);

			sprintf(line, "%d", nicCount);
			list_append_raw(&info->meshDevs, WIFI_KEYWORD_MESH_NUM, line, NULL, NULL);
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
	list_create(&info->globalCfgs);
	if (readconfig((type==CGI_WIRELESS_WIFI)?WIFI_CONFIG_FILE:MESH_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}

	list_create(&info->nicCfgs);
	if (readconfig((type==CGI_WIRELESS_WIFI)?WIFI_CONF_NETWORK:MESH_CONF_NETWORK, ITEM_EQU, NULL, &info->nicCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}

	return info;
}

int cgi_wireless_save_config(WIFI_INFO *info)
{
	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);

	if(writeconfig((info->type==CGI_WIRELESS_WIFI)?WIFI_CONFIG_FILE:MESH_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}
	return 0;
}

int cgi_mesh_save_config(WIFI_INFO *info)
{
	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);

	if(writeconfig((info->type==CGI_WIRELESS_WIFI)?WIFI_CONFIG_FILE:MESH_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}
	return 0;
}


