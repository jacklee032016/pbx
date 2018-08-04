/*
 * $Id: cgi_navigate.c,v 1.38 2007/09/15 19:45:14 lizhijie Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cgi_lib.h"
#include "as_version.h"
#include "cgi_navigate.h"

#include "sipUa.h"
#include "servicesMisc.h"
#include "cgi_iptables.h"
#include "cgi_partysip.h"
#include "cgi_pbx.h"
#include "cgi_pbxHwCheck.h"
#include "cgi_data.h"
#include "cgi_sysinfo.h"
#include "cgi_wifi.h"
#if WITH_MESH
#include "cgi_mesh.h"
#endif
#include "cgi_video.h"
#include "cgi_scan.h"
#include "cgi_mail.h"
#include "cgi_upload_file.h"
#include "cgi_syscfg.h"
#include "cgi_alert.h"
#include "cgi_cdr.h"

#include "cgiDdns.h"		/* 2007.05.31 */
#include "cgi_vpn.h"		/* 2007.08.30 */

NAVIGATE_FOLDER 	folders [] = 
{
	{
		.id			=	NAVI_FOLDER_ROOT,
		.name		=	"foldersTree",
		.menuName	=	"AS600",
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_NOT_EXIST,
		.where		=	MENU_WHERE_ANYWHERE,
	},
	{
		.id			=	NAVI_FOLDER_NETWORK,
		.name		=	"network",
		.menuName	=	_("Network Management"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	
	{/*added 2007.05.31, lizhijie */
		.id			=	NAVI_FOLDER_SERVICES_DDNS,
		.name		=	"servicesDdns",
		.menuName	=	_("Dynamic DNS"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},
	
	{
		.id			=	NAVI_FOLDER_SYSTEM,
		.name		=	"system",
		.menuName	=	_("System Management"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_SOFT_HARDWARE,
		.name		=	"SoftHardware",
//		.menuName	=	_("Software&Hardware Management"),
		.menuName	=	_("Environs Backup/Restore"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_UPDTAE,
		.name		=	"softwareUpdate",
		.menuName	=	_("Software Update"),
		.iconName	=	"iJvm",
		.hReference	=	"",
//		.parentId		=	NAVI_FOLDER_SOFT_HARDWARE,
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_BACKUP,
		.name		=	"softwareBackup",
		.menuName	=	_("Environs Backup"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_SOFT_HARDWARE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_RESTORE,
		.name		=	"softwareRestore",
		.menuName	=	_("Environs Restore"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_SOFT_HARDWARE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_SYSTEM_DISK,
		.name		=	"systemDisk",
		.menuName	=	CGI_STR_DISK_MANAGE,
		.iconName	=	"iJvm",
		.hReference	=	"",
//		.parentId		=	NAVI_FOLDER_SOFT_HARDWARE,
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_SERVICES,
		.name		=	"services",
		.menuName	=	_("System Services"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.id			=	NAVI_FOLDER_SERVICES_CONFIG,
		.name		=	"servicesCfg",
		.menuName	=	_("Services Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_SERVICES,
		.where		=	MENU_WHERE_SERVICES,
	},

	{
		.id			=	NAVI_FOLDER_VPN,
		.name		=	"VPNCfg",
		.menuName	=	_("VPN"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.id			=	NAVI_FOLDER_VPNSERVER,
		.name		=	"VPNServer",
		.menuName	=	_("VPN Server"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VPN,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.id			=	NAVI_FOLDER_VPNACCOUNT,
		.name		=	"VPNServer",
		.menuName	=	_("VPN Accounts"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VPNSERVER,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.id			=	NAVI_FOLDER_VPNCLIENT,
		.name		=	"VPNClient",
		.menuName	=	_("VPN Client"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VPN,
		.where		=	MENU_WHERE_SERVICES,
	},
	
	{
		.id			=	NAVI_FOLDER_FIREWALL,
		.name		=	"Firewall",
		.menuName	=	_("Firewall"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.id			=	NAVI_FOLDER_FIREWALL_RULE,
		.name		=	"FirewallRule",
		.menuName	=	_("Rule Defination"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_FIREWALL,
		.where		=	MENU_WHERE_SERVICES,
	},
	
	{
		.id			=	NAVI_FOLDER_QOS,
		.name		=	"QoSCfg",
		.menuName	=	_("QoS"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVICES,
	},
	
	{
		.id			=	NAVI_FOLDER_DIALPLAN,
		.name		=	"DialPlan",
		.menuName	=	_("Dial Plan"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_DIALPLAN_TELE,
		.name		=	"DialPlanTele",
		.menuName	=	_("Telephone Side"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_DIALPLAN,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_DIALPLAN_IP,
		.name		=	"DialPlanIP",
		.menuName	=	_("IP Side"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_DIALPLAN,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_DIALPLAN_IP_RULE,
		.name		=	"DialPlanIPRule",
		.menuName	=	_("Other Number"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_DIALPLAN_IP,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_PBX,
		.name		=	"PBX",
		.menuName	=	_("Telephone Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_PBX_EXT,
		.name		=	"EXTs",
		.menuName	=	_("Extensions Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_PBX_LINES,
		.name		=	"Lines",
		.menuName	=	_("Lines Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_PBX_OTHERS,
		.name		=	"Others",
		.menuName	=	_("Others Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},
#if __WITH_NEW_STC__
	{
		.id			=	NAVI_FOLDER_PBX_HW_CHECK,
		.name		=	"HardwareCheck",
		.menuName	=	_("Hardware Check"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif

	{
		.id			=	NAVI_FOLDER_SIP_UA,
		.name		=	"SipUA",
		.menuName	=	_("SIP UA"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_SIP_UA_OTHERS,
		.name		=	"OtherSipUA",
		.menuName	=	_("Other Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_SIP_UA,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_SIP_SERVER,
		.name		=	"SipServer",
		.menuName	=	CGI_STR_SIP_SERVER,
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_ALERT_VOICE,
		.name		=	"AlertVoice",
		.menuName	=	_("IP Alert Voice"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_RUNTIME,
		.name		=	"RuntimeInfo",
		.menuName	=	_("Runtime Infos"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.id			=	NAVI_FOLDER_WIRELESS,
		.name		=	"Wireless",
		.menuName	=	_("Wireless Router"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_WIRELESS_AODV,
		.name		=	"AODV",
		.menuName	=	_("Wireless AODV Router"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
#if WITH_MESH
	{
		.id			=	NAVI_FOLDER_MESH,
		.name		=	"WirelessMESH",
		.menuName	=	_("MESH Router"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_MESH_CONTROL,
		.name		=	"MeshControl",
		.menuName	=	_("MESH Control"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_MESH,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_MESH_STATE,
		.name		=	"MeshState",
		.menuName	=	_("MESH Status"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_MESH,
		.where		=	MENU_WHERE_BASE,
	},
#endif

	{
		.id			=	NAVI_FOLDER_MAIL,
		.name		=	"Mail",
		.menuName	=	_("Mail Server"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.id			=	NAVI_FOLDER_MAIL_USER,
		.name		=	"MailUser",
		.menuName	=	_("Mail User Manager"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_MAIL,
		.where		=	MENU_WHERE_SERVERS,
	},
#if 0	
	{
		.id			=	NAVI_FOLDER_SERVERS,
		.name		=	"Server",
		.menuName	=	_("Servers"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},
#endif	
	{
		.id			=	NAVI_FOLDER_SERVER_WWW,
		.name		=	"ServerWWW",
		.menuName	=	_("WWW Server"),
		.iconName	=	"iJvm",
		.hReference	=	"",
//		.parentId		=	NAVI_FOLDER_SERVERS,
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.id			=	NAVI_FOLDER_VIDEO,
		.name		=	"VideoMonitor",
		.menuName	=	_("Video Monitor"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_VIDEO,
	}

};

NAVIGATE_MENU_ITEM	menus[] = 
{
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Quick Wizard"),
		.alertMsg		=	_("Quick setup network for LAN and WAN port"),
		.iconName	=	"iWhatsNew.gif",
		.hReference	=	WEB_URL_QUICK_INSTALL,
		.folderId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},

	/* network menu items */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Host Name"),
		.alertMsg		=	_("Address and FQDN of host"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_HOSTNAME,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_WAN_CONFIG, //_("WAN Configuration"),
		.alertMsg		=	_("Setup the Internet Connection on WAN Port"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_WAN,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_LAN_CONFIG,
		.alertMsg		=	_("Setup IP Address of LAN Port"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_LAN,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},

	/* 20007.05.31 */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Peanut Hull Configuration"),
		.alertMsg		=	_("Dynamic DNS of Peanut Hull"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES_DDNSD"?"CGI_KEYWORD_OPERATOR"="PEANUT_OP_CONFIG,
		.folderId		=	NAVI_FOLDER_SERVICES_DDNS,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Peanut Hull Test"),
		.alertMsg		=	_("Test Peanut Hull After DDNS Daemon has started"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES_DDNSD"?"CGI_KEYWORD_OPERATOR"="PEANUT_OP_TEST,
		.folderId		=	NAVI_FOLDER_SERVICES_DDNS,
		.where		=	MENU_WHERE_BASE,
	},

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Route Configuration"),
		.alertMsg		=	_("Setup network routing table"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSTEM_ROUTE,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},

	/* system menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System Information"),
		.alertMsg		=	_("System Version and releases Information"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_VERSION,
		.folderId		=	NAVI_FOLDER_SYSTEM,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System Task"),
		.alertMsg		=	_("System Task of Voice Communication"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_SYS_PROC,
//		.folderId		=	NAVI_FOLDER_SYSTEM,
		.folderId		=	NAVI_FOLDER_RUNTIME,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Language Setup"),
		.alertMsg		=	_("Setup Language of WebAdmin,Such Chinese or Japanese"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_LOCALE,
		.folderId		=	NAVI_FOLDER_SYSTEM,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System Time Setup"),
		.alertMsg		=	_("Setup System Current Time"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_SYSTEM_INFO"?"CGI_KEYWORD_OPERATOR"="CGI_SYSINFO_OP_TIME,
		.folderId		=	NAVI_FOLDER_SYSTEM,
		.where		=	MENU_WHERE_BASE,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System Programs"),
		.alertMsg		=	_("This Function is disabled"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?op=1&DEVICE=sysproc",
		.folderId		=	NAVI_FOLDER_SYSTEM,
		.where		=	MENU_WHERE_BASE,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System User Management"),
		.alertMsg		=	_("System Version and releases Information"),
		.iconName	=	"iUsers.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_USER,
		.folderId		=	NAVI_FOLDER_SYSTEM,
		.where		=	MENU_WHERE_BASE,
	},

	/* system/software&Data sub menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Software Update"),
		.alertMsg		=	_("Update a Software Package or the Whole System handly"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_UPLOAD_ADMIN"?"CGI_KEYWORD_OPERATOR"="CGI_UPLOAD_UPDATE,
		.folderId		=	NAVI_FOLDER_UPDTAE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("System Install"),
		.alertMsg		=	_("Install the Whole System on new Mass Storage Device"),
		.iconName	=	"iJDBC.gif",
//		.hReference	=	WEB_URL_DATA"?"CGI_KEYWORD_OPERATOR"="CGIDATA_OP_AUTO_UPDATE,
		.hReference	=	WEB_URL_WIZARD,
		.folderId		=	NAVI_FOLDER_UPDTAE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Local Backup"),
		.alertMsg		=	_("Backup runtime or configration Data to Client Local"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_DATA"?"CGI_KEYWORD_OPERATOR"="CGIDATA_OP_BACK_LOCAL,
		.folderId		=	NAVI_FOLDER_BACKUP,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Server Backup"),
		.alertMsg		=	_("Backup runtime or configration Data to Server"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_DATA"?"CGI_KEYWORD_OPERATOR"="CGIDATA_OP_BACK_SERVER,
		.folderId		=	NAVI_FOLDER_BACKUP,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Local Restore"),
		.alertMsg		=	_("Restore System from the Backup Data Saved in Client"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_UPLOAD_ADMIN"?"CGI_KEYWORD_OPERATOR"="CGI_UPLOAD_RESTORE,
		.folderId		=	NAVI_FOLDER_RESTORE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Data Restore"),
		.alertMsg		=	_("Restore System from the Backup Data Saved in Server"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_DATA"?"CGI_KEYWORD_OPERATOR"="CGIDATA_OP_RESTORE_SERVER,
		.folderId		=	NAVI_FOLDER_RESTORE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Default Restore"),
		.alertMsg		=	_("Restore System to the State of Factory Default"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_DATA"?"CGI_KEYWORD_OPERATOR"="CGIDATA_OP_RESTORE_INIT,
		.folderId		=	NAVI_FOLDER_RESTORE,
		.where		=	MENU_WHERE_BASE,
	},

	/* system/Disk sub menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Disk Install Wizard"),
		.alertMsg		=	_("Fdisk,Format and Mount a Disk before Servers bootup"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_DISK_MGMT"?op=install",
		.folderId		=	NAVI_FOLDER_SYSTEM_DISK,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Disk Detect"),
		.alertMsg		=	_("Detect Disk Infomation and mount status"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_DISK_MGMT,
		.folderId		=	NAVI_FOLDER_SYSTEM_DISK,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Disk Managment"),
		.alertMsg		=	_("unmount disk or remount disk"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_DISK_MGMT"?"CGI_KEYWORD_OPERATOR"=management",
		.folderId		=	NAVI_FOLDER_SYSTEM_DISK,
		.where		=	MENU_WHERE_BASE,
	},

	/* Services menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Services Management"),
		.alertMsg		=	_("System Version and releases Information"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES,
		.folderId		=	NAVI_FOLDER_SERVICES,
		.where		=	MENU_WHERE_SERVICES,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_DHCP_CONFIG,
		.alertMsg		=	_("DHCP Server Configuration and Management"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_DHCP,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_SERVICES,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("FTP Configuration"),
		.alertMsg		=	_("FTP Server Configuration and Management"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_FTP,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Syslog Configuration"),
		.alertMsg		=	_("System Log for different application"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_SYSLOG,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("UPNP Configuration"),
		.alertMsg		=	_("Configuration of Universal PnP Gateway Device"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_UPNP,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("RIP Configuration"),
		.alertMsg		=	_("Dynamic Route Protocol of Route Info Protocol"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_RIP,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_SERVICES,
	},


	/* VPN Server & client menu */
	/* VPN Server menu */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_CONFIGURATION,
		.alertMsg		=	CGI_STR_CONFIGURATION,
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_SERVER_CONFIG,
		.folderId		=	NAVI_FOLDER_VPNSERVER,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Status"),
		.alertMsg		=	_("Runtime Status"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_SERVER_STATUS,
		.folderId		=	NAVI_FOLDER_VPNSERVER,
		.where		=	MENU_WHERE_SERVICES,
	},

	/* VPN client */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_CONFIGURATION,
		.alertMsg		=	CGI_STR_CONFIGURATION,
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_CLIENT_CONFIG,
		.folderId		=	NAVI_FOLDER_VPNCLIENT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Status"),
		.alertMsg		=	_("Runtime Status"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_CLIENT_STATUS,
		.folderId		=	NAVI_FOLDER_VPNCLIENT,
		.where		=	MENU_WHERE_SERVICES,
	},

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_SERVER,
		.alertMsg		=	_("VPN Accounts of Server which are used as authenticate server"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_ACCOUNT_SERVER,
		.folderId		=	NAVI_FOLDER_VPNACCOUNT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AS600 Clients"),
		.alertMsg		=	_("VPN Client Accounts used by our AS600"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_ACCOUNT_AS600,
		.folderId		=	NAVI_FOLDER_VPNACCOUNT,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Other Clients"),
		.alertMsg		=	_("VPN Client Accounts used by the thrid party, such as Windows and PIX"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_VPN"?"CGI_KEYWORD_OPERATOR"="CGI_VPN_OP_ACCOUNT_OTHERS,
		.folderId		=	NAVI_FOLDER_VPNACCOUNT,
		.where		=	MENU_WHERE_SERVICES,
	},

	/* Firewall menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Firewall Status"),
		.alertMsg		=	_("View Current status of Firewall and different Rules Created"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL,
		.folderId		=	NAVI_FOLDER_FIREWALL,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Primary Configutation"),
		.alertMsg		=	_("Create New NAT Rule which can be activated"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_SNAT,
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Middle Configuration"),
		.alertMsg		=	_("Create New Rule with more Control"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_MIDDLE,
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Expert Configuration"),
		.alertMsg		=	_("Create New Rule with Expert Experiences"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_EXPERT,
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("NAT Configuration"),
		.alertMsg		=	_("Configuration of Network Address Translation"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL_NAT"?ct=nat",
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Facility Configuration"),
		.alertMsg		=	_("Repid Configuration of Firewall"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL_FACILTY"?vt=1&ct=common_iptables",
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Full Configuration"),
		.alertMsg		=	_("Full Configuration of Firewall"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL_FULL"?ct=filter",
		.folderId		=	NAVI_FOLDER_FIREWALL_RULE,
		.where		=	MENU_WHERE_SERVICES,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Timer Task"),
		.alertMsg		=	_("Start Different Firewall Configuration at Different Time"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_TIMER,
		.folderId		=	NAVI_FOLDER_FIREWALL,
		.where		=	MENU_WHERE_SERVICES,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Connection Track"),
		.alertMsg		=	_("Track current IP Connections"),
		.iconName	=	"iEJB.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_TRACK,
		.folderId		=	NAVI_FOLDER_FIREWALL,
		.where		=	MENU_WHERE_SERVICES,
	},

	/* QOS menu */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("QoS Configuration"),
		.alertMsg		=	_("Configuration of Voice Traffic's QoS"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_FIREWALL"?"CGI_KEYWORD_OPERATOR"="FIREWALL_OP_QOS,
		.folderId		=	NAVI_FOLDER_QOS,
		.where		=	MENU_WHERE_SERVICES,
	},

	/* SIP UA menu items, total 9 items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_BASIC_CONFIG,
		.alertMsg		=	_("Parameters Configuration of SIP Protocol"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"=protocol",
		.folderId		=	NAVI_FOLDER_SIP_UA,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Server Configuration"),
		.alertMsg		=	_("Proxy and Register Server for UAs"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"=SipServer",
		.folderId		=	NAVI_FOLDER_SIP_UA,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Gain Control"),
		.alertMsg		=	_("Law and Gain Control of RTP Decoder and Encoder"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_GAIN_CONTROL,
		.folderId		=	NAVI_FOLDER_SIP_UA,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("IP Dialing"),
		.alertMsg		=	_("Set the IP Dialing Method"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_DIAL_PATTERN,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Global Number"),
		.alertMsg		=	_("Setup Device ID and Console"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_IP_DEVICE,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Local Number"),
		.alertMsg		=	_("Rule Configuration of Local Dial"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_LOCAL_DIAL,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("PSTN Lines"),
		.alertMsg		=	_("PSTN Lines Status in IP Side"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_PSTN_LINE,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Direct Dial"),
		.alertMsg		=	_("Rule Configuration of Speed Dial"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_SPEED_DIAL,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP_RULE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Call Back"),
		.alertMsg		=	_("Phone Number and Method of Call Back"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_CALL_RETURN,
		.folderId		=	NAVI_FOLDER_DIALPLAN_IP_RULE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Black List View"),
		.alertMsg		=	_("Lookup and View of Black List"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"=back_list",
		.folderId		=	NAVI_FOLDER_SIP_UA_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("No Disturb Function"),
		.alertMsg		=	_("Service of No Disturb"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"=NoDisturbF",
		.folderId		=	NAVI_FOLDER_SIP_UA_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Black List Configuration"),
		.alertMsg		=	_("Management of Black List"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"=blackList",
		.folderId		=	NAVI_FOLDER_SIP_UA_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("No Disturb Mode"),
		.alertMsg		=	_("Mode of No Disturb"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_SIP_UA"?"CGI_KEYWORD_OPERATOR"="SIPUA_OP_DISTURBLIST,
		.folderId		=	NAVI_FOLDER_SIP_UA_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("CDR Configuration"),
		.alertMsg		=	_("Configuration of Call Detail Record"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_CDR"?"CGI_KEYWORD_OPERATOR"="CDR_OP_CONFIG,
		.folderId		=	NAVI_FOLDER_SIP_UA_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("CDR View"),
		.alertMsg		=	_("Lookup Call Information Recorded Now"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_CDR"?"CGI_KEYWORD_OPERATOR"="CDR_OP_VIEW,
		.folderId		=	NAVI_FOLDER_RUNTIME,
		.where		=	MENU_WHERE_IPPBX,
	},

	/* PBX menu items, 10 items  */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Global Configuration"),
		.alertMsg		=	_("Configration Global Information of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BASE_CONFIG,
		.folderId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	"Telephone Device",
		.alertMsg		=	_("All Telephone Device of Our PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_GLOBAL_DEVICE,
		.folderId		=	NAVI_FOLDER_DIALPLAN,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Dial Out Mode"),
		.alertMsg		=	_("Configration Global Phone Number of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_GLOBAL_NUMBER,
		.folderId		=	NAVI_FOLDER_DIALPLAN_TELE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Branch Number"),
		.alertMsg		=	_("Branch Phone Number of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_NUMBER,
		.folderId		=	NAVI_FOLDER_DIALPLAN_TELE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_BASIC_CONFIG,
		.alertMsg		=	_("Configuration Attributes of PBX Branch"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_BASE,
		.folderId		=	NAVI_FOLDER_PBX_EXT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Special Configuration"),
		.alertMsg		=	_("Special Connfiguration of PBX Branch"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_SPEC,
		.folderId		=	NAVI_FOLDER_PBX_EXT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Branch Transfer"),
		.alertMsg		=	_("Transfer Call to PBX Branch"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_EXT_TRANSFER,
		.folderId		=	NAVI_FOLDER_PBX_EXT,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Call Transfer"),
		.alertMsg		=	_("Transfer Call of PBX Branch"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_CALL_TRANSFER,
		.folderId		=	NAVI_FOLDER_PBX_EXT,
		.where		=	MENU_WHERE_IPPBX,
	},
#if __WITH_NEW_STC__
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Night Service"),
		.alertMsg		=	_("Night Service of PBX Branch"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BRANCH_NIGHT_SERVICE,
		.folderId		=	NAVI_FOLDER_PBX_EXT,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Extern Line Configuration"),
		.alertMsg		=	_("Configuration the external Lines of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_LINES,
		.folderId		=	NAVI_FOLDER_PBX_LINES,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Input Relay Configuration"),
		.alertMsg		=	_("Configuration the Input Relay Lines of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_TRUNCK,
		.folderId		=	NAVI_FOLDER_PBX_LINES,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Account Configuration"),
		.alertMsg		=	_("Configuration the Accounts of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_ACCOUNT,
		.folderId		=	NAVI_FOLDER_PBX_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Special Service Configuration"),
		.alertMsg		=	_("Configuration the Specail Service of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_SERVICES_SPEC,
		.folderId		=	NAVI_FOLDER_PBX_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
#if __WITH_NEW_STC__
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("BlackList Configuration"),
		.alertMsg		=	_("Configuration the Black List of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BLACKLIST,
		.folderId		=	NAVI_FOLDER_PBX_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Bill Configuration"),
		.alertMsg		=	_("Bill Info View and Management"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_BILL,
		.folderId		=	NAVI_FOLDER_PBX_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Tel Conference Configuration"),
		.alertMsg		=	_("Configuration of Telephone Conference"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_PBX"?"CGI_KEYWORD_OPERATOR"="CGIPBX_OP_CONFERENCE,
		.folderId		=	NAVI_FOLDER_PBX_OTHERS,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif

#if __WITH_NEW_STC__
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("RTC Check"),
		.alertMsg		=	_("Check Realtime Clock Chip"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_RTC,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("NvRam Check"),
		.alertMsg		=	_("Check Non-Volatile Ram"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_NVRAM,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Array Check"),
		.alertMsg		=	_("Check Switch Array, DTMF Encoder/Decoder"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_ARRAY,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("CallerId Check"),
		.alertMsg		=	_("DTMF Encoder/Decoder"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_CALLID_CHIP,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("PCM Check"),
		.alertMsg		=	_("Check PCM Channel between Analog and Digit Devices"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_PCM,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Ringer Check"),
		.alertMsg		=	_("Check All Rigners"),
		.iconName	=	"iLocalMachine.gif",
		.hReference	=	WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"="PBXHW_OP_CHECK_RINGER,
		.folderId		=	NAVI_FOLDER_PBX_HW_CHECK,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif
		
	{/* 2007.06.01 added */
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Firmware Update"),
		.alertMsg		=	_("Upload and Update Firmware of PBX"),
		.iconName	=	"iClone.gif",
		.hReference	=	WEB_URL_UPLOAD_FIRMWARE,
		.folderId		=	NAVI_FOLDER_PBX,
		.where		=	MENU_WHERE_IPPBX,
	},

	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_(CGI_STR_CONFIGURATION),
		.alertMsg		=	_("Setup Alerting Voice Recording Parameters"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_PARAMETER,
		.folderId		=	NAVI_FOLDER_ALERT_VOICE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_(CGI_STR_RECORD),
		.alertMsg		=	_("Record Alerting Voice of SIP UA"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_ALERT"?"CGI_KEYWORD_OPERATOR"="ALERT_OP_RECORD,
		.folderId		=	NAVI_FOLDER_ALERT_VOICE,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_(CGI_STR_MANAGEMENT),
		.alertMsg		=	_("Play Alerting Voice of SIP UA and Activate"),
		.iconName	=	"iEjbProperties.gif",
		.hReference	=	WEB_URL_ALERT,
		.folderId		=	NAVI_FOLDER_ALERT_VOICE,
		.where		=	MENU_WHERE_IPPBX,
	},

	/* SIP Server menu items, 10 items  */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_BASIC_CONFIG,
		.alertMsg		=	_("Global Environments that SIP Server used"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_PARTY,
		.folderId		=	NAVI_FOLDER_SIP_SERVER,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("User Configuration"),
		.alertMsg		=	_("Users that use this SIP Server"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_USER,
		.folderId		=	NAVI_FOLDER_SIP_SERVER,
		.where		=	MENU_WHERE_IPPBX,
	},
#if 0
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Filter External Plugin"),
		.alertMsg		=	_("External Filter Rules that used"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_FILTER_EXT,
		.folderId		=	NAVI_FOLDER_SIP_SERVER,
		.where		=	MENU_WHERE_IPPBX,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Filter Internal Plugin"),
		.alertMsg		=	_("Internal Filter Rules that used"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_FILTER_INT,
		.folderId		=	NAVI_FOLDER_SIP_SERVER,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Registered UAs"),
		.alertMsg		=	_("Lookup UA registered on this server now"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_LOOKUP,
		.folderId		=	NAVI_FOLDER_RUNTIME,
		.where		=	MENU_WHERE_IPPBX,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Filter External Plugin"),
		.alertMsg		=	_("External Filter Rules that used"),
		.iconName	=	"iServlets.gif",
//		.hReference	=	WEB_URL_SIP_SERVER_EXTERNAL"?vt=1&ct=forward",
		.hReference	=	WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_FILTER,
		.folderId		=	NAVI_FOLDER_SIP_SERVER,
		.where		=	MENU_WHERE_IPPBX,
	},
#endif
	/* Wireless menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Basic Configration"),
		.alertMsg		=	_("Use Mode(Ad Hoc/AP) of Wireless Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIRELESS_OP_MODE,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Wireless NIC Configration"),
		.alertMsg		=	_("IP Configuration of Wifi NIC"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_NIC,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("MAC Filter"),
		.alertMsg		=	_("Station MAC Address that be Denied/Allowed"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_ACL,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Authen&Crypto"),
		.alertMsg		=	_("Wifi User Authentication and Data Crypto"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AUTHEN,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Wifi Crypto"),
		.alertMsg		=	_("Crypt the frame with WEP or WPA"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_CRYPT,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Wifi Status"),
		.alertMsg		=	_("Status Of Wireless Host"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_STATUS,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
#if 0	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Wifi Scan"),
		.alertMsg		=	_("Status Of Wireless Host"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_SCAN,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
#endif	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Station Infos"),
		.alertMsg		=	_("Status Of Associated Stations"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_STATION,
		.folderId		=	NAVI_FOLDER_WIRELESS,
		.where		=	MENU_WHERE_BASE,
	},
	
	/* Wireless AODV menu items */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AODV Control"),
		.alertMsg		=	_("Clear AODV Route Item or Enable Debug"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_CTRL,
		.folderId		=	NAVI_FOLDER_WIRELESS_AODV,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AODV Route"),
		.alertMsg		=	_("Lookup AODV Information of Route"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_ROUTE,
		.folderId		=	NAVI_FOLDER_WIRELESS_AODV,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AODV Neighbor"),
		.alertMsg		=	_("Lookup AODV Information of Neighbor"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_NEIGH,
		.folderId		=	NAVI_FOLDER_WIRELESS_AODV,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AODV Timer"),
		.alertMsg		=	_("Lookup AODV Information of Timer"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_TIMER,
		.folderId		=	NAVI_FOLDER_WIRELESS_AODV,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AODV Flood"),
		.alertMsg		=	_("Lookup AODV Information of Flood"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_FLOOD,
		.folderId		=	NAVI_FOLDER_WIRELESS_AODV,
		.where		=	MENU_WHERE_BASE,
	},

#if WITH_MESH
	/* Wireless MESH menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	CGI_STR_BASIC_CONFIG,
		.alertMsg		=	_("Setup Basic Information for MESH SubSystem"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_WIRELESS_OP_MODE,
		.folderId		=	NAVI_FOLDER_MESH,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Portal Configuration"),
		.alertMsg		=	_("Configuration of MESH Portal Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_PORTAL,
		.folderId		=	NAVI_FOLDER_MESH,
		.where		=	MENU_WHERE_BASE,
	},
	/* Wireless MESH Control menu items */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Stop MESH"),
		.alertMsg		=	_("Stop All TX/RX/FWD Functions"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_CTRL_STOP,
		.folderId		=	NAVI_FOLDER_MESH_CONTROL,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("FWD Items"),
		.alertMsg		=	_("Add or Delete Forward Item"),
		.iconName	=	"iServlets.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_CTRL_FWD,
		.folderId		=	NAVI_FOLDER_MESH_CONTROL,
		.where		=	MENU_WHERE_BASE,
	},
	/* Wireless MESH Status menu items */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("MESH Devices"),
		.alertMsg		=	_("Lookup Device Informations of this MESH Node"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_STAT_DEVICE,
		.folderId		=	NAVI_FOLDER_MESH_STATE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Forward Table"),
		.alertMsg		=	_("Lookup forward item in MESH"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_STAT_FWD,
		.folderId		=	NAVI_FOLDER_MESH_STATE,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Statistics"),
		.alertMsg		=	_("Lookup Statistics Information of MESH"),
		.iconName	=	"iClassFile.gif",
		.hReference	=	WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_STAT_STATISTICS,
		.folderId		=	NAVI_FOLDER_MESH_STATE,
		.where		=	MENU_WHERE_BASE,
	},

#endif

	/* Mail menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("SMTP Server"),
		.alertMsg		=	_("Simple Mail Transfer Protocol Server Configuration"),
		.iconName	=	"iJODBC.gif",
		.hReference	=	WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_SMTP,
		.folderId		=	NAVI_FOLDER_MAIL,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("POP/IMAP Server"),
		.alertMsg		=	_("POP3(Post Office Protocol)/IMAP(Internet Mail Access Protocol) Server"),
		.iconName	=	"iJODBC.gif",
		.hReference	=	WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_POPIMAP,
		.folderId		=	NAVI_FOLDER_MAIL,
		.where		=	MENU_WHERE_SERVERS,
	},

	/* Mail/User Sub menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Aliases Management"),
		.alertMsg		=	_("Create Alias for System User"),
		.iconName	=	"iUsers.gif",
		.hReference	=	WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_ALIAS"&"MAIL_OP_TYPE"="MAIL_OP_TYPE_ALIAS,
		.folderId		=	NAVI_FOLDER_MAIL_USER,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Aliases Group Management"),
		.alertMsg		=	_("Create Group for Mail Users,such as supports or sales"),
		.iconName	=	"iUsers.gif",
		.hReference	=	WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_ALIAS"&"MAIL_OP_TYPE"="MAIL_OP_TYPE_GROUP,
		.folderId		=	NAVI_FOLDER_MAIL_USER,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Account Activating"),
		.alertMsg		=	_("Activating alias and group in Mail Server"),
		.iconName	=	"iUsers.gif",
		.hReference	=	WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_ALIAS"&"CGI_ACTION"="MAIL_OP_ACTIVATE,
		.folderId		=	NAVI_FOLDER_MAIL_USER,
		.where		=	MENU_WHERE_SERVERS,
	},

	/* Servers menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("File Server"),
		.alertMsg		=	_("Setup File Server"),
		.iconName	=	"iJODBC.gif",
		.hReference	=	"/cgi-bin/swat",
//		.folderId		=	NAVI_FOLDER_SERVERS,
		.folderId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Printer Server"),
		.alertMsg		=	_("Lookup and Configuration Printer"),
		.iconName	=	"iJODBC.gif",
		.hReference	=	"/cgi-bin/admin.cgi",
//		.folderId		=	NAVI_FOLDER_SERVERS,
		.folderId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Scanner"),
		.alertMsg		=	_("Scanner based on Web"),
		.iconName	=	"iJODBC.gif",
		.hReference	=	WEB_URL_SCANNER"?"CGI_KEYWORD_OPERATOR"="CGI_SCAN_OP_SETUP,
		.folderId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_SERVERS,
	},

	/* Servers/WWW Sub menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Upload Pages"),
		.alertMsg		=	_("Pages should be package with winrar or tar"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_UPLOAD_WWW,
		.folderId		=	NAVI_FOLDER_SERVER_WWW,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Pages Test"),
		.alertMsg		=	_("Browsing Pages in LAN port"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	"/wwwtest/",
		.folderId		=	NAVI_FOLDER_SERVER_WWW,
		.where		=	MENU_WHERE_SERVERS,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Status Detect"),
		.alertMsg		=	_("Detect Problem when WWW Server startup"),
		.iconName	=	"iJDBC.gif",
		.hReference	=	WEB_URL_WWW_DETECT"?cmd=lookup",
		.folderId		=	NAVI_FOLDER_SERVER_WWW,
		.where		=	MENU_WHERE_SERVERS,
	},
	/* video monitor menuitems */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Device Config"),
		.alertMsg		=	_("Detect and Config Video Camera"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_VIDEO,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Video View"),
		.alertMsg		=	_("View of Camera"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_VIEW,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_VIDEO,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Recorder Status"),
		.alertMsg		=	_("Status of Camera Record"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_VIDEO,
	}
};

NAVIGATE_MENU_ITEM rebootMenu =
{
	.target		=	NAVIGATE_TARGET_MAIN_FRAME,
	.name		=	CGI_STR_REBOOT,
	.alertMsg		=	_("In order to activate configuration, please reboot"),
	.iconName	=	"iLocalMachine.gif",
	.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_REBOOT,
	.folderId		=	NAVI_FOLDER_ROOT,
	.where		=	MENU_WHERE_ANYWHERE,
};


#define	FOLDER_OUTPUT(folder, parentFolder )		\
	do{ if( (parentFolder)==NULL) 					\
		printf("%s = gFld(\"%s|%s\", \"\") \n", (folder)->name, gettext((folder)->menuName), (folder)->iconName); \
		else												\
			printf("%s = insFld(%s, gFld(\"%s|%s\", \"\"))\n", (folder)->name, (parentFolder)->name, gettext((folder)->menuName), (folder)->iconName ); \
	}while(0)

#define	ITEM_OUTPUT(item, parentFolder )		\
	do{ 						 					\
		printf("insDoc(%s, gLnk(%d, \"%s^%s^%s\", \"%s\") )\n", \
		(parentFolder)->name, (item)->target, gettext((item)->name), gettext((item)->alertMsg), (item)->iconName, (item)->hReference); \
	}while(0)

static void __navigate_begin()
{
	printf("<html>\n<head>\n\n");
	printf("<SCRIPT SRC=\"/include/ftree.js\"></SCRIPT>\n");
	printf("<SCRIPT SRC=\"/include/help.js\"></SCRIPT>\n\n");
	printf("<SCRIPT>\n");
}

static void __navigate_end()
{
	printf("</SCRIPT>\n\n");
	printf("<SCRIPT>\ninitializeDocument()\n</SCRIPT>\n");
	printf("</HEAD>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n");
	printf("<title>NINGBO ASSIST, Inc.</title>\n");
	printf("<LINK href=\"/include/style.css\" rel=stylesheet type=text/css>\n");
	printf("</HTML>\n");
}

static NAVIGATE_FOLDER *__navigate_find_parent(FOLDER_ID parentId, int where)
{
	int	i= 0;
	int	count = sizeof(folders)/sizeof(folders[0]);
	NAVIGATE_FOLDER	*iter = folders;

	for(i = 0; i< count; i++)
	{
		if( iter->id == parentId && (iter->where&where) )
		{
			return iter;
		}
		
		iter++;
	}

	return NULL;
}

static void __navigate_iterate(BASE_INFO *info)
{
	int	i= 0, j;
	int	count = sizeof(folders)/sizeof(folders[0]);
	int	menuCount = sizeof(menus)/sizeof(menus[0]);
	NAVIGATE_FOLDER	*iter = folders;
	NAVIGATE_MENU_ITEM	*item = NULL;

	for(i = 0; i< count; i++)
	{
		if(( iter->where&info->where) )
		{
			FOLDER_OUTPUT(iter, __navigate_find_parent(iter->parentId, info->where) );

			item = menus;
			for(j=0; j< menuCount;j++)
			{
				if(item->folderId == iter->id && (item->where&info->where) )
				{
					ITEM_OUTPUT(item, iter);
				}
				item++;
			}
			printf("\n");
		}
		
		iter++;
	}

	ITEM_OUTPUT( &rebootMenu, __navigate_find_parent(NAVI_FOLDER_ROOT, MENU_WHERE_ANYWHERE));
}


static int	__navigate_init(BASE_INFO *info)
{
	char		*cmd;
	list_create( &info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	info->type = WEB_TYPE_BASE;
	info->where = MENU_WHERE_BASE;
	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !strcasecmp(cmd, CGI_NAVIGATE_KEYWORD_IPPBX) )
	{
		info->type = WEB_TYPE_IP_PBX;
		info->where = MENU_WHERE_IPPBX;
	}
	else if( !strcasecmp(cmd, CGI_NAVIGATE_KEYWORD_SERVERS) )
	{
		info->type = WEB_TYPE_SERVERS;
		info->where = MENU_WHERE_SERVERS;
	}
	else if( !strcasecmp(cmd, CGI_NAVIGATE_KEYWORD_SERVICES) )
	{
		info->type = WEB_TYPE_SERVICES;
		info->where = MENU_WHERE_SERVICES;
	}
	else if( !strcasecmp(cmd, CGI_NAVIGATE_KEYWORD_VIDEO) )
	{
		info->type = WEB_TYPE_VIDEO;
		info->where = MENU_WHERE_VIDEO;
	}
	
	cgi_html_header();

	return 0;
}

int main(int argc, char *argv[])
{
	BASE_INFO	info;
	
	__navigate_init( &info);
	__navigate_begin( );

	__navigate_iterate( &info);
	
	__navigate_end();
	
	return 0;
}


