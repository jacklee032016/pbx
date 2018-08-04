/*$Id: fwExpert.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__FW_EXPERT_H__
#define	__FW_EXPERT_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"

char *fwExpert_info(iptables_item *header );
char *fwExpert_buttons(FW_INFO *info );

#endif

