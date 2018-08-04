/* cgi_lib.h - header file for cgi-lib.c
$Author: lizhijie $
$Revision: 1.37 $
*/

#ifndef __CGI_LIB_H__
#define __CGI_LIB_H__

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <errno.h>
#include "cgi_sigdef.h"
#include "cgi_llist.h"

#if 0
/* put this defination into Makefile and Makefile.pre */
#define		WITH_MESH					0		/* 1 */
#endif

/* MACRO for conditional compiling */
#define 		DEBUG						0
#define		WITH_STATIC_HTML			0		/* function for old style of parse static html file */
#define		WITH_MISC_HTML			0		/* not used html tag functions */

// for i18n
#define	PACKAGE						"webadmin"
#define	TEXT_NOT_FOUND				"locale text not found"

#define _(x)								(x)

#define LINE_MAX_LENGTH		1024
#define ITEM_LENGTH 			256//64

#define ITEM_ONE 					1
#define ITEM_DBL 				2
#define ITEM_TRI 				3
#define ITEM_TRI_DHCP			10
#define ITEM_FOR 				4
#define ITEM_BLCK_LIST			5
#define ITEM_IP_FORWARD  		6
#define ITEM_EQU				99

//for raw read or write cofigure
#define	LINE4RAW				128

//for cgi_html_output
#define	SPECOUT	 			"!@#$"
#define	SPECOUT1				"%*$%"
#define	SPECOUT2				"#@$%"

//following are names from html but should not write in conf files
#define	SUBMIT					"WriteToConf"
#define	RESET					"ReadFromConf"
#define	CTYPE					"ct"
#define	VTYPE					"vt"

#define	PPP0 					"ppp0"
#define	ETH0					"eth0"
#define	ETH1					"eth1"
#define	WIFI0					"wifi0"
#define	MESH_PORTAL			"mbr"

#define	SYS_CFG_PATH			"/etc/sys"

#define  CGI_DOMAIN_FILE		"/etc/web/domain.conf"
#define  CGI_HOST_NAME_FILE	"/etc/web/hostname.conf"

#define	SYSUSER_CONF_FILE		"/etc/web/user_list.conf"


#define	HDDISK_PART_MOUNT_POINT_1_ROOT			"/"
#define	HDDISK_PART_MOUNT_POINT_2_SPOOL		"/var/spool"
#define	HDDISK_PART_MOUNT_POINT_3_FILESERVER	"/var/data/share"

//#define	HDDISK_PART_MOUNT_POINT_2_SPOOL		"/var/data"

/* change this if you are using HTTP upload */
#ifndef UPLOADDIR
#define UPLOADDIR							"/tmp"
#endif


#if 		DEBUG
#define 	cgidebug(fmt, args...) 	fprintf(stderr, fmt, ##args)
#define	cgitrace					fprintf(stderr, "%s.%s()-%d lines\n", __FILE__, __FUNCTION__, __LINE__)
#else
#define	cgidebug(fmt, args...) 	do{}while(0)
#define 	cgitrace					do{}while(0)
#endif


#define	SYSTEM_CALL(cmd)	\
	system(cmd); cgidebug("system command : %s\n", cmd);

#define	DIR_DEVICE						"/etc/deviceID"
#define	PARSE_FILE_MAX_LENGTH		1024
#define	KEY_MAX_LENGTH				96
#define	VALUE_MAX_LENGTH				PARSE_FILE_MAX_LENGTH-KEY_MAX_LENGTH-32

#include "cgi_llist.h"

//#define trace 	printf("%s.%s[%d]\r\n",  __FILE__, __FUNCTION__, __LINE__ );
/* CGI Environment Variables */
#define SERVER_SOFTWARE 			getenv("SERVER_SOFTWARE")
#define SERVER_NAME 				getenv("SERVER_NAME")
#define GATEWAY_INTERFACE 			getenv("GATEWAY_INTERFACE")

#define SERVER_PROTOCOL 			getenv("SERVER_PROTOCOL")
#define SERVER_PORT 				getenv("SERVER_PORT")
#define REQUEST_METHOD 			getenv("REQUEST_METHOD")
#define PATH_INFO 					getenv("PATH_INFO")
#define PATH_TRANSLATED 			getenv("PATH_TRANSLATED")
#define SCRIPT_NAME 				getenv("SCRIPT_NAME")
#define QUERY_STRING 				getenv("QUERY_STRING")
#define REMOTE_HOST 				getenv("REMOTE_HOST")
#define REMOTE_ADDR 				getenv("REMOTE_ADDR")
#define AUTH_TYPE 					getenv("AUTH_TYPE")
#define REMOTE_USER 				getenv("REMOTE_USER")
#define REMOTE_IDENT 				getenv("REMOTE_IDENT")
#define CONTENT_TYPE 				getenv("CONTENT_TYPE")
#define CONTENT_LENGTH 			getenv("CONTENT_LENGTH")

#define HTTP_USER_AGENT 			getenv("HTTP_USER_AGENT")
#define LOCALE						getenv("LC_ALL")

typedef enum
{
	WEB_TYPE_WWW = 0,		/* upload all WWW pages for Internet Web Server */
	WEB_TYPE_ADMIN,			/* upload admin package */	
	WEB_TYPE_PBX_FIRMWARE
}web_type_t;

typedef struct 
{
	char 	name[5];
	char 	addr[20];
	char 	MAC[20];
	char 	netmask[20];
	char 	bcast[20];
	char		netAddress[24];
	int 		state;
	web_type_t	type;
}interface_info;


typedef	struct
{
	interface_info		wanInfo;
	interface_info		lanInfo;

	interface_info		wifiInfo;
}network_info;

typedef struct
{
	char		*key;
	char		*value;
}conf_item_t;

typedef struct 
{
	char		year[32];
	char		month[32];
	char		day[32];
	char		current_time[64];
}current_time_value_t;


#define		ROOT_PATH_WWW								"/var/www"
#define		ROOT_PATH_ADMIN								"/var/admin"


#define		WEB_CFG_WWW									"/etc/boa/www.conf"
#define		WEB_CFG_ADMIN								"/etc/boa/admin.conf"

#define		SYS_CFG_LOCALE								"/etc/boa/admin.locale"

#define		BOA_KEYWORD_LISTEN							"Listen"


typedef enum
{
	CGI_HELP_MSG_UPLOAD_ADMIN = 0,
	CGI_HELP_MSG_UPLOAD_WWW ,
	CGI_HELP_MSG_MAIL_ALIAS ,
	CGI_HELP_MSG_SMTP ,
	CGI_HELP_MSG_POP_IMAP ,
	CGI_HELP_MSG_WWW ,
	CGI_HELP_MSG_SERVICES ,
	CGI_HELP_MSG_SYSTEM_USER ,
	CGI_HELP_MSG_HOST_NAME,			/* host name amd domain name */
	CGI_HELP_MSG_LAN_CONFIG,			/* LAN config */
	CGI_HELP_MSG_WAN_CONFIG,		/* LAN config */
	CGI_HELP_MSG_STATIC_WAN_CONFIG,               /* WAN config */
	CGI_HELP_MSG_PPPOE_CONFIG,		/* LAN config */
	CGI_HELP_MSG_DISK_MGMT,			/* hard disk or usb disk info */
	CGI_HELP_MSG_QUICK_INSTALL,		/* hard disk or usb disk info */
	CGI_HELP_MSG_QUICK_INSTALL_LAN,
	CGI_HELP_MSG_LOCALE,				/* language setup */
	CGI_HELP_MSG_SYSTIME,				/* system time, such as NTP or HW  */
	CGI_HELP_MSG_SERVICES_DHCP,
	CGI_HELP_MSG_SERVICES_SYSLOG,
	CGI_HELP_MSG_SERVICES_FTP,
	CGI_HELP_MSG_SERVICES_PPTP,
	CGI_HELP_MSG_SERVICES_QOS,
	CGI_HELP_MSG_SERVICES_UPNP,
	CGI_HELP_MSG_SERVICES_RIP,
	CGI_HELP_MSG_RECORD,				/* Record and play of PBX alert voice   */
	CGI_HELP_MSG_PBX_BASIC,			/* PBX */
	CGI_HELP_MSG_PBX_BRANCH_BASIC,	/* PBX */
	CGI_HELP_MSG_PBX_BRANCH_SPEC,			/* PBX */
	CGI_HELP_MSG_PBX_LINES,			/* PBX */
	CGI_HELP_MSG_PBX_TRUNCK,			/* PBX */
	CGI_HELP_MSG_PBX_ACCOUNT,		/* PBX */
	CGI_HELP_MSG_PBX_SPECIAL_SERVICE,/* PBX */
	CGI_HELP_MSG_PBX_BILL,			/* PBX */
	CGI_HELP_MSG_PBX_CONFERENCE,	/* PBX */
	CGI_HELP_MSG_SIPUA_PROTOCOL,	/* Protocol Configuration for SIP UA */
	CGI_HELP_MSG_SIPUA_DIAL_RULE,	/* Dial Rule Configuration for SIP UA */
	CGI_HELP_MSG_PARTY_GLOBAL,		/* Global Configuration for Party SIP */
	CGI_HELP_MSG_BLACKLIST_MGMT,	/* BlackList Management  */
	CGI_HELP_MSG_ROUTE,				/* system Route  */
	CGI_HELP_MSG_SNAT,				/* source NAT  */
	CGI_HELP_MSG_VPN,					/* VPN based on PPTP */
	CGI_HELP_MSG_QOS,
	CGI_HELP_MSG_BACKUP_SERVER,
	CGI_HELP_MSG_SYSTEM_PROC,
	CGI_HELP_MSG_MESH_PORTAL,
	CGI_HELP_MSG_INVALIDATE			/* this must be the last item */
}cgi_help_type_t;


typedef enum
{
	NIC_TYPE_WAN = 0,
	NIC_TYPE_LAN,
	NIC_TYPE_WIFI
}nic_type_t;

#if 0
/* modify configuration file for WWW and WebAdmin as the run-time config of LAN/WAN */
int cgi_boa_modify_restart(web_type_t type );
#endif

int cgi_boa_rewrite_configuration(interface_info *intf  );


/* CGI URL definations */
#define	WEB_CGI_ROOT						"/cgi-bin/"

#define	WEB_URL_SYSCONFIG				WEB_CGI_ROOT"sysconf.cgi"


#define	WEB_URL_DISK_MGMT				WEB_CGI_ROOT"system/disk.cgi"

#define	WEB_URL_QUICK_INSTALL			WEB_CGI_ROOT"quick.cgi"

/* network URL */
#define	WEB_URL_SYSTEM_ROUTE			WEB_CGI_ROOT"network/route.cgi"

/* system URL */
#define	WEB_URL_SYSTEM_INFO				WEB_CGI_ROOT"system/sysInfo.cgi"
#define	WEB_URL_DATA						WEB_CGI_ROOT"system/sysData.cgi"
#define	WEB_URL_UPLOAD_ADMIN			WEB_CGI_ROOT"system/uploadAdmin.cgi"

/* services URL */
#define	WEB_URL_SERVICES					WEB_CGI_ROOT"services/servicesmisc.cgi"
#define	WEB_URL_SERVICES_DDNSD			WEB_CGI_ROOT"services/ddns.cgi"
#define	WEB_URL_SERVICES_PPTP			WEB_CGI_ROOT"pptp.cgi"

/* firewall URL */
#define	WEB_URL_FIREWALL					WEB_CGI_ROOT"network/firewall.cgi"

/* SIP UA URL */
#define	WEB_URL_SIP_UA					WEB_CGI_ROOT"sip/sipUa.cgi"

/* PBX URL */
#define	WEB_URL_PBX						WEB_CGI_ROOT"sip/pbxClient.cgi"
#define	WEB_URL_PBX_HW					WEB_CGI_ROOT"sip/pbxCheck.cgi"
#define	WEB_URL_UPLOAD_FIRMWARE		WEB_CGI_ROOT"sip/uploadFirmware.cgi"
#define	WEB_URL_UPDATE_FIRMWARE		WEB_CGI_ROOT"sip/updateFirmware.cgi"
#define	PBX_FIRMWARE_HOME				"/etc/pbx"

/* SIP Server URL*/
#define	WEB_URL_PARTY						WEB_CGI_ROOT"sip/sipServer.cgi"
#define	WEB_URL_ALERT						WEB_CGI_ROOT"sip/alert.cgi"
#define	WEB_URL_CDR						WEB_CGI_ROOT"sip/cdr.cgi"

#define	WEB_URL_WLAN						WEB_CGI_ROOT"network/wifi.cgi"

#define	WEB_URL_MESH						WEB_CGI_ROOT"network/mesh.cgi"

#define	WEB_URL_MAIL_ROOT				WEB_CGI_ROOT"servers/"
#define	WEB_URL_MAIL						WEB_URL_MAIL_ROOT"mail.cgi"


#define	WEB_URL_UPLOAD_WWW				WEB_CGI_ROOT"servers/uploadWww.cgi"
#define	WEB_URL_WWW_DETECT				WEB_CGI_ROOT"servers/www.cgi"

#define	WEB_URL_VIDEO						WEB_CGI_ROOT"network/video.cgi"

#define	WEB_URL_INDEX						WEB_CGI_ROOT"index.cgi"
#define	WEB_URL_NAVIGATE					WEB_CGI_ROOT"navigate.cgi"
#define	WEB_URL_CONTENT					WEB_CGI_ROOT"content.cgi"

#define	WEB_URL_WIZARD					WEB_CGI_ROOT"wizard.cgi"

#define	WEB_URL_SCANNER					WEB_CGI_ROOT"network/scanner.cgi"

/*VPN URL */
#define	WEB_URL_VPN						WEB_CGI_ROOT"network/vpn.cgi"

#define 	MAIL_SMTP_ALIASES_FILE			"/etc/postfix/aliases"
#define	MAIL_SMTP_POSTFIX_CONFIG			"/etc/postfix/main.cf"

#define	MAIL_POP_IMAP_CONF				"/etc/dovecot/dovecot.conf"

#define	WEB_ALIASES_DB					"/etc/web/mail_aliases.conf"
#define	WEB_GROUP_DB						"/etc/web/mail_group.conf"


#define		CGI_STR_ERROR					_("Error")
#define		CGI_STR_SUCCESS				_("Success")
#define		CGI_STR_SAVE					_("Save")
#define		CGI_STR_ADD					_("Add")
#define		CGI_STR_DELETE					_("Delete")
#define		CGI_STR_ACTIVATE				_("Activate")
#define		CGI_STR_DEACTIVATE			_("Deactivate")
#define		CGI_STR_MODIFY				_("Modify")
#define		CGI_STR_DEFAULT				_("Default")
#define		CGI_STR_RESET					_("Reset")
#define		CGI_STR_SUBMIT				_("Submit")
#define		CGI_STR_MANAGEMENT			_("Management")

#define		CGI_STR_STOP					_("Stop")
#define		CGI_STR_RUNNING				_("Running")

#define		CGI_STR_NOTE					_("Note")
#define		CGI_STR_DEVICE					_("Device")
#define		CGI_STR_TYPE					_("Type")

#define		CGI_STR_YES					_("Yes")
#define		CGI_STR_NO						_("No")
#define		CGI_STR_YES_CASE				_("YES")
#define		CGI_STR_NO_CASE				_("NO")
#define		CGI_STR_ON						_("On")
#define		CGI_STR_OFF					_("Off")
#define		CGI_STR_ENABLE				_("Enable")
#define		CGI_STR_DISABLE				_("Disable")
#define		CGI_STR_BEGIN					_("Begin")
#define		CGI_STR_END					_("End")
#define		CGI_STR_EXIT					_("Exit")
#define		CGI_STR_PLAY					_("Play")
#define		CGI_STR_RECORD				_("Record")
#define		CGI_STR_THANKS				_("Thanks")

#define		CGI_STR_DISK_MANAGE			_("Disk Management")
#define		CGI_STR_RESTORE_DEFAULT		_("Restore Default")

#define		CGI_STR_NAME					_("Name")
#define		CGI_STR_SYSTEM_USER			_("System User")
#define		CGI_STR_USER_NAME			_("User Name")
#define		CGI_STR_PASSWORD				_("Password")

#define		CGI_STR_NEXT					_("Next")
#define		CGI_STR_STATUS				_("Status")
#define		CGI_STR_ALL					_("All")
#define		CGI_STR_INDEX					_("Index")

#define		CGI_STR_DHCP_CONFIG			_("DHCP Configuration")

#define		CGI_STR_LAN_CONFIG			_("LAN Configuration")
#define		CGI_STR_WAN_CONFIG			_("WAN Configuration")
#define		CGI_STR_PROTOCOL				_("Protocol")
#define		CGI_STR_IP_ADDRESS			_("IP Address")
#define		CGI_STR_NETMASK				_("Netmask")
#define		CGI_STR_GATEWAY				_("Gateway")
#define		CGI_STR_PHONE_NUMBER			_("Phone Number")

#define		CGI_STR_NETWORK_ADDRESS		_("Network Address")

#define		CGI_STR_PARTITION				_("Partition")
#define		CGI_STR_AUTHORITY				_("Authority")
#define		CGI_STR_AUTHENTICATION		_("Authentication")

#define		CGI_STR_HAS_EXIST				_("has exist")
#define		CGI_STR_IS_NOT_EXIST			_("is not exist")
#define		CGI_STR_SOME_ERROR			_("Some Errors in your releases")
#define		CGI_STR_PLEASE_WAIT			_("Please Waiting.....")
#define		CGI_STR_WAIT					_("Waiting")

#define		CGI_STR_ARE_YOU_SURE			_("ARE YOU SURE")
#define		CGI_STR_SORRY					_("Sorry")
#define		CGI_STR_WARN					_("Warning")
#define		CGI_STR_FAIL					_("Failed")

#define		CGI_STR_REBOOT				_("Reboot")
#define		CGI_STR_UPLOAD				_("Upload")
#define		CGI_STR_SET_ON_PAGAE			_("are set on this page")

#define		CGI_STR_SAVE_CONFIG			_("Save Configuration")

#define		CGI_STR_SPEED_DIAL			_("SPEED_DIAL")
#define		CGI_STR_INTERNAL_DIAL			_("INTERNAL_DIAL")
#define		CGI_STR_NORMAL_DIAL			_("NORMAL_DIAL")
#define		CGI_STR_LOCAL_DIAL			_("LOCAL_DIAL")
#define		CGI_STR_CALL_RETURN			_("CALL_RETURN")
#define		CGI_STR_URL_DIAL				_("URL_DIAL")
#define		CGI_STR_PSTN_DIAL				_("PSTN_DIAL")
#define		CGI_STR_DIRECT_DIAL			_("DIRECT_DIAL")


#define		CGI_STR_WAN_PORT				_("WAN Port")
#define		CGI_STR_LAN_PORT				_("LAN Port")
#define		CGI_STR_WIFI_PORT				_("WIFI Port")
#define		CGI_STR_MESH_PORT				_("MESH Port")


#define		CGI_STR_PREV					_("Prev")
#define		CGI_STR_SKIP					_("Skip")
#define		CGI_STR_STEP					_("Step")

#define		CGI_STR_MODE					_("Mode")
#define		CGI_STR_MAC_ADDRESS			_("MAC Address")
#define		CGI_STR_SOURCE_ADDRESS		_("Source Address")
#define		CGI_STR_DESTIN_ADDRESS		_("Destination Address")

#define		CGI_STR_QUICK_INSTALL			_("Quick Install")
#define		CGI_STR_FUTURE_VERSION		_("will be provided in future version")

#define		CGI_STR_START_WHEN_BOOT		_("Startup when boot")
#define		CGI_STR_FW_MIDDLE_CONFIG	_("Firewall Middle Configuration")
#define		CGI_STR_NO_OPERATION			_("No This Operation")
#define		CGI_STR_WITHOUT_CONF_FILE	_("Without Configuration File")

#define		CGI_STR_DNS_SERVER			_("DNS Server")
#define		CGI_STR_FIRST_DNS_SERVER		_("First DNS Server")
#define		CGI_STR_SECOND_DNS_SERVER	_("Second DNS Server")

#define		CGI_STR_PARTITION_INFO		_("Partition Info")

#define		CGI_STR_MESH_DEVICE			_("MESH Device")
#define		CGI_STR_WORK_MODE			_("Work Mode")
#define		CGI_STR_AD_HOC				_("Ad Hoc")
#define		CGI_STR_PHY_MODE				_("PHY Mode")
#define		CGI_STR_START_AODV			_("Start AODV")
#define		CGI_STR_ESSID					_("ESSID(Extended Service ID)")
#define		CGI_STR_NICKNAME				_("Nick Name")
#define		CGI_STR_RATE					_("Rate")
#define		CGI_STR_CHANNEL_FREQ			_("Channel(Fequency)")
#define		CGI_STR_CHANNEL				_("Channel")
#define		CGI_STR_AUTO					_("Auto")

#define		CGI_STR_SECONDS				_("Seconds")
#define		CGI_STR_MSECONDS				_("ms")
#define		CGI_STR_SIP_URL				_("SIP URL")
#define		CGI_STR_BASIC_CONFIG			_("Basic Configuration")

#define		CGI_STR_SIP_SERVER			_("SIP Server")
#define		CGI_STR_SIP_MY_UA				_("SIP UA Daemon")
#define		CGI_STR_CGIPBX					_("PBX MGR Task")
#define		CGI_STR_UARTD					_("PBX Signal Task")

#define		CGI_STR_ALERT_VOICE			_("Alert Voice")

#define		CGI_STR_ID						_("ID")
#define		CGI_STR_START_CYLINDER		_("Start Cylinder")
#define		CGI_STR_END_CYLINDER			_("End Cylinder")
#define		CGI_STR_MOUNT_POINT			_("Mount Point")

#define		CGI_STR_CHINESE				_("Chinese")
#define		CGI_STR_JAPANESE				_("Japanese")
#define		CGI_STR_ENGLISH				_("English")

#define		CGI_WIFI_ACL_OPEN_STR		_("NO")
#define		CGI_WIFI_ACL_ALLOW_STR		_("ALLOW")
#define		CGI_WIFI_ACL_DENY_STR		_("DENY")
#define		CGI_WIFI_ACL_FLUSH_STR		_("FLUSH")
#define		CGI_WIFI_ACL_DEATTACH_STR	_("DEATTACH")

#define		CGI_STR_TEST_RESULT			_("Test Result")
#define		CGI_STR_DAEMON				_("Daemon")

#define		CGI_STR_PEANUT_NAME			_("Peanut Hull")
#define		CGI_STR_LOG_CONFIGURATION	_("Log Configuration")
#define		CGI_STR_LOG_FILE				_("Log File")
#define		CGI_STR_LOG_LEVEL				_("Log Level")
#define		CGI_STR_CONFIGURATION		_("Configuration")
#define		CGI_STR_PORT					_("Port")

#define		CGI_STR_HELP_NOT_DEFINED		_("Help has not defined for this")
#define		CGI_STR_HELP_IN_THIS_PAGE	_("In this page you can config")

#define	SYSTIME_STR_CONFIGURATION			_("System Time Configuration")
#define	SYSTIME_STR_STEP						CGI_STR_STEP
#define	SYSTIME_STR_CURRENT_TIME				_("Current System Time")
#define	SYSTIME_STR_SYSTEM_ERROR				_("System Internal Error")
#define	SYSTIME_STR_TIME_ZONE				_("Time Zone")
#define	SYSTIME_STR_TIME_MODE				_("Time Mode")
#define	SYSTIME_STR_CURRENT					_("Current")
#define	SYSTIME_STR_NTP_SERVER				_("NTP Server")

#define	CGI_STR_SERVER							_("Server")

#define	CGI_STR_LOCAL							_("Local")
#define	CGI_STR_REMOTE						_("Remote")

#define	SYS_LOCALE_CONF					"/etc/sys/locale.conf"
#define	SYS_TIMEZONE						"TZ"

#define		CGI_KEYWORD_OPERATOR					"op"


#define	CGI_ACTION							"action"
#define	CGI_ACTION_ADD					CGI_STR_ADD
#define	CGI_ACTION_ACTIVATE				CGI_STR_ACTIVATE
#define	CGI_ACTION_DEACTIVATE			CGI_STR_DEACTIVATE
#define	CGI_ACTION_DEL					CGI_STR_DELETE
#define	CGI_ACTION_MODIFY					CGI_STR_MODIFY
#define	CGI_ACTION_SELECT					_("Select")
#define	CGI_ACTION_DEFAULT				CGI_STR_DEFAULT
#define	CGI_ACTION_SAVE					CGI_STR_SAVE
#define	CGI_ACTION_RESET					CGI_STR_RESET /* reset PBX */


#define	GET_CGI_OP(cgiVars)					GET_VALUE((cgiVars), CGI_KEYWORD_OPERATOR)
#define	GET_CGI_ACTION(cgiVars)			GET_VALUE((cgiVars), CGI_ACTION)

#define	CGI_COLOR_RED						"#ff0000"
#define	CGI_COLOR_GREEN					"#00ff00"
#define	CGI_COLOR_BLUE					"#0000ff"

#define	CGI_SPRINTF(_msg,_len, _fmt, ...)				\
	snprintf(_msg+_len, sizeof(_msg)-_len,  _fmt, ##__VA_ARGS__)

#define	CGI_INSTANCE_INFO(msg, color)	\
	do{\
		printf("<br><font color=\"%s\">%s</font>", color, msg); \
		fflush(stdout);	\
	}while(0)

#define	SYS_PROC_PBX						"pbx"
#define	SYS_PROC_CGIPBX					"cgipbx"
#define	SYS_PROC_UARTD					"uartd"
#define	SYS_PROC_SIPSERVER				"partysip"
#define	SYS_PROC_RECORD					"alert"

/* form processing routines */
void unescape_url(char *url);
int read_cgi_input(llist* entries, int (*callback)(char *, void *), void *data);
int getline(char s[], int lim);

#if WITH_MISC_HTML
short accept_image();
char *cgi_value(llist l,char *name);
/* miscellaneous CGI routines */
int parse_cookies(llist *entries);
void print_cgi_env();
#endif

char **cgi_val_multi(llist l, char *name);
char **cgi_val_all(llist l);
void cgi_locale_filename(char *filename, char *localename);
node *cgi_locatename(llist *l, char *name,int rigor);
node *cgi_locatename_raw(llist *l, char *name, int note);
char *cgi_name(llist l,char *value);
char **cgi_name_multi(llist l, char *value);
int cgi_html_parse_input(char *linein,char *lineout, llist *l);
int cgi_html_parse_select(char *linein,char *lineout, llist *l);
int cgi_searchvalue_fromname(char *s,char *strname,char *strvalue);

#if DEBUG
void print_entries(llist l);
#endif

void cgi_html_header();
void *wrapper_malloc(size_t size);

char *escape_input(char *str);
int read_file_upload(llist *entries, int maxfilesize);

#if WITH_STATIC_HTML 
/* boolean functions */
short is_form_empty(llist l);
short is_field_exists(llist l, char *str);
short is_field_empty(llist l, char *str);

/* from file of html_lib.h  */
void cgi_mime_header(char *mime);
void cgi_nph_header(char *status);
void cgi_show_html_page(char *loc);
void cgi_status(char *status);
void cgi_pragma(char *msg);
void cgi_set_cookie(char *name, char *value, char *expires, char *path,
		char *domain, short secure);
void cgi_html_begin(char *title);
void cgi_html_end();

/* better to do printf inside of function, or return string? */
void cgi_h1(char *header);
void cgi_h2(char *header);
void cgi_h3(char *header);
void cgi_h4(char *header);
void cgi_h5(char *header);
void cgi_h6(char *header);
void cgi_hidden(char *name, char *value);
void cgi_error(char *errstring);
void cgi_html_output(char *filename,llist *l,void (*specout)());
#endif

char *gettext(char *text);

/* from cgi_signal.c */
int cgi_directory_filt_by_number(const struct dirent *namelist);
void cgi_seng_signal_to_process_by_name(int sig, char *prog_name);
void fork_cmd(const char *cmd) ;

/* from cgi_file_func.c */
void cgi_parse_config(char *key, char *value);
int cgi_create_out_html_file(char *filename,  void(*specialparse)(char *, void *), void *data );
void cgi_read_local_config(char *filename,void(*parseLocalConfig)(char *, void *), void *data);

/*check family*/
int check_isdigit(char * pvalue);
int check_entries_value(llist * tmplist, int flag);
int check_ip_value(char * pvalue);
int check_ip_entries_error(llist * tmplist);
/*from filecommonfunc.c */
int readline(entrytype *entry,int itemnum, FILE* fname);
int readconfig(char *filename,int itemnum,char *ctype,llist *l);
int readconfig_raw(char *filename,llist *l);
int writeline(entrytype *entry,int itemnum, FILE* fname);
int writeconfig(char *filename,int itemnum,char *ctype,llist *l);
int writeconfig_no_override(char *filename,void (*dealline)(char *line,void *data,void *refdata), llist *l);
void get_pid_name(char * pidfile,char  *name ,char * pid);
int process_is_run(char * name, char *  pid );


void kill_process(char * process_name);
void int_to_str(unsigned short d,char * str);
void cgi_syscfg_update_host_conf();


int get_nic_info(interface_info  * peth,char * eth_name);
network_info	*cgi_get_network_info(int isCgi);

char *cgi_domain_name_read();
int cgi_domain_name_write(char *domainname);
char *cgi_host_name_read();
int cgi_host_name_write(char *hostname);

int cgi_refresh_page(int timeout, char *target, char *header, char *msg);
int cgi_error_page(char *target, char *header, char *errmsg);
int cgi_info_page( char *header, char *infomsg, char *othermsg);
void cgi_invalidate_page( );
char *cgi_refresh_script(char *target, int seconds);
int cgi_future_page( char *cmdName, char *module);

char *cgi_help_button( cgi_help_type_t msgType);
char *cgi_button(char *name, char *onclick_target);
char *cgi_button_alert(char *name, char *onclick_target, char *alertMsg );
char *cgi_submit_button(char *name );
char *cgi_submit_button_with_alert(char *name , char *alertMsg, char *formName);
char *cgi_reset_button(char *name );

int config_file_replace_field(char *filename, int itemnum,char *key, char *newValue);

int list_add_config_items(llist *list, conf_item_t *items, int size);

/* return 0 : not validate IP address */
int isValidateIpAddress(char *ipAddrStr);

int list_replace_with_new_list(llist *rawList, llist *newList);

int	cgi_check_pci_device(char *key, char *pciDeviceId);
int	cgi_check_usb_device(char *vendor, char *usbDeviceId);
int	cgi_check_usb_driver(char *driverName);

int	cgi_nic_refresh_net_and_broad_address(llist *nicCfgs);
int	cgi_get_files_in_dir(char *dirName, llist *filesList);

current_time_value_t *systime_get_current_time(void);

#endif

