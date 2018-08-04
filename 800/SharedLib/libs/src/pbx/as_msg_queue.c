 /*
 * $Log: as_msg_queue.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.2  2005/07/08 01:23:47  lijie
 * no message
 *
 * Revision 1.1  2005/06/20 09:37:57  lijie
 * 消息队列封装到共享库
 *
 * Revision 1.2  2005/06/03 03:42:05  lizhijie
 * debug the timeout operation of 'select', refer to comments
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_msg_queue.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "as_global.h"
#include "as_msg_queue.h"
#include "aslib.h"

static AS_MSG_Q_HANDLER msgHandler;

static int __as_msg_queue_send( int  msgid,  AS_MESSAGE *msg)
{
	int res;
	int try_send=0;

	do
	{
		res=msgsnd( msgid, msg, sizeof(AS_MESSAGE) - sizeof(long), IPC_NOWAIT);
		try_send++;
	}while(res==-1&&try_send<3);
	
	if(res <0 )	
	{	
		PDEBUG("snd msg to IXP thread error from uartd, error is '%s'\r\n" ,strerror(errno));	
		return AS_FAIL;		
	}	

	
	return AS_OK;
}

static PBX_COMMAND *__as_msg_queue_receive(int msgid, AS_MESSAGE_TYPE type)
{
	int res;
	AS_MESSAGE msg;
	PBX_COMMAND *ret;

	ret = NULL;
		
	res = msgrcv(msgid, &msg, sizeof(PBX_COMMAND), (long )type, IPC_NOWAIT);
	if(res!=-1)
	{
		ret = (PBX_COMMAND *)malloc(sizeof(PBX_COMMAND) );
		assert(ret);
		memcpy( ret, &(msg.cmd), sizeof(PBX_COMMAND) );
	}
	return ret;
	
}

void  as_msg_queue_init(  )
{	
	key_t event_key,rx_key,send_key;

      	if((rx_key=ftok(RMSG_FILE,'a'))==-1)
	{
		fprintf(stderr,"Create Key Error for %s:%s\r\n",RMSG_FILE, strerror(errno));
		exit(1);
	}

	if((msgHandler.rmsgid=msgget(rx_key,0777|IPC_CREAT))==-1)
	{
		fprintf(stderr,"Create Message Error:%s\r\n",strerror(errno));
		exit(1);
	}

	if((send_key=ftok(SMSG_FILE,'a'))==-1)
	{
		fprintf(stderr,"Create Key Error for %s: %s\r\n",SMSG_FILE , strerror(errno));
		exit(1);
	}

	if((msgHandler.smsgid=msgget(send_key,0777|IPC_CREAT))==-1)
	{
		fprintf(stderr,"Create Message Error:%s\r\n",strerror(errno));
		exit(1);
	}

	if((event_key=ftok(EMSG_FILE,'a'))==-1)
	{
		fprintf(stderr,"Create Key Error for %s:%s\r\n",EMSG_FILE , strerror(errno));
		exit(1);
	}
	
	if((msgHandler.event_msgid=msgget(event_key,0777|IPC_CREAT))==-1)
	{
		printf("create message error!\r\n");
		exit(1);
	}
	
}

	   
void as_msg_queue_destroy()
{
	if(msgctl(msgHandler.rmsgid, IPC_RMID, 0)==-1)
	{
		printf("msgctl failed, '%s'!\r\n", strerror(errno) ); 
	}
	
	if(msgctl( msgHandler.smsgid, IPC_RMID,0)==-1)
	{
		printf("msgctl failed, '%s'!\r\n", strerror(errno) );
	}

	if(msgctl( msgHandler.event_msgid, IPC_RMID, 0)==-1)
	{
		printf("msgctl failed, '%s'!\r\n", strerror(errno) );
	}
}

PBX_COMMAND *as_cmd_uart_from_sip( )
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive( msgHandler.rmsgid, AS_MSG_TYPE_SIP );
	
	if(!cmd)
		return NULL;

	PDEBUG("Receive uart cmd from sip : 0p=%0x, id= %0x\r\n", cmd->op, cmd->id);

	return cmd;
}

PBX_COMMAND *as_cmd_uart_from_cgi( )
{
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.rmsgid, AS_MSG_TYPE_CGI );
	if(!cmd)
		return NULL;
	
	PDEBUG("Receive uart cmd from CGI : 0p=%0x, id= %0x\r\n", cmd->op, cmd->id);

	return cmd;
}

int as_cmd_uart_to_cgi( PBX_COMMAND *cmd )
{
	AS_MESSAGE  msg;
	int res;

	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) AS_MSG_TYPE_CGI;

	res = __as_msg_queue_send( msgHandler.smsgid,  &msg);

	return res;
}


int as_cmd_uart_to_sip( PBX_COMMAND *cmd )
{
	AS_MESSAGE  msg;
	int res;

	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) AS_MSG_TYPE_SIP;

	res = __as_msg_queue_send( msgHandler.smsgid,  &msg );

	return res;

}

int as_cmd_cgi_to_uart(PBX_COMMAND *cmd )
{
	AS_MESSAGE  msg;
	int res;
	
	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) AS_MSG_TYPE_CGI;

	res = __as_msg_queue_send( msgHandler.rmsgid,  &msg );

	return res;

}

int as_cmd_sip_to_uart( PBX_COMMAND *cmd )
{
	AS_MESSAGE  msg;
	int res;

	memset(&msg, 0, sizeof(AS_MESSAGE) );
	
	msg.cmd.op = cmd->op;
	msg.cmd.id = cmd->id;
	memcpy(msg.cmd.value, cmd->value, CMD_VALUE_MAX);
	msg.type = (long) AS_MSG_TYPE_SIP;

	res = __as_msg_queue_send( msgHandler.rmsgid,  &msg );

	return res;

}

PBX_COMMAND *as_cmd_sip_from_uart()
{

	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.smsgid, AS_MSG_TYPE_SIP );
	if(!cmd)
		return NULL;
	
	PDEBUG("Receive sip cmd from uart : 0p=%0x, id= %0x\r\n", cmd->op, cmd->id);
	
	return cmd;
}

PBX_COMMAND *as_cmd_cgi_from_uart()
{
	
	PBX_COMMAND *cmd;

	cmd = __as_msg_queue_receive(msgHandler.smsgid, AS_MSG_TYPE_CGI );
	if(!cmd)
		return NULL;
	
	PDEBUG("Receive CGI cmd from uart : 0p=%0x, id= %0x\r\n", cmd->op, cmd->id);

	return cmd;
}
