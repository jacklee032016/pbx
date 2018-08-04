/*
* $Id: as_uartd_message_queue.c,v 1.3 2007/03/16 20:39:41 lizhijie Exp $
*/

#include "as_uartd_message_queue.h"
#include "as_global.h"
#include "aslib.h"

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

static int __as_msg_queue_send( int  msgid, void *msg, int length)
{
	int res;
	int try_send=0;

	do
	{
		res=msgsnd( msgid, msg, length-sizeof(long), IPC_NOWAIT);
		try_send++;
	}while(res==-1&&try_send<3);
	
	if(res <0 )	
	{	
		AS_MSG_LOG(AS_LOG_ERR, "snd msg to IXP thread error from uartd, error is '%s'" ,strerror(errno));	
		return AS_FAIL;		
	}	
#if 0
	sigsend(uart_ctrl.cgi_pid,SIG_PBX_COM,0);
	uartstat.wpbx2cgi++; 
#endif
	AS_MSG_DEBUG(AS_LOG_DEBUG, "V0=%x,v1=%x",cmd->pbx_cmd.cmd_value[0],cmd->pbx_cmd.cmd_value[1]);
	return AS_OK;
}

static AS_MESSAGE *__as_msg_queue_receive(AS_MSG_Q_HANDLER *handler, AS_MESSAGE_TYPE type)
{
	int res;
	AS_MESSAGE msg;
	AS_MESSAGE *msgp;

	res = msgrcv( handler->rmsgid, &(msg.cmd), sizeof(AS_MESSAGE)-sizeof(long), (long )type, IPC_NOWAIT);
	if(res!=-1)
	{
		msgp = (AS_MESSAGE *)malloc(sizeof(AS_MESSAGE) );
		assert(msgp);
		memcpy( msgp, &msg, sizeof(AS_MESSAGE) );
		msgp->type = (long)type;

		return msgp;
	}
	return NULL;
}
	   
void as_msg_queue_destroy(AS_MSG_Q_HANDLER *handler)
{
	if(msgctl(handler->rmsgid, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "msgctl failed, '%s'!", strerror(errno) ); 
	}
	
	if(msgctl( handler->smsgid, IPC_RMID,0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "msgctl failed, '%s'!", strerror(errno) );
	}

	if(msgctl( handler->event_msgid, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "msgctl failed, '%s'!", strerror(errno) );
	}
}

int as_msg_queue_init( AS_MSG_Q_HANDLER *handler )
{
	key_t event_key,rx_key,send_key;
	
       if((rx_key=ftok(RMSG_FILE,'a'))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Create Key Error for %s:%s",RMSG_FILE, strerror(errno));
		exit(1);
	}

	if((handler->rmsgid=msgget(rx_key,0777|IPC_CREAT))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Create Message Error:%s",strerror(errno));
		exit(1);
	}

	if((send_key=ftok(SMSG_FILE,'a'))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,  "Create Key Error for %s: %s",SMSG_FILE , strerror(errno));
		exit(1);
	}

	if((handler->smsgid=msgget(send_key,PERM|IPC_CREAT))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Create Message Error:%s",strerror(errno));
		exit(1);
	}

	if((event_key=ftok(EMSG_FILE,'a'))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Create Key Error for %s:%s", EMSG_FILE , strerror(errno));
		exit(1);
	}
	
	if((handler->event_msgid=msgget(event_key,0777|IPC_CREAT))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "create message error!");
		exit(1);
	}
	return AS_OK;
}


AS_MESSAGE *as_msg_rx_sip(AS_MSG_Q_HANDLER *handler )
{
	AS_MESSAGE *msg;

	msg = __as_msg_queue_receive(handler, AS_MSG_TYPE_SIP );
	if(!msg)
		return NULL;

	switch( msg->cmd.op)
	{
		case 0x14:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "ip idle(Play ringback tone now)");
			break;
		case 0x15:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "open channel(Far end offhook)");
			break;
		case 0x11:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "dial valid");
			break;
		case 0x03:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "ring the PBX branch");
		default:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "recv cmd");
	}

	return msg;
}

AS_MESSAGE *as_msg_rx_cgi(AS_MSG_Q_HANDLER *handler )
{
	AS_MESSAGE *msg;

	msg = __as_msg_queue_receive(handler, AS_MSG_TYPE_CGI );
	if(!msg)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG, "Receive CGI msg from CGI : 0p=%c, id= %c, msg=%s", msg->cmd.op, msg->cmd.id, msg->cmd.value);

	switch( msg->cmd.op)
	{
		case 0x14:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "ip idle(Play ringback tone now)");
			break;
		case 0x15:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "open channel(Far end offhook)");
			break;
		case 0x11:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "dial valid");
			break;
		case 0x03:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "ring the PBX branch");
		default:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "recv cmd");
	}

	return msg;
}

int as_msg_send_cgi( AS_MSG_Q_HANDLER *handler, PBX_COMMAND *cmd )
{
	AS_MESSAGE  msg;
	int res;

	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) AS_MSG_TYPE_CGI;

	res = __as_msg_queue_send( handler->smsgid,  (void *)&msg, sizeof(AS_MESSAGE) );

	return res;
}


static int __as_msg_send_sip( AS_MSG_Q_HANDLER *handler, PBX_COMMAND *cmd , AS_MSG_SIP_TYPE  type)
{
	int res=0;
	int i;
	AS_SIP_MESSAGE  msg;
	
	memset(&msg, 0, sizeof(AS_SIP_MESSAGE) );

	msg.channelId = (int)cmd->value[0];
	msg.phoneId = (int)cmd->value[1];

	memset( msg.dialDigital, 0, DIAL_DIGITAL_MAX);
	msg.type= (long) AS_MSG_TYPE_SIP ;

	if(type==AS_MSG_SIP_TYPE_DIAL_DIGITAL)
	{
		for(i=2; i<=13; i++)
		{
			if( cmd->value[i]==0xff)
				break;
		}
		memcpy(msg.dialDigital, &cmd->value[2], i );	 
	}

	res = __as_msg_queue_send( handler->event_msgid,  (void *)&msg, sizeof(AS_SIP_MESSAGE) );

	return res;
}

int as_msg_send_sip( AS_MSG_Q_HANDLER *handler, PBX_COMMAND *cmd )
{
	AS_MSG_SIP_TYPE type = -1;
	switch( cmd->op)
	{
		case  AS_PBX_CMD_TYPE_CALLER_OFFHOOK: 
			AS_MSG_DEBUG(AS_LOG_DEBUG, "OffHook on device(pcm channel) id %d(PBX physical ID is %d)", (int)cmd->value[0], (int)cmd->value[1] );
			type =  AS_MSG_SIP_TYPE_OFFHOOK ;
			break;
		case AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT: //get dial digital
			AS_MSG_DEBUG(AS_LOG_DEBUG, "Get Digit '%c' of %d channel\r\n",cmd->value[2], (int)cmd->value[0] );
			type = AS_MSG_SIP_TYPE_DIAL_DIGITAL;
			break;
		case AS_PBX_CMD_TYPE_CALLER_ONHOOK:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "PBX ONHOOK on device(pcm channel) id %d(PBX physical ID is %d)", (int)cmd->value[0], (int)cmd->value[1] );
			type = AS_MSG_SIP_TYPE_CALLER_ONHOOK;
			break;
		case AS_PBX_CMD_TYPE_CALLEE_OFFHOOK :
			AS_MSG_DEBUG(AS_LOG_DEBUG, "PBX Callee OFFHOOK on device(pcm channel) id %d(PBX physical ID is %d)", (int)cmd->value[0], (int)cmd->value[1] );
			type = AS_MSG_SIP_TYPE_CALLEE_OFFHOOK ;
			break;
		case AS_PBX_CMD_TYPE_CALLEE_SEND_PCM_CHANNEL:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "PBX CALLEE_SEND_PCM_CHANNEL on device(pcm channel) id %d(PBX physical ID is %d)", (int)cmd->value[0], (int)cmd->value[1] );
			type = AS_MSG_SIP_TYPE_CALLEE_SENDCHANNEL ;
			break;
		case AS_PBX_CMD_TYPE_CALLEE_BUSY_HERE:
			AS_MSG_DEBUG(AS_LOG_DEBUG, "PBX CALLEE_BUSY_HERE on device(pcm channel) id %d(PBX physical ID is %d)", (int)cmd->value[0], (int)cmd->value[1] );
			type = AS_MSG_SIP_TYPE_CALLEE_BUSY ;
			break;			
		default:
			break;
	}
	if(-1 != type )
		return __as_msg_send_sip(handler, cmd, type);

	return AS_FAIL;
}

