/*
* $Id: cgi_vpn.h,v 1.1 2007/08/31 20:31:19 lizhijie Exp $
*/
/* pages for VPN */

#ifndef	__CGI_VPN_H__
#define	__CGI_VPN_H__

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include	<sys/sem.h>
#include "cgiLib.h"
#include "cgi_llist.h"
#include "sigdef.h"

#include "cgi_services.h"

#define	VPN_ROUTE_CONFIG_FILE				"/etc/ppp/vpn_route.conf"
#define	VPN_RUNTIME_INFO						"/tmp/log/vpnInfo"

#define	VPN_KEYWORK_SERVER_IP				"VPN_SERVER_IP"	/* IP Address or FQDN */

#define	CGI_VPN_OP_SERVER_STATUS				"VpnServerStatus"
#define	CGI_VPN_OP_SERVER_CONFIG				"VpnServerConfig"
#define	CGI_VPN_OP_CLIENT_STATUS				"VpnClientStatus"
#define	CGI_VPN_OP_CLIENT_CONFIG				"VpnClientConfig"
#define	CGI_VPN_OP_ACCOUNT_SERVER			"VpnAccountServer"
#define	CGI_VPN_OP_ACCOUNT_AS600			"VpnAccountAS600"
#define	CGI_VPN_OP_ACCOUNT_OTHERS			"VpnAccountOther"

typedef	enum
{
	VPN_ACCOUNT_TYPE_SERVER,
	VPN_ACCOUNT_TYPE_AS600,
	VPN_ACCOUNT_TYPE_OTHER,
}VPN_ACCOUNT_T;


#define	VPN_SERVER_DEFAULT_IP_PARAM			"assistVpnServer"
#define	VPN_SERVER_DEFAULT_NAME				"vpn"


#endif

