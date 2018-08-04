/*$Id: fwMiddle.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__FW_MIDDLE_H__
#define	__FW_MIDDLE_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"

#define	IPT_MIDDLE_SERVICES			"Services"

char *fwMiddle_info_buttons(iptables_item *header, llist *serviceCfg);
char *fwMiddle_services_buttons(FW_INFO *info );
char *fwMiddle_address_buttons(FW_INFO *info, llist *middleCfgs );

int fwMiddle_save_rule(iptables_item *cfgRules, FW_INFO *info);

#endif

