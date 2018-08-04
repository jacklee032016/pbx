/*
* $Id: syscfg_lan.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "sysconf.h"

#define	FORM_NAME			"lanForm"

static char *__cgi_output_lan(SYSCFG_INFO *info)
{
	char msg[4096];
	int length = 0;

	length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>",
		gettext(CGI_STR_IP_ADDRESS) , gettext(CGI_STR_NETMASK));
		
	length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",
		GET_VALUE(&info->lanCfgs, NIC_KEYWORD_IP_ADDRESS) );
	length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,
		GET_VALUE(&info->lanCfgs, NIC_KEYWORD_NETMASK ) ); 

	return strdup(msg);

}

static char *__cgi_lan_add_buttons(SYSCFG_INFO *info)
{
	char buf[2048];
	int 	length = 0;
	char	msg[256];

	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", FORM_NAME, WEB_URL_SYSCONFIG );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_LAN );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH1);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong></TD>", 
		gettext("LAN IP Address") );
	length += sprintf(buf+length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS, NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->lanCfgs, NIC_KEYWORD_IP_ADDRESS) );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong></TD>", 
		gettext("LAN Netmask") );
	length += sprintf(buf+length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK,NIC_KEYWORD_NETMASK, GET_VALUE(&info->lanCfgs, NIC_KEYWORD_NETMASK) );
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	sprintf(msg, "%s,%s?", gettext("WebAdmin will reboot after LAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );
	length += sprintf(buf+length, cgi_submit_button_with_alert(gettext(CGI_STR_SAVE), msg, FORM_NAME ) );
	length += sprintf(buf+length, "</TD>\r\n");
#if 0
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_reset_button(gettext(CGI_STR_RESET) ) );
	length += sprintf(buf+length, "</TD>\r\n");
#endif

	length += sprintf(buf+length, "<TD  align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_LAN_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}


int sysconfig_lan(SYSCFG_INFO *info)
{
	char *ip, *mask;
	int n;
	char	*action = GET_VALUE(&info->cgiVariables, CGI_ACTION);
	
	if(!strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{
		ip = GET_VALUE( &info->cgiVariables, NIC_KEYWORD_IP_ADDRESS);
		mask= GET_VALUE( &info->cgiVariables, NIC_KEYWORD_NETMASK );
	
//		cgi_info_page(gettext("Alerting"), NULL, info);

//		cgi_refresh_page(5, "", char * header, char * msg)
		cgi_syscfg_write_nic_config(&info->cgiVariables, SYSCFG_TYPE_NORMAL);

		n = strlen(ip);
		read_dhcp_conf(CONF_DH,ip, mask,n, info->cgiVariables);
		return 0;
	}
	
	cgi_info_page(gettext(CGI_STR_LAN_CONFIG), __cgi_output_lan(info), __cgi_lan_add_buttons(info) );

	return 0;
}


