/*
* $Id: servicesMiscDhcp.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"

#define	DHCP_CFG_KEYWORD_START			"start"
#define	DHCP_CFG_KEYWORD_END			"end"
#define	DHCP_CFG_KEYWORD_INTFACE		"interface"
#define	DHCP_CFG_KEYWORD_SUBNET			"subnet"
#define	DHCP_CFG_KEYWORD_ROUTER			"router"
#define	DHCP_CFG_KEYWORD_DNS			"dns"
#define	DHCP_CFG_KEYWORD_LEASE			"lease"


#define	DHCP_KEYWORD_OPTION				"opt"

const char	*dhcp_options[]=
{
	DHCP_CFG_KEYWORD_ROUTER,
	DHCP_CFG_KEYWORD_SUBNET,
	DHCP_CFG_KEYWORD_DNS,
	DHCP_CFG_KEYWORD_LEASE
};

static llist  	dhcpCfgs;


static char *__servicesMisc_dhcp_status()
{
	char 	buf[4096];
	int 		length = 0;
	
	length += CGI_SPRINTF(buf,length, "<strong>%s %s</strong>\n", 
		gettext("All Parameters of DHCP"), gettext(CGI_STR_SET_ON_PAGAE) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("DHCP Server Status"), cgi_service_get_state_name(SERVICE_NAME_DHCP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *__servicesMisc_dhcp_buttons()
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"dhcpForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_DHCP);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Start IP Address"),DHCP_CFG_KEYWORD_START, DHCP_CFG_KEYWORD_START, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_START));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("End IP Address"),DHCP_CFG_KEYWORD_END, DHCP_CFG_KEYWORD_END,  GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_END));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_GATEWAY),DHCP_CFG_KEYWORD_ROUTER, DHCP_CFG_KEYWORD_ROUTER,GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_ROUTER));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_NETMASK),DHCP_CFG_KEYWORD_SUBNET,DHCP_CFG_KEYWORD_SUBNET, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_SUBNET));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_DNS_SERVER),DHCP_CFG_KEYWORD_DNS,DHCP_CFG_KEYWORD_DNS, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_DNS));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("Release Duration"),DHCP_CFG_KEYWORD_LEASE, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_LEASE), gettext(CGI_STR_SECONDS) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\r\n", 
		gettext("Interface"),DHCP_CFG_KEYWORD_INTFACE, gettext(CGI_STR_LAN_PORT));
//		gettext("Interface"),DHCP_CFG_KEYWORD_INTFACE, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_INTFACE));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 >%s %s %s</TD></TR>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_submit_button(gettext(CGI_STR_SUBMIT)), cgi_help_button(CGI_HELP_MSG_SERVICES_DHCP));

	return strdup(buf);
}

int	__servicesMisc_dhcp_convert_cfg(llist *cgis)
{
	node	*current;

	current = cgis->head;
	if(!current )
		return -1;
	
	while (current != 0) 
	{
		entrytype  *_node = &current->entry;
		if(!strcasecmp(_node->name, DHCP_KEYWORD_OPTION) )
		{
			sprintf(_node->name, "%s", _node->type);
		}
		else
		{
			sprintf(_node->value, "%s", _node->type);
		}
		_node->type[0] = '\0';
		current = current->next;
	}

	return 0;
}

int	__servicesMisc_dhcp_convert_cgi(llist *cgis)
{
	node	*current;
	int		i;
	
	current = cgis->head;
	if(!current )
		return -1;
	
	while (current != 0) 
	{
		int isTri = 0;
		entrytype  *_node = &current->entry;

		for(i=0; i<4/* sizeof(dhcp_options)/sizeof(dhcp_options[0])*/; i++)
		{
			if(!strcasecmp(_node->name, dhcp_options[i]) )
			{
				sprintf(_node->type, "%s", _node->name);
				sprintf(_node->name, "%s", DHCP_KEYWORD_OPTION);
				isTri = 1;
			}
		}
		
		if(!isTri)
		{
			sprintf(_node->type, "%s", _node->value);
			_node->value[0] = '\0';
		}
		current = current->next;
	}

	return 0;
}

int	servicesMisc_Dhcp(SERVICE_INFO *info)
{
	char		*action;
	list_create(&dhcpCfgs);

	if (readconfig(DHCP_CONF_FILE, ITEM_TRI, NULL, &dhcpCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		__servicesMisc_dhcp_convert_cfg(&dhcpCfgs);
		
		cgi_info_page(gettext(CGI_STR_DHCP_CONFIG), __servicesMisc_dhcp_status(),__servicesMisc_dhcp_buttons() );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char	target[256];
		
		__servicesMisc_dhcp_convert_cfg(&dhcpCfgs);
		list_replace_with_new_list(&dhcpCfgs, &info->cgiVariables );
		__servicesMisc_dhcp_convert_cgi(&dhcpCfgs);

		sprintf(target, "%s?%s=%s", WEB_URL_SERVICES, CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_DHCP );
		cgi_refresh_page(1, target, gettext("Save DHCP Config File"), gettext(CGI_STR_PLEASE_WAIT) );

		writeconfig(DHCP_CONF_FILE, ITEM_TRI, NULL, &dhcpCfgs);
	}

	return 0;
}

