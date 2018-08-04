/*
* $Id: renew.c,v 1.7 2007/03/25 08:32:47 lizhijie Exp $
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "aslib.h"
#include "as_version.h"

#define	RESET_COUNT		4
#define	TIME_SLEEP			1

#define	INIT_STATE	    		-1
#define   RESET_STATE 		1

static void do_renew(void * arg2 )
{
	static int count=0;
	int ret=-1;
	static int flag_w = INIT_STATE;
	
	ret=as_get_button_states( );
	if( 0 == ret )
	{
		if(flag_w == INIT_STATE)
		{
			count ++ ;				
			
			if(count >= RESET_COUNT)
			{
				system("touch /etc/renew.flg");
				as_beep_long(3);
				flag_w = RESET_STATE;
				system("reboot");
			}
		}
	}
	else if( 1 == ret )
	{
		count=0;
		flag_w = INIT_STATE;
	}
	else
	{
		count=0;
		flag_w = INIT_STATE;
	}

}

static void  main_proccess(void * arg1, void( * function) (void *) ,void * arg2)
{
	while(1)
	{
		if( NULL != function )
		{
			(*function) (arg2);
			sleep(TIME_SLEEP);
		}
	}
}

#define	RENEW_NAME	"RENEW"
log_stru_t renewLog = 
{
	name		:	RENEW_NAME,
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_NOTICE,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_RENEW,
	isDaemonized	:	1
};


int main()
{
#if 0
	printf("proccess of renew configure Button turn on\n");
#endif
	as_ver_debug(AS_VERSION_INFO(AS_RENEW_MODULE_NAME));	
	as_ver_log(AS_RENEW_MODULE_NAME,AS_VERSION_INFO(AS_RENEW_MODULE_NAME));

	if (as_log_init( &renewLog) < 0)
	{
		printf("RENEW Log Init Failed.\n");
		return 0;
	}
//	as_daemon_init( );

	main_proccess(NULL,do_renew,NULL);

	return 0;
}

