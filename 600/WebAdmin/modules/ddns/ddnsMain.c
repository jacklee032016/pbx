/*
* $Id: ddnsMain.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "_ddns.h"
#include "as_version.h"

ddns_task_t	myDdns;
/*= 
{
	serverIp		:	DDNS_SERVER_IP,
	tcpShakePort	:	DDNS_SERVER_PORT,
	
	udpMyPort	:	DDNS_SERVER_PORT,
	udpDestPort	:	DDNS_SERVER_PORT,

	userName	:	DDNS_MY_NAME,
	passwd		:	DDNS_MY_PASSWD
};
*/

log_stru_t  ddnsDaemonLog = 
{
	name		:	DDNS_NAME,
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_DEBUG,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_PEANUT,
	isDaemonized	:	1
};


// process terminate & interrupt signal handler
static void my_handleSIG (int sig)
{
	if (sig == SIGINT)
	{
		if( (time(0) - myDdns.lastInterruptTime) >= 1)
		{
			myDdns.interruptCount = 0;
		}
		
		myDdns.lastInterruptTime = time(0);
		AS_MSG_LOG(AS_LOG_WARNING, "signal = SIGINT %d\n",myDdns.interruptCount);
		myDdns.interruptCount++;
		if (myDdns.interruptCount > 5)
		{
			exit(0);
		}
	}
	
	if (sig == SIGTERM)
		printf ("signal = SIGTERM\n");
	signal (sig, my_handleSIG);

	myDdns.quitFlags = 1;
}

/*
 * Create a file containing our process ID.
 */
static void createDdnsPidFile(void)
{
	FILE *pidfile;
	char pidfilename[128];
	sprintf(pidfilename, "%s", DDNS_PID_FILE);

	if ((pidfile = fopen(pidfilename, "w")) != NULL)
	{
		fprintf(pidfile, "%d\n", getpid());
		fclose(pidfile);
	}
	else
	{
		AS_MSG_LOG(AS_LOG_WARNING, " Failed to create pid file %s: %m", pidfilename);
		pidfilename[0] = 0;
	}
}

int	__initDdns(ddns_task_t *task)
{
	void (*ohandler) (int);

retry:
	task->serverHost = gethostbyname(task->serverIp);
	if ( task->serverHost == NULL )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Get \"%s\" hostbyname failed!", task->serverIp);
		sleep(3);
		goto retry;
	}

	task->shakeHandSocket = -1;
	task->udpSocket = -1;
	task->sessionCode = 0;
	task->sequence = 0;

	task->challengeLength = 0;
	memset(task->plainChallenge, 0 , DDNS_DEFAULT_BUFFER_LENGTH);

	task->lastInterruptTime = time(0);
	task->interruptCount = 0;
	task->quitFlags = 0;

	Blowfish_Create(&task->blowfish, NULL, 0);
	
	ohandler = signal (SIGINT, my_handleSIG);
	if (ohandler != SIG_DFL)
	{
		AS_MSG_LOG(AS_LOG_WARNING, "previous signal handler for SIGINT is not a default handler\n");
		signal (SIGINT, ohandler);
	}

	/*ohandler = signal (SIGTERM, my_handleSIG);
	if (ohandler != SIG_DFL) {
		printf ("previous signal handler for SIGTERM is not a default handler\n");
		signal (SIGTERM, ohandler);
	}*/
	
	createDdnsPidFile();

	return 0;
}

int main(int argc, char *argv[])
{
#if 0
	REQUEST requestoutput;
	ACCEPT input;
	AFFIRM affirmoutput;
	EXIT exitoutput;
	CONNECTINFO condata;
	STREAMHEAD sysinput;
#endif
	ddns_task_t	*task = &myDdns;

	memset(task, 0, sizeof(ddns_task_t) );
	if(PeanutInitConfig(&task->info) )
	{
		sprintf(task->serverIp, "%s", DDNS_SERVER_IP);
		task->tcpShakePort = DDNS_SERVER_PORT;
		task->udpMyPort = DDNS_SERVER_PORT;
		task->udpDestPort  = DDNS_SERVER_PORT;
		sprintf(task->userName, "%s", DDNS_MY_NAME);
		sprintf(task->passwd, "%s", DDNS_MY_PASSWD);
	}
	else
	{
		sprintf(task->serverIp, "%s", task->info.serverIp);
		task->tcpShakePort = task->info.port;
		task->udpDestPort  = task->info.port;
		task->udpMyPort = task->info.localUdpPort;
		sprintf(task->userName, "%s", task->info.username);
		sprintf(task->passwd, "%s", task->info.password);

		ddnsDaemonLog.logFileName = GET_VALUE(&task->info.configs, PEANUT_KEYWORD_LOG_FILE);
		ddnsDaemonLog.llevel = task->info.logLevel;
	}

	as_ver_debug(AS_VERSION_INFO(AS_MODULENAME));
	as_ver_log(AS_MODULENAME,AS_VERSION_INFO(AS_MODULENAME));

	if(as_log_init( &ddnsDaemonLog) < 0)
	{
		printf("%s Log File %s Init Failed.\n", ddnsDaemonLog.name, ddnsDaemonLog.logFileName);
	}

	AS_MSG_LOG(AS_LOG_NOTICE, "DDNS Runtime Info:");
	AS_MSG_LOG(AS_LOG_NOTICE, "\tServer \t\t: %s", task->serverIp );
	AS_MSG_LOG(AS_LOG_NOTICE, "\tShakehand Port \t: %d", task->tcpShakePort );
	AS_MSG_LOG(AS_LOG_NOTICE, "\tHeartbeat Port \t: %d", task->udpDestPort );
	AS_MSG_LOG(AS_LOG_NOTICE, "\tLocal UDP Port \t: %d", task->udpMyPort );
	AS_MSG_LOG(AS_LOG_NOTICE, "\tUser Name \t: %s", task->userName);
	AS_MSG_LOG(AS_LOG_NOTICE, "\tPassword \t: %s", task->passwd);
	AS_MSG_LOG(AS_LOG_NOTICE, "\tFQDN \t\t: %s", task->info.fqdn);

	__initDdns(task);
	
	if(ddnsShakeHand( task))
		return -1;
	
	ddnsHeartBeatLoop(task);

	return 0;	
}


