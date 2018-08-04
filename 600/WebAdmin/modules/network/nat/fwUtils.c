/*
* $Id: fwUtils.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/
#include "cgi_lib.h"
#include "cgi_iptables.h"

ipt_protocol iptables_protocols[] = 
{
	{
		IPTABLES_PROTOCOL_ID_ALL, 
		CGI_STR_ALL
	},
	{
		IPTABLES_PROTOCOL_ID_ICMP, 
		_("ICMP")
	},
	{
		IPTABLES_PROTOCOL_ID_TCP, 
		_("TCP")
	},
	{
		IPTABLES_PROTOCOL_ID_UDP, 
		_("UDP")
	}
};

nat_type_t	nat_types[] = 
{
	{
		0,
		IPTABLES_KEYWORD_SNAT	
	},
	{
		1,
		IPTABLES_KEYWORD_MASQUERADE
	}
};


int	fw_save_rule_name(FW_INFO *info)
{
	char 	cmd[128];
	char		*action;
	node	*current = info->ruleCfgs.head;
	char		*name, *type, *desc;
	
	action = GET_VALUE(&info->cgiVariables, CGI_ACTION ) ;
	
	name = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME);
	type = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_TYPE);
	desc = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_DESCRIPTOR);
	if( !name||!strlen(name) ||!type||!strlen(type) )//||!desc || !strlen(desc) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Rule Name Field is not filled"));
		return -1;
	}

	while(current)
	{
		if( !strcasecmp(current->entry.name, name) &&
			!strcasecmp(current->entry.type, type) 
			)
		{
			break;
		}
		current = current->next;
	}

	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(current && !strcasecmp(action, CGI_ACTION_SELECT) )   )
	{
		char msg[256];
		sprintf(msg, "\"%s('%s', '%s')\" %s", gettext("Rule Name"), name, type, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD)||
		!strcasecmp(action, CGI_ACTION_SELECT) )
	{
		list_append_raw(&info->ruleCfgs, name, desc, type, NULL);
		sprintf(cmd, "touch %s/%s > /dev/null", IPTABLES_CONFIG_DIR, name );
	}
	else
	{/* delete */
		list_delete_node(&info->ruleCfgs, current );
		sprintf(cmd, "rm -rf %s/%s > /dev/null", IPTABLES_CONFIG_DIR, name );
	}

	writeconfig(IPTABLES_RULE_CONFIG, ITEM_TRI, NULL, &info->ruleCfgs);

	system(cmd);
	
	return 0;
}


iptables_item *fw_get_rule(llist *cgiOrCfgs, int index)
{
	char 		fieldName[128];
	iptables_item	*p;

	p = (iptables_item *)malloc(sizeof(iptables_item) );
	if(!p)
		return NULL;
	memset(p, 0 ,sizeof(iptables_item) );

	GET_FIELD_NAME(p->protocol,cgiOrCfgs ,fieldName, IPT_KW_PROTOCOL, index);
	GET_FIELD_NAME(p->action, cgiOrCfgs ,fieldName, IPT_KW_TARGET, index);
	GET_FIELD_NAME(p->rule,cgiOrCfgs ,fieldName, IPT_KW_RULE, index);

	GET_FIELD_NAME(p->extAddrBegin,cgiOrCfgs ,fieldName, IPT_KW_ExtAddress, index);
	GET_FIELD_NAME(p->extPortBegin,cgiOrCfgs ,fieldName, IPT_KW_ExtPortBegin, index);
	GET_FIELD_NAME(p->extPortEnd,cgiOrCfgs ,fieldName, IPT_KW_ExtPortEnd, index);

	GET_FIELD_NAME(p->interAddr,cgiOrCfgs ,fieldName, IPT_KW_InterAddress, index);
	GET_FIELD_NAME(p->interPortBegin,cgiOrCfgs ,fieldName, IPT_KW_InterPortBegin, index);
	GET_FIELD_NAME(p->interPortEnd,cgiOrCfgs ,fieldName, IPT_KW_InterPortEnd, index);

	GET_FIELD_NAME(p->mac, cgiOrCfgs ,fieldName, IPT_KW_MAC_ADDRESS, index);
	return p;
	
}

/* get a iptabels_item list from config list */
iptables_item *fw_parse_rule_config(llist *cfgList)
{
	int		ruleCounts;
	int		i;
	iptables_item *p, *header = NULL, *last = NULL;
	
	char *count = GET_VALUE(cfgList, IPT_RULE_CFG_ITEM_NUM);
	ruleCounts = (!count || !strlen(count) ) ? 0 : atoi(count);
	for(i=0; i< ruleCounts; i++)
	{
		p = fw_get_rule( cfgList, i);

#if 0		
		char 	fieldName[128];
		p = (iptables_item *)malloc(sizeof(iptables_item) );
		if(!p)
			return NULL;
		memset(p, 0 ,sizeof(iptables_item) );

		GET_FIELD_NAME(p->protocol,natRuleCfg ,fieldName, IPT_KW_PROTOCOL, i);
		GET_FIELD_NAME(p->action, natRuleCfg ,fieldName, IPT_KW_TARGET, i);
		GET_FIELD_NAME(p->rule,natRuleCfg ,fieldName, IPT_KW_RULE, i);

		GET_FIELD_NAME(p->extAddrBegin,natRuleCfg ,fieldName, IPT_KW_ExtAddress, i);
		GET_FIELD_NAME(p->extPortBegin,natRuleCfg ,fieldName, IPT_KW_ExtPortBegin, i);
		GET_FIELD_NAME(p->extPortEnd,natRuleCfg ,fieldName, IPT_KW_ExtPortEnd, i);

		GET_FIELD_NAME(p->interAddr,natRuleCfg ,fieldName, IPT_KW_InterAddress, i);
		GET_FIELD_NAME(p->interPortBegin,natRuleCfg ,fieldName, IPT_KW_InterPortBegin, i);
		GET_FIELD_NAME(p->interPortEnd,natRuleCfg ,fieldName, IPT_KW_InterPortEnd, i);
#endif
		if(!header )
			header = p;
		if(last)
			last->next = p;
		last = p;
	}

	return header;
}


iptables_item *fw_parse_rule_config_file(char *name)
{
	llist 		natRuleCfg;
	char		filename[128];
#if 0	
	int		ruleCounts;
	int		i;
	iptables_item *p, *header = NULL, *last = NULL;
#endif
	if(!name || !strlen(name) )
		return NULL;
	sprintf(filename, "%s%s", IPTABLES_CONFIG_DIR, name);
	list_create(&natRuleCfg);
	if( readconfig(filename, ITEM_EQU, NULL, &natRuleCfg) != 0)
	{
		return NULL;
	}

	return fw_parse_rule_config(&natRuleCfg);

#if 0
	ruleCounts = atoi(GET_VALUE(&natRuleCfg, IPT_RULE_CFG_ITEM_NUM) );
	for(i=0; i< ruleCounts; i++)
	{
		p = fwNat_get_rule(&natRuleCfg, i);

#if 0		
		char 	fieldName[128];
		p = (iptables_item *)malloc(sizeof(iptables_item) );
		if(!p)
			return NULL;
		memset(p, 0 ,sizeof(iptables_item) );

		GET_FIELD_NAME(p->protocol,natRuleCfg ,fieldName, IPT_KW_PROTOCOL, i);
		GET_FIELD_NAME(p->action, natRuleCfg ,fieldName, IPT_KW_TARGET, i);
		GET_FIELD_NAME(p->rule,natRuleCfg ,fieldName, IPT_KW_RULE, i);

		GET_FIELD_NAME(p->extAddrBegin,natRuleCfg ,fieldName, IPT_KW_ExtAddress, i);
		GET_FIELD_NAME(p->extPortBegin,natRuleCfg ,fieldName, IPT_KW_ExtPortBegin, i);
		GET_FIELD_NAME(p->extPortEnd,natRuleCfg ,fieldName, IPT_KW_ExtPortEnd, i);

		GET_FIELD_NAME(p->interAddr,natRuleCfg ,fieldName, IPT_KW_InterAddress, i);
		GET_FIELD_NAME(p->interPortBegin,natRuleCfg ,fieldName, IPT_KW_InterPortBegin, i);
		GET_FIELD_NAME(p->interPortEnd,natRuleCfg ,fieldName, IPT_KW_InterPortEnd, i);
#endif
		if(!header )
			header = p;
		if(last)
			last->next = p;
		last = p;
	}

	return header;
#endif

}

