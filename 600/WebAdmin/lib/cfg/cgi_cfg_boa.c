/*
* $Id: cgi_cfg_boa.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
//#include "cgi_cfg_file.h"
#include "cgi_services.h"

static conf_item_t   boa_admin_cfgs[] =
{
	{"Port",				"80"},
	{"Listen",				"eth1"},
	{"User",				"root"},
	{"Group",			"root"},
	{"ErrorLog",			"/tmp/log/boa/admin_error_log"},
	{"AccessLog",			"/tmp/log/boa/admin_access_log"},
	{"DocumentRoot",		"/var/admin"},
	{"UserDir",			"public_html"},
	{"DirectoryIndex",		"index.html"},
	{"DirectoryMaker",		"/usr/bin/boa_indexer"},
	{"KeepAliveMax",		"1000"},
	{"KeepAliveTimeout",	"10"},
	{"MimeTypes",		"/etc/mime.types"},
	{"DefaultType",		"text/plain"},
	{"CGIPath",			"/bin:/usr/bin:/usr/sbin:/sbin"},
	{"SinglePostLimit",		"16777216"},
	{"Alias",				"/wwwtest/ /var/www/"},
	{"ScriptAlias",			"/cgi-bin/ /var/admin/cgi-bin/"},
	{"BaseAuthen",		"Yes"},
};

static conf_item_t   boa_www_cfgs[] =
{
	{"Port",				"80"},
	{"Listen",				"eth0"},
	{"User",				"root"},
	{"Group",			"root"},
	{"ErrorLog",			"/tmp/log/boa/www_error_log"},
	{"AccessLog",			"/tmp/log/boa/www_access_log"},
	{"DocumentRoot",		ROOT_PATH_WWW},
	{"UserDir",			"public_html"},
	{"DirectoryIndex",		"index.html"},
	{"DirectoryMaker",		"/usr/bin/boa_indexer"},
	{"KeepAliveMax",		"1000"},
	{"KeepAliveTimeout",	"10"},
	{"MimeTypes",		"/etc/mime.types"},
	{"DefaultType",		"text/plain"},
	{"CGIPath",			"/bin:/usr/bin:/usr/sbin:/sbin"},
	{"SinglePostLimit",		"16777216"},
	{"Alias",				"/doc  /usr/doc"},
	{"ScriptAlias",			"/cgi-bin/ /var/www/cgi-bin/"},
	{"BaseAuthen",		"No"},
};

#if 0
int cgi_write_boa_config(web_type_t type, char *ipAddress)
{
	FILE			*fp;
	conf_item_t	*cfg;
	char			*filename;
	llist			rawCfgs;
	int			result;
	
	if(type== WEB_TYPE_WWW)
	{
		cfg = boa_www_cfgs;
		size = sizeof(boa_www_cfgs)/ sizeof(conf_item_t) ;
		filename = WEB_CFG_WWW;
	}	
	else
	{
		cfg = boa_admin_cfgs;
		size = sizeof(boa_admin_cfgs)/ sizeof(conf_item_t) ;
		filename = WEB_CFG_ADMIN;
	}

	list_create( &rawCfgs);
	list_add_config_items( &rawCfgs, cfg, size);

	if(ipAddress)
	{
		list_create(&newCfgs);
		list_append_raw(newCfgs, "Listen", ipAddress, NULL, NULL);
	}

	if(( fp=fopen(filename,"w"))==NULL)
	{
		cgidebug("Open file %s failure : %s\n", filename, strerror(errno) );
		return 0;
	}
	
	result = cgi_write_config(filename, rawCfgs, (ipAddress==NULL)?NULL:&newCfgs, _cgi_write_config_item_with_space );

	if(result != size )
	{
		cgidebug("WARNING : write only %d records not %d records in file of %s\n", result, size, filename);
	}
	
	fclose(fp);

	return 0;
}
#endif

#if 0
/* modify configuration file for WWW and WebAdmin as the run-time config of LAN/WAN */
int cgi_boa_modify_restart(web_type_t type )
{
	char			*filename;
	llist			rawCfgs;
	llist			newCfgs;
	char			*service;
	network_info	*netInfo;
	interface_info *intf = NULL;
	conf_item_t	*cfg;
	int 			size;

	netInfo = cgi_get_network_info(1);
	list_create(&rawCfgs);

	if(type== WEB_TYPE_WWW)
	{
		filename = WEB_CFG_WWW;
		service = SERVICE_NAME_WWW;
		intf = netInfo->wanInfo;
		
		cfg = boa_www_cfgs;
		size = sizeof(boa_www_cfgs)/ sizeof(conf_item_t) ;
	}	
	else
	{
		filename = WEB_CFG_ADMIN;
		service = SERVICE_NAME_ADMIN;
		intf = netInfo->lanInfo;
		
		cfg = boa_admin_cfgs;
		size = sizeof(boa_admin_cfgs)/ sizeof(conf_item_t) ;
	}
	cgidebug("size of conf_item is %d\n", size);
	
	readconfig( filename, ITEM_DBL, NULL, &rawCfgs);

	if( strcasecmp(intf->addr, GET_VALUE(&rawCfgs, BOA_KEYWORD_LISTEN) ) )
	{

		list_create( &newCfgs);
		list_add_config_items( &newCfgs, cfg, size);
		list_replace_value_by_key(&newCfgs, BOA_KEYWORD_LISTEN, intf->addr);
		
		writeconfig( filename, ITEM_DBL, NULL, &newCfgs);

		cgi_service_restart(service);
	}	

	return 0;
}
#endif

/* modify configuration file for WWW and WebAdmin as the run-time config of LAN/WAN */
int cgi_boa_rewrite_configuration(interface_info *intf  )
{
	char			*filename;
	llist			rawCfgs;
	llist			newCfgs;
	char			*service;
	conf_item_t	*cfg;
	int 			size;

	list_create(&rawCfgs);

	if(intf->type== WEB_TYPE_WWW)
	{
		filename = WEB_CFG_WWW;
		service = SERVICE_NAME_WWW;
		
		cfg = boa_www_cfgs;
		size = sizeof(boa_www_cfgs)/ sizeof(conf_item_t) ;
	}	
	else
	{
		filename = WEB_CFG_ADMIN;
		service = SERVICE_NAME_ADMIN;
		
		cfg = boa_admin_cfgs;
		size = sizeof(boa_admin_cfgs)/ sizeof(conf_item_t) ;
	}
//	cgidebug("size of conf_item is %d\n", size);
	
	readconfig( filename, ITEM_DBL, NULL, &rawCfgs);

//	if( strcasecmp(intf->addr, GET_VALUE(&rawCfgs, BOA_KEYWORD_LISTEN) ) )
	if( strcasecmp(intf->name, GET_VALUE(&rawCfgs, BOA_KEYWORD_LISTEN) ) &&
		strlen(intf->name) )
	{
		list_create( &newCfgs);
		list_add_config_items( &newCfgs, cfg, size);
		list_replace_value_by_key(&newCfgs, BOA_KEYWORD_LISTEN, intf->name );
		
		writeconfig( filename, ITEM_DBL, NULL, &newCfgs);
	}	
	return 0;
}

