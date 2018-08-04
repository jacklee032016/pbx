/*
* $Id: cgiVpnClient.c,v 1.3 2007/09/01 14:33:38 lizhijie Exp $
*/

#include "_vpn.h"

static char *__vpn_client_buttons(VPN_INFO *info )
{
	char 		buf[8192*2];
	int 			length = 0;
	char			*clientName;

	clientName = GET_VALUE(&info->clientProvider, VPN_CLIENT_KEY_USER_NAME);
	
	length += CGI_SPRINTF(buf, length, "<form name=\"fwTimerForm\" method=\"post\" action=\"%s\">\n", WEB_URL_VPN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"75%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong>\r\n", gettext("VPN Server") );
	
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" title=\"%s\"></TD></TR>\r\n", 
		VPN_KEYWORK_SERVER_IP, GET_VALUE(&info->vpnRoutes, VPN_KEYWORK_SERVER_IP), gettext("IP Address of FQDN of VPN(PPTP) Server"));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Authentication Server Account") );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Server Name"),PPPOE_KEYWORD_SERVER_NAME, info->serverName);

	node *current = info->accountConfig.head;
	while(current)
	{
		if((!strcmp(current->entry.type,info->serverName ) )&& 
			(!strcmp(current->entry.name, clientName )) )
		{
			break;
		}
		current = current->next;
	}
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_PASSWORD),PPPOE_KEYWORD_PASSWORD, (current!= NULL)?current->entry.value:"" );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Authentication Client Account") );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_USER_NAME),VPN_CLIENT_KEY_USER_NAME, clientName );

	current = info->accountConfig.head;
	while(current)
	{
		if(!strcmp(current->entry.type, clientName) )
		{
			break;
		}
		current = current->next;
	}
	
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_PASSWORD),VPN_CLIENT_KEY_PASSWD, (current!=NULL)? current->entry.value:"" );
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_VPN) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s!<br>* %s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), 
		gettext("Authentication Account must be same as that defined in VPN Server"),
		gettext("Because authentication is Peer to Peer"), 
		gettext("so both Client account and Server account must be provied"));
	
	return strdup(buf);
}

int  ppp_chap_add_or_replace_account(llist *chapAccounts, char *userName, char *passwd, char *peerName)
{
	node *current = chapAccounts->head;
	while(current)
	{
		if(!strcmp(current->entry.type, userName) )
		{
			break;
		}
		current = current->next;
	}

	if(current!= NULL)
	{
		list_delete_node(chapAccounts, current);
	}
	
	list_append_raw(chapAccounts, peerName, passwd, userName, PPPOE_KEYWORD_SERVER_NAME_ASTERIC);

	return 0;
}

static int __vpn_client_save_task(VPN_INFO *info )
{
	char		value[256];
	char		*clientName, *serverName;
	
	clientName = GET_VALUE(&info->cgiVariables, VPN_CLIENT_KEY_USER_NAME);
	serverName = GET_VALUE(&info->cgiVariables, PPPOE_KEYWORD_SERVER_NAME);
	sprintf(value, "\"pptp %s --nolaunchpppd\"", GET_VALUE(&info->cgiVariables,VPN_KEYWORK_SERVER_IP) );
	list_replace_value_by_key(&info->clientProvider, VPN_CLIENT_KEY_DEVICE_NAME, value);
	list_replace_value_by_key(&info->clientProvider, VPN_CLIENT_KEY_USER_NAME, clientName);
	list_replace_value_by_key(&info->clientProvider, VPN_CLIENT_KEY_REMOTE_NAME, serverName);
	
	writeconfig(VPN_CLIENT_PROVIDER, ITEM_DBL, NULL, &info->clientProvider);
	
	list_replace_value_by_key(&info->vpnRoutes, VPN_KEYWORK_SERVER_IP, GET_VALUE(&info->cgiVariables, VPN_KEYWORK_SERVER_IP));
	writeconfig(VPN_ROUTE_CONFIG_FILE, ITEM_EQU, NULL, &info->vpnRoutes);


	ppp_chap_add_or_replace_account(&info->accountConfig, clientName, 
		GET_VALUE(&info->cgiVariables, VPN_CLIENT_KEY_PASSWD), serverName );
	
	ppp_chap_add_or_replace_account(&info->accountConfig,serverName , 
		GET_VALUE(&info->cgiVariables, PPPOE_KEYWORD_PASSWORD), clientName );
	
	writeconfig(CONF_PPP_CHAP, ITEM_FOR, NULL, &info->accountConfig);
	return 0;
}

int vpn_client_config(VPN_INFO *info)
{
	char *action;

	action = GET_CGI_ACTION(&info->cgiVariables);

	if(!strcasecmp(action, CGI_ACTION_ADD ) ||
		!strcasecmp(action, CGI_ACTION_DEL)  )
	{
		__vpn_client_save_task(info);
			
		char	target[256];
		sprintf(target, "%s?%s=%s", WEB_URL_VPN, CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
		cgi_refresh_page(1, target, gettext(CGI_STR_SAVE_CONFIG), gettext(CGI_STR_PLEASE_WAIT) );

		return 0;
	}
	

	cgi_info_page(gettext("VPN Client Configuration"), "", __vpn_client_buttons(info) );
		
	return 0;	
}

