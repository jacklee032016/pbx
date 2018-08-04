/*
* $Id: cgiVpnMain.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "_vpn.h"
#include "cgi_syscfg.h"

VPN_INFO	_vpnInfo;

static int __pptpd_parse(VPN_INFO *info, char *key, char *value)
{
	node *current = info->pppdConfig.head;
	char *type;
	while(current)
	{
		cgidebug("PPPD : '%s'%s\n", current->entry.name, current->entry.value);
		type = strstr(current->entry.name, key);
		if(!type)
		{
			current = current->next;
			continue;
		}
		else
		{
			int i = 0;
			char	*tmp;
			type += strlen(key);
			while(type[i]==' '|| type[i]=='\t' || type[i]=='\n' || type[i]=='\r' )//|| line[i]=='=')
				i++;
			tmp = type+1;
			strncpy(value, tmp, strlen(tmp));
			cgidebug("VPN Server Name : '%s'\n", info->serverName);
			break;
		}

		current = current->next;
	}

	return 0;
}


static int __read_vpn_confg(VPN_INFO *info)
{
//	char	*type;
	list_create(&info->pptpdConfig);
	list_create(&info->pppdConfig);
	list_create(&info->accountConfig);
	list_create(&info->vpnRoutes);
	memset(info->userName, 0, sizeof(info->userName) );
	memset(info->serverName, 0, sizeof(info->serverName) );
	memset(info->password, 0, sizeof(info->password) );

	if (readconfig(VPNS_PPTPD_CONFIG_FILE, ITEM_DBL, NULL, &info->pptpdConfig) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	if (readconfig(VPNS_PPPD_CONFIG_FILE, ITEM_ONE, NULL, &info->pppdConfig) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	__pptpd_parse( info, VPNS_PPPD_AUTH_SERVER_NAME, info->serverName);
	__pptpd_parse( info, VPNS_PPPD_MS_DNS, info->ms_dns);
	__pptpd_parse( info, VPNS_PPPD_MS_WINS, info->ms_wins);
	__pptpd_parse( info, VPNS_PPPD_IP_PARAM, info->ipParam );
	__pptpd_parse( info, VPNS_PPPD_LOG_FILE, info->logFile);

	
	if (readconfig(CONF_PPP_CHAP, ITEM_FOR, NULL, &info->accountConfig) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}
	
	if (readconfig(VPN_ROUTE_CONFIG_FILE, ITEM_EQU, NULL, &info->vpnRoutes) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

#if 1
	node *current = info->accountConfig.head;
	while(current)
	{
		cgidebug("User Name : %s\t: %s\t: %s\t: %s\n", current->entry.type, current->entry.name, current->entry.value, current->entry.rule);
		if(!strcmp(current->entry.type, info->serverName) )
		{
//			int i = 0;
//			type += strlen(VPNS_PPPD_AUTH_SERVER_NAME);
//			while(type[i]==' '|| type[i]=='\t' || type[i]=='\n' || type[i]=='\r' )//|| line[i]=='=')
//				i++;
//			strncpy(info->userName, current->entry.type+1, strlen(current->entry.type)-2  );
//			strncpy(info->password, current->entry.value+1, strlen(current->entry.value)-2 );
			strncpy(info->userName, current->entry.type, strlen(current->entry.type)  );
			strncpy(info->password, current->entry.value, strlen(current->entry.value) );
			cgidebug("User Name : %s\tPassword : %s\n", info->userName, info->password);
			break;
		}

		current = current->next;
	}
#endif

	list_create( &info->clientProvider);
	if (readconfig(VPN_CLIENT_PROVIDER, ITEM_DBL, NULL, &info->clientProvider) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}
	
	list_create( &info->runtime);
	if (readconfig(VPN_LOG_FILE, ITEM_EQU, NULL, &info->runtime) != 0)
	{
//		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Config of VPN is found") );
	}

	return 0;
}


static int __vpn_init(VPN_INFO *info)
{
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	if(__read_vpn_confg(info) )
		return -1;
	return 0;
}

int main(int argc, char *argv[])
{
	char 	*cmd=NULL;
	VPN_INFO *info = &_vpnInfo;
	
	if(__vpn_init(info) )
	{
		cgi_error_page(NULL, gettext("Services Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_CGI_OP(&info->cgiVariables);
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, CGI_VPN_OP_SERVER_STATUS ) )
	{
		return vpn_status_info(info);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_SERVER_CONFIG) )
	{
		return vpn_server_config(info);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_CLIENT_STATUS) )
	{
		return vpn_status_info(info);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_CLIENT_CONFIG) )
	{
		return vpn_client_config(info);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_ACCOUNT_SERVER) )
	{
		return vpn_account_config(info, VPN_ACCOUNT_TYPE_SERVER);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_ACCOUNT_AS600) )
	{
		return vpn_account_config(info, VPN_ACCOUNT_TYPE_AS600);
	}
	else	 if(!strcasecmp(cmd, CGI_VPN_OP_ACCOUNT_OTHERS) )
	{
		return vpn_account_config(info, VPN_ACCOUNT_TYPE_OTHER);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


