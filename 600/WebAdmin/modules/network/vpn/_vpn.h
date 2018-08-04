/*
* $Id: _vpn.h,v 1.3 2007/09/21 21:16:02 lizhijie Exp $
*/

#ifndef	___VPN_H__
#define	___VPN_H__

#include "cgi_vpn.h"
#include "cgi_syscfg.h"

#define	VPNS_PPTPD_CONFIG_FILE					"/etc/pptpd.conf"
#define	VPNS_PPPD_CONFIG_FILE					"/etc/ppp/pptpd-options"


#define	SYSLOGD_CFG_KEYWORD_MAX_SIZE			"log_maxsize"
#define	SYSLOGD_CFG_KEYWORD_HOME_DIR			"home_dir"

#define	SYSLOGD_CFG_DEFAULT_HOME_DIR			"/var/adm"

/* pptpd */
#define	VPNS_PPTPD_LOCAL_IP						"localip"
#define	VPNS_PPTPD_REMOTE_IP						"remoteip"
#define	VPNS_PPTPD_REMOTE_IP_BEGIN				"ftp"
#define	VPNS_PPTPD_REMOTE_IP_END				"ftp"

/* pppd */
/* one unit item*/
#define	VPNS_PPPD_AUTH							"auth"
#define	VPNS_PPPD_LOCK							"lock"
#define	VPNS_PPPD_DUMP							"dump"
#define	VPNS_PPPD_DEBUG							"debug"
#define	VPNS_PPPD_PROXYARP						"proxyarp"
#define	VPNS_PPPD_MULTILINK						"multilink"

/* two units item*/
#define	VPNS_PPPD_LOG_FILE						"logfile"
#define	VPNS_PPPD_MS_WINS					"ms-wins"
#define	VPNS_PPPD_MS_DNS						"ms-dns"
#define	VPNS_PPPD_IP_PARAM					"ipparam"
//#define	VPNS_PPPD_LOG_FILE						"logfile"

#define	VPNS_PPPD_AUTH_SERVER_NAME				"name"

#define	VPNS_PPPD_AUTH_EAP						"AUTH_EAP"
#define	VPNS_PPPD_AUTH_PAP						"AUTH_PAP"
#define	VPNS_PPPD_AUTH_CHAP						"AUTH_CHAP"
#define	VPNS_PPPD_AUTH_MS_CHAP					"AUTH_MS_CHAP"
#define	VPNS_PPPD_AUTH_MS_CHAP_V2				"AUTH_MS_CHAP_V2"
#define	VPNS_PPPD_AUTH_MPPE						"AUTH_MPPE"


typedef struct
{
	char		*key;
}pppd_item;

typedef	struct
{
	char		*key;
	char		*description;
	char		*yes;
	char		*no;
}auth_protocol_t;


#define	VPN_CLIENT_PROVIDER					"/etc/ppp/peers/provider"

#define	VPN_CLIENT_KEY_USER_NAME			"name"
#define	VPN_CLIENT_KEY_PASSWD				"passwd"
#define	VPN_CLIENT_KEY_DEVICE_NAME			"pty"
#define	VPN_CLIENT_KEY_REMOTE_NAME			"remotename"

#define	VPN_CLIENT_KEY_SERVER_NAME			"PPTP"


typedef	struct
{
	llist				cgiVariables;

	llist				pptpdConfig;
	llist				pppdConfig;
	llist				accountConfig;/* pap-secret */
	llist				vpnRoutes;

	/* for 2 units item of pppd configuration file */
	char				ms_wins[256];
	char				ms_dns[256];
	char				ipParam[256];
	char				logFile[256];
	
	char				serverName[256];
	
	char				userName[256];
	char				password[256];

	llist				clientProvider;

	llist				runtime;

}VPN_INFO;


char *vpn_tunnel_info(VPN_INFO *info);
int vpn_status_info(VPN_INFO *info);

int vpn_client_config(VPN_INFO *info);
int	vpn_server_config(VPN_INFO *info);

int	vpn_account_config(VPN_INFO *info, VPN_ACCOUNT_T type);


char *vpn_users_buttons(VPN_INFO *info, VPN_ACCOUNT_T type );
 char *vpn_as600_account_info(VPN_INFO *info);

int vpn_client_account_save(VPN_INFO *info, VPN_ACCOUNT_T type);

#endif

