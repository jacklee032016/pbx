/*
 * $Author: lizhijie $
 * $Log: as_utils.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.3  2005/08/02 01:34:21  wuaiwu
 * added interfaces and implement of syslog
 *
 * Revision 1.2  2005/07/08 01:38:41  lijie
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_utils.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/
#include "as_utils.h"
#include <stdio.h> 
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>

#define MAXFD				64
static const char* priname[] =
{
      "EMRG",
      "ALRT",
      "CRIT",
      "ERR ",
      "WARN",
      "NOTC",
      "INFO",
      "DBUG",
      0
};

static log_stru_t logobj = 
{
	lstyle	:	USE_SYSLOG,
	llevel	: 	LOG_EMERG,
	lfacility	:	AS_LOG_UNKNOW
};

void as_daemon_init()
{
	pid_t pid;
	int i,fd;
	
	if((pid = fork()) != 0)
		exit(0);
	
	setsid();	 //become session leader 
	signal(SIGHUP,SIG_IGN);
	if((pid = fork()) != 0)
		exit(0); 
	
	chdir("/");
	umask(0);
	for( i = 0; i < MAXFD; i++)
		close(i);

//        printf("close file desc!\r\n");
	fd=open("/dev/null",O_RDWR);
	dup(0);
	dup(1);
	dup(2);
}


int as_log_init(log_stru_t lobj)
{
	if (lobj.llevel < LOG_EMERG);
	else if (lobj.llevel > LOG_DEBUG) logobj.llevel= LOG_DEBUG;
	else logobj.llevel = lobj.llevel;

	logobj.lstyle = lobj.lstyle;
	if (logobj.lstyle == USE_SYSLOG)
	{
		return sys_log_init(lobj.lfacility);
	}
	else return 0;
}

void log_information(int pri, const char* file, int line, const char* frmt,...)
{
	va_list ap;
	va_start(ap, frmt);
	char buf[1024] = {0};
	vsnprintf(buf, 1024, frmt, ap);
	va_end(ap);

	struct timeval tv;
	struct tm *ptm;

	char date[32] = {0};
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", ptm);
	int pri_ = (pri <= LOG_DEBUG) ? pri : LOG_DEBUG;
	
	if (logobj.lstyle == USE_SYSLOG)
	{
		syslog(pri_, "%s [%s] : %s:%d | %s\n", date, priname[pri_], file, line,buf);
	}
	else
	{
		printf("%s [%s] : %s:%d | %s\n", date, priname[pri_], file, line,buf);
	}
	
	
}

int sys_log_init(log_facility_t facility)
{   
	log_facility_t temp = facility >> 3;
	if (temp >= 16 && temp <= 23)
		logobj.lfacility = facility;
	else return -1; // facility not exist

	openlog("assist",  LOG_CONS, logobj.lfacility);

	return 0; //success
}

log_level_t get_current_level()
{
	return logobj.llevel;
}
