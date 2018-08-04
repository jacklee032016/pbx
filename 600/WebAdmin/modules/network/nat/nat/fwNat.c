/*
* $Id: fwNat.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "fwNat.h"

iptables_item	natCgi;		/* NAT config read from CGI */

NAT_enum_t	currentNatType;

#if 0
static int __iptables_parse_cgi(llist *cgis, iptables_item *iptItem )
{
#define	CHECK_STR_VALUE(value)	( value && strlen(value) )
	char mac[6][3];
	char key[6];
	char tmp[10];
	char portto[128];
	int i=0;
	char *value=NULL;

#if 0	
	value= GET_VALUE(cgis,  IPT_KW_InterDevice);
	if(CHECK_STR_VALUE(value) )
		sprintf(iptItem->interDevice,"%s", value);
	value= GET_VALUE(cgis, IPT_KW_ExtDevice);
	if(CHECK_STR_VALUE(value) )
		sprintf(iptItem->extDevice,"%s", value);
#endif

	value= GET_VALUE(cgis, IPT_KW_PROTOCOL );
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->protocol,"%s", value);
#if 0	
	value= GET_VALUE(cgis, "action");		
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->action,value);
#endif		
	value= GET_VALUE(cgis, IPT_KW_TARGET);		
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->action,value);
	
	value= GET_VALUE(cgis, IPT_KW_InterAddress);
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->interAddr,value);
	value=GET_VALUE(cgis, IPT_KW_InterPortBegin);
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->interPortBegin,value);
	value=GET_VALUE(cgis, IPT_KW_InterPortEnd);
	if(CHECK_STR_VALUE(value))
	{
		sprintf(iptItem->interPortEnd,":%s", value);
	}

#if 0	
	value= GET_VALUE(cgis, IPT_KW_ExtAddressBegin );
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->extAddrBegin,"%s", value);

	value= GET_VALUE(cgis, IPT_KW_ExtAddressEnd );
	if(CHECK_STR_VALUE(value))
	{
		sprintf(iptItem->extAddrEnd,"-%s",portto);
	}
#else
	value= GET_VALUE(cgis, IPT_KW_ExtAddress );
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->extAddrBegin,"%s", value);
#endif
	value= GET_VALUE(cgis, IPT_KW_ExtPortBegin );
	if(CHECK_STR_VALUE(value))
	{
#if 0
			if(strcmp(tmpp,"delete"))
			{
				memset(portto,0,strlen(portto));
				sprintf(portto,":%s",value);
				strcpy(iptItem->extPortBegin,portto);
			 }
			else
#endif
			{
				strcpy(iptItem->extPortBegin,value);
			}
		}
	value= GET_VALUE(cgis, IPT_KW_ExtPortEnd );
	if(CHECK_STR_VALUE(value))
	{
		sprintf(iptItem->extPortEnd, "-%s", portto);
	}

	value= GET_VALUE(cgis, IPT_KW_RULE);		
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->rule,"%s", value);

	value= GET_VALUE(cgis, "state1");
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->state1,"%s", value);
	value= GET_VALUE(cgis, "state2");
	if(CHECK_STR_VALUE(value))
		sprintf(iptItem->state2,"%s", value);

	//get mac address
	value= GET_VALUE(cgis, IPT_KW_MAC_ADDRESS);
	if(CHECK_STR_VALUE(value) )
		sprintf(iptItem->mac,"%s", value);
	else
	{
		for(i=0;i<6;i++)
		{
			memset(key,0,strlen(key));
			sprintf(key,"%s%c",IPT_KW_MAC_ADDRESS,  i+'0');
			value = GET_VALUE(cgis, key);
			if(CHECK_STR_VALUE(value) )
			{
				strcpy(mac[i], value);
				if(!strlen(mac[i]))
				{
					memset(iptItem->mac,0,strlen(iptItem->mac));
					break;
				}
				memset(tmp,0,strlen(tmp));
				sprintf(tmp,"%s:",mac[i]);	
				strcat(iptItem->mac,tmp);	
			}
				
		}
		iptItem->mac[strlen(iptItem->mac)-1]='\0';
	}

#if 0	
	get_WAN_interface(iptItem->interDevice);
	get_WAN_interface(iptItem->extDevice);
#endif

	return 0;
}
#endif

static int __fwNat_init(FW_INFO *info)
{

#if 0
	__iptables_parse_cgi(&info->cgiVariables, &natCgi);
cgitrace;

	if(!strcasecmp(GET_VALUE(&fwCfgs, DEFAULT_MASQURADE), CGI_STR_YES_CASE) )
		currentNatType = NAT_DEFAULT;
	else
#endif		
	if(!strcasecmp(info->netInfo->wanInfo.name, PPP0) )
	{
		currentNatType = NAT_MASQURADE;
	}
	else
	{
		currentNatType = NAT_SNAT;
	}
	
	return 0;
}

int cgiFw_Nat(FW_INFO *info)
{
	char *action;
	char *rulesName;
	iptables_item *rulesCfg;

	__fwNat_init( info);

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	rulesName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME );
	rulesCfg = fw_parse_rule_config_file( rulesName);
#if 0	
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Firewall NAT Configuration"), "", fw_rule_name_buttons( info) );
		return 0;
	}
	
	
 #if 0	
		if(!strcasecmp(natCgi.action, IPTABLES_KEYWORD_MASQUERADE ) )
		{
			char	target[256];
			char	info[128];

#if IPTABLES_DEBUG
			fwNat_debug_item(&natCgi);
#endif
			sprintf(target, "%s?%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_SNAT);
			sprintf(info, "'%s' %s", gettext(IPTABLES_KEYWORD_MASQUERADE), gettext("is activating") );
			cgi_refresh_page(1, target, info, gettext(CGI_STR_PLEASE_WAIT) );

			iptables_activate_snat( &natCgi);

//			cgi_info_page(gettext("NAT Configuration"), __fwNat_status_info(),__fwNat_select_buttons() );
			return 0;
		}
#endif
 	
	if(!strcasecmp(action, CGI_ACTION_SELECT ) )
	{
		if(fw_save_rule_name(info)< 0)
			return 1;
	}
	else 
#endif

	if(!strcasecmp(action, CGI_ACTION_ADD ) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		if(fwNat_save_rule(rulesCfg, info))
			return 1;
	}

	/* renew this list to display in pages */
	rulesCfg = fw_parse_rule_config_file( rulesName);
	cgi_info_page(gettext("SNAT Configuration"), fwNat_info_buttons(rulesCfg, currentNatType), fwNat_snat_buttons(info, currentNatType) );
		
	return 0;	
}

