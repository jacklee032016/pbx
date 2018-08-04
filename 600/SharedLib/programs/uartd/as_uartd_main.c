/*
 * $Id: as_uartd_main.c,v 1.6 2007/03/25 08:32:47 lizhijie Exp $
 */
#include <getopt.h>
#include <stdlib.h>

#include "as_version.h"
#include "aslib.h"

int as_uartd_threads_init();
void as_uartd_threads_destory();

#if 0
static struct	timeval tv;
#define AST_MUTEX_KIND		 PTHREAD_MUTEX_RECURSIVE_NP

int ast_mutex_init(pthread_mutex_t *pmutex)
{	
#if 0
       pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
#endif	
	//pthread_mutexattr_settype(&attr, AST_MUTEX_KIND);
	return pthread_mutex_init(pmutex, NULL);
}

void sigsend(pid_t pid, int signo,int opcode)
{	
	union sigval sval;	
	sval.sival_int=opcode;	
	sigqueue(pid, signo, sval);
}
#endif

int disp_h()
{
	printf("\t-h : help;\r\n\t-s : syslogd;\r\n\t-d level : debug level\r\n\t\t5 : Dialed Digits\r\n\t\t6 : MSG/CMD\r\n");
	exit(1);
}

#define	UARTD_NAME	"UARTD"

log_stru_t uartdLog = 
{
	name		:	UARTD_NAME,
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_NOTICE,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_UARTD,
	isDaemonized	:	1
};

int  main(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	int	isLogSys = 0;
	int	level = AS_LOG_NOTICE;
#if 0	
	static struct option long_options[] = 
	{
		{"version", 0, 0, 0},
		{0, 0, 0, 0}
	};
#endif
	as_ver_debug(AS_VERSION_INFO(AS_UARTD_MODULE_NAME));
	as_ver_log(AS_UARTD_MODULE_NAME,AS_VERSION_INFO(AS_UARTD_MODULE_NAME));

	while (1)
	{
//		c = getopt_long (argc, argv, "a+bB:cCDeEhm:M:OprRqs:St:uUvdw:XZy",long_options, (int *) 0);
		c = getopt (argc, argv, "hd:vs");
		if (c == -1)
			break;

		switch (c)
		{
			case 'v':
				as_ver_log(AS_UARTD_MODULE_NAME,AS_VERSION_INFO(AS_UARTD_MODULE_NAME));
				exit(0);
				break;
			case 'h':
				disp_h();
				break;
			case 'd':
				level = atoi(optarg);
				break;
			case 's':
				isLogSys = 1;
				break;
			default:
				disp_h();
				break;
		}
		option_index++;
	}	

	if(level <= AS_LOG_DEBUG && level>= AS_LOG_EMERG)
		uartdLog.llevel = level;

	if(isLogSys)
		uartdLog.lstyle = USE_SYSLOG;
	
	if (as_log_init( &uartdLog) < 0)
	{
		printf("UARTD Log Init Failed.\n");
		return 0;
	}
	

	as_uartd_threads_init();
	as_uartd_threads_destory();
	return 0;
} 

