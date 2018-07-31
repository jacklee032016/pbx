/*$Id: syscfgMain.c,v 1.8 2007/09/21 21:16:02 lizhijie Exp $*/

#include "sysconf.h"

static SYSCFG_INFO __syscfgInfo;
	
static int _sysconf_cgi_init()
{
	cgi_html_header();
	read_cgi_input(&__syscfgInfo.cgiVariables, NULL, NULL);
	if(readconfig(ETH0_CONFIG_FILE, ITEM_EQU,NULL, &__syscfgInfo.wanCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}
	
	
//	cgi_syscfg_pppoe_read_conf(CONF_PPP_PAP,&__syscfgInfo.pppoeCfgs);
	if(readconfig(CONF_PPPOE, ITEM_EQU, NULL, &__syscfgInfo.pppoeCfgs) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}
	
	if (readconfig( SYSUSER_CONF_FILE, ITEM_DBL, NULL, &__syscfgInfo.usersCfgs ) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}
	return 0;
}


int main(int argc, char *argv[])
{
	char		*cmd;
	if(_sysconf_cgi_init()<0 )
	{
		//cgi_error_page(NULL, gettext("Help Failed"), gettext( "Some Errors in your releases for HELP function" ) );
		return 1;
	}
	
	cmd = GET_CGI_OP( &__syscfgInfo.cgiVariables );
	
	if(!cmd || !strlen(cmd)|| !strcasecmp(cmd, CGI_SYSCFG_OP_HOSTNAME) )
	{
	cgitrace;
		return sysconfig_hostname(&__syscfgInfo );
	}
	else if(!strcasecmp(cmd, CGI_SYSCFG_OP_WAN))
	{
		return sysconfig_wan(&__syscfgInfo);
	}
	else if(!strcasecmp(cmd, CGI_SYSCFG_OP_REBOOT))
	{
		return sysconfig_reboot(&__syscfgInfo);
	}
	else if(!strcasecmp(cmd, CGI_SYSCFG_OP_USER))
	{
		return sysconfig_user(&__syscfgInfo);
	}
	else if(!strcasecmp(cmd, CGI_SYSCFG_OP_DESCRIPTION))
	{
		return sysconfig_descrip(&__syscfgInfo);
	}
	
	cgidebug("sysconf command '%s' is not found\n", cmd );
	cgi_error_page(NULL, gettext("System Configuration Error"), gettext("No command for this menu") );
	
	return 0;	
}

