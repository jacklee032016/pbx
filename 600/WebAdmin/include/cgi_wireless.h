/*
* $Id: cgi_wireless.h,v 1.5 2007/02/07 19:04:16 lizhijie Exp $
*/
#ifndef	__CGI_WIRELESS_H__
#define	__CGI_WIRELESS_H__

#include "libwifi.h"
#if WITH_MESH
#include "libmesh.h"
#endif

#include "g_list.h"

typedef	enum
{
	CGI_WIRELESS_MESH		= 0,
	CGI_WIRELESS_WIFI		= 1	
}wireless_type_t;

typedef enum
{
	WIFI_AUTH_NONE	= 0,
	WIFI_AUTH_OPEN	= 1,		/* open */
	WIFI_AUTH_SHARED	= 2,		/* shared-key */
	WIFI_AUTH_8021X	= 3,		/* 802.1x */
	WIFI_AUTH_AUTO	= 4,		/* auto-select/accept */
	WIFI_AUTH_WPA	= 5,		/* WPA/RSN w/ 802.1x/PSK */
}wifi_auth_type_t;

typedef enum
{
	WIFI_MAC_MASTER	=	0,
	WIFI_MAC_ADHOC,
	WIFI_MAC_STATTION,
}wifi_mac_t;

typedef enum
{
	WIFI_MODE_AUTO	= 0,	/* autoselect */
	WIFI_MODE_11A		= 1,	/* 5GHz, OFDM */
	WIFI_MODE_11B		= 2,	/* 2GHz, CCK */
	WIFI_MODE_11G		= 3,	/* 2GHz, OFDM */
	WIFI_MODE_FH		= 4,	/* 2GHz, GFSK */
	WIFI_MODE_TURBO	= 5,	/* 5GHz, OFDM, 2x clock */
}wifi_phy_t;

/* RATE for 802.11B/G */
#define	WIFI_RATE_AUTO	0
#define	WIFI_RATE_1M		1
#define	WIFI_RATE_2M		2
#define	WIFI_RATE_5_5M	5.5	
#define	WIFI_RATE_11M		11	
#define	WIFI_RATE_6M		6	
#define	WIFI_RATE_9M		9	
#define	WIFI_RATE_12M		12	
#define	WIFI_RATE_18M		18	
#define	WIFI_RATE_24M		24	
#define	WIFI_RATE_36M		36	
#define	WIFI_RATE_48M		48	
#define	WIFI_RATE_54M		54	

#define	WIRELESS_NAME_MESH		"MESH"
#define	WIRELESS_NAME_WIFI		"Wifi"


typedef	enum
{
	CGI_WIFI_ACL_OPEN		=	0,
	CGI_WIFI_ACL_ALLOW,
	CGI_WIFI_ACL_DENY,
	CGI_WIFI_ACL_FLUSH,
	CGI_WIFI_ACL_DEATTACH
}CGI_WIFI_ACL_CMD;

typedef	enum
{
	CGI_WIFI_AUTHEN_OPEN		=	1,
	CGI_WIFI_AUTHEN_SHARED_KEY,
	CGI_WIFI_AUTHEN_802DOT1X,
	CGI_WIFI_AUTHEN_AUTO,
	CGI_WIFI_AUTHEN_WPA
}CGI_WIFI_AUTHEN;


typedef struct
{
	int		policy;
	llist		macCfgs;
	
	llist		macRunning;
	int		countMacs;
}wifi_acl_info_t;

typedef struct
{
	llist		authenCfgs;
}wifi_authen_info_t;

typedef	struct
{
	char		macAddress[64];
	
	int		txPackets;
	int		rxPackets;

	int		failedPackets;
}wifi_station_info_t;

typedef	struct
{
	wireless_type_t	type;
	char				name[64];
	
	int				isPlugged;
	
	int				socketFd;

	wireless_info		wifiInfo;
#if WITH_MESH
	mesh_info_t		meshInfo[4];
#endif

	llist 				cgiVariables;
	
	llist				globalCfgs;
	llist				nicCfgs;

	network_info		*wanInfo;

#if WITH_MESH
	llist				meshDevs;
#endif

	wifi_acl_info_t		aclInfo;

	wifi_authen_info_t	authenInfo;

	glist				*stations;

}WIFI_INFO;

struct mac_t
{
	char		*name;//[32];
	char		*description;//[64];
};

struct phy_mode
{
	wifi_phy_t	type;
	char			*description;//[64];
};

typedef struct
{
//	int		type;
	char		*type;//[64];
	char		*description;//[64];
}rate_type_t;


/* global config*/
#define	WIFI_KEYWORD_MESH_NUM					"MESH_NUM"
#define	WIFI_KEYWORD_ONBOOT						"WIFI_ONBOOT"

/* config for every NIC */
#define	WIFI_KEYWORD_MODE						"WIFI_MODE"
#define	WIFI_KEYWORD_ESSID						"WIFI_ESSID"
#define	WIFI_KEYWORD_NICKNAME					"WIFI_NAME"
#define	WIFI_KEYWORD_PHY_MODE					"WIFI_PHY_MODE"
#define	WIFI_KEYWORD_KEY							"WIFI_KEY"

#define	WIFI_KEYWORD_RATE						"WIFI_RATE"
#define	WIFI_KEYWORD_AODV						"WIFI_AODV"


#define	WIFI_KEYWORD_CHANNEL					"WIFI_CHANNEL"

#define	WIFI_VALUE_ON								CGI_STR_YES_CASE
#define	WIFI_VALUE_OFF							CGI_STR_NO_CASE

#define	WIFI_KEYWORD_ACL_POLICY					"WIFI_ACL_POLICY"
#define	WIFI_KEYWORD_ACL_MAC					"WIFI_ACL_MAC"
#define	WIFI_KEYWORD_ACL_MAC_NUM				"WIFI_ACL_MAC_NUM"

#define	WIFI_KEYWORD_AUTHEN_MODE				"WIFI_AUTHEN_MODE"
#define	WIFI_KEYWORD_AUTHEN_KEY_POLICY			"WIFI_AUTHEN_KEY_POLICY"
#define	WIFI_KEYWORD_AUTHEN_KEY					"WIFI_AUTHEN_KEY"
#define	WIFI_KEYWORD_AUTHEN_KEY_NUM			"WIFI_AUTHEN_KEY_NUM"


#define	CGI_WIRELESS_OP_MODE						"WifiMode"		/* select MAC mode: AP/AdHoc/Station */

#if WITH_MESH
#define	MESH_PROC_DEVICES							"/proc/mesh/dev"
#endif

WIFI_INFO *cgi_wireless_init(wireless_type_t type);
int 	cgi_wireless_save_config(WIFI_INFO *info);
int	cgi_wireless_mode(WIFI_INFO *info);
int cgi_mesh_save_config(WIFI_INFO *info);
int	cgi_mesh_mode(WIFI_INFO *info);

extern	struct mac_t _wifimacs[];
extern	struct phy_mode _wifiphys[];
extern	rate_type_t	rates_11b[] ;
extern	rate_type_t	rates_11g[] ;

#endif

