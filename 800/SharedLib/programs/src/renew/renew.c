#include <stdio.h>
#include <unistd.h>

//#include "cgi_lib.h"
#include "aslib.h"

#define	RESET_COUNT		4
#define	TIME_SLEEP			1

#define	INIT_STATE	    	-1
#define   RESET_STATE 	1

#define RENEW_NAME  "renew"

static void do_renew(void * arg2);
static void  main_proccess(void * arg1, void( * function) (void *) ,void * arg2);

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


int main()
{

	printf("proccess of renew configure Button turn on\n");

	as_daemon_init( );

	main_proccess(NULL,do_renew,NULL);

	return 0;
}

