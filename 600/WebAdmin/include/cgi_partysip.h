#ifndef	__CGI_PARTY_SIP_H__
#define	__CGI_PARTY_SIP_H__

#include "cgi_lib.h"

#define	SIPPARTY_CONF_FILE 				"/usr/etc/partysip.conf"

#define	PARTY_OP_GLOBAL					"GLOBAL"
#define	PARTY_OP_USER						"USER"
#define	PARTY_OP_FILTER_EXT				"EXT_FILTER"
#define	PARTY_OP_FILTER_INT				"INT_FILTER"
#define	PARTY_OP_LOOKUP					"LOOKUP"	/* lookup UA registered now */


#define	PARTY_CFG_SERVER_IP				"serverip"
#define	PARTY_CFG_DYNAMIC_IP				"dynamic_ip"
#define	PARTY_CFG_ENABLE_IPV6			"enable_ipv6"
#define	PARTY_CFG_SERVER_NAME			"servername"
#define	PARTY_CFG_SERVER_PORT			"serverport_udp"
#define	PARTY_CFG_TRANSFORT				"transport"
#define	PARTY_CFG_MULTICAST				"multicast"
#define	PARTY_CFG_MULTICAST_DEV			"if_mcast"
#define	PARTY_CFG_PLUGINS					"plugins"
#define	PARTY_CFG_PLUGINS_CONF			"plugins_config"
#define	PARTY_CFG_RECOVERY_FILE			"recovery_file"
#define	PARTY_CFG_RECOVERY_DELAY		"recovery_delay"
#define	PARTY_CFG_SERVER_REALM			"serverrealm"
#define	PARTY_CFG_AUTHEN					"authentication"

#define	PARTY_CFG_AUTHEN_407				"force_use_of_407"

#define	PARTY_CFG_LOG_FILE				"log"
#define	PARTY_CFG_LOG_LEVEL				"loglevel"

#define	PARTY_LOG_MAX						8
#define	PARTY_LOG_MIN						1

#define	PARTY_CFG_USER_URL				"UserUrl"
#define	PARTY_CFG_USER_NAME				"UserName"
#define	PARTY_CFG_USER_PASSWD			"UserPasswd"


#define	PARTY_CFG_FILTER_KEYWORD		"forward"

#define	PARTY_CFG_FILTER_DIAL_PLAN		"dialingplan"
#define	PARTY_CFG_FILTER_MODE			"mode"
#define	PARTY_CFG_FILTER_ROUTE_RECORD	"record-route"
#define	PARTY_CFG_FILTER_REGULAR_EXP	"RegulateExp"


#define	PARTY_VALUE_ON					"on"
#define	PARTY_VALUE_OFF					"off"

#define	PARTY_FILTER_TYPE_EXT				0
#define	PARTY_FILTER_TYPE_INT				1

typedef	struct
{
	llist 				cgiVariables;
	
	llist				globalCfgs;		/* EQL items */
	llist				extFilterCfgs;		
	llist				intFilterCfgs;		
	llist				userCfgs;		/* four items */

	char				proxyAuth[64];		/* 407 proxy authen */
	network_info		*netInfo;
	
}PARTY_INFO;


#define	IS_ON_P_G_VALUE(info, key)	\
	(!strcasecmp(GET_VALUE(&info->globalCfgs, key), PARTY_VALUE_ON) ) 


int cgiParty_Globals(PARTY_INFO *info);
int cgiParty_Users(PARTY_INFO *info);
int cgiParty_Filters(PARTY_INFO *info, int filterType );
int cgiParty_Lookup(PARTY_INFO *info);

int	party_save_config(PARTY_INFO *info);

#endif

