/*
* $Id: fwExpert.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "fwExpert.h"

iptables_item	*expertCgi;		/* NAT config read from CGI */

int cgiFw_Expert(FW_INFO *info)
{
	char *action;
	char *rulesName;
	iptables_item *rulesCfg;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
#if 0	
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Firewall Expert Configuration"), "", fw_rule_name_buttons( info) );
		return 0;
	}
	
 	
		if(!strcasecmp(action, CGI_ACTION_SELECT ) )
		{
			if(fw_save_rule_name(info)< 0)
				return 1;
		}
		else
#endif			
	rulesName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME );
	rulesCfg = fw_parse_rule_config_file( rulesName);
	if(!strcasecmp(action, CGI_ACTION_ADD ) ||
		!strcasecmp(action, CGI_ACTION_DEL) )
	{
		if(fwNat_save_rule(rulesCfg, info))
			return 1;
	}

	rulesName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME );
	/* renew this list to display in pages */
	rulesCfg = fw_parse_rule_config_file( rulesName);
	cgi_info_page(gettext("Firewall Expert Configuration"), fwExpert_info(rulesCfg), fwExpert_buttons(info) );
		
	return 0;	
}

