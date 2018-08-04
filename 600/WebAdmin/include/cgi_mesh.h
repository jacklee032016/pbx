/*
* $Id: cgi_mesh.h,v 1.2 2007/02/07 19:04:16 lizhijie Exp $
*/
/*
* pages for MESH 
*/

#ifndef	__CGI_MESH_H__
#define	__CGI_MESH_H__

#include "cgi_wireless.h"


#define	MESH_PROC_INFO_DIR					"/proc/mesh/"
#define	MESH_TBL_PORTAL						MESH_PROC_INFO_DIR"portal"
#define	MESH_TBL_ROUTES						MESH_PROC_INFO_DIR"routes"
#define	MESH_TBL_TIMERS						MESH_PROC_INFO_DIR"timers"
#define	MESH_TBL_FWD							MESH_PROC_INFO_DIR"mac_fwd"
#define	MESH_TBL_MGR							MESH_PROC_INFO_DIR"meshMgr"

#define	MESH_AODV_PROC_CTRL_DIR					"/proc/sys/net/mesh/"
#define	MESH_AODV_CTRL_CLEAR						WIFI_AODV_PROC_CTRL_DIR"clear"
#define	MESH_AODV_CTRL_DEBUG						WIFI_AODV_PROC_CTRL_DIR"debug"


#define	CGI_MESH_OP_MODE							"MeshMode"		/* select MAC mode: AP/AdHoc/Station */
#define	CGI_MESH_OP_PORTAL						"MeshPortal"
#define	CGI_MESH_OP_SCAN							"WifiScan"
#define	CGI_MESH_OP_STATUS						"WifiStatus"
#define	CGI_MESH_OP_NIC							"WifiNIC"	
#define	CGI_MESH_OP_NIC_DHCP						"WifiNIC_DHCP"	
#define	CGI_MESH_OP_ACL							"WifiACL"					/* MAC Address filter */

#define	CGI_MESH_OP_CTRL_STOP					"MeshCtrlStop"			/* Stop MESH subsystem */
#define	CGI_MESH_OP_CTRL_FWD						"MeshCtrlFwd"				/* Add/Delete fwd item */

#define	CGI_MESH_OP_STAT_NEIGH					"MeshStatNeigh"			/* AODV */
#define	CGI_MESH_OP_STAT_ROUTE					"MeshStatRoute"			/* AODV */
#define	CGI_MESH_OP_STAT_STATISTICS				"MeshStatStatistics"		/* Statistics */
#define	CGI_MESH_OP_STAT_FWD					"MeshStatFwd"			/* forward table */
#define	CGI_MESH_OP_STAT_DEVICE					"MeshStatDevice"			/* Device(include Portal) Info */

#define	MESH_KEYWORD_ONBOOT						"WIFI_ONBOOT"
#define	MESH_KEYWORD_MODE						"WIFI_MODE"
#define	MESH_KEYWORD_ESSID						"WIFI_ESSID"
#define	MESH_KEYWORD_NICKNAME					"WIFI_NAME"
#define	MESH_KEYWORD_PHY_MODE					"WIFI_PHY_MODE"
#define	MESH_KEYWORD_KEY							"WIFI_KEY"

#define	MESH_KEYWORD_RATE						"WIFI_RATE"
#define	MESH_KEYWORD_AODV						"WIFI_AODV"


#define	MESH_KEYWORD_CHANNEL					"WIFI_CHANNEL"
#define	MESH_KEYWORD_								""

#define	MESH_KEYWORD_PORTAL						"MESH_PORTAL"
#define	MESH_KEYWORD_PORTAL_DEV					"MESH_PORTAL_DEVICE"
#define	MESH_KEYWORD_PORTAL_MESH				"MESH_PORTAL_MESH"

#define	MESH_VALUE_ON								CGI_STR_YES_CASE
#define	MESH_VALUE_OFF							CGI_STR_NO_CASE


#define	MESH_KEYWORD_FWD_MAC					"FwdMac"

#define	MESH_BIN_MGR_CONFIG						"/sbin/mgrconfig"
#define	MESH_BIN_MESH_CONFIG						"/sbin/meshconfig"

int	mesh_portal_ctrl(WIFI_INFO *info);
int	mesh_status_infos(WIFI_INFO *info);

#endif

