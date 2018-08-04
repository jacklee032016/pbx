/*
* $Id: quickWizard.h,v 1.3 2006/12/11 08:27:37 lizhijie Exp $
*/

#ifndef __QUICK_WIZARD_H__
#define __QUICK_WIZARD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include "cgi_lib.h"

#include "cgi_syscfg.h"

#define	ADSL_CONNECT		"/usr/sbin/adsl-connect"
#define	CONF_HOST_NAME	"/etc/hosts"

#define	CONF_DNS			"/etc/resolv.conf"

#define	END_ADDR			"255.255.255.254"
#define	LOCAL_HOST			"localhost"
#define	LOCAL_ADDR			"127.0.0.1"
#define	PPOE_NAME			"adsl-connect"
#define	DHCP_NAME			"udhcpd"
#define	IXP_422				"IXP422"

typedef	struct
{
	llist 				cgiVariables;
	
	llist				wanCfgs;
	llist				pppoeCfgs;
	llist				pppSecretCfgs;
	
	llist				lanCfgs;

	llist				defaultCfgs;			/* content of /etc/sys/data.conf */
#if 0
	interface_info		*wanInfo;
	interface_info		*lanInfo;
#endif	
}QUICK_INFO;


int quickWizardWan(QUICK_INFO *info);
int quickWizardLan(QUICK_INFO *info);

char	*quickWizardAddFinishButton();

int quickWizardWanProtocol(QUICK_INFO *info);

#endif

