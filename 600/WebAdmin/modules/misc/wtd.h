/*$Id: wtd.h,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $*/

#ifndef _WTD_H_
#define	_WTD_H_

#include <sys/wait.h>
#include "cgi_lib.h"
#include "as_version.h"

#define	CONF_PROC	"/etc/web/proclist.conf"

static void wtd_visit(entrytype *item,void *);
static void wtd_proc_check();
static void wtd_wait();
static void wtd_sigchild_handler(int sig,siginfo_t *info,void *obsolete);

#endif
