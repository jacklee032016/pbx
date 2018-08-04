/*$Id: fwNat.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__FW_NAT_H__
#define	__FW_NAT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"

#define			NAT_CONF				"/etc/rc.d/rc.local"
#define			NAT_TMP				"/etc/tmp/nat.txt"
#define			FILTER_TMP				"/etc/tmp/filter.txt"
#define			IPTABLE_TMP			"/etc/tmp/iptable.txt"


char *fwNat_info_buttons(iptables_item *header, NAT_enum_t natType);
char *fwNat_snat_buttons(FW_INFO *fwInfo, NAT_enum_t natType);

int fwNat_save_rule(iptables_item *cfgRules, FW_INFO *info);

#if IPTABLES_DEBUG
void fwNat_debug_item(iptables_item *item);
#endif

#endif

