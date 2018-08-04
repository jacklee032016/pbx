/*
 * $Author: lizhijie $
 * $Id: as_utils.c,v 1.14 2007/03/25 08:32:47 lizhijie Exp $
*/
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

#include "as_utils.h"
#include "aslib.h"

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
	lstyle		:	USE_SYSLOG,
	llevel		: 	AS_LOG_WARNING,
	lfacility		:	AS_LOG_UNKNOW,
	isDaemonized	:	0,
	logFileName	:	NULL,
	fp			:	NULL
};

void as_daemon_init()
{
	pid_t pid;
	int fd;
	
	if((pid = fork()) != 0)
		exit(0);
	
	setsid();	 //become session leader 
//	signal(SIGHUP,SIG_IGN);
	if((pid = fork()) != 0)
		exit(0); 
	
	chdir("/");
	umask(0);
	
	close(0);
	close(1);
	close(2);
	
	fd=open("/dev/null",O_RDWR);
	dup(0);
	dup(1);
	dup(2);
}

int sys_log_init(log_facility_t facility)
{   
	log_facility_t temp = facility >> 3;
	if (temp >= 16 && temp <= 23)
		logobj.lfacility = facility;
	else
		return -1; // facility not exist

	openlog("assist",  LOG_CONS, logobj.lfacility);

	return 0; //success
}

log_level_t get_current_level()
{
	return logobj.llevel;
}

int as_log_init(log_stru_t *lobj)
{
	int	isStdOut = 0;
	
	if (lobj->llevel < AS_LOG_EMERG)
		;
	else if (lobj->llevel > AS_LOG_DEBUG) 
		logobj.llevel= AS_LOG_DEBUG;
	else 
		logobj.llevel = lobj->llevel;

	sprintf(logobj.name, "%s", lobj->name);
	logobj.lstyle = lobj->lstyle;
	logobj.isDaemonized = lobj->isDaemonized;
	
	if (logobj.lstyle == USE_SYSLOG)
	{
#ifndef   __AS_RELEASE__
		fprintf(stderr, "Use SysLog\n");
#endif
		return sys_log_init(lobj->lfacility);
	}
	else if(logobj.lstyle == USE_FILE)
	{
		logobj.fp = fopen( lobj->logFileName, "w+");
		if(!logobj.fp)
		{
			fprintf(stderr, "Log File '%s' initialized fail. %s\n\n",lobj->logFileName, strerror(errno) );
			logobj.fp = stderr;
			isStdOut = 1;
		}
#ifndef   __AS_RELEASE__
		fprintf(stderr, "Use File as Log\n");
#endif
	}
	else
	{
#ifndef   __AS_RELEASE__
		fprintf(stderr, "Use stderr as Log\n");
#endif
		logobj.fp = stderr;
		isStdOut = 1;
	}
	
	if(!isStdOut && logobj.isDaemonized )
	{
		as_daemon_init();
	}

	{
		struct timeval tv;
		struct tm *ptm;

		char date[32] = {0};
		gettimeofday(&tv, NULL);
		ptm = localtime(&tv.tv_sec);
		strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", ptm);

#ifndef   __AS_RELEASE__
		fprintf(stderr, "Startup at %s\n", date);
#endif
		AS_MSG_LOG(AS_LOG_NOTICE, "%s startup at %s, Log Level is %s",logobj.name, date, priname[logobj.llevel] );
	}

	return 0;
}

#ifndef   __AS_RELEASE__
void log_information(int pri, const char* file, int line, const char* frmt,...)
#else
void log_information(int pri, const char* frmt,...)
#endif
{
	va_list ap;
	va_start(ap, frmt);
	char buf[1024] = {0};
	vsnprintf(buf, 1024, frmt, ap);
	va_end(ap);

#ifndef   __AS_RELEASE__
	struct timeval tv;
	struct tm *ptm;

	char date[32] = {0};
	gettimeofday(&tv, NULL);
	ptm = localtime(&tv.tv_sec);
	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", ptm);
#endif	
	int pri_ = (pri <= AS_LOG_DEBUG) ? pri : AS_LOG_DEBUG;
	
	if (logobj.lstyle == USE_SYSLOG)
	{
#ifndef   __AS_RELEASE__
		syslog(pri_, "%s [%s] : %s:%d | %s\n", date, priname[pri_], file, line,buf);
#else
		syslog(pri_, "[%s] : %s\n", priname[pri_], buf);
#endif
	}
	else
	{
#ifndef   __AS_RELEASE__
		fprintf(stderr, "Write File or STDERR Log :%s \n", buf);
		fprintf(logobj.fp, "%s [%s] : %s:%d | %s\n", date, priname[pri_], file, line,buf);
#else
		fprintf(logobj.fp, "[%s] : %s\n", priname[pri_], buf);
#endif
		fflush(logobj.fp);	/* added this line, lizhijie, 2007.03.16 */
	}
	
}

int safe_open (const char *pathname,int flags)
{
   	int fd;

   	fd = open (pathname,flags);
   	if (fd < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR,"While trying to open %s",pathname);
		if (flags & O_RDWR)
			AS_MSG_LOG(AS_LOG_ERR," for read/write access");
		else if (flags & O_RDONLY)
			AS_MSG_LOG(AS_LOG_ERR," for read access");
		else if (flags & O_WRONLY)
			AS_MSG_LOG(AS_LOG_ERR," for write access");
		
		AS_MSG_LOG(AS_LOG_ERR,": %m\n");
		exit (EXIT_FAILURE);
	}
	return (fd);
}

void safe_rewind (int fd, int offset, const char *filename)
{
   	if (lseek(fd, offset,SEEK_SET) < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "While seeking to start of %s: %m",filename);
		exit (EXIT_FAILURE);
	}
}

void safe_read (int fd,const char *filename,void *buf,size_t count, int verbose)
{
   	ssize_t result;

   	result = read (fd,buf,count);
   	if (count != result)
	{
		if (verbose)
			AS_MSG_LOG(AS_LOG_INFO,"\n");
		
		if (result < 0)
		{
			AS_MSG_LOG(AS_LOG_ERR,"While reading data from %s: %m",filename);
			exit (EXIT_FAILURE);
		}
		
		AS_MSG_LOG(AS_LOG_ERR,"Short read count returned while reading from %s",filename);
		exit (EXIT_FAILURE);
	}
}


