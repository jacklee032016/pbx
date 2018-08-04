/*
* $Id: wifiNetwork.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_wifi.h"

static char *__cgi_wifi_lan(WIFI_INFO *info)
{
	char msg[4096];
	int length = 0;

	length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>",
		gettext(CGI_STR_IP_ADDRESS) , gettext(CGI_STR_NETMASK));

		
	length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",
		GET_VALUE(&info->nicCfgs, NIC_KEYWORD_IP_ADDRESS) );
	length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,
		GET_VALUE(&info->nicCfgs, NIC_KEYWORD_NETMASK ) ); 

	return strdup(msg);

}

static char *__cgi_wifi_add_buttons(WIFI_INFO *info)
{
	char buf[2048];
	int 	length = 0;

	length += sprintf(buf+length, "<TR><TD colspan=2><strong>%s</strong></TD></TR>\r\n"
		, gettext("Network Configuration for WiFi Device" ) );

	length += sprintf(buf+length, "<form name=\"lanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WLAN);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_KEYWORD_OPERATOR, CGI_WIFI_OP_NIC);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_ACTION, CGI_ACTION_ADD );
	
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, DEFAULT_WIFI_DEVICE_NAME );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_IP_ADDRESS) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS, NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->nicCfgs, NIC_KEYWORD_IP_ADDRESS) );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_NETMASK) );
	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK,NIC_KEYWORD_NETMASK, GET_VALUE(&info->nicCfgs, NIC_KEYWORD_NETMASK) );
	
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2>%s %s</TD></TR>\r\n", 
		cgi_submit_button(gettext(CGI_STR_SAVE) ), cgi_help_button(CGI_HELP_MSG_LAN_CONFIG ) );

	return strdup(buf);
}


int wifi_Network(WIFI_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION);
	if( action && !strcasecmp(action, CGI_ACTION_ADD ) )
	{
		list_replace_with_new_list(&info->nicCfgs, &info->cgiVariables);
		if(writeconfig(WIFI_CONF_NETWORK, ITEM_EQU, NULL, &info->nicCfgs) )
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Wifi Network Configuration Failed") );
			return 1;
		}
	}
	
	cgi_info_page(gettext("WiFi Network Configuration"), __cgi_wifi_lan(info), __cgi_wifi_add_buttons(info) );

	return 0;
}

