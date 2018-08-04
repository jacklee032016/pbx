/*
* $Id: fwNatCfg.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "fwNat.h"

int fwNat_save_rule(iptables_item *cfgRules, FW_INFO *info)
{
	FILE 	*fp;
	char		fileName[128];
	int 		i = 0;
	char		*ruleName;
	char		*action;
	iptables_item *current = cfgRules;
	iptables_item *ruleCgi = fw_get_rule(&info->cgiVariables, -1);

	while(current)
	{
		if( !strcasecmp(current->action, ruleCgi->action) &&
			!strcasecmp(current->protocol, ruleCgi->protocol ) &&
			!strcasecmp(current->interAddr, ruleCgi->interAddr) 
			)
		{
			break;
		}
		current = current->next;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION ) ;
	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		char msg[256];
		sprintf(msg, "\"%s('%s', '%s')\" %s", gettext("Rule"), ruleCgi->protocol, ruleCgi->interAddr, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{
		current = ruleCgi;
		current->next = cfgRules;
	}
	else
	{/* delete */
		if(current == cfgRules)
		{
			current = cfgRules->next;
		}
		else
		{
			iptables_item *last, *tmp;
			tmp = cfgRules;
			last = cfgRules;
			while(tmp)
			{
				if(tmp == current )
				{
					last->next = current->next;
					current = cfgRules;
					break;
				}
				last = tmp;
				tmp = tmp->next;
			}
		}
		
	}

	ruleName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) ;
	sprintf(fileName, "%s%s", IPTABLES_CONFIG_DIR, ruleName );
	if(  (fp =fopen(fileName, "w") ) == NULL )
	{
		sprintf(fileName, "'%s' %s", ruleName, gettext("rule file is not found"));
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), fileName );
		return -1;
	}

	i = 0;
	while(current)
	{
		fprintf(fp, "%s_%d=%s\n", IPT_KW_PROTOCOL, i, current->protocol);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_TARGET, i, current->action);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_RULE, i, current->rule);
		
		fprintf(fp, "%s_%d=%s\n", IPT_KW_ExtAddress, i, current->extAddrBegin);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_ExtPortBegin, i, current->extPortBegin);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_ExtPortEnd, i, current->extPortEnd);

		fprintf(fp, "%s_%d=%s\n", IPT_KW_InterAddress, i, current->interAddr);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_InterPortBegin, i, current->interPortBegin);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_InterPortEnd, i, current->interPortEnd);

		i++;
		current = current->next;
	}
	fprintf(fp, "%s=%d\n", IPT_RULE_CFG_ITEM_NUM, i);
	
	fclose(fp);

	return 0;
}

