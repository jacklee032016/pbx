/*
* $Id: cgiVpnAccountAs600.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "_vpn.h"


 char *vpn_as600_account_info(VPN_INFO *info)
{
	char 	buf[8192*2];
	int 		length = 0;
	int 		i = 0;

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_USER_NAME), gettext(CGI_STR_PASSWORD), gettext(CGI_STR_NETWORK_ADDRESS), gettext(CGI_STR_MANAGEMENT));

	node *current = info->accountConfig.head;
	while(current)
	{
		if(!strcmp(current->entry.name, info->serverName) )
		{
			char		key[256];
			char		*value;

			sprintf(key, "%s_%s", NIC_KEYWORD_NET_ADDRESS, current->entry.type);
			value = GET_VALUE(&info->vpnRoutes, key);
			if(value && (strlen(value)!= 0) )
			{
				i++;
				length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%i</TD>\r\n<TD align=\"center\">%s</TD>\r\n", 
					i, current->entry.type);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", GET_VALUE(&info->vpnRoutes, key) );

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

char *vpn_users_buttons(VPN_INFO *info, VPN_ACCOUNT_T type )
{
	char 		buf[4096];
	int 			length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"partyUserForm\" method=\"post\" action=\"%s\">\n", WEB_URL_VPN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_USER_NAME) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_USER );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_PASSWORD) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"password\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_PASSWORD);

	if(type == VPN_ACCOUNT_TYPE_AS600)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_NETWORK_ADDRESS) );
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\"  onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  value=\"192.168.\" maxlength=\"50\" ></TD></TR>\r\n", 
			NIC_KEYWORD_NET_ADDRESS, NIC_KEYWORD_NET_ADDRESS );	

		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_NETMASK) );
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\"  onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  value=\"255.255.255.0\" maxlength=\"50\" ></TD></TR>\r\n", 
			NIC_KEYWORD_NETMASK, NIC_KEYWORD_NETMASK );
	}
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s</TD></TR>\r\n</FORM></TABLE>\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	if(type == VPN_ACCOUNT_TYPE_AS600)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s<br>* %s, %s</TD></TR>\r\n",
			gettext(CGI_STR_NOTE), 
			gettext("Account defined here is for AS600 clients"),
			gettext("In order to make every subnetwork behind our AS600 are interconnected"), 
			gettext("every client must has different network address of 192.168.0.0/24"));
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s<br>* %s, %s<br>* %s</TD></TR>\r\n",
			gettext(CGI_STR_NOTE), 
			gettext("Account defined here is for other clients such as Windows or PIX"),
			gettext("For compatible, only client name and password are defined"), 
			gettext("so maybe some route entry must be added manually"),
			gettext("One account can be used by more than one client"));
	}

	return strdup(buf);
}

	
int vpn_client_account_save(VPN_INFO *info, VPN_ACCOUNT_T type)
{
	char 	*userName;
	char		*action;
	char 	msg[256];
	node 	*current ;
	char		key[256];
	
	userName = GET_VALUE(&info->cgiVariables, PPPOE_KEYWORD_USER);

	current = info->accountConfig.head;
	while(current)
	{
		if( !strcasecmp(current->entry.type, userName)  ) 
		{
			break;
		}
		current = current->next;
	}

	action = GET_CGI_ACTION(&info->cgiVariables) ;
	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s')\" %s", gettext(CGI_STR_USER_NAME), userName, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );

		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}
	cgidebug("Action : %s\n", action);

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{
		list_append_raw(&info->accountConfig, userName, info->password, 
			info->userName, GET_VALUE(&info->cgiVariables,PPPOE_KEYWORD_PASSWORD) );
		list_append_raw(&info->accountConfig, info->userName, GET_VALUE(&info->cgiVariables,PPPOE_KEYWORD_PASSWORD) , 
			userName, info->password);

		if(type == VPN_ACCOUNT_TYPE_AS600)
		{
			sprintf(key, "%s_%s", NIC_KEYWORD_NET_ADDRESS, userName);
			list_append_raw(&info->vpnRoutes, key, GET_VALUE(&info->cgiVariables,NIC_KEYWORD_NET_ADDRESS), NULL, NULL );

			sprintf(key, "%s_%s", NIC_KEYWORD_NETMASK, userName);
			list_append_raw(&info->vpnRoutes, key, GET_VALUE(&info->cgiVariables,NIC_KEYWORD_NETMASK), NULL, NULL );
		}
	}
	else
	{/* delete */
		node 	*next;
		current =info->accountConfig.head;

		while(current)
		{
			next = current->next;
			
			if( !strcasecmp(current->entry.type, userName)  ) 
			{
				list_delete_node(&info->accountConfig, current);
			}
			else if( !strcasecmp(current->entry.name, userName)  ) 
			{
				list_delete_node(&info->accountConfig, current);
			}
			
			current = next;
		}

		if(type == VPN_ACCOUNT_TYPE_AS600)
		{
			current =info->vpnRoutes.head;

			while(current)
			{
				next = current->next;
				
				sprintf(key, "%s_%s", NIC_KEYWORD_NET_ADDRESS, userName);
				if( !strcasecmp(current->entry.name, key)  ) 
				{
					list_delete_node(&info->vpnRoutes, current);
				}

				sprintf(key, "%s_%s", NIC_KEYWORD_NETMASK, userName);
				if( !strcasecmp(current->entry.name, key)  ) 
				{
					list_delete_node(&info->vpnRoutes, current);
				}

				current = next;
			}
		}
	}

	if(type == VPN_ACCOUNT_TYPE_AS600)
	{
		writeconfig(VPN_ROUTE_CONFIG_FILE, ITEM_EQU, NULL, &info->vpnRoutes);
	}

	writeconfig(CONF_PPP_CHAP, ITEM_FOR, NULL, &info->accountConfig);
	
	return 0;
}

