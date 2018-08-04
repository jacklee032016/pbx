/*$Id: pbx_main.c,v 1.17 2007/06/21 20:30:02 lizhijie Exp $*/

#include <pthread.h>
#include <errno.h>
#include "as_version.h"
#include "as_common_version.h"
#include "_pbx.h"

llist		pbxentries;
llist		sipblck_list;

#define	CGIPBX_NAME		"CGIPBX"
log_stru_t pbxCgiLog = 
{
	name		:	CGIPBX_NAME,
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_NOTICE,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_CGIPBX,
	isDaemonized	:	1
};

int pbx_usage()
{
	printf("\t-h : help;\r\n\t-s : syslogd;\r\n\t-d level : debug level\r\n\t\t5 : Dialed Digits\r\n\t\t6 : MSG/CMD\r\n");
	exit(1);
}


int main(int argc, char *argv[])
{
	pthread_t id;
	int ret;
	int c;
	int	isLogSys = 0;
	int	level = AS_LOG_NOTICE;

	as_ver_debug(AS_VERSION_INFO(AS_MODULENAME));	
	as_ver_log(AS_MODULENAME,AS_VERSION_INFO(AS_MODULENAME));

	while (1)
	{
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
				pbx_usage();
				break;
			case 'd':
				level = atoi(optarg);
				break;
			case 's':
				isLogSys = 1;
				break;
			default:
				pbx_usage();
				break;
		}
	}	

	if(level <= AS_LOG_DEBUG && level>= AS_LOG_EMERG)
		pbxCgiLog.llevel = level;

	if(isLogSys)
		pbxCgiLog.lstyle = USE_SYSLOG;

	if(as_log_init( &pbxCgiLog) < 0)
	{
		printf("%s Log File %s Init Failed.\n", pbxCgiLog.name, pbxCgiLog.logFileName);
	}
	as_msg_queue_init(0);
	
	list_create(&pbxentries);
	list_create(&sipblck_list);
	readconfig(FILE_PBX_CONF,ITEM_DBL, NULL, &pbxentries);

	/* set all configuration into PBX*/
	pbx_setup_pbx_when_startup(&pbxentries);

	ret=pthread_create(&id,NULL,(void *)read_serial_cmd_thread,NULL);
 	if(ret!=0)
 	{
		AS_MSG_LOG(AS_LOG_EMERG, "CGIPBX create uart read thread failed :%s",strerror(errno) );
		return -1;
 	}

//	pbx_write_com(&pbxentries);

	/* register Signal Handler */
	cgi_signal_register_handler(SIG_PBX_CGI,pbx_handler_cgisig );

	pbx_send_comcmd(CMD_PC_GET_CONFIG, NULL, NULL);     //get system parameter
#if 0	
	pbx_send_comcmd(0x5a,NULL,NULL);     //get date from PBX
#endif
	cgidebug("proc: %s(%d) waiting...\n", PROC_PBX, getpid() );
	while(1)
	{
		sleep(6);
	}

//	as_msg_queue_destroy();
	return 1;
}

