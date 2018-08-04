/*
* $Id: cgiVpnAccount.c,v 1.2 2007/09/01 08:48:47 lizhijie Exp $
*/

#include "_vpn.h"

static char *__vpn_server_account_info(VPN_INFO *info)
{
	char 	buf[8192*2];
	int 		length = 0;
	int 		i = 0;

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD>%s<strong>'%s'</strong>%s</TD></TR></TABLE>\r\n",
		gettext("VPN Server"), info->serverName, gettext(" has following Clients"));

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext(CGI_STR_INDEX), gettext("Client Name") );

	node *current = info->accountConfig.head;
	while(current)
	{
		if(!strcmp(current->entry.type, info->serverName) )
		{
			length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%i</TD>\r\n<TD align=\"center\">%s</TD></TR>\r\n", 
				++i, current->entry.name);
		}
		current = current->next;
	}
		
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	return strdup(buf);
}

static char *__vpn_server_account_buttons(VPN_INFO *info)
{
	char 	buf[8192*2];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"syslogForm\" method=\"post\" action=\"%s\">\n", WEB_URL_VPN );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

//	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_USER_NAME) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">%s</TD></TR>\r\n", info->userName );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_PASSWORD) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_PASSWORD, info->password);
		
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"75%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><br><p></TD></TR><TR><TD align=\"center\" colspan=2>%s </TD></TR></FROM>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) );
/*
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s<br>* %s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), 
		gettext("VPN Server Name are defined in VPN Server Configration"),
		gettext("After Server Name changed, all clients are invalidate"),
		gettext("Because authentication is Peer to Peer"), 
		
		gettext("so both Client account and Server account must be provied"));
*/	
	return strdup(buf);
}

static char *__vpn_other_account_buttons(VPN_INFO *info)
{
	char 	buf[8192*2];
	int 		length = 0;
	int 		i = 0;

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_USER_NAME), gettext(CGI_STR_PASSWORD), gettext(CGI_STR_MANAGEMENT));

	node *current = info->accountConfig.head;
	while(current)
	{
		if(!strcmp(current->entry.name, info->serverName) )
		{
			char		key[256];
			char		*value;

			sprintf(key, "%s_%s", NIC_KEYWORD_NET_ADDRESS, current->entry.type);
			value = GET_VALUE(&info->vpnRoutes, key);
			if(!value || (strlen(value)== 0) )
			{
				i++;
				length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%i</TD>\r\n<TD align=\"center\">%s</TD>\r\n", 
					i, current->entry.type);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", 
					current->entry.value);
				sprintf(key, "%s?%s=%s&%s=%s&%s=%s", WEB_URL_VPN, CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables), CGI_ACTION, CGI_ACTION_DEL, 
					PPPOE_KEYWORD_USER, current->entry.type );

				length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), key, gettext(CGI_STR_ARE_YOU_SURE) ));
			}
		}
		current = current->next;
	}
		
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	return strdup(buf);
}

static int __write_server_password(VPN_INFO *info)
{
	char 	*passwd;
	node 	*current ;
	
	passwd = GET_VALUE(&info->cgiVariables, PPPOE_KEYWORD_PASSWORD);

	current = info->accountConfig.head;
	while(current)
	{
		if( !strcasecmp(current->entry.type, info->userName)  ) 
		{
			free(current->entry.value);
			current->entry.value = cgi_new_string(passwd);
			sprintf(current->entry.value, "%s", passwd);
		}
		current = current->next;
	}

	writeconfig(CONF_PPP_CHAP, ITEM_FOR, NULL, &info->accountConfig);
	
	return 0;	
}


int	vpn_account_config(VPN_INFO *info, VPN_ACCOUNT_T type)
{
	char		*action;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		if(type == VPN_ACCOUNT_TYPE_SERVER )
		{
			cgi_info_page(gettext("VPN Server Accounts Configuration"), __vpn_server_account_info(info),__vpn_server_account_buttons(info) );
		}
		else if(type == VPN_ACCOUNT_TYPE_AS600)
		{
			cgi_info_page(gettext("VPN AS600 Accounts Configuration"), vpn_as600_account_info(info),vpn_users_buttons(info, type) );
		}
		else
		{
			cgi_info_page(gettext("VPN Other Accounts Configuration"), __vpn_other_account_buttons(info), vpn_users_buttons(info, type) );
		}
		
		return 0;
	}
	else
	{
		char	target[256];
		if(type== VPN_ACCOUNT_TYPE_OTHER || type == VPN_ACCOUNT_TYPE_AS600)
		{
			if(vpn_client_account_save(info, type) )
				return 0;
		}
		else
		{
			__write_server_password(info);
		}
		
		sprintf(target, "%s?%s=%s", WEB_URL_VPN, CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
		cgi_refresh_page(1, target, gettext(CGI_STR_SAVE_CONFIG), gettext(CGI_STR_PLEASE_WAIT) );
	}
	
	return 0;
}

