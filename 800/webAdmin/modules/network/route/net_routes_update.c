/*
* $Id: net_routes_update.c,v 1.2 2007/02/07 19:04:16 lizhijie Exp $
*/

#include "net_routes.h"

int net_routes_write_conf()
{
	FILE 			*fp;
	cgi_route_entry_t *entry = runtimeRoutes;
	int				i = 0;
	
	if((fp=fopen(NET_ROUTE_CONF,"w"))==NULL)
		return -1;

	fprintf(fp, "# Routes Info Configuration\n\n");
	
	while( entry )
	{
		if(strchr( entry->flag, 'G' ) || strchr(entry->flag, 'H' ) )
		{
			fprintf(fp , "%s_%d=%s\n", NET_ROUTE_TYPE,  i, entry->type);
			fprintf(fp , "%s_%d=%s\n", NET_ROUTE_NAME_DESTINATION,  i, entry->dest );
			fprintf(fp , "%s_%d=%s\n", NET_ROUTE_NAME_GATEWAY,  i, entry->gateway);
			fprintf(fp , "%s_%d=%s\n", NET_ROUTE_NAME_NETMASK,  i, entry->mask);
			fprintf(fp , "%s_%d=%s\n\n", NET_ROUTE_NAME_INTERFACE,  i, entry->iface );
			i++;
		}

		entry = entry->next;

	}

	fprintf(fp, "ROUTE_NUM=%d\n\n", i);
	
	fclose(fp);		

	return 0;
}

int net_routes_read_route_table( )
{
#define			ROUTE_TMP				"/tmp/route.txt"
	int 				i,linelen;
	FILE 			*fp;
	char 			line[PARSE_FILE_MAX_LENGTH];
	cgi_route_entry_t 		*route, *last = NULL;
	int				count;
	
	system(NET_ROUTE_COMMAND" -n > "ROUTE_TMP);
	
	if( (fp=fopen(ROUTE_TMP,"r") )==NULL )
		return -1;
	
	memset(line,0,sizeof(line));
	while (fgets(line,PARSE_FILE_MAX_LENGTH, fp))
	{
		linelen=strlen(line);
		for(i=linelen-1;i>0;i--)
		{
			if( (line[i]=='\t' ) || ( line[i]=='\n') || (line[i]=='\r') )
			line[i]=' ';
		}
		
		if( (line[0]>='0') && (line[0]<='9') )
		{
			route = (cgi_route_entry_t *)malloc(sizeof(cgi_route_entry_t) );
			if(!route )
				return -1;
			
			memset( route, 0, sizeof(cgi_route_entry_t) );

//			cgidebug("raw = %s\n", line);
			
			count = sscanf(line,"%s %s %s %s %s %s %s %s",
				route->dest,route->gateway,route->mask,route->flag,
				route->metric,route->ref,route->use,route->iface);
			if(count != 8)
				return -1;

			if( strchr(route->flag, 'H' ) )
				sprintf(route->type, "%s", NET_ROUTE_TYPE_HOST);
			else if( !strcasecmp(route->dest, "0.0.0.0") )
				sprintf(route->type, "%s", NET_ROUTE_TYPE_DEFAULT );
			else
				sprintf(route->type, "%s", NET_ROUTE_TYPE_NET);
				
			if(runtimeRoutes == NULL)
				runtimeRoutes = route;
			
			if(last )
				last->next = route;
			last = route;

		}
		memset(line,0,sizeof(line));
	}

	return 0;
}

static int __net_routes_manipuldate(cgi_route_entry_t *route)
{
	char cmd[256];
	char	mask[64];
	char	type[64];

	sprintf(mask, "%s %s", (strlen(route->mask ))?"netmask":"",  (strlen(route->mask ))?route->mask:"" );
	sprintf(type, "%s%s", 
		(!strcasecmp(route->type, NET_ROUTE_TYPE_DEFAULT))?"":"-"  ,
		(!strcasecmp(route->type, NET_ROUTE_TYPE_DEFAULT))?"":route->type  );

//	if(!strcasecmp(route->op, NET_ROUTE_OP_ADD) )
	{
		sprintf(cmd,"%s %s %s %s %s gw %s",
			NET_ROUTE_COMMAND, route->op, type, 
			route->dest, mask, route->gateway);//, route->iface);	
	}
//	else
//		sprintf(cmd,"%s %s %s %s ",
//			NET_ROUTE_COMMAND, route->op, type, 
//			route->dest);
	cgidebug("\n\t%s\n", cmd);
	
	system(cmd);

	return 0;
}


int net_routes_update(cgi_route_entry_t *route)
{
	char target[128];
	if(!route )
		return -1;

	__net_routes_manipuldate(route);

	sprintf(target, "%s?%s=%s", WEB_URL_SYSTEM_ROUTE, CGI_KEYWORD_OPERATOR, NET_ROUTE_OP_SAVE );
	cgi_refresh_page(1,target , gettext(CGI_STR_WAIT), gettext("Update Route now") );
	return 0;
}


