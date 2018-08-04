/*
* $Id: net_routes.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "net_routes.h"

static ROUTE_INFO	_routeInfo;

cgi_route_entry_t		*runtimeRoutes = NULL;

cgi_route_entry_t *__net_routes_get_entry(ROUTE_INFO *info)
{
	cgi_route_entry_t 		*entry;
	char				*value;
	int				isError = 0;
	char				msg[2048];
	int				len = 0;
	char				*type;

	entry = (cgi_route_entry_t *)malloc(sizeof(cgi_route_entry_t) );
	if(!entry )
		return NULL;

	memset(entry, 0, sizeof(cgi_route_entry_t) );

	type = GET_VALUE(&info->cgiVariables, NET_ROUTE_TYPE);
	if(!strlen(type) )
	{
		len += CGI_SPRINTF(msg, len, "'%s'", gettext("Route Type") );
		isError = 1;
	}
	else
		sprintf( entry->type, "%s", type);


	value = GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if(value == NULL)
	{
		len += CGI_SPRINTF(msg, len, "'%s'", gettext("Operator") );
		isError = 1;
	}
	else
	{
		sprintf( entry->op, "%s", value);
	}

	value = GET_VALUE(&info->cgiVariables, NET_ROUTE_NAME_DESTINATION );
	if(!value ||
		( strcasecmp(value, NET_ROUTE_TYPE_DEFAULT) && ! isValidateIpAddress(value) ))
	{
		len += CGI_SPRINTF(msg, len, "'%s'", gettext("Destination") );
		isError = 1;
	}
	else
		sprintf( entry->dest, "%s",value);

//	if(! strcasecmp( entry->op ,NET_ROUTE_OP_ADD) )
	{

		if(!strcasecmp(NET_ROUTE_TYPE_NET, type)!= 0 )
		{
			value = GET_VALUE( &info->cgiVariables, NET_ROUTE_NAME_NETMASK);
			if(!value ||! isValidateIpAddress(value))
			{
				len += CGI_SPRINTF(msg, len, " '%s'", gettext(CGI_STR_NETMASK) );
				isError = 1;
			}
			else
				sprintf( entry->mask, "%s",value);
		}
		else if(!strcasecmp(NET_ROUTE_TYPE_HOST, type)!= 0 )
			sprintf( entry->mask, "%s", "");
//			sprintf( entry->mask,"255.255.255.255");

		value = GET_VALUE(&info->cgiVariables, NET_ROUTE_NAME_GATEWAY);
		if(!value ||! isValidateIpAddress(value))
		{
			len += CGI_SPRINTF(msg, len, " '%s'", gettext(CGI_STR_GATEWAY) );
			isError = 1;
		}
		else
		{
			sprintf( entry->gateway, "%s",value);
			cgidebug("gw =%s\n", entry->gateway);
		}	

		value = GET_VALUE(&info->cgiVariables, NET_ROUTE_NAME_INTERFACE);
		if(!value)
		{
			sprintf( entry->iface, "%s",ETH0);
//			len += CGI_SPRINTF(msg, len, "'%s'", gettext(CGI_STR_NETMASK) );
//			isError = 1;
		}
		else
			sprintf( entry->iface, "%s",value);
	
	}
	
	if(isError )
	{
		len += CGI_SPRINTF(msg, len, "%s", gettext("is invalidate value") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return NULL;
	}
	
	return entry;
}

static char *__net_routes_output_entry(cgi_route_entry_t *proute)
{
	char		href[1024];
	int 		len = 0;
	char		target[256];
	
	len += CGI_SPRINTF(href, len, "\t<tr><td align=\"center\">%s</td>\n", proute->dest);
	len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", proute->mask);
	len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", proute->gateway);
	len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", proute->flag);
	//printf("<td align=\"center\">%s</td>", proute->metric);
	//printf("<td align=\"center\">%s</td>", proute->ref);
	len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", proute->use);
	len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", proute->iface);

	if(strchr(proute->flag, 'G' ) || strchr(proute->flag, 'H' ) )
	{
		sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_SYSTEM_ROUTE, 
			CGI_KEYWORD_OPERATOR, NET_ROUTE_OP_DEL, 
			NET_ROUTE_TYPE, strchr(proute->flag, 'H')? NET_ROUTE_TYPE_HOST:NET_ROUTE_TYPE_NET, 
			NET_ROUTE_NAME_DESTINATION, proute->dest,
			NET_ROUTE_NAME_NETMASK, proute->mask,
			NET_ROUTE_NAME_GATEWAY, proute->gateway);
		
		len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\">%s</td>\n", 
			cgi_button_alert(gettext(CGI_STR_DELETE), target, gettext(CGI_STR_ARE_YOU_SURE)  ) );
	}
	else
		len += CGI_SPRINTF(href, len, "\t\t<td align=\"center\"></td>\n" );
	
	len += CGI_SPRINTF(href, len, "</tr>");

	return strdup(href);
}

static char *__net_routes_select_buttons()
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"routeForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSTEM_ROUTE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, NET_ROUTE_TYPE );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\r\n", gettext("Route Type") );

	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", NET_ROUTE_TYPE );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
		NET_ROUTE_TYPE_NET,  gettext("Network Route"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
		NET_ROUTE_TYPE_HOST,  gettext("Host Route"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
		NET_ROUTE_TYPE_DEFAULT,  gettext("Default Route"));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext(CGI_STR_NEXT) ) );//, msg) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_ROUTE) );

	return strdup(buf);
}


static char *__net_routes_add_buttons(ROUTE_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	char		*type = GET_VALUE(&info->cgiVariables, NET_ROUTE_TYPE);
	char		*routeType;
	
	interface_info *wanInfo = &info->netInfo->wanInfo, *lanInfo = &info->netInfo->lanInfo;
	
	if(!strcasecmp(type, NET_ROUTE_TYPE_DEFAULT) )
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TR>\r\n", 
			gettext("Default Route") );
		routeType = NET_ROUTE_TYPE_DEFAULT;
	}
	else if(!strcasecmp(type, NET_ROUTE_TYPE_HOST) )
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TR>\r\n", 
			gettext("Host Route") );
		routeType = NET_ROUTE_TYPE_HOST;
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TR>\r\n", 
			gettext("Network Route") );
		routeType = NET_ROUTE_TYPE_NET;
	}
	
	length += CGI_SPRINTF(buf, length, "<form name=\"routeForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSTEM_ROUTE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, NET_ROUTE_OP_ADD );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NET_ROUTE_TYPE, routeType );

	if(strcasecmp(NET_ROUTE_TYPE_DEFAULT, type )!= 0 )
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD bgcolor=\"#cccccc\" align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext("Destination"), NET_ROUTE_NAME_DESTINATION, NET_ROUTE_NAME_DESTINATION);
	}
	else
		length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NET_ROUTE_NAME_DESTINATION, NET_ROUTE_TYPE_DEFAULT);
		

	if(!strcasecmp(NET_ROUTE_TYPE_NET, type)!= 0 )
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD bgcolor=\"#cccccc\" align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext(CGI_STR_NETMASK), NET_ROUTE_NAME_NETMASK, NET_ROUTE_NAME_NETMASK);
	}

	length += CGI_SPRINTF(buf,length, "<TR><TD bgcolor=\"#cccccc\" align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_GATEWAY), NET_ROUTE_NAME_GATEWAY, NET_ROUTE_NAME_GATEWAY);

	length += CGI_SPRINTF(buf, length, "\t<TD bgcolor=\"#cccccc\" align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext(CGI_STR_DEVICE), NET_ROUTE_NAME_INTERFACE);
	if( wanInfo->state == 1  )
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", wanInfo->name,  gettext("WAN") );
	if( lanInfo->state == 1 )
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", lanInfo->name,  gettext("LAN") );
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext( CGI_STR_ACTIVATE ) ) );//, msg) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_ROUTE) );
	
	return strdup(buf);
}

static char *__net_routes_status_list( )
{
	char buf[4096];
	int 	length = 0;
	cgi_route_entry_t *route = runtimeRoutes;
	
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>\n", 
		gettext("Destination"),gettext(CGI_STR_NETMASK),gettext(CGI_STR_GATEWAY),gettext("Flags"),gettext("Cast"),gettext(CGI_STR_DEVICE),gettext(CGI_STR_MANAGEMENT)  );

	while(route)
	{
		length += CGI_SPRINTF(buf, length, "%s", __net_routes_output_entry(route) );
		route = route->next;	
	}

	return strdup(buf);
}

static int __net_routes_init(ROUTE_INFO *info)
{
	cgi_html_header();

	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);
	
	list_create(&info->routeCfgs);

	if (readconfig( SYS_CFG_LOCALE, ITEM_EQU, NULL, &info->routeCfgs) != 0)
	{
		return -1;
	}

	info->netInfo = cgi_get_network_info(1);

	if(net_routes_read_route_table() )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(SYSTIME_STR_SYSTEM_ERROR) );
		return -1;
	}
	return 0;
}

int main()
{
	char *cmd=NULL;
	char	 	title[256];
	int		len = 0;
	ROUTE_INFO	*info= &_routeInfo;

	if(__net_routes_init(info) )
	{
		cgi_error_page(NULL, gettext("Route Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	len += CGI_SPRINTF(title, len, "%s : %s 1, ", gettext(NET_ROUTE_STR_CONFIGURATION), gettext(NET_ROUTE_STR_STEP) );

	cmd = GET_VALUE( &info->cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, NET_ROUTE_OP_SAVE) )
	{
		if( cmd && !strcasecmp(cmd, NET_ROUTE_OP_SAVE) )
				net_routes_write_conf();

		len += CGI_SPRINTF(title, len, "%s", gettext("Route Type") );
		cgi_info_page(title , __net_routes_status_list(), __net_routes_select_buttons()  );
		return 0;
	}
	else if(!strcasecmp(cmd, NET_ROUTE_TYPE) )
	{
		len += CGI_SPRINTF(title, len, "%s", gettext("Add Route") );
		cgi_info_page(title , "", __net_routes_add_buttons(info)  );
		return 0;
	}
	else if(!strcasecmp(cmd, NET_ROUTE_OP_ADD) || !strcasecmp(cmd, NET_ROUTE_OP_DEL ) )
	{
		cgi_route_entry_t	*route = __net_routes_get_entry(info);
		if(route)
			net_routes_update(route);
		return 0;
	}
	else
	{
		cgidebug("Language command '%s' is not found\n", cmd );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}	
	
