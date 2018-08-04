/*
* $Id: cgi_wifi.h,v 1.4 2007/01/22 21:00:55 lizhijie Exp $
*/
/*
* pages for Wifi 
*/

#ifndef	__CGI_WIFI_H__
#define	__CGI_WIFI_H__

#include "cgi_wireless.h"

#define	WIFI_AODV_PROC_INFO_DIR					"/proc/mesh/"
#define	WIFI_AODV_TBL_NEIGHBORS					WIFI_AODV_PROC_INFO_DIR"neighs"
#define	WIFI_AODV_TBL_ROUTES						WIFI_AODV_PROC_INFO_DIR"routes"
#define	WIFI_AODV_TBL_TIMERS						WIFI_AODV_PROC_INFO_DIR"timers"
#define	WIFI_AODV_TBL_FLOODS						WIFI_AODV_PROC_INFO_DIR"floods"

#define	WIFI_AODV_PROC_CTRL_DIR					"/proc/sys/net/mesh/"
#define	WIFI_AODV_CTRL_CLEAR						WIFI_AODV_PROC_CTRL_DIR"clear"
#define	WIFI_AODV_CTRL_DEBUG						WIFI_AODV_PROC_CTRL_DIR"debug"


#define	CGI_WIFI_OP_PARAMETER					"WifiParam"
#define	CGI_WIFI_OP_SCAN							"WifiScan"
#define	CGI_WIFI_OP_STATUS						"WifiStatus"
#define	CGI_WIFI_OP_STATION						"WifiStation"
#define	CGI_WIFI_OP_NIC							"WifiNIC"	
#define	CGI_WIFI_OP_NIC_DHCP						"WifiNIC_DHCP"	
#define	CGI_WIFI_OP_ACL							"WifiACL"					/* MAC Address filter */
#define	CGI_WIFI_OP_AUTHEN						"WifiAuthen"					/* MAC Address filter */
#define	CGI_WIFI_OP_CRYPT							"WifiCrypt"					/* MAC Address filter */

#define	CGI_WIFI_OP_AODV_CTRL					"WifiAodvCtrl"				/* AODV */
#define	CGI_WIFI_OP_AODV_NEIGH					"WifiAodvNeigh"			/* AODV */
#define	CGI_WIFI_OP_AODV_ROUTE					"WifiAodvRoute"			/* AODV */
#define	CGI_WIFI_OP_AODV_TIMER					"WifiAodvTimer"			/* AODV */
#define	CGI_WIFI_OP_AODV_FLOOD					"WifiAodvFlood"			/* AODV */


int cgi_wifi_save_config(WIFI_INFO *info);

int	wifi_Mode(WIFI_INFO *info);
int 	wifi_Network(WIFI_INFO *info);
int	wifi_Info(WIFI_INFO *info);

int	wifi_Acl(WIFI_INFO *info);
int	wifi_Authen(WIFI_INFO *info);
int	wifi_Stations(WIFI_INFO *info);

int	wifi_Aodv_Info(WIFI_INFO *info);
int	wifi_Aodv_Ctrl(WIFI_INFO *info);

#endif

