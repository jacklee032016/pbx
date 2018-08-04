/*
* $Id: cgi_lan_dhcp.c,v 1.7 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"
#include "cgi_syscfg.h"

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

static char *__cgi_lan_dhcp_status(LAN_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	
	length += CGI_SPRINTF(buf,length, "<strong>%s : %s %s</strong>\n", info->info->name,
		gettext("All Parameters of DHCP"), gettext(CGI_STR_SET_ON_PAGAE) );
#if 0
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("DHCP Server Status"), cgi_get_program_state_name(info->info->dhcpCmd ), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );
#else
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td></TR>",
		gettext("DHCP Server Status"), cgi_get_program_state_name(info->info->dhcpCmd ) );
#endif

	return strdup(buf);
}

static char *__cgi_lan_dhcp_buttons(LAN_INFO *info, int where)
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"dhcpForm\" method=\"post\" action=\"%s\">\n", info->info->url);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, info->info->urlDhcp);
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
		gettext("Interface"),DHCP_CFG_KEYWORD_INTFACE, GET_VALUE( &dhcpCfgs, DHCP_CFG_KEYWORD_INTFACE));
//		gettext("Interface"),DHCP_CFG_KEYWORD_INTFACE, gettext(CGI_STR_LAN_PORT));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2>\r\n" ); 
	if(where == CGI_LAN_WHERE_WIZARD )
	{
		length += sprintf(buf+length, "%s", cgi_button(gettext(CGI_STR_PREV) , WEB_URL_QUICK_INSTALL"?"CGI_KEYWORD_OPERATOR"="CGI_QUICK_OP_LAN ) );
	}

	length += CGI_SPRINTF(buf,length, "%s %s %s</TD></TR>\r\n", 
		cgi_submit_button( (where == CGI_LAN_WHERE_WIZARD)?gettext("Finish") :gettext(CGI_STR_SAVE) ) , cgi_submit_button(gettext(CGI_STR_SUBMIT)), cgi_help_button(CGI_HELP_MSG_SERVICES_DHCP));

	return strdup(buf);
}

int	__cgi_lan_dhcp_convert_cfg(llist *cgis)
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

int	__cgi_lan_dhcp_convert_cgi(llist *cgis)
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

int	cgi_lan_dhcp(LAN_INFO *info, int where)
{
	char		*action;
	list_create(&dhcpCfgs);

	if (readconfig(info->info->dhcpConfigFile, ITEM_TRI_DHCP, NULL, &dhcpCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE( info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) || strcasecmp(action, CGI_ACTION_ADD) )
	{
		char	title[512];
		__cgi_lan_dhcp_convert_cfg(&dhcpCfgs);

		if(where == CGI_LAN_WHERE_WIZARD )
			sprintf(title, "%s : %s", gettext(CGI_STR_QUICK_INSTALL), gettext(CGI_STR_DHCP_CONFIG));
		else
			sprintf(title, "%s", gettext(CGI_STR_DHCP_CONFIG));
		cgi_info_page(title, __cgi_lan_dhcp_status(info),__cgi_lan_dhcp_buttons(info, where) );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char	target[256];
		
		__cgi_lan_dhcp_convert_cfg(&dhcpCfgs);
		list_replace_with_new_list(&dhcpCfgs, info->cgiVariables );
		__cgi_lan_dhcp_convert_cgi(&dhcpCfgs);

		writeconfig(info->info->dhcpConfigFile, ITEM_TRI, NULL, &dhcpCfgs);
		
		if( where == CGI_LAN_WHERE_WIZARD)
			return quickWizardFinish();
		
		sprintf(target, "%s?%s=%s", info->info->url, CGI_KEYWORD_OPERATOR, info->info->urlDhcp );
		cgi_refresh_page(1, target, gettext("Save DHCP Config File"), gettext(CGI_STR_PLEASE_WAIT) );

	}

	return 0;
}

