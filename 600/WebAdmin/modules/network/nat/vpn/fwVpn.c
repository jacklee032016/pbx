/*
* $Id: fwVpn.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "fwVpn.h"

#if 0
static pptp_type	_types[] = 
{
		{
			VPN_PPTP_IN,
			VPN_PPTP_TYPE_IN
		},
		{
			VPN_PPTP_OUT,
			VPN_PPTP_TYPE_OUT
		},
		{
			VPN_PPTP_BOTH,
			VPN_PPTP_TYPE_BOTH
		}
};
#endif


static char *__fwVpn_info(llist *pptp )
{
	char 		buf[4096];
	int 			length = 0;
	node		*current = pptp->head;
//	char			target[256];

	if(!current)
	{
		length += CGI_SPRINTF(buf,length, "%s<br>", gettext("No VPN Tunnel is activating now!") );
		length += CGI_SPRINTF(buf,length, "<strong>%s</strong>: %s.<br>%s!<br>",gettext(CGI_STR_NOTE),
			gettext("Only one VPN tunnel can be activate at the same time"), gettext("Old tunnel will be deleted automatically after new tunnel is added"));
	}
	else
	{
		length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
			gettext("VPN Gateway"), gettext("Local VPN Host(Client or Server)") );

//		while(current )
		{
			length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s</td>", GET_VALUE(pptp, VPN_CFG_GATEWAY) );//current->entry.name);
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", GET_VALUE(pptp, VPN_CFG_LOCAL_IP)  );
#if 0
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.type );

			sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_VPN, CGI_ACTION, CGI_ACTION_DEL, 
				VPN_CFG_GATEWAY, current->entry.name, VPN_CFG_LOCAL_IP, current->entry.value, VPN_CFG_TYPE,current->entry.type );

			length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));
#endif

//			current = current->next;
		}
	}
	return strdup(buf);
}


static char *__fwVpn_buttons(FW_INFO *info )
{
	char 		buf[8192*2];
	int 			length = 0;
//	int			i;

	length += CGI_SPRINTF(buf, length, "<form name=\"fwTimerForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_VPN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
#if 0
	length += CGI_SPRINTF(buf,length, "\n<TR><TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("VPN Type"), VPN_CFG_TYPE );
	for(i=0; i<3; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n",_types[i].description, _types[i].description);
	}
	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n" );
#endif

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("VPN Gateway IP Address") );
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled title=\"%s\"></TD></TR>\r\n", 
		VPN_CFG_GATEWAY, VPN_CFG_GATEWAY, info->netInfo->wanInfo.addr, gettext("WAN IP Address") );
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("VPN Host/Server IP Address") );
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"\" maxlength=\"50\" title=\"%s\"></TD></TR>\r\n", 
		VPN_CFG_LOCAL_IP, VPN_CFG_LOCAL_IP, gettext("VPN Host In Local Network, can be PPTP client or PPTP Server"));
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_VPN) );

	return strdup(buf);
}

static int __vpn_init(llist *pptpCfg)
{
	list_create( pptpCfg);
	if (readconfig(IPTABLES_VPN_CONFIG, ITEM_EQU, NULL, pptpCfg) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Config of VPN is found") );
		return -1;
	}
	return 0;
}

static int __vpn_save_task(FW_INFO *info, llist *pptpCfg)
{
	char		msg[256];
	char		command[256];
	
	char		*gw, *type, *ip;
	node	*current;
	char 	*action;
	llist		newVpn;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	
	gw = info->netInfo->wanInfo.addr;//GET_VALUE(&info->cgiVariables, VPN_CFG_GATEWAY);
//	type = GET_VALUE(&info->cgiVariables, VPN_CFG_TYPE );
	ip = GET_VALUE(&info->cgiVariables, VPN_CFG_LOCAL_IP );
	
	if(!gw ||!strlen(gw) || !ip ||!strlen(ip)  )
	{
		sprintf(msg, "%s", gettext("Error Fields exist when update VPN configuration") );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
		return -1;	
	}
	
	current = pptpCfg->head;

	while(current)
	{
		if(!strcasecmp(current->entry.name, gw) &&
			!strcasecmp(current->entry.value, ip)  )
		{
			break;
		}	
		current = current->next;
	}

	if((!current && !strcasecmp(action, CGI_ACTION_DEL ) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s', '%s')\" %s", gw?gw:"NULL", ip?ip:"NULL", 
			type?type:"NULL", (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	current = pptpCfg->head;
	if( current)
	{
		sprintf(command, "%s DELETE", "/sbin/vpn.sh");
		system(command);
	}
	
	list_create(&newVpn);
	list_append_raw(&newVpn, VPN_CFG_GATEWAY, gw, NULL, NULL );
	list_append_raw(&newVpn, VPN_CFG_LOCAL_IP, ip, NULL, NULL );
	
	writeconfig(IPTABLES_VPN_CONFIG, ITEM_EQU, NULL, &newVpn);

	sprintf(command, "%s ADD", "/sbin/vpn.sh");
	system(command);
	return 0;
}

int cgiFw_VPN(FW_INFO *info)
{
	char *action;
	llist	oldCfg, newCfg;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
#if 0
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("VPN Configuration"), __fwVpn_info(&pptpCfg), __fwVpn_buttons( info) );
		return 0;
	}
#endif	
 	if (__vpn_init(&oldCfg) )
	{
		return -1;
	}
	
	if(!strcasecmp(action, CGI_ACTION_ADD ) ||
		!strcasecmp(action, CGI_ACTION_DEL)  )
	{
		if(__vpn_save_task(info, &oldCfg)< 0)
			return 1;

	}
	
	if (__vpn_init(&newCfg) )
	{
		return -1;
	}

	cgi_info_page(gettext("VPN Configuration"), __fwVpn_info(&newCfg), __fwVpn_buttons(info) );
		
	return 0;	
}

