/*
* $Id: syscfg_wan.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "sysconf.h"

static char *__cgi_output_wan(SYSCFG_INFO *info)
{
	char msg[4096];
	int length = 0;
	char *protocol;

	protocol = GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL);
	if(!protocol || !strlen(protocol) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), "protocol for WAN is invalidate");
		return NULL;
	}

	if(! strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_PPPOE) )
	{
//		cgi_refresh_page(0, "", gettext(CGI_STR_WAIT), gettext("Redirect to PPPOE Configuration\n") );
//		return NULL;
		return  syscfg_pppoe_checkup();
	}
	else if(! strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_DHCP) )
	{
		interface_info wan;
		if( get_nic_info(&wan, ETH0) <0 )
		{
			length += sprintf(msg+length, "<tr><font color=\"#ff0000\">%s<br>\n", gettext("DHCP Fail")  );
			length += sprintf(msg+length, "%s<br>", gettext("Maybe no DHCP Server is available") );
			length += sprintf(msg+length, "%s", gettext("Or cable is not connected") );
			length += sprintf(msg+length, "</font></tr>\n" );
		}
		else
		{
			length += sprintf(msg+length, "<tr><td colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n",
				gettext("DHCP WAN Configuration")  );

			length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" >%s</td></tr>\n",
				gettext(CGI_STR_IP_ADDRESS) , wan.addr );
		
			length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" >%s</td></tr>\n",
				gettext(CGI_STR_NETMASK), wan.netmask );
		}
//		length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" >%s</td></tr>",
//			gettext(CGI_STR_GATEWAY), GET_VALUE( &wanCfgs, NIC_KEYWORD_GATEWAY) );
	}
	else //if(! strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_STATIC ) )
	{
		length += sprintf(msg+length, "<tr><td colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n",
			gettext("Static WAN Configuration")  );

		length += sprintf(msg+length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></td><td>%s</td></tr>\n",
			gettext(CGI_STR_IP_ADDRESS) , GET_VALUE( &info->wanCfgs, NIC_KEYWORD_IP_ADDRESS ) );
	
		length += sprintf(msg+length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></td><td>%s</td></tr>\n",
			gettext(CGI_STR_NETMASK), GET_VALUE( &info->wanCfgs, NIC_KEYWORD_NETMASK) );

		length += sprintf(msg+length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></td><td>%s</td></tr>\n",
			gettext(CGI_STR_GATEWAY), GET_VALUE( &info->wanCfgs, NIC_KEYWORD_GATEWAY) );

		length += sprintf(msg+length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></td><td>%s</td></tr>\n",
			gettext(CGI_STR_FIRST_DNS_SERVER), GET_VALUE( &info->wanCfgs, PPPOE_KEYWORD_DNS1 ) );

		length += sprintf(msg+length, "<tr><td align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></td><td>%s</td></tr>\n",
			gettext(CGI_STR_SECOND_DNS_SERVER), GET_VALUE( &info->wanCfgs, PPPOE_KEYWORD_DNS2) );
		
		
	}
#if 0	
	else
	{
		return NULL;
	}
#endif

	return strdup(msg);

}

static char *__cgi_wan_select_buttons( SYSCFG_INFO *info)
{
	char buf[2048];
	int length = 0;

	length += sprintf(buf+length, "<form name=\"wanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSCONFIG );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ADD );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );

	length += sprintf(buf+length, "<TR><TD colspan=2 align=\"center\"><strong>%s</strong></TD></TR>", 
		gettext("Select Protocol for WAN Port") );

	length += sprintf(buf+length, "<TR><TD colspan=2 align=\"center\"><select name=\"%s\"  maxlength=\"30\">\n", NIC_KEYWORD_PROTOCOL );

	length += sprintf(buf+length, "<option value=\"%s\" %s>%s</option>", NIC_KEYWORD_PROTOCOL_STATIC, 
		(!strcasecmp(NIC_KEYWORD_PROTOCOL_STATIC, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL)))?"selected":"" , 
		gettext("static") );
	length += sprintf(buf+length, "<option value=\"%s\" %s>%s</option>", NIC_KEYWORD_PROTOCOL_DHCP, 
		(!strcasecmp(NIC_KEYWORD_PROTOCOL_DHCP, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL)))?"selected":"", 
		gettext("DHCP") );
	length += sprintf(buf+length, "<option value=\"%s\" %s>%s</option>", NIC_KEYWORD_PROTOCOL_PPPOE, 
		(!strcasecmp(NIC_KEYWORD_PROTOCOL_PPPOE, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL) ) )?"selected":"" , 
		gettext("PPPOE") );

	length += sprintf(buf+length, "</select></TD></TR>\r\n");
	
	length += sprintf(buf+length, "<TR><TD><br><br></TD></TR>" );
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext("OK") ) );
	length += sprintf(buf+length, "</TD>\r\n");
#if 0
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_reset_button(gettext(CGI_STR_RESET) ) );
	length += sprintf(buf+length, "</TD>\r\n");
#endif

	length += sprintf(buf+length, "<TD  align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_WAN_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}

#define	FORM_NAME			"wanForm"

static char *__cgi_wan_add_buttons(SYSCFG_INFO *info)
{
	char buf[2048];
	int 	length = 0;
	char	msg[256];

	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_SYSCONFIG );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_WAN);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC);

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", 
		gettext("WAN IP Address") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS, NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_IP_ADDRESS) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext("WAN Netmask") );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK, NIC_KEYWORD_NETMASK, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_NETMASK ) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_GATEWAY) );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_GATEWAY,NIC_KEYWORD_GATEWAY, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_GATEWAY ) );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_FIRST_DNS_SERVER) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS1,PPPOE_KEYWORD_DNS1, GET_VALUE(&info->wanCfgs, PPPOE_KEYWORD_DNS1 ) );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_SECOND_DNS_SERVER) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS2, PPPOE_KEYWORD_DNS2, GET_VALUE(&info->wanCfgs, PPPOE_KEYWORD_DNS2) );
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	sprintf(msg, "%s,%s?", gettext("WWW Server will reboot after WAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );
	length += sprintf(buf+length, cgi_submit_button_with_alert(gettext(CGI_STR_SAVE), msg, FORM_NAME ) );
	length += sprintf(buf+length, "</TD>\r\n");
#if 0
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_reset_button(gettext(CGI_STR_RESET) ) );
	length += sprintf(buf+length, "</TD>\r\n");
#endif

	length += sprintf(buf+length, "<TD  align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_WAN_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}

void __cgi_syscfg_update_dns_conf(llist *cgiVariables )
{
	FILE * fp;

	fp = fopen (CONF_DNS, "w") ;
	if( NULL == fp )
	{
		printf( "open file %s failed\n", CONF_DNS);
		return ;
	}

//	printf("; generated by WebAdmin\n" );

	fprintf(fp,"; generated by WebAdmin\n" );

	fprintf(fp,"nameserver\t\t%s\n", GET_VALUE(cgiVariables, PPPOE_KEYWORD_DNS1) );
	fprintf(fp,"nameserver\t\t%s\n", GET_VALUE(cgiVariables, PPPOE_KEYWORD_DNS2) );
	fprintf(fp,"search localdomain\n" );
	
	fclose(fp);
	
}

int sysconfig_wan(SYSCFG_INFO *info)
{
	char	*action = GET_CGI_ACTION(&info->cgiVariables );
	char *proto = GET_VALUE( &info->cgiVariables, NIC_KEYWORD_PROTOCOL );
	
	if( !action||!strlen(action) )
	{/* select protocol of WAN */
		char *info2, *button;
		info2 = __cgi_output_wan(info);
//		if(! info2 )
//			return 0;

		button = __cgi_wan_select_buttons(info);
		cgi_info_page(gettext("Current Ethernet Configuration"), (info2)?info2:"", button);
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/* after select the protocol of WAN */
		
		if(!proto)
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Protocol is selected for WAN port") );
			return 0;
		}

		if(!strcasecmp(NIC_KEYWORD_PROTOCOL_STATIC, proto))
		{
			cgi_info_page(gettext("Static Configuration"), "", __cgi_wan_add_buttons(info));
			return 0;
		}
		else if (!strcmp(NIC_KEYWORD_PROTOCOL_PPPOE, proto))
		{ 
//			fork_cmd("/usr/sbin/pppoe-connect &");

			sysconfig_pppoe(info);
		}
		else if (!strcmp(NIC_KEYWORD_PROTOCOL_DHCP, proto))
		{  /* activate the DHCP immediately */

			cgi_syscfg_wan_activate_dhcp(&info->cgiVariables, SYSCFG_TYPE_NORMAL );
		}
		else 
		{
			cgi_info_page(gettext(CGI_STR_DHCP_CONFIG), __cgi_output_wan(info), "");
		}

		return 2;
	}
	else if( !strcasecmp(action, CGI_ACTION_ACTIVATE)  )
	{
		if(!strcasecmp(NIC_KEYWORD_PROTOCOL_STATIC, proto))
		{
			__cgi_syscfg_update_dns_conf(&info->cgiVariables);
			cgi_syscfg_write_nic_config( &info->cgiVariables, SYSCFG_TYPE_NORMAL);

			return 0;
		}
		else if (!strcmp(NIC_KEYWORD_PROTOCOL_PPPOE, proto))
		{ 
//			fork_cmd("/usr/sbin/pppoe-connect &");
			sysconfig_pppoe(info);
			return 0;
		}
	}
	else if( !strcasecmp(action, CGI_ACTION_MODIFY ))
	{/* config WAN */
		sysconfig_pppoe(info);
		return 0;
#if 0	
		char buf[2048];
		int length =0;
		node* current;
		char		*ip;

		current = sysCgiVariables.head;
		while (current != 0) 
		{
			if(strstr(current->entry.name, "IPADDR") )
			{
				length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext(CGI_STR_IP_ADDRESS), current->entry.value);
				ip = current->entry.value;
			}	
			if(strstr(current->entry.name, "NETMASK") )
				length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext(CGI_STR_NETMASK), current->entry.value);
			if(strstr(current->entry.name, "GATEWAY") )
				length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext(CGI_STR_GATEWAY), current->entry.value );

			if(strstr(current->entry.name, "nameserverdns") )
				length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext(CGI_STR_DNS_SERVER), current->entry.value );

			if(strstr(current->entry.name, "nameserver") )
				length += sprintf(buf+length, "<tr><td bgcolor=\"#cccccc\"><strong>%s</strong></td><td>%s</td></tr>", gettext(CGI_STR_DNS_SERVER), current->entry.value );
			current = current->next;
		}
		cgi_info_page("WAN Config Status", buf, NULL);
#endif

	}

	cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Not supportted command in WAN configuration") );
//	cgi_info_page(gettext(CGI_STR_LAN_CONFIG), "", __cgi_lan_add_buttons() );

	return 0;
}

