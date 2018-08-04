#ifndef  __AS_UARTD_MESSAGE_QUEUE_H__
#define __AS_UARTD_MESSAGE_QUEUE_H__


#define  STATFILE     	"/tmp/msgfile/statis.data"
#define  SMSG_FILE 	"/tmp/msgfile/rmsg.data"           /* send by uartd and received by SIP/CGI */
#define  RMSG_FILE 	"/tmp/msgfile/smsg.data"		/* send by SIP/CGI and received by uartd */
#define  EMSG_FILE	"/tmp/msgfile/emsg.data"
#define  LOG_FILE	"/tmp/msgfile/log.data"

#include "as_pbx_cmd.h"

#define  DIAL_DIGITAL_MAX               	20              //phone number max

#define _S_IRUSR   2
#define _S_IWUSR  3
#define PERM           _S_IRUSR|_S_IWUSR           //#define PERM S_IRUSR|S_IWUSR


#define BUFFER        255

typedef enum
{
	AS_MSG_TYPE_SIP = 	1,
	AS_MSG_TYPE_CGI = 2
}AS_MESSAGE_TYPE;

typedef enum
{
	AS_MSG_SIP_TYPE_OFFHOOK 					= 1,
	AS_MSG_SIP_TYPE_DIAL_DIGITAL					= 3,
	AS_MSG_SIP_TYPE_CALLER_ONHOOK 			= 7,
	AS_MSG_SIP_TYPE_CALLEE_OFFHOOK 			= 8,
	AS_MSG_SIP_TYPE_CALLEE_SENDCHANNEL 	= 9,
	AS_MSG_SIP_TYPE_CALLEE_BUSY 				= 10
}AS_MSG_SIP_TYPE;


/* assist message exchanged between uartd and SIP/CGI */
typedef struct _msg_type
{
	long type;
	PBX_COMMAND 	cmd;	/* command commu between PBX and IXP */
}AS_MESSAGE;


typedef struct _sip_msg_sig
{
	long 		type;
	int 			channelId;
	int 			phoneId;
	unsigned char dialDigital[DIAL_DIGITAL_MAX]; 
}AS_SIP_MESSAGE;


typedef struct
{
	int rmsgid; 			/* handler which uartd monior */
	int smsgid; 			/* handler which uartd used to send out message to SIP or CGI */
	int event_msgid; 		/* event msg queue for SIP telephone event */
	int pid_msgid;
	int uart_fd;
}AS_MSG_Q_HANDLER;

int as_msg_queue_init( AS_MSG_Q_HANDLER *handler );
void as_msg_queue_destroy(AS_MSG_Q_HANDLER *handler);
AS_MESSAGE *as_msg_rx_sip(AS_MSG_Q_HANDLER *handler );
AS_MESSAGE *as_msg_rx_cgi(AS_MSG_Q_HANDLER *handler );
int as_msg_send_cgi( AS_MSG_Q_HANDLER *handler, PBX_COMMAND *cmd );
int as_msg_send_sip( AS_MSG_Q_HANDLER *handler, PBX_COMMAND *cmd );

#endif

