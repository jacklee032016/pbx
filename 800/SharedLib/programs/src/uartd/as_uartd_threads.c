#include "aslib.h"
#include <stdlib.h>


//this thread recive command from msg queue and send it to the uart
static void  *ixp2pbx_thread(void *data)
{    	
	PBX_COMMAND *cmd;
       
	while(1)
	{       
		//trace;
		cmd = as_cmd_uart_from_sip( );
		if(cmd)
		{
			as_cmd_to_uart(cmd);
			free(cmd);
		}
		
		cmd = as_cmd_uart_from_cgi( );
		if(cmd)
		{
			as_cmd_to_uart(cmd);
			free(cmd);
		}
		
		usleep(1000);
		
	}	                      
}


/* thread  reading from Serial Port and send it to msg queue */
static void  *pbx2ixp_thread(void *data)
{
	PBX_COMMAND 	*cmd;
   
	while(1)
	{
		//trace;
		cmd = as_cmd_from_uart( );
		if( cmd )
		{
			if( ( SIP_CMD_MIN1 < cmd->op && cmd->op < SIP_CMD_MAX1 ) 
				||(SIP_CMD_MIN2 < cmd->op && cmd->op < SIP_CMD_MAX2) )
			{
				as_error_msg("SIP command parsed, op=%x\r\n", cmd->op);
				as_cmd_uart_to_sip(cmd);
			}
   			else
   			{
				as_error_msg("CGI command parsed, op=%x\r\n" ,cmd->op);
				as_cmd_uart_to_cgi(cmd);
			}
			free(cmd);
		}
		usleep(1000);
	}
 
}


static as_thread_t rxThread =
{
	id		:	-1,
	name	:	"IXP-->PBX",
	priority	:	3,
	handler	:	ixp2pbx_thread,
	log		:	as_error_msg//,
	//private	:	NULL
};

static as_thread_t sendThread =
{
	id		:	-1,
	name	:	"PBX-->IXP",
	priority	:	3,
	handler	:	pbx2ixp_thread,
	log		:	as_error_msg//,
	//private	:	NULL
};

int as_uartd_threads_init()
{
	int res;

	res = as_cmd_uart_init(0, 0);
	if(res < 0)
		exit(1);
	
	as_msg_queue_init( );
	
	as_thread_create( &rxThread );
	as_thread_create( &sendThread);

	as_thread_join( &rxThread);
	as_thread_join( &sendThread);

	return 0;
}

void as_uartd_threads_destory()
{
	as_cmd_uart_destroy();
	as_msg_queue_destroy();
}

