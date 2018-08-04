/*
 * $Log: as_uartd_main.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.4  2005/09/01 10:22:04  tangr
 * fix version
 *
 * Revision 1.3  2005/09/01 08:59:58  tangr
 * deal version
 *
 * Revision 1.2  2005/07/08 01:44:14  lijie
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * as_uartd_main.c : UART daemon for PBX
 * Li Zhijie, 2005.05.26 
 * $Id: as_uartd_main.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
 */
#include <getopt.h>
#include <stdlib.h>

#include "as_version.h"
#include "aslib.h"

int as_uartd_threads_init();
void as_uartd_threads_destory();

 int     DD=0;
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
	printf("-h: help;\r\n-d: for debug;\r\n");
	return 0;
}

int  main(int argc, char *argv[])
{
	int c;
	int option_index = 0;
	
	static struct option long_options[] = 
	{
		{"version", 0, 0, 0},
		{0, 0, 0, 0}
	};

	//version
	as_ver_debug(AS_VERSION_INFO(AS_UARTD_MODULE_NAME));
	as_ver_log(AS_UARTD_MODULE_NAME,AS_VERSION_INFO(AS_UARTD_MODULE_NAME));

	while (1)
	{
		c = getopt_long (argc, argv, "a+bB:cCDeEhm:M:OprRqs:St:uUvdw:XZy",long_options, (int *) 0);
		if (c == -1)
			break;

		switch (c)
		{
			case 'v':
			case 0:
				//version
				as_ver_log(AS_UARTD_MODULE_NAME,AS_VERSION_INFO(AS_UARTD_MODULE_NAME));
				exit(0);
				break;
			case 'h':
				disp_h();
				return 0;
			case 'd':
				DD = 1; 
		}
		option_index++;
	}	

	if(!DD)
		as_daemon_init();
#if 0	
       uart_ctrl.sip_pid=-1;
       uart_ctrl.cgi_pid=-1;

       uart_ctrl.sip_pid=(pid_t)uartgetpid(PROC_PBX);   
       uart_ctrl.cgi_pid=(pid_t)uartgetpid(PROC_CGI); 
	gettimeofday(&tv,NULL);
#endif       
	
	as_uartd_threads_init();
	as_uartd_threads_destory();
	return 0;
} 

