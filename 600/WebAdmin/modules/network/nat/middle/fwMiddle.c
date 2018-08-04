/*
* $Id: fwMiddle.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "fwMiddle.h"

llist 			middleCfgs;
iptables_item 	*middleHeader;

int __middle_init(FW_INFO *info)
{
	char		fileName[128];

	sprintf(fileName, "%s%s", IPTABLES_CONFIG_DIR, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME ) );
	list_create(&middleCfgs);
	if (readconfig(fileName, ITEM_EQU, NULL, &middleCfgs) != 0)
	{
		return -1;
	}

	middleHeader = fw_parse_rule_config(&middleCfgs);
	if(!middleHeader)
		return -1;

	return 0;
}

int cgiFw_Middle(FW_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
#if 0	
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext(CGI_STR_FW_MIDDLE_CONFIG), "", fw_rule_name_buttons( info) );
		return 0;
	}
	
//	rulesName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME );
 	
		if(!strcasecmp(action, CGI_ACTION_SELECT ) )
		{/* display service select pages */
			if(fw_save_rule_name(info)< 0)
				return 1;
		}
#endif
	if(!strcasecmp(action, CGI_ACTION_SELECT ) )
	{
		cgi_info_page(gettext(CGI_STR_FW_MIDDLE_CONFIG), gettext("Select Services"), fwMiddle_services_buttons(info) );
		return 0;
	}
	
	if(!strcasecmp(action, IPT_MIDDLE_SERVICES) )
	{/* save services list for this rule set and display rule add buttons */
		fwMiddle_save_rule(NULL, info);// middleHeader, info);
		
	__middle_init(info);
		cgi_info_page(gettext(CGI_STR_FW_MIDDLE_CONFIG), 
			fwMiddle_info_buttons(middleHeader, &middleCfgs), fwMiddle_address_buttons(info, &middleCfgs) );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/* added rule */
	__middle_init(info);
		fwMiddle_save_rule( middleHeader, info);

	__middle_init(info);
		cgi_info_page(gettext(CGI_STR_FW_MIDDLE_CONFIG), 
			fwMiddle_info_buttons(middleHeader, &middleCfgs), fwMiddle_address_buttons(info, &middleCfgs) );
		return 0;
	}		

	__middle_init(info);
	cgi_info_page(gettext(CGI_STR_FW_MIDDLE_CONFIG), 
			fwMiddle_info_buttons(middleHeader, &middleCfgs), fwMiddle_address_buttons(info, &middleCfgs) );
	return 0;	
}

