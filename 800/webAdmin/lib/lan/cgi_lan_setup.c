/*
* $Id: cgi_lan_setup.c,v 1.6 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_syscfg.h"
#include "cgi_wifi.h"
#include "cgi_mesh.h"
#include "cgi_services.h"

#define	FORM_NAME			"lanForm"

int	cgi_lan_dhcp(LAN_INFO *info, int where);

lan_cfg_t	eth1Info =
{
		CGI_STR_LAN_PORT,
		ETH1,	
		ETH1_CONFIG_FILE,
		
		WEB_URL_SYSCONFIG,
		CGI_SYSCFG_OP_LAN,
		CGI_SYSCFG_OP_LAN_DHCP,
		
		UDHCP_CONF_FILE,
		UDHCP_CMD
};

lan_cfg_t wifiInfo = 
{
		CGI_STR_WIFI_PORT,
			
		WIFI0,
		WIFI_CONF_NETWORK,

		WEB_URL_WLAN,
		CGI_WIFI_OP_NIC,
		CGI_WIFI_OP_NIC_DHCP,
		
		WDHCP_CONF_FILE,
		WDHCP_CMD
};


static char *__cgi_output_lan(LAN_INFO *info)
{
	char msg[4096];
	int length = 0;

	length += CGI_SPRINTF(msg,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>",
		gettext(CGI_STR_IP_ADDRESS) , gettext(CGI_STR_NETMASK), gettext("DHCP Service"));
		
	length += CGI_SPRINTF(msg,length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",
		GET_VALUE(&info->cfgs, NIC_KEYWORD_IP_ADDRESS) );
	length += CGI_SPRINTF(msg,length, "<td align=\"center\"><font size=2>%s&nbsp </font></td>" ,
		GET_VALUE(&info->cfgs, NIC_KEYWORD_NETMASK ) ); 
	length += CGI_SPRINTF(msg,length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,
		GET_VALUE(&info->cfgs, NIC_KEYWORD_DHCP_SERVICE) ); 

	if(!strcasecmp(GET_VALUE(&info->cfgs, NIC_KEYWORD_DHCP_SERVICE) , NIC_VALUE_YES))
	{
		length += CGI_SPRINTF(msg, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" colspan=2>%s</td></TR>",
		gettext("DHCP Server Status"), cgi_get_program_state_name(info->info->dhcpCmd ) );
	}
	return strdup(msg);
}

static char *__cgi_lan_add_buttons(LAN_INFO *info, int where)
{
	char buf[2048];
	int 	length = 0;
	char	msg[256];
	char	*value;
	int isOn = 0;

	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", FORM_NAME, info->info->url );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_KEYWORD_OPERATOR, info->info->urlNic );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, info->info->devName );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_IP_ADDRESS) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_IP_ADDRESS, NIC_KEYWORD_IP_ADDRESS, GET_VALUE(&info->cfgs, NIC_KEYWORD_IP_ADDRESS) );

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_NETMASK) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		NIC_KEYWORD_NETMASK,NIC_KEYWORD_NETMASK, GET_VALUE(&info->cfgs, NIC_KEYWORD_NETMASK) );
	
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext("Start DHCP Service") );
	value = GET_VALUE(&info->cfgs, NIC_KEYWORD_DHCP_SERVICE);
	if(!strcasecmp(value, NIC_VALUE_YES) )
		isOn = 1;
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		NIC_KEYWORD_DHCP_SERVICE, NIC_VALUE_YES, (isOn==1)?"checked":"", gettext(CGI_STR_ON), NIC_KEYWORD_DHCP_SERVICE, NIC_VALUE_NO, (isOn==1)?"":"checked", gettext(CGI_STR_OFF) );


	length += sprintf(buf+length, "<TR><TD align=\"right\">\r\n");
	sprintf(msg, "%s,%s?", gettext("WebAdmin will reboot after LAN configration"), gettext(CGI_STR_ARE_YOU_SURE) );

	if(where == CGI_LAN_WHERE_WIZARD )
	{
		length += sprintf(buf+length, "%s", cgi_button(gettext(CGI_STR_PREV) , WEB_URL_QUICK_INSTALL"?"CGI_KEYWORD_OPERATOR"="CGI_QUICK_OP_LAN ) );
	}
	
	length += sprintf(buf+length, cgi_submit_button_with_alert((where==CGI_LAN_WHERE_WIZARD)?gettext(CGI_STR_NEXT):gettext(CGI_STR_SAVE), msg, FORM_NAME ) );
	length += sprintf(buf+length, "</TD>\r\n");

	length += sprintf(buf+length, "<TD  align=\"left\">\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_LAN_CONFIG ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}


int cgi_lan_setup(llist *cgiVariables, int type, int where)
{
	LAN_INFO	info;
#if 0	
	char *ip, *mask;
	int n;
#endif	
	char		*cmd;
	char		msg[512];
	char	*action = GET_VALUE(cgiVariables, CGI_ACTION);
	cmd = GET_VALUE( cgiVariables, CGI_KEYWORD_OPERATOR );
	if(type==CGI_LAN_EHT1)
	{
		info.info = &eth1Info;
	}
	else if( type == CGI_LAN_WIFI )
	{
		info.info = &wifiInfo;
	}

	info.cgiVariables = cgiVariables;

	if(where == CGI_LAN_WHERE_WIZARD && type ==CGI_LAN_EHT1)
	{
		sprintf(info.info->url, "%s", WEB_URL_QUICK_INSTALL );
	}
#if 0	
	else
	{
		sprintf(info.info->url, "%s", WEB_URL_SYSCONFIG );
	}
#endif

	if(!strcasecmp(cmd, info.info->urlDhcp) )
		return cgi_lan_dhcp(&info, where);
	
	if(readconfig(info.info->nicConfigFile, ITEM_EQU, NULL, &info.cfgs))
	{
		sprintf(msg,"%s : %s(%s)", gettext(CGI_STR_WITHOUT_CONF_FILE), info.info->name, info.info->nicConfigFile );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}
	
	if(!strcasecmp(action, CGI_ACTION_ACTIVATE) )
	{
#if 1
		list_replace_with_new_list(&info.cfgs, info.cgiVariables);

		cgi_nic_refresh_net_and_broad_address(&info.cfgs);
		if(writeconfig(info.info->nicConfigFile, ITEM_EQU, NULL, &info.cfgs) )
		{
			sprintf(msg, "%s : %s", gettext(CGI_STR_WITHOUT_CONF_FILE), info.info->name );
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
			return 1;
		}

		if(!strcasecmp(GET_VALUE(cgiVariables, NIC_KEYWORD_DHCP_SERVICE), NIC_VALUE_YES) )
		{
			return cgi_lan_dhcp(&info, where);
		}
		else if(where == CGI_LAN_WHERE_WIZARD )
		{
			return quickWizardFinish();
		}

#else	
		ip = GET_VALUE( info.cgiVariables, NIC_KEYWORD_IP_ADDRESS);
		mask= GET_VALUE( info.cgiVariables, NIC_KEYWORD_NETMASK );
	
//		cgi_info_page(gettext("Alerting"), NULL, info);

//		cgi_refresh_page(5, "", char * header, char * msg)
		cgi_syscfg_write_nic_config(info.cgiVariables, SYSCFG_TYPE_NORMAL);

		n = strlen(ip);
		read_dhcp_conf(CONF_DH,ip, mask,n, *info.cgiVariables);
		return 0;
#endif		
	}

	if(where == CGI_LAN_WHERE_WIZARD )
		sprintf(msg, "%s : %s", gettext(CGI_STR_QUICK_INSTALL), gettext(CGI_STR_LAN_CONFIG) );
	else
		sprintf(msg, "%s : %s", gettext(CGI_STR_LAN_CONFIG), info.info->name );

	cgi_info_page(msg , __cgi_output_lan(&info), __cgi_lan_add_buttons(&info, where) );

	return 0;
}

int quickWizardFinish()
{
	char msg[4096];
	int length = 0;

	length += sprintf(msg+length, "<tr><td><font size=2>" );
	length += sprintf(msg+length, "%s!&nbsp",
		gettext("Congratulation") ); 
	length += sprintf(msg+length, "%s&nbsp",
		gettext("For other Information") ); 
	length += sprintf(msg+length, "%s.&nbsp",
		gettext("Please consult the user manual provided by Assist Corp") ); 
	
	length += sprintf(msg+length, "</font></td></tr>"); 

	cgi_info_page(gettext(CGI_STR_QUICK_INSTALL), gettext("Quick Install Finished"), msg);

	return 0;
}



