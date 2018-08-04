/*
* $Author: lizhijie $
* $Id: servicesMisc.h,v 1.2 2006/12/11 08:27:37 lizhijie Exp $
*/

#ifndef __SERVICES_MISC_H__
#define __SERVICES_MISC_H__

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
#include "cgi_lib.h"
#include "cgi_llist.h"
#include "sigdef.h"

#include "cgi_services.h"

/* top level menu for misc services */
#define		SERVICES_MISC_OP_CONFIG				"Config"
#define		SERVICES_MISC_OP_DHCP				"DHCP"
#define		SERVICES_MISC_OP_FTP					"FTP"
#define		SERVICES_MISC_OP_PPTP					"PPTP"
#define		SERVICES_MISC_OP_QOS					"QOS"
#define		SERVICES_MISC_OP_UPNP				"UPNP"
#define		SERVICES_MISC_OP_RIP					"RIP"
#define		SERVICES_MISC_OP_SYSLOG				"SYSLOG"
#define		SERVICES_MISC_OP_VPNSERVER			"VPNSERVER"

typedef	struct
{
	llist	cgiVariables;
	
}SERVICE_INFO;

int	servicesMisc_Dhcp(SERVICE_INFO *info);
int	servicesMisc_Ftp(SERVICE_INFO *info);
int	servicesMisc_Upnp(SERVICE_INFO *info);
int	servicesMisc_Rip(SERVICE_INFO *info);
int	servicesMisc_Config(SERVICE_INFO *info);
int	servicesMisc_Syslogd(SERVICE_INFO *info);
int	servicesMisc_VpnServer(SERVICE_INFO *info);

#endif

