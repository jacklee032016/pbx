/*$Id: fwTimer.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef	__FW_TIMER_H__
#define	__FW_TIMER_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "cgi_lib.h"

#include "cgi_iptables.h"

typedef struct
{
	char		mimute[32];
	char		hour[32];
	char		dayBegin[32];
	char		dayEnd[32];
	char		monthBegin[32];
	char		monthEnd[32];
	char		weekBegin[32];
	char		weekEnd[7];
}ipt_timer_t;

char *fwTimer_info(llist *timers );
char *fwTimer_buttons(FW_INFO *info );

#endif

