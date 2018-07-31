/*
 * $Id: cgi_navigate.c,v 1.38 2007/09/15 19:45:14 lizhijie Exp $
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "cgiLib.h"
#include "cgi_version.h"
#include "cgi_navigate.h"

#include "servicesMisc.h"
#include "cgi_data.h"
#include "cgi_sysinfo.h"
#include "cgi_wifi.h"

#include "cgi_video.h"
#include "cgi_upload_file.h"
#include "cgi_syscfg.h"
#include "cgiDdns.h"	

NAVIGATE_FOLDER 	folders [] = 
{
	{
		.id			=	NAVI_FOLDER_ROOT,
		.name		=	"foldersTree",
		.menuName	=	"SONiX",
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
		.id			=	NAVI_FOLDER_SERVICES,
		.name		=	"services",
		.menuName	=	_("System Services"),
		.iconName	=	"iWebServer",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_SERVICES_CONFIG,
		.name		=	"servicesCfg",
		.menuName	=	_("Services Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_SERVICES,
		.where		=	MENU_WHERE_BASE,
	},


	{
		.id			=	NAVI_FOLDER_WIRELESS,
		.name		=	"Wireless",
		.menuName	=	_("Wifi Device Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},


	{
		.id			=	NAVI_FOLDER_VIDEO,
		.name		=	"MediaConfiguration",
		.menuName	=	_("Media Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_AUDIO_CONFIG,     			//wyb
		.name		=	"AudioStatic",
		.menuName	=	_("Audio Config"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_VIDEO_CONFIG,
		.name		=	"VideoStatic",
		.menuName	=	_("Video Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.name		=	"VideoDynamic",
		.menuName	=	_("Video Dynamic Configuration"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.id			=	NAVI_FOLDER_VIDEO_ACTION,
		.name		=	"VideoMonitor",
		.menuName	=	_("Media Monitor"),
		.iconName	=	"iJvm",
		.hReference	=	"",
		.parentId		=	NAVI_FOLDER_ROOT,
		.where		=	MENU_WHERE_BASE,
	},

};

NAVIGATE_MENU_ITEM	menus[] = 
{

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
		.name		=	CGI_STR_ETH_CONFIG, //_("WAN Configuration"),
		.alertMsg		=	_("Setup the Internet Connection on Ethernet Port"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_WAN,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},

	/*added by Bob,camera description,2008-12-16,15:30*/
		
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Camera Description"),
		.alertMsg		=	_("IpCamera Description"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_DESCRIPTION,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},

	/* 20007.05.31 */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("DDNS Configuration"),
		.alertMsg		=	_("Client of Dynamic DNS"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES_DDNSD"?"CGI_KEYWORD_OPERATOR"="DDNS_OP_CONFIG,
		.folderId		=	NAVI_FOLDER_SERVICES_DDNS,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("DDNS Test"),
		.alertMsg		=	_("Test Ddns After DDNS Client Daemon has started"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES_DDNSD"?"CGI_KEYWORD_OPERATOR"="DDNS_OP_TEST,
		.folderId		=	NAVI_FOLDER_SERVICES_DDNS,
		.where		=	MENU_WHERE_BASE,
	},

#if 0
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Route Configuration"),
		.alertMsg		=	_("Setup network routing table"),
		.iconName	=	"iApplicationAttribute.gif",
		.hReference	=	WEB_URL_SYSTEM_ROUTE,
		.folderId		=	NAVI_FOLDER_NETWORK,
		.where		=	MENU_WHERE_BASE,
	},
#endif

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


	/* Services menu items */	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Services Management"),
		.alertMsg		=	_("System Version and releases Information"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES,
		.folderId		=	NAVI_FOLDER_SERVICES,
		.where		=	MENU_WHERE_BASE,
	},

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Syslog Configuration"),
		.alertMsg		=	_("System Log for different application"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_SYSLOG,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("UPNP Configuration"),
		.alertMsg		=	_("Configuration of Universal PnP Gateway Device"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_UPNP,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},

  	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("IPSET"),
		.alertMsg		=	_("ipset function"),
		.iconName	=	"iServer.gif",
		.hReference	=	WEB_URL_SERVICES"?"CGI_KEYWORD_OPERATOR"="SERVICES_MISC_OP_IPSET,
		.folderId		=	NAVI_FOLDER_SERVICES_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},

	
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

	/* video configuration menuitems */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Quality Config"),
		.alertMsg		=	_("Quality Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_QUALITY,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Flicker Config"),
		.alertMsg		=	_("Flicker Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_FLICKER,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},

/*
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Resolution Config"),
		.alertMsg		=	_("Resolution Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_RESOLUTION,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
*/	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Bitrate Config"),
		.alertMsg		=	_("Bitrate Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_BITRATE,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("FrameRate Config"),
		.alertMsg		=	_("FrameRate Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_FRAMERATE,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},

	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Stream config"),
		.alertMsg		=	_("Stream config of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STREAM_CFG,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Stream desc"),
		.alertMsg		=	_("Stream desc of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STREAM_DESC,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("enauth"),
		.alertMsg		=	_("enauth config of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_ENAUTH,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("enwatchdog"),
		.alertMsg		=	_("enwatchdog config of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_WATCHDOG,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	}, 
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("mediaserver log"),
		.alertMsg		=	_("mediaserver log config of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_MEDIASERVER_LOG,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	}, 
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("ACL"),
		.alertMsg		=	_("ACL config of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_ACL,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	}, 
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("AudioE Config"),
		.alertMsg		=	_("AudioE Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_AUDIO,
		.folderId		=	NAVI_FOLDER_AUDIO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Audiosr Config"),
		.alertMsg		=	_("Audiosr Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_AUDIOSR,
		.folderId		=	NAVI_FOLDER_AUDIO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Audioch Config"),
		.alertMsg		=	_("Audioch Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_AUDIOCH,
		.folderId		=	NAVI_FOLDER_AUDIO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	/////////////////
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Brightness Config"),
		.alertMsg		=	_("Brightness Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_BRIGHTNESS,
		.folderId		=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Contrast Config"),
		.alertMsg		=	_("Contrast Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_CONTRAST,
		.folderId		=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Saturation Config"),
		.alertMsg		=	_("Saturation Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_SATURATION,
		.folderId		=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Hue Config"),
		.alertMsg		=	_("Hue Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_HUE,
		.folderId		=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Sharpness Config"),
		.alertMsg		=	_("Sharpness Configuration of Video Device"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_SHARPNESS,
		.folderId		=	NAVI_FOLDER_VIDEO_DYNAMIC,
		.where		=	MENU_WHERE_BASE,
	},
	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("View Size"),
		.alertMsg		=	_("Size of Media View"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_RESOLUTION,
		.folderId		=	NAVI_FOLDER_VIDEO_CONFIG,
		.where		=	MENU_WHERE_BASE,
	},
	#if 0
	{/* Audio */
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Audio Config"),
		.alertMsg		=	_("Disable or Enable Audio Stream"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_AUDIO,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	#endif

	/* media view etc. */
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Media View"),
		.alertMsg		=	_("Realtime View of Media Stream"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_VIEW,
		.folderId		=	NAVI_FOLDER_VIDEO_ACTION,
		.where		=	MENU_WHERE_BASE,
	},
	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Media Record"),
		.alertMsg		=	_("Record Media Stream as AVI"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_RECORD,
		.folderId		=	NAVI_FOLDER_VIDEO_ACTION,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Image Capture"),
		.alertMsg		=	_("Capture a frame as a picture"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_CAPTURE,
		.folderId		=	NAVI_FOLDER_VIDEO_ACTION,
		.where		=	MENU_WHERE_BASE,
	},
	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Media Status"),
		.alertMsg		=	_("Status of Media Server"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Mirror Image"),
		.alertMsg		=	_("Image mirror horizontally"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_MIRROR,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("VFlip Image"),
		.alertMsg		=	_("Image mirror vertically"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_SETUP_VFLIP,
		.folderId		=	NAVI_FOLDER_VIDEO,
		.where		=	MENU_WHERE_BASE,
	},
	
	{
		.target		=	NAVIGATE_TARGET_MAIN_FRAME,
		.name		=	_("Motion Detect"),
		.alertMsg		=	_("Parameters of Motion Detection"),
		.iconName	=	"iUsingJMC.gif",
		.hReference	=	WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_MOTION_DETECT,
		.folderId		=	NAVI_FOLDER_VIDEO_ACTION,
		.where		=	MENU_WHERE_BASE,
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
	printf("<title>SONiX Technology, Inc.</title>\n");
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

	read_cgi_input(&info->cgiVariables, NULL, NULL);

	info->type = WEB_TYPE_BASE;
	info->where = MENU_WHERE_BASE;
	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);

	if( !strcasecmp(cmd, CGI_NAVIGATE_KEYWORD_SERVERS) )
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


