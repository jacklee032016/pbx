/*
* $Id: wizardMain.c,v 1.9 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"
#include "cgi_syscfg.h"
#include "cgi_data.h"

/*
* When update from Web, it detect the target block device is USB flash Disk or HD Disk,
* then partition and format the disk differently, finally mount all partitions and extract File System Package 
* This CGI can be executed on Flash JFFS2 and USB Disk
*/

static int _wizard_cgi_init(WIZARD_INFO *info)
{
	int res = 0;
	cgi_html_header();
	
	list_create(&info->lanCfgs);
	if(readconfig(ETH1_CONFIG_FILE, ITEM_EQU, NULL, &info->lanCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&info->wanCfgs);
	if(readconfig(ETH0_CONFIG_FILE, ITEM_EQU,NULL, &info->wanCfgs))
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

	list_create(&info->pppSecretCfgs);
	if(readconfig(CONF_PPP_PAP, ITEM_FOR, NULL, &info->pppSecretCfgs) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}
	
	list_create(&info->pppoeCfgs);
//	cgi_syscfg_pppoe_read_conf(CONF_PPP_PAP,&__quickInfo.pppoeCfgs);
	if(readconfig(CONF_PPPOE, ITEM_EQU, NULL, &info->pppoeCfgs) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE));
		return -1;
	}

#if 0
	list_create(&info->defaultCfgs);
	if (readconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &info->defaultCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Restore configuration file"));
		return -1;
	}
#endif

	info->netInfo = cgi_get_network_info(1);

	info->disks = as_cgi_get_disks();
	if(!info->disks )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Disk is check in this system") );
		res = -WIZARD_ERROR_NO_DISK;
	}

	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, wizard_upload_callback, info);

	return res;
}

static int __cgi_wizard_prepare(WIZARD_INFO *info )
{
	char 	status[2048];
	char 	alert[2048];
	char 	leftButton[2048];
	char 	rightButton[2048];
	char		target[256];
	int 		length = 0;

	length += CGI_SPRINTF(status,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(status, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n", 
		gettext(CGI_STR_DEVICE), gettext(CGI_STR_PROTOCOL), gettext(CGI_STR_IP_ADDRESS));
	length += CGI_SPRINTF(status, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s(%s)</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></tr>\n", 
		gettext(CGI_STR_WAN_PORT),info->netInfo->wanInfo.name,  GET_VALUE(&info->wanCfgs, NIC_KEYWORD_PROTOCOL), info->netInfo->wanInfo.addr);
	length += CGI_SPRINTF(status, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s(%s)</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></tr>\n", 
		gettext(CGI_STR_LAN_PORT), info->netInfo->lanInfo.name, GET_VALUE(&info->lanCfgs, NIC_KEYWORD_PROTOCOL), info->netInfo->lanInfo.addr );
	length += CGI_SPRINTF(status, length, "</table>\n" );

	length = 0;	
	length += CGI_SPRINTF(alert, length, "<table height=\"21\" align=\"center\" bgColor=#f7f3f7>\n");
	length += CGI_SPRINTF(alert, length, "<tr><td>%s%s.\n", gettext("Ningbo Assist AS600 system software and data"), gettext("should be installed on CompactFlash card, Flash Disk or Hard Disk"));
	length += CGI_SPRINTF(alert, length, "%s.\n", gettext("Main purpose of WIZARD is install or update system remotely" ));
	length += CGI_SPRINTF(alert, length, "%s</td></tr>\n", gettext("Please click 'Hardware Check' to begin the install porcess") );
	length += CGI_SPRINTF(alert, length, "</table>\n" );

	length = 0;
	if(info->disks )
	{
		length += CGI_SPRINTF(rightButton,length,"%s", cgi_submit_button(gettext(WIZARD_STR_HARDWARE) ) );
	}
	else
	{
		length += CGI_SPRINTF(rightButton,length,"%s", gettext("No Disk is found") );
	}
	
	length = 0;
	sprintf(target, "%s", WEB_URL_WIZARD"?"CGI_KEYWORD_OPERATOR"="WIZARD_OP_WANCONFIG);
	length += CGI_SPRINTF(leftButton,length,"%s", cgi_button(gettext(CGI_STR_WAN_CONFIG), target) );
	
	return wizard_page( gettext(WIZARD_STR_PREPARE), status, alert, leftButton, rightButton, info);
}


int main()
{
	char	*cmd;
	WIZARD_INFO	zInfo, *info;
	int result;

	info = &zInfo;
	memset(info, 0, sizeof(WIZARD_INFO) );
	
	result = _wizard_cgi_init( info);
	if( result<0 )
	{
		if(result != -WIZARD_ERROR_NO_DISK )
			return 1;
	}

	cmd = GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR );   
	if( !cmd || !strlen(cmd) )
	{
		return __cgi_wizard_prepare(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_WANCONFIG))
	{
		return wizard_cgi_wan(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_HARDWARE))
	{
		return wizard_cgi_hardware_check(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_PARTITION))
	{
		return wizard_cgi_hd_partition(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_FORMAT))
	{
		return wizard_cgi_hd_format(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_UPLOAD))
	{
		return wizard_cgi_upload(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_UPDATE))
	{
		return wizard_cgi_update(info);
	}
	else if(!strcasecmp(cmd, WIZARD_OP_REBOOT))
	{
		return wizard_cgi_reboot(info);
	}

	cgi_error_page(NULL, gettext("Wizard Error"), gettext("No command for this menu") );
	
	return 0;	
}

