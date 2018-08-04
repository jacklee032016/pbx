/*
* $Id: pbx_cgi_command.c,v 1.8 2007/03/26 09:06:26 lizhijie Exp $
* read command from CGI and send it to serial port
*/
#include "_pbx.h"

void feedback_delayed_signal_to_cgi(int pid, int opCode, int responseCode)
{
	sigset_t			tmpst;
	siginfo_t			tmpinfo;
	struct timespec	tmpts;
	int 				retval;
	int				i = 0;

	cgi_sig_send(pid,SIG_PBX_CGI, opCode);
	/*suspends execution of the calling process until one of the  signals in set is delivered. */
	sigemptyset(&tmpst);
	sigaddset(&tmpst,SIG_PBX_CGI);
	tmpts.tv_sec=2;
	tmpts.tv_nsec=0;
	do
	{
		retval=sigtimedwait(&tmpst,&tmpinfo,&tmpts);
		tmpts.tv_sec-=1;
		i++;
		if(i > 10 )
			break;
	}while((retval>0&&retval!=SIG_PBX_CGI)||tmpinfo.si_pid!= pid||tmpinfo.si_int != responseCode);
}


void pbx_handler_cgisig(int signo, siginfo_t *info,void * val)
{
	llist	*newCfgs, tmpList;

	switch(info->si_int)
	{
		case OP_PBX_WRITE_REQ:
			newCfgs = &tmpList;
			list_create( newCfgs);
			if(readconfig(FILE_PBX_TMP,ITEM_DBL, NULL, newCfgs))
			{
				AS_MSG_LOG(AS_LOG_ERR, "Can not read Data File %s send by CGI", FILE_PBX_TMP);
				list_clear( newCfgs);
				return;
			}

			list_replace_with_new_list(&pbxentries, newCfgs);
			if(pbx_setup_pbx( newCfgs) )
			{
				AS_MSG_LOG(AS_LOG_ERR, "error when write UART message queue to config AnalogPBX");
				list_clear(newCfgs);					
				return ;
			}
			list_clear(newCfgs);
			
			if(writeconfig(FILE_PBX_CONF, ITEM_DBL, NULL, &pbxentries))
			{
				AS_MSG_LOG(AS_LOG_ERR,"error in writeconfig");
				return;
			}

			feedback_delayed_signal_to_cgi(info->si_pid, OP_PBX_WRITE_RPL, OP_PBX_WRITE_OK);

			break;
			
		case OP_PBX_READ_REQ:
			feedback_delayed_signal_to_cgi(info->si_pid, OP_PBX_READ_RPL, OP_PBX_READ_OK);
			break;
			
		default:
			AS_MSG_DEBUG(AS_LOG_ALERT, "unknown opcode %d\n", info->si_int);
	}
	
}

