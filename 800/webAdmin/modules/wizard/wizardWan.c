/*
* $Id: wizardWan.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"
#include "cgi_syscfg.h"


static char *__cgi_wizard_wan_static_address(WIZARD_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;

	length += sprintf(buf+length, "<TR><TD colspan=2></TD><TD align=\"right\"><TABLE align=\"right\">\n" );
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
	length += CGI_SPRINTF(buf,length, "</TABLE></TD></TR>\r\n");

	return strdup(buf);
}


static char *__cgi_wizard_wan_pppoe(WIZARD_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;

	length += sprintf(buf+length, "<TR><TD colspan=2></TD><TD align=\"left\"><TABLE align=\"right\">\n" );
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
	length += CGI_SPRINTF(buf,length, "</TABLE></TD></TR>\r\n");

	return strdup(buf);
}

static int __cgi_wizard_wan(WIZARD_INFO *info)
{
	char 	status[2048*10];
	char 	alert[2048*10];
	char 	leftButton[2048];
	char 	rightButton[2048];

	int 		length = 0;
	
	length += CGI_SPRINTF(status, length, "<table width=\"100%%s\" align=\"center\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n");
	length += CGI_SPRINTF(status, length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD align=\"left\" ><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD><TD ></TD></TR>\n", 
		gettext("DHCP"), NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_DHCP, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_DHCP)))?"checked":"" );

	length += CGI_SPRINTF(status, length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD align=\"left\" ><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD><TD ></TD></TR>\n", 
		gettext("static"), NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_STATIC)))?"checked":"" );
	length += CGI_SPRINTF(status, length, "%s", __cgi_wizard_wan_static_address(info) );

	length += CGI_SPRINTF(status, length, "<TR><TD align=\"right\"><strong>%s(%s)</strong></TD><TD align=\"left\" ><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD><TD ></TD></TR>\n", 
		gettext("PPPOE"), gettext(CGI_STR_DEFAULT),  NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_PPPOE, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_PPPOE)))?"checked":"" );
	length += CGI_SPRINTF(status, length, "%s", __cgi_wizard_wan_pppoe(info) );
	length += CGI_SPRINTF(status, length, "\n</table>\n" );

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s, %s!\n", gettext("In order to update from Internet"),gettext("your WAN configuration must be correct") );
	length += CGI_SPRINTF(alert, length, "<tr><td>%s, %s.\n", gettext("After WAN configuration"), gettext("System must be reboot to make the configuration activating") );
	length += CGI_SPRINTF(alert, length, "<tr><td><strong>%s</strong><br>%s!\n", gettext("Suggestion"), gettext("Update your system in LAN environments") );

	length += CGI_SPRINTF(alert, length, "</td></tr>\n</table>\n" );
	
	length = 0;
	length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(CGI_STR_SAVE) ) );

	length = 0;
	length += CGI_SPRINTF(leftButton,length, "%s", cgi_button(gettext("Return"), WEB_URL_WIZARD) );
	
	return wizard_page(gettext(CGI_STR_ETH_CONFIG), status, alert, leftButton, rightButton, info);
}

int wizard_cgi_wan(WIZARD_INFO *info)
{
	char		*protocol;
	char		target[128];

	protocol = GET_VALUE(&info->cgiVariables, NIC_KEYWORD_PROTOCOL );
	if(!protocol || !strlen(protocol) )
	{
		__cgi_wizard_wan( info);
		return 0;
	}

	sprintf(target, "%s?%s=%s", WEB_URL_WIZARD, CGI_KEYWORD_OPERATOR, WIZARD_OP_REBOOT );
	cgi_refresh_page(5, target, gettext("Saving WAN Configuration now"), gettext(CGI_STR_PLEASE_WAIT) );

	list_replace_with_new_list(&info->wanCfgs, &info->cgiVariables);
//	list_replace_value_by_key(&info->wanCfgs, NIC_KEYWORD_PROTOCOL, protocol );
	writeconfig(ETH0_CONFIG_FILE, ITEM_EQU,NULL, &info->wanCfgs );

	if(!strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_DHCP) )
	{
//		cgi_syscfg_wan_activate_dhcp(&info->cgiVariables, SYSCFG_TYPE_WIZARD );
	}
	else if(!strcasecmp(protocol,NIC_KEYWORD_PROTOCOL_STATIC) )
	{
//		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("WAN IP Address Configuration") ); 
//		cgi_info_page(title, "", __cgi_wan_address(info) );
	}
	else //if(!strcmp(protocol, NIC_KEYWORD_PROTOCOL_PPPOE) )
	{
		cgi_syscfg_wan_activate_pppoe(&info->cgiVariables );
	}
	
	return 0;
}

