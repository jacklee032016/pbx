/*
* $Id: quickWizardLan.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "quickWizard.h"
#include "cgi_data.h"

#define	FORM_NAME		"lanForm"

#if 0
static char *__cgi_lan_address_buttons(QUICK_INFO *info)
{
	char 	buf[2048];
	int 		length = 0;
	char		msg[256];
	char		target[256];

	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", FORM_NAME, WEB_URL_QUICK_INSTALL );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH1 );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC );

	length += sprintf(buf+length, "<TR><TD  align=\"center\"><strong>%s</strong></TD>", 
		gettext("LAN IP Address") );
	length += sprintf(buf+length, "<TD  align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(IPADDR.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->lanCfgs, NIC_KEYWORD_IP_ADDRESS) );
	length += sprintf(buf+length, "<TR>\r\n");

	length += sprintf(buf+length, "<TR><TD  align=\"center\"><strong>%s</strong></TD>", 
		gettext("LAN Netmask") );
	length += sprintf(buf+length, "<TD  align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(NETMASK.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK, GET_VALUE(&info->lanCfgs, NIC_KEYWORD_NETMASK) );
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");

	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN ); 
	length += sprintf(buf+length, cgi_button(gettext(CGI_STR_PREV),target ) );
	
	sprintf(msg, "%s,%s?", gettext("WebAdmin will reboot after LAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );
	length += sprintf(buf+length, cgi_submit_button_with_alert(gettext("Finish"), msg, FORM_NAME ) );

//	length += sprintf(buf+length,"%s", quickWizardAddFinishButton() );

	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_LAN_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}
#endif

static char *__cgi_lan_initial_config(QUICK_INFO *info)
{
	char 	buf[2048];
	int 		length = 0;

	length += sprintf(buf+length, "<form name=\"initForm\" method=\"post\" action=\"%s\">\n", WEB_URL_QUICK_INSTALL );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_DEFAULT );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGIDATA_KEYWORD_RESET_FLAG,CGIDATA_KEYWORD_RESET_TFTP );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>", 
		gettext("Input address of server if initial config is needed"), CGIDATA_KEYWORD_SERVER_IP, CGIDATA_KEYWORD_SERVER_IP, 
		GET_VALUE(&info->defaultCfgs, CGIDATA_KEYWORD_SERVER_IP )  );

	length += sprintf(buf+length, "<TR><TD>%s,%s</TD></TR>", 
		gettext("Configuration data Will be restored from Server"), gettext("Normally Server is provided by Assist Corp.") );
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button_with_alert(gettext("Initial Configuration"), gettext("Restore will be reboot system"), "initForm" ) );

//	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_QUICK_INSTALL ) );
	length += sprintf(buf+length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}


static char *__cgi_lan_protocol_select_buttons(QUICK_INFO *info)
{
	char 	buf[2048];
	int 		length = 0;
	char		target[256];

	length += sprintf(buf+length, "<form name=\"wanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_QUICK_INSTALL );
#if 0	
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN );
#else
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_LAN );
#endif
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH1 );

#if 0
	length += sprintf(buf+length, "<TR><TD ><strong>%s(%s)</strong></TD><TD><input name=\"%s\" type=\"radio\" value=\"%s\" checked></TD></TR>", 
		gettext(CGI_STR_DEFAULT), gettext("DHCP"), NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_DHCP );

	length += sprintf(buf+length, "<TR><TD ><strong>%s</strong></TD><TD><input name=\"%s\" type=\"radio\" value=\"%s\"></TD></TR>", 
		gettext("Redefine LAN Address"), NIC_KEYWORD_PROTOCOL,NIC_KEYWORD_PROTOCOL_STATIC  );
#else
	length += sprintf(buf+length, "<TR><TD><br><br></TD></TR>" );
	length += sprintf(buf+length, "<TR><TD colspan=2><strong>%s</strong></TD></TR>", gettext("Redefine LAN Address") );
#endif
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2>\r\n");

	sprintf(target, "%s", WEB_URL_QUICK_INSTALL ); 
	length += sprintf(buf+length, cgi_button(gettext(CGI_STR_PREV),target ) );

	length += sprintf(buf+length, cgi_submit_button(gettext(CGI_STR_NEXT) ) );

//	length += sprintf(buf+length,"%s", quickWizardAddFinishButton() );

	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_QUICK_INSTALL ) );
	length += sprintf(buf+length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}

int quickWizardLan(QUICK_INFO *info)
{
	char title[128];
	char		*cmd;
	
	cmd = GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR );
	if( !strcasecmp(cmd, CGI_QUICK_OP_LAN) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("Select mode of LAN and Initial Configuration") ); 
		cgi_info_page(title, __cgi_lan_initial_config(info), __cgi_lan_protocol_select_buttons(info));

		return 0;
	}

	return cgi_lan_setup(&info->cgiVariables, CGI_LAN_EHT1, CGI_LAN_WHERE_WIZARD);

#if 0	
	protocol = GET_VALUE(&info->cgiVariables, NIC_KEYWORD_PROTOCOL );
	if( !protocol || !strlen(protocol) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("Select mode of LAN and Initial Configuration") ); 
		cgi_info_page(title, __cgi_lan_initial_config(info), __cgi_lan_protocol_select_buttons(info));

		return 0;
	}

	if(! strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_DHCP) )
	{/* default : keep current LAN configuration */

		if( cgi_syscfg_lan_update_dhcp_server() )
		{
			cgi_error_page(NULL, gettext(CGI_STR_FAIL), gettext("LAN Configuration is incorrect") );
			return 0;
		}
		
		cgi_syscfg_update_host_conf();
		cgi_syscfg_lan_auto_iptables();

		quickWizardFinish();
		return 0;

	}
	else if(!strcasecmp(protocol, NIC_KEYWORD_PROTOCOL_STATIC) )
	{/* redefine the static LAN address */
		ip = GET_VALUE(&info->cgiVariables, NIC_KEYWORD_IP_ADDRESS); 
		if(!ip || !strlen(ip) )
		{/* display LAN configuration form */
			sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext(CGI_STR_LAN_CONFIG) ); 
			return cgi_info_page(title, "", __cgi_lan_address_buttons(info) );
		}

		cgi_syscfg_write_nic_config( &info->cgiVariables, SYSCFG_TYPE_WIZARD );
		if( cgi_syscfg_lan_update_dhcp_server() )
		{
			cgi_error_page(NULL, gettext(CGI_STR_FAIL), gettext("LAN Configuration is incorrect") );
			return 0;
		}
		
		cgi_syscfg_update_host_conf();
		cgi_syscfg_lan_auto_iptables();

		return 0;
	}
#endif

	return 0;
}

