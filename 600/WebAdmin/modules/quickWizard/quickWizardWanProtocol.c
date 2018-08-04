/*
* $Id: quickWizardWanProtocol.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "quickWizard.h"

#define	FORM_NAME		"wanForm"

static char *__cgi_wan_address(QUICK_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	char		msg[256];
	char		target[256];

	length += CGI_SPRINTF(buf,length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", FORM_NAME, WEB_URL_QUICK_INSTALL );
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN );
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL , NIC_KEYWORD_PROTOCOL_STATIC);

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", 
		gettext("WAN IP Address") );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS,NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_IP_ADDRESS) );
	length += CGI_SPRINTF(buf,length, "<TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext("WAN Netmask") );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK,NIC_KEYWORD_NETMASK, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_NETMASK ) );

	length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_GATEWAY) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_GATEWAY,NIC_KEYWORD_GATEWAY, GET_VALUE(&info->wanCfgs, NIC_KEYWORD_GATEWAY ) );

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_FIRST_DNS_SERVER) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS1, PPPOE_KEYWORD_DNS1,GET_VALUE(&info->wanCfgs,PPPOE_KEYWORD_DNS1) );

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		gettext(CGI_STR_SECOND_DNS_SERVER) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_DNS2, PPPOE_KEYWORD_DNS2, GET_VALUE(&info->wanCfgs, PPPOE_KEYWORD_DNS2) );

	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2>\r\n");
	
//	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_SELECT ); 
	sprintf(target, "%s", WEB_URL_QUICK_INSTALL ); 
	length += CGI_SPRINTF(buf,length, cgi_button(gettext(CGI_STR_PREV),target ) );
	
	sprintf(msg, "%s,%s?", gettext("WWW Server will reboot after WAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );
	length += CGI_SPRINTF(buf,length, cgi_submit_button_with_alert(gettext(CGI_STR_NEXT), msg, FORM_NAME ) );

	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN ); 
	length += CGI_SPRINTF(buf,length, cgi_button(gettext(CGI_STR_SKIP),target ) );

//	length += CGI_SPRINTF(buf,length,"%s", quickWizardAddFinishButton() );

	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_WAN_CONFIG ) );
	length += CGI_SPRINTF(buf,length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}


static char *__cgi_pppoe_add_buttons(QUICK_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	char		msg[256];
	char		target[256];

	length += CGI_SPRINTF(buf,length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME,  WEB_URL_QUICK_INSTALL);
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_WAN);
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_PPPOE);

	/* server name */
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PPPOE_KEYWORD_SERVER_NAME, PPPOE_KEYWORD_SERVER_NAME_ASTERIC);
	/* User/Passwd */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_USER_NAME) );
//	length += sprintf(buf+length, "<TD ><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
	length += CGI_SPRINTF(buf,length, "<TD ><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_USER, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_USER) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_PASSWORD) );
//	length += sprintf(buf+length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
	length += CGI_SPRINTF(buf,length, "<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		PPPOE_KEYWORD_PASSWORD, GET_VALUE(&info->pppoeCfgs, PPPOE_KEYWORD_PASSWORD) );


	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2>\r\n");

//	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_SELECT ); 
	sprintf(target, "%s", WEB_URL_QUICK_INSTALL ); 
	length += CGI_SPRINTF(buf,length, cgi_button(gettext(CGI_STR_PREV),target ) );
	
	sprintf(msg, "%s,%s?", gettext("WWW Server will reboot after WAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );
	length += CGI_SPRINTF(buf,length, cgi_submit_button_with_alert(gettext(CGI_STR_NEXT), msg ,FORM_NAME ) );

	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL,NIC_KEYWORD_DEVICE, ETH1); 
	length += CGI_SPRINTF(buf,length, cgi_button(gettext(CGI_STR_SKIP), target ) );

//	length += CGI_SPRINTF(buf,length,"%s", quickWizardAddFinishButton() );
	
	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_PPPOE_CONFIG ) );
	length += CGI_SPRINTF(buf,length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}

int quickWizardWanProtocol(QUICK_INFO *info)
{
	char		*protocol;
	char title[256];

	protocol = GET_VALUE(&info->cgiVariables, NIC_KEYWORD_PROTOCOL );
	if(!protocol || !strlen(protocol) )
	{
		cgi_error_page(NULL, CGI_STR_ERROR, gettext("No Protocol is defined for WAN") );
		return 0;
	}

	list_replace_value_by_key(&info->wanCfgs, NIC_KEYWORD_PROTOCOL, protocol );
	writeconfig(ETH0_CONFIG_FILE, ITEM_EQU,NULL, &info->wanCfgs );

	if(!strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_DHCP) )
	{
		char		target[128];
		sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN );
		cgi_refresh_page(5, target, gettext("Start DCHP for WAN Port"), gettext(CGI_STR_PLEASE_WAIT) );

		cgi_syscfg_wan_activate_dhcp(&info->cgiVariables, SYSCFG_TYPE_WIZARD );
//		__cgi_redirect_to_quick_lan();
//		return 0;
	}
	else if(!strcasecmp(protocol,NIC_KEYWORD_PROTOCOL_STATIC) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("WAN IP Address Configuration") ); 
		cgi_info_page(title, "", __cgi_wan_address(info) );
//		return 0;
	}
	else //if(!strcmp(protocol, NIC_KEYWORD_PROTOCOL_PPPOE) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("WAN ADSL Configuration") ); 
		cgi_info_page(title, "", __cgi_pppoe_add_buttons(info) );
//		return 0;
	}
	
	return 0;
}

