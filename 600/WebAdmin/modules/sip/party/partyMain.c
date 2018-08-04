/*
* $Id: partyMain.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"

#include "ppl.h"

PARTY_INFO		*_info;

#define	PARTY_GLOBAL_ITEM(info, key)	\
	list_append_raw(&info->globalCfgs, key, getConfigElement(key), NULL, NULL)

static int __cgiParty_Globals(PARTY_INFO *info)
{
/*
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_SERVER_IP);
*/	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_DYNAMIC_IP);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_ENABLE_IPV6);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_SERVER_NAME);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_SERVER_PORT);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_TRANSFORT);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_MULTICAST);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_MULTICAST_DEV);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_PLUGINS);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_PLUGINS_CONF);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_RECOVERY_DELAY);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_RECOVERY_FILE);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_SERVER_REALM);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_AUTHEN);
	
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_LOG_FILE);
	PARTY_GLOBAL_ITEM(info, PARTY_CFG_LOG_LEVEL);

	return 0;
}

static int __cgiParty_filter(llist *filters, char *filtername)
{
	config_element_t *previous = NULL;
	config_element_t *elem;
	char				*p;
	int				i = 0;
	char				index[32];

	elem = getConfigSubElement("forward", filtername, NULL);
  	while (elem != NULL)
	{
		cgidebug("added %s forward : %s\n", filtername, elem->value);
		p = strchr(elem->value, '|');
		*p = '\0';
		p++;
		sprintf(index, "%d", i);
		list_append_raw(filters, "forward", elem->value, p, index);
		i++;

		previous = elem;
		elem = getConfigSubElement("forward", filtername, previous);
		
	}
	
	elem = getConfigSubElement(PARTY_CFG_FILTER_DIAL_PLAN, filtername, NULL);
	list_append_raw(filters, PARTY_CFG_FILTER_DIAL_PLAN, (elem==NULL)?"external":elem->value, NULL, NULL);

	elem = getConfigSubElement(PARTY_CFG_FILTER_MODE, filtername, NULL);
	list_append_raw(filters, PARTY_CFG_FILTER_MODE, (elem==NULL)?"statefull":elem->value, NULL, NULL);

	elem = getConfigSubElement(PARTY_CFG_FILTER_ROUTE_RECORD, filtername, NULL);
	list_append_raw(filters, PARTY_CFG_FILTER_ROUTE_RECORD, (elem==NULL)?PARTY_VALUE_OFF:elem->value, NULL, NULL);

	return 0;
}

static int __cgiPartyMain_init()
{
	config_element_t	*elem;
	char				*value;
	
	if(configLoad(SIPPARTY_CONF_FILE))
		return -1;

	_info = (PARTY_INFO *)malloc( sizeof(PARTY_INFO) );
	if(!_info)
		return -1;
	memset(_info, 0, sizeof(PARTY_INFO) );
	
	cgi_html_header();
	list_create(&_info->cgiVariables);
	read_cgi_input(&_info->cgiVariables, NULL, NULL);

	_info->netInfo = cgi_get_network_info(1);
	
	list_create(&_info->globalCfgs);
#if 1
	__cgiParty_Globals(_info);
#else
	if (readconfig(SIPPARTY_CONF_FILE, ITEM_EQU, NULL, &_info->globalCfgs) != 0)
	{
		cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP Server Configuration is not exist"));
		return -1;
	}
#endif

	elem = getConfigSubElement(PARTY_CFG_AUTHEN_407, "auth", NULL);
	value = (elem && elem->value)? elem->value:CGI_STR_OFF;
#if 0	
	snprintf(_info->proxyAuth, (sizeof(_info->proxyAuth)< strlen(value))?sizeof(_info->proxyAuth):strlen(value) ,"%s", value );
#else
	sprintf(_info->proxyAuth, "%s", value );
#endif

	list_create(&_info->userCfgs);
	if (readconfig(SIPPARTY_CONF_FILE, ITEM_TRI, NULL, &_info->userCfgs) != 0)
	{
		cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP Server Configuration is not exist"));
		return -1;
	}
	list_create(&_info->extFilterCfgs);
	__cgiParty_filter(&_info->extFilterCfgs, "filter_external");

	list_create(&_info->intFilterCfgs);
	__cgiParty_filter(&_info->intFilterCfgs, "filter_internal");

	return 0;
}

int main()
{
	char 	*cmd=NULL;
//	char		*action;
//	char title[128];

	if(__cgiPartyMain_init() ==-1 )
	{
		cgi_error_page(NULL, gettext("SIP Server Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE( &_info->cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, PARTY_OP_GLOBAL) )
	{
		return cgiParty_Globals(_info);
	}
	if( !strcasecmp(cmd, PARTY_OP_USER) )
	{
		return cgiParty_Users(_info);
	}
	if( !strcasecmp(cmd, PARTY_OP_FILTER_EXT) )
	{
		return cgiParty_Filters(_info, PARTY_FILTER_TYPE_EXT);
	}
	if( !strcasecmp(cmd, PARTY_OP_FILTER_INT) )
	{
		return cgiParty_Filters(_info, PARTY_FILTER_TYPE_INT);
	}
	if( !strcasecmp(cmd, PARTY_OP_LOOKUP) )
	{
		return cgiParty_Lookup(_info);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


