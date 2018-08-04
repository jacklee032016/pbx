/*
* $Id: cgiVpnStatus.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "_vpn.h"

char *vpn_tunnel_info(VPN_INFO *info)
{
#define	VPN_RUNTIME_KEYWORK_NAME		"VPN_NAME"
#define	VPN_RUNTIME_KEYWORK_STATE		"STATE"
#define	VPN_RUNTIME_KEYWORK_DEVICE		"DEVICE"
#define	VPN_RUNTIME_KEYWORK_LOCAL		"LOCAL_IP"
#define	VPN_RUNTIME_KEYWORK_REMOTE		"REMOTE_IP"
#define	VPN_RUNTIME_KEYWORK_ADDRESS	"NET_ADDRESS"
#define	VPN_RUNTIME_KEYWORK_MASK		"NETMASK"
#define	VPN_RUNTIME_KEYWORK_DATE		"DATE"

#define	VPN_RUNTIME_KEYWORK_TIME		"CONNECT_TIME"
#define	VPN_RUNTIME_KEYWORK_SEND		"BYTES_SEND"
#define	VPN_RUNTIME_KEYWORK_RCVD		"BYTES_RCVD"

	char 	buf[4096*4];
	int 		length = 0;
	char		key[256];
	int		i = 0;
	
	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	node *current = info->runtime.head;
	while(current)
	{
		if(!strcasecmp(current->entry.name, VPN_RUNTIME_KEYWORK_NAME) )
		{
			sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_STATE, current->entry.value);
			if(!strcasecmp(GET_VALUE(&info->runtime, key) , "running") )
			{
				if(i == 0 )
				{
					length += CGI_SPRINTF(buf,length,"<TR><TD colspan=6 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("Current Active Tunnels") );
					length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
						gettext(CGI_STR_NAME), gettext(CGI_STR_DEVICE), gettext(CGI_STR_LOCAL), gettext(CGI_STR_REMOTE), gettext(CGI_STR_NETWORK_ADDRESS), gettext("Time"));
				}
				i++;
				
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_DEVICE, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n<TD align=\"center\">%s</TD>\r\n", 
					current->entry.value, GET_VALUE(&info->runtime, key) );
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_LOCAL, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", GET_VALUE(&info->runtime, key));
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_REMOTE, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", 	GET_VALUE(&info->runtime, key));
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_ADDRESS, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", 	GET_VALUE(&info->runtime, key));
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_DATE, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", GET_VALUE(&info->runtime, key));

			}
		}
		current = current->next;
	}
	if(i == 0 )
	{
		length += CGI_SPRINTF(buf,length,"<TR><TD><font color=\"#0000FF\"><strong>%s</strong></font></TD></TR>", gettext("No Active Tunnel exist currently!") );
	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	i = 0;
	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	current = info->runtime.head;
	while(current)
	{
		if(!strcasecmp(current->entry.name, VPN_RUNTIME_KEYWORK_NAME) )
		{
			sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_STATE, current->entry.value);
			if(!strcasecmp(GET_VALUE(&info->runtime, key) , "down") )
			{
				if(i == 0 )
				{
					length += CGI_SPRINTF(buf,length,"<TR><TD colspan=5 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("Tunnels has been Broken") );
					length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD><TD align=\"center\"  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
						gettext(CGI_STR_NAME), gettext("Time"),  gettext("Connection(Seconds)"),  gettext("Send(Bytes)"),  gettext("Rcvd(Bytes)"));
				}
				i++;
				
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_DATE, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n<TD align=\"center\">%s</TD>\r\n", 
					current->entry.value, GET_VALUE(&info->runtime, key) );
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_TIME, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", GET_VALUE(&info->runtime, key));
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_SEND, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD>\r\n", 	GET_VALUE(&info->runtime, key));
				sprintf(key, "%s_%s", VPN_RUNTIME_KEYWORK_RCVD, current->entry.value);
				length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", GET_VALUE(&info->runtime, key));

			}
		}
		current = current->next;
	}
	if(i == 0 )
	{
		length += CGI_SPRINTF(buf,length,"<TR><TD><font color=\"#0000FF\"><strong>%s</strong></font></TD></TR>", gettext("No Tunnel has been down currently!") );
	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	return strdup(buf);
	
}

static int __vpn_agent_status(VPN_INFO *info, char *vpnServiceName )
{
	char 		buf[8192];
	int 			length = 0;
	char			target[512];
	char			*tunnelInfo = NULL;

	length += CGI_SPRINTF(buf,length,"<TABLE align=center border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space  width=400\r\n");
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR></TABLE>",
		(strcasecmp(vpnServiceName, SERVICE_NAME_VPNSERVER)==0)?gettext("VPN Server Status"):gettext("VPN Client Status"), cgi_service_get_state_name(vpnServiceName), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	sprintf(target, "%s?%s=%s", WEB_URL_VPN,CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length, "%s\n", cgi_refresh_script(target, 5) );

	if(cgi_service_get_state(vpnServiceName) == SERVICE_STATE_RUNNING )
	{
		tunnelInfo = vpn_tunnel_info(info);
	}
	
	cgi_info_page(gettext("VPN Runtime Status"),  buf, (tunnelInfo!= NULL)?tunnelInfo:"" );
	
	return 0;
}


int vpn_status_info(VPN_INFO *info)
{
	char *cmd;

	cmd = GET_CGI_OP(&info->cgiVariables);
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, CGI_VPN_OP_SERVER_STATUS ) )
	{
		__vpn_agent_status( info, SERVICE_NAME_VPNSERVER);
	}
	else
	{
		__vpn_agent_status( info, SERVICE_NAME_VPNCLIENT);
	}
	
	return 0;	
}

