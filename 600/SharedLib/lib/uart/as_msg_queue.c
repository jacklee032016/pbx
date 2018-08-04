/*
* $Id: as_msg_queue.c,v 1.11 2007/06/21 16:44:04 lizhijie Exp $
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
	   
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "as_global.h"
#include "as_msg_queue.h"
#include "aslib.h"

static AS_MSG_Q_HANDLER msgHandler;
/*
* System V Message Queue
*/
static int __as_msg_queue_send( int  msgid,  AS_MESSAGE *msg)
{
	int res;

retry:	
	res=msgsnd( msgid, msg, sizeof(AS_MESSAGE) - sizeof(long), 0);
	if(res <0 )	
	{	
		if(errno==EINTR)
		{/*it is interrupted by Signal Handler : signal rxed */
			goto retry;
		}
		AS_MSG_LOG(AS_LOG_ERR, "send msg(type=%d cmd.op=0x%x) to into message queue, error is '%s'", msg->type, msg->cmd.op, strerror(errno) );	
		return AS_FAIL;		
	}	
	AS_MSG_DEBUG(AS_LOG_DEBUG, "send msg(type=%d cmd.op=0x%x) to into message queue", msg->type, msg->cmd.op);	

	
	return AS_OK;
}

PBX_COMMAND *__as_msg_queue_receive(int msgid, AS_MESSAGE_TYPE type)
{
	int res;
	AS_MESSAGE msg;
	PBX_COMMAND *cmd = NULL;

retry:
	res = msgrcv(msgid, &msg, sizeof(AS_MESSAGE) - sizeof(long), (long )type, 0);
	if( res> 0)
	{
		cmd = (PBX_COMMAND *)malloc(sizeof(PBX_COMMAND) );
		if(!cmd)
			return NULL;
		memset(cmd, 0, sizeof(PBX_COMMAND) );
		memcpy( cmd, &(msg.cmd), res );
	}
	else
	{
		if(errno == EINTR)
		{
			AS_MSG_DEBUG(AS_LOG_NOTICE, "rx msg is interruptted by Signal");
			goto retry;
		}
		AS_MSG_DEBUG(AS_LOG_INFO, "rx msg(type=%d) from message queue failed, error is %s", type, strerror(errno) );
	}
	
	return cmd;
	
}

void  as_msg_queue_init( int isUartd )
{	
	key_t uartd_key,cgi_key, sip_key, record_key, self_key;

      	if((uartd_key=ftok(MSGQ_FILE_UARTD, 0xffffff01 ))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Create Key Error for %s : %s",MSGQ_FILE_UARTD, strerror(errno));
		exit(1);
	}
	if((cgi_key=ftok(MSGQ_FILE_CGI, 0xfeffff02))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Create Key Error for %s : %s",MSGQ_FILE_CGI , strerror(errno));
		exit(1);
	}
	if(( sip_key=ftok(MSGQ_FILE_SIP, 0xfdffff03))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Create Key Error for %s : %s", MSGQ_FILE_SIP , strerror(errno));
		exit(1);
	}
	if(( record_key=ftok(MSGQ_FILE_RECORD, 0xfcffff04))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Create Key Error for %s : %s", MSGQ_FILE_RECORD , strerror(errno));
		exit(1);
	}
	if(( self_key=ftok(MSGQ_FILE_SELFTEST, 0xfcffff05))==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Create Key Error for %s : %s", MSGQ_FILE_SELFTEST , strerror(errno));
		exit(1);
	}
	
	if(isUartd)
	{/* */
		if( (msgHandler.uartdMsgId = msgget(uartd_key,IPC_CREAT))==-1)
		{
			msgctl(msgHandler.uartdMsgId, IPC_RMID,NULL);
			if( (msgHandler.uartdMsgId = msgget(uartd_key,IPC_CREAT))==-1)
			{
				AS_MSG_LOG(AS_LOG_ERR,"Create UARTD MessageQueue Error:%s",strerror(errno));
				exit(1);
			}	
		}

		if( (msgHandler.cgiMsgId = msgget(cgi_key,IPC_CREAT))==-1)
		{
			msgctl(msgHandler.cgiMsgId, IPC_RMID,NULL);
			if( (msgHandler.cgiMsgId = msgget(cgi_key,IPC_CREAT))==-1)
			{
				AS_MSG_LOG(AS_LOG_ERR,"Create CGI MessageQueue Error:%s",strerror(errno));
				exit(1);
			}	
		}

		if( (msgHandler.recordMsgId = msgget(record_key,IPC_CREAT))==-1)
		{
			msgctl(msgHandler.recordMsgId, IPC_RMID,NULL);
			if( (msgHandler.recordMsgId = msgget(record_key,IPC_CREAT))==-1)
			{
				AS_MSG_LOG(AS_LOG_ERR,"Create Record MessageQueue Error:%s",strerror(errno));
				exit(1);
			}	
		}
		if( (msgHandler.selfMsgId = msgget(self_key,IPC_CREAT))==-1)
		{
			msgctl(msgHandler.selfMsgId, IPC_RMID,NULL);
			if( (msgHandler.selfMsgId = msgget(self_key,IPC_CREAT))==-1)
			{
				AS_MSG_LOG(AS_LOG_ERR,"Create Selftest MessageQueue Error:%s",strerror(errno));
				exit(1);
			}	
		}
		if( (msgHandler.sipMsgId = msgget(sip_key,IPC_CREAT))==-1)
		{
			msgctl(msgHandler.sipMsgId, IPC_RMID,NULL);
			if( (msgHandler.sipMsgId = msgget(sip_key,IPC_CREAT))==-1)
			{
				AS_MSG_LOG(AS_LOG_ERR,"Create SIP MessageQueue Error:%s",strerror(errno));
				exit(1);
			}	
		}
	}
	else
	{
		if((msgHandler.uartdMsgId =msgget( uartd_key,0) )==-1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"Create UARTD(s) MessageQueue Error:%s",strerror(errno));
			exit(1);
		}

		if((msgHandler.cgiMsgId = msgget(cgi_key,0))==-1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"Create CGI MessageQueue(s) Error:%s",strerror(errno));
			exit(1);
		}

		if((msgHandler.sipMsgId = msgget( sip_key,0) )==-1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"create SIP MessageQueue(s) error!");
			exit(1);
		}
		if((msgHandler.recordMsgId = msgget( record_key,0) )==-1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"create Record MessageQueue(s) error!");
			exit(1);
		}
		if((msgHandler.selfMsgId = msgget( self_key,0) )==-1)
		{
			AS_MSG_LOG(AS_LOG_ERR,"create Selftest MessageQueue error!");
			exit(1);
		}
	}
	
}

	   
void as_msg_queue_destroy()
{
	if(msgctl(msgHandler.uartdMsgId, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"msgctl failed UARTD, '%s'!", strerror(errno) ); 
	}
	
	if(msgctl( msgHandler.cgiMsgId, IPC_RMID,0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"msgctl failed UARTD, '%s'!", strerror(errno) );
	}

	if(msgctl( msgHandler.sipMsgId, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"msgctl failed SIP, '%s'!", strerror(errno) );
	}
	if(msgctl( msgHandler.recordMsgId, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"msgctl failed Record, '%s'!", strerror(errno) );
	}
	if(msgctl( msgHandler.selfMsgId, IPC_RMID, 0)==-1)
	{
		AS_MSG_LOG(AS_LOG_ERR,"msgctl failed Selftest, '%s'!", strerror(errno) );
	}
}

PBX_COMMAND *as_cmd_uart_from_apps( )
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.uartdMsgId, 0L);
	if(!cmd)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,"Receive uart cmd from CGI/SIP : 0p=%0x, id= %0x", cmd->op, cmd->id);

	return cmd;
}

int as_cmd_uart_to_apps( PBX_COMMAND *cmd, AS_MESSAGE_TYPE type)
{
	AS_MESSAGE  msg;
	int res;

	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) type;

	if(type==AS_MSG_TYPE_CGI)
		res = __as_msg_queue_send( msgHandler.cgiMsgId,  &msg);
	else if(type==AS_MSG_TYPE_SELFTEST)
		res = __as_msg_queue_send( msgHandler.selfMsgId,  &msg);
	else if(type==AS_MSG_TYPE_RECORD)
		res = __as_msg_queue_send( msgHandler.recordMsgId,  &msg);
	else
		res = __as_msg_queue_send( msgHandler.sipMsgId,  &msg);

	return res;
}

PBX_COMMAND *as_cmd_cgi_from_uart()
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.cgiMsgId, 0 );
	if(!cmd)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,"Receive CGI cmd from uart : 0p=%0x, id= %0x", cmd->op, cmd->id);

	return cmd;
}

PBX_COMMAND *as_cmd_selftest_from_uart()
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.selfMsgId, 0 );
	if(!cmd)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG,"Receive SELFTEST cmd from uart : 0p=%0x, id= %0x", cmd->op, cmd->id);

	return cmd;
}

PBX_COMMAND *as_cmd_record_from_uart()
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.recordMsgId, 0 );
	if(!cmd)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG, "Receive RECORD cmd from uart : 0p=%0x, id= %0x", cmd->op, cmd->id);
	
	return cmd;
}

PBX_COMMAND *as_cmd_sip_from_uart()
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.sipMsgId, 0 );
	if(!cmd)
		return NULL;
	
	AS_MSG_DEBUG(AS_LOG_DEBUG, "Receive sip cmd from uart : 0p=%0x, id= %0x", cmd->op, cmd->id);
	
	return cmd;
}


int as_cmd_apps_to_uart(PBX_COMMAND *cmd, AS_MESSAGE_TYPE type)
{
	AS_MESSAGE  msg;
	int res;
	
	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) type;

	res = __as_msg_queue_send( msgHandler.uartdMsgId,  &msg );

	return res;
}

