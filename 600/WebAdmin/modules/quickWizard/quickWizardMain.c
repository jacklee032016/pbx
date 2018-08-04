/*
* $Id: quickWizardMain.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "quickWizard.h"
#include "cgi_data.h"
#include "aslib.h"

static 	QUICK_INFO	__quickInfo;

char	*quickWizardAddFinishButton()
{
	char		button[512];
	char		target[256];
	
	sprintf(target, "%s?%s=%d&%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, QUICK_WIZARD_OP_FINISHED,
		NIC_KEYWORD_DEVICE, ETH0 ); 
	sprintf(button, cgi_button(gettext("Finish"),target ) );

	return strdup(button);
}


static char *__cgi_wan_protocol_select_buttons(QUICK_INFO *info )
{
	char 	buf[2048];
	int 		length = 0;
	char		target[256];

	length += sprintf(buf+length, "<form name=\"wanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_QUICK_INSTALL );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_SELECT );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", NIC_KEYWORD_DEVICE, ETH0 );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD></TR>\n", 
		gettext("static"), NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_STATIC, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_STATIC)))?"checked":"" );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s</strong><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD></TR>\n", 
		gettext("DHCP"), NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_DHCP, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_DHCP)))?"checked":"" );

	length += sprintf(buf+length, "<TR><TD align=\"center\"><strong>%s(%s)</strong><input name=\"%s\" type=\"radio\" value=\"%s\" %s></TD></TR>\n", 
		gettext("PPPOE"), gettext(CGI_STR_DEFAULT),  NIC_KEYWORD_PROTOCOL, NIC_KEYWORD_PROTOCOL_PPPOE, 
		(!(strcasecmp(GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_PPPOE)))?"checked":"" );

	
	length += sprintf(buf+length, "<TR><TD><br><br></TD></TR>\n" );
	
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2>\r\n");
#if 0	
	sprintf(target, "%s", WEB_URL_QUICK_INSTALL ); 
	length += sprintf(buf+length, cgi_button(gettext(CGI_STR_PREV),target ) );
#endif

	length += sprintf(buf+length, cgi_submit_button(gettext(CGI_STR_NEXT) ) );

//	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL,NIC_KEYWORD_DEVICE, ETH1); 
	sprintf(target, "%s?%s=%s", WEB_URL_QUICK_INSTALL, CGI_KEYWORD_OPERATOR, CGI_QUICK_OP_LAN ); 
	length += sprintf(buf+length, cgi_button(gettext(CGI_STR_SKIP), target ) );

//	length += sprintf(buf+length,"%s", quickWizardAddFinishButton() );
	
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_QUICK_INSTALL ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}


/* restore data from TFTP Server which is implemented by get_config_data.c */
int quickWizardRestoreInit(QUICK_INFO *info)
{
	char  	target[128];
	char		msg[256];
	int		length = 0;

 	as_beep_short(1);

	list_replace_with_new_list(&info->defaultCfgs, &info->cgiVariables );
	writeconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &info->defaultCfgs );
	
	sprintf(target, "%s", WEB_URL_CONTENT  );
	length += sprintf(msg+ length, "%s,",gettext("Restore From Init will cost 1 minuts or more") );
	length += sprintf(msg+ length, "%s!",gettext("it is determined by the services you selected"));
	cgi_refresh_page(60, target, gettext("Rebooting....."), msg ) ;
	
	system("reboot");
	return 0;	
}

static int _quickWizard_cgi_init()
{
	
	cgi_html_header();
	list_create(&__quickInfo.cgiVariables);
	read_cgi_input(&__quickInfo.cgiVariables, NULL, NULL);

	list_create(&__quickInfo.lanCfgs);
	if(readconfig(ETH1_CONFIG_FILE, ITEM_EQU, NULL, &__quickInfo.lanCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&__quickInfo.wanCfgs);
	if(readconfig(ETH0_CONFIG_FILE, ITEM_EQU,NULL, &__quickInfo.wanCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&__quickInfo.pppSecretCfgs);
	if(readconfig(CONF_PPP_PAP, ITEM_FOR, NULL, &__quickInfo.pppSecretCfgs) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}
	
	list_create(&__quickInfo.pppoeCfgs);
//	cgi_syscfg_pppoe_read_conf(CONF_PPP_PAP,&__quickInfo.pppoeCfgs);
	if(readconfig(CONF_PPPOE, ITEM_EQU, NULL, &__quickInfo.pppoeCfgs) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&__quickInfo.defaultCfgs);
	if (readconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &__quickInfo.defaultCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	return 0;
}


int main()
{
	char	*cmd;
	char title[128];
	
	if(_quickWizard_cgi_init()<0 )
	{
		return 1;
	}

	cmd = GET_VALUE(&__quickInfo.cgiVariables, CGI_KEYWORD_OPERATOR );   
	if( !cmd || !strlen(cmd) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("Select mode of WAN") ); 
		cgi_info_page(title, "", __cgi_wan_protocol_select_buttons(&__quickInfo) );
		return 0;
	}
	
#if 0
	if(op == QUICK_WIZARD_OP_FINISHED)
		return quickWizardFinish();
#endif

	if( !strcasecmp(cmd, CGI_QUICK_OP_SELECT) )
	{/* WAN */
		return quickWizardWanProtocol(&__quickInfo);//sysconfig_hostname();
	}
	else
		if( !strcasecmp(cmd, CGI_QUICK_OP_WAN) )
	{/* LAN */

		return quickWizardWan(&__quickInfo);
	}	
#if 0
	else if( !strcasecmp(cmd, CGI_QUICK_OP_LAN)||!strcasecmp(cmd, CGI_SYSCFG_OP_LAN_DHCP) )
	{
		return cgi_lan_setup(&__syscfgInfo.cgiVariables, CGI_LAN_EHT1, CGI_LAN_WHERE_WIZARD);
//		return sysconfig_lan(&__syscfgInfo);
	}
#else	
	else if(!strcasecmp(cmd, CGI_QUICK_OP_LAN) ||
		!strcasecmp(cmd, CGI_SYSCFG_OP_LAN) ||
		!strcasecmp(cmd, CGI_SYSCFG_OP_LAN_DHCP) )
	{
		return quickWizardLan(&__quickInfo);
	}
#endif

	else if(!strcasecmp(cmd, CGI_QUICK_OP_DEFAULT))
	{

		return quickWizardRestoreInit(&__quickInfo);
	}
	else if(!strcasecmp(cmd, CGI_QUICK_OP_FINISH ))
	{
		return quickWizardFinish();
	}

	cgi_error_page(NULL, gettext("System Configuration Error"), gettext("No command for this menu") );
	
	return 0;	
}

