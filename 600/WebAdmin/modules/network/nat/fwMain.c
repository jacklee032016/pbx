/*
* $Id: fwMain.c,v 1.6 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_iptables.h"

FW_INFO		*_fwInfo;

static int __cgiFwMain_init()
{
	_fwInfo = (FW_INFO *)malloc( sizeof(FW_INFO) );
	if(!_fwInfo)
		return -1;
	memset(_fwInfo, 0 ,sizeof(FW_INFO) );
	
	cgi_html_header();
	list_create(&_fwInfo->cgiVariables);
	read_cgi_input(&_fwInfo->cgiVariables, NULL, NULL);

	_fwInfo->netInfo = cgi_get_network_info(1);

	list_create(&_fwInfo->globalCfgs);
	if (readconfig(IPTABLES_CONFIG_FILE, ITEM_EQU, NULL, &_fwInfo->globalCfgs) != 0)
	{
		return -1;
	}

	list_create(&_fwInfo->ruleCfgs);
	if (readconfig(IPTABLES_RULE_CONFIG, ITEM_TRI, NULL, &_fwInfo->ruleCfgs) != 0)
	{
		return -1;
	}
	list_create(&_fwInfo->timerCfgs);
	if (readconfig(IPTABLES_TIMER_CONFIG, ITEM_TRI, NULL, &_fwInfo->timerCfgs) != 0)
	{
		return -1;
	}
	
	return 0;
}

int main()
{
	char 	*cmd=NULL;
	char		*action;
		char title[128];

	if(__cgiFwMain_init() ==-1 )
	{
		cgi_error_page(NULL, gettext("Firewall Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE(&_fwInfo->cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) )
	{
		return cgiFwLookup(_fwInfo );
	}

/*
	if(!strcasecmp(cmd, FIREWALL_OP_VPN) )
	{
		return cgiFw_VPN(_fwInfo);
	}
*/

	if(!strcasecmp(cmd, FIREWALL_OP_QOS) )
	{
		return cgiNet_QoS(_fwInfo);
	}

	if(!strcasecmp(cmd, FIREWALL_OP_TIMER) )
	{
		return cgiFw_Timer(_fwInfo);
	}
	if(!strcasecmp(cmd, FIREWALL_OP_TRACK) )
	{
		return cgiFw_ConnTrack(_fwInfo);
	}

	action = GET_VALUE(&_fwInfo->cgiVariables, CGI_ACTION );
	if( !action || !strlen(action)  )
	{/* display add rule file name pages */
		sprintf(title, "%s %s", gettext(cmd), gettext(CGI_STR_CONFIGURATION) );
		cgi_info_page(title, "", fw_rule_name_buttons( _fwInfo) );
		return 0;
	}

	if(!strcasecmp(action, FIREWALL_SUB_OP_DEL_RULE ) || !strcasecmp(action, CGI_ACTION_SELECT) )
	{/* save rule name and create rule file, then continue */
		if(fw_save_rule_name(_fwInfo)< 0)
			return 1;
		
		if(!strcasecmp(action, FIREWALL_SUB_OP_DEL_RULE ) )
		{
			cgi_refresh_page(1, WEB_URL_FIREWALL, gettext("Delete Rule"), gettext(CGI_STR_PLEASE_WAIT) );
			return 0;
		}
	}

	if(!strcasecmp(cmd, FIREWALL_OP_SNAT) )
	{
		return cgiFw_Nat(_fwInfo);
	}
	else	 if(!strcasecmp(cmd, FIREWALL_OP_MIDDLE) )
	{
		return cgiFw_Middle(_fwInfo);
	}
	else	 if(!strcasecmp(cmd, FIREWALL_OP_EXPERT) )
	{
		return cgiFw_Expert(_fwInfo);
	}
	else
	{
		cgidebug("Firewall command '%s' is not found\n", cmd );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


