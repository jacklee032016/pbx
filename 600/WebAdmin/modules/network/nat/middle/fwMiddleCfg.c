/*
* $Id: fwMiddleCfg.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "fwMiddle.h"

#define	WRITE_MIDDLE_SERVICE(fp, serviceName, cgiVariables)	\
	do{ fprintf(fp, "%s=%s\n", serviceName, GET_VALUE(cgiVariables, serviceName) ); \
	}while(0)
	
int fwMiddle_save_rule(iptables_item *cfgRules, FW_INFO *info)
{
	FILE 	*fp;
	char		fileName[128];
	int 		i = 0;
	char		*ruleName;
	char		*action;
	iptables_item *current = cfgRules;
	iptables_item *ruleCgi = fw_get_rule(&info->cgiVariables, -1);
	char 	msg[256];
	int 		isError = 0;

	ruleName = GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) ;
	sprintf(fileName, "%s%s", IPTABLES_CONFIG_DIR, ruleName );
	if(  (fp =fopen(fileName, "w") ) == NULL )
	{
		sprintf(fileName, "'%s' %s", ruleName, gettext("rule file is not found"));
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), fileName );
		return -1;
	}

	WRITE_MIDDLE_SERVICE(fp, IPT_SERVICE_WWW, &info->cgiVariables);
	WRITE_MIDDLE_SERVICE(fp, IPT_SERVICE_FTP, &info->cgiVariables);
	WRITE_MIDDLE_SERVICE(fp, IPT_SERVICE_EMAIL, &info->cgiVariables);
	WRITE_MIDDLE_SERVICE(fp, IPT_SERVICE_QQ, &info->cgiVariables);

	if(!current )
	{
		fprintf(fp, "%s_%d=%s\n", IPT_KW_InterAddress, i, ruleCgi->interAddr);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_MAC_ADDRESS, i, ruleCgi->mac);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_TARGET, i, ruleCgi->action );

		fprintf(fp, "%s=%d\n", IPT_RULE_CFG_ITEM_NUM, ++i);
		
		fclose(fp);
		return 0;
	}
	
	while(current)
	{
		if( !strcasecmp(current->interAddr, ruleCgi->interAddr) &&
			!strcasecmp(current->mac, ruleCgi->mac ) 
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
		sprintf(msg, "\"%s('%s', '%s')\" %s", gettext("Rule"), ruleCgi->interAddr, ruleCgi->mac, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		isError = 10;
		current = cfgRules;
		goto write;
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

write:
	i = 0;
	while(current)
	{
		fprintf(fp, "%s_%d=%s\n", IPT_KW_InterAddress, i, current->interAddr);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_MAC_ADDRESS, i, current->mac);
		fprintf(fp, "%s_%d=%s\n", IPT_KW_TARGET, i, current->action );

		i++;
		current = current->next;
	}
	fprintf(fp, "%s=%d\n", IPT_RULE_CFG_ITEM_NUM, i);
	
	fclose(fp);

	if(isError)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}
	return 0;
}

