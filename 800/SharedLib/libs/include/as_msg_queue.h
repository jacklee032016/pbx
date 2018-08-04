#ifndef  __AS_MSG_QUEUE_H__
#define __AS_MSG_QUEUE_H__

typedef struct
{
	int rmsgid; 			/* handler which uartd monior */
	int smsgid; 			/* handler which uartd used to send out message to SIP or CGI */
	int event_msgid; 		/* event msg queue for SIP telephone event */
	int pid_msgid;
	int uart_fd;
}AS_MSG_Q_HANDLER;

typedef enum
{
	AS_MSG_TYPE_SIP = 	1,
	AS_MSG_TYPE_CGI = 2
}AS_MESSAGE_TYPE;

#define  CMD_VALUE_MAX   14

struct _ip_pbx_cmd
{
	unsigned char op;
	unsigned char id;
	unsigned char value[CMD_VALUE_MAX];
};

/* message (command ) commnicate between PBX and IXP422 */
typedef struct _ip_pbx_cmd PBX_COMMAND;

/* assist message exchanged between uartd and SIP/CGI */
typedef struct _msg_type
{
	long type;
	PBX_COMMAND 	cmd;	/* command commu between PBX and IXP */
}AS_MESSAGE;



#define SIP_CMD_MAX1 	0x88 /*modified from 85 to 88 by lijie*/
#define SIP_CMD_MIN1  	0x80

#define SIP_CMD_MAX2 	0x95  /*modified from 94 to 95 by lijie*/
#define SIP_CMD_MIN2  	0x90

#define  STATFILE     	"/tmp/msgfile/statis.data"
#define  SMSG_FILE 	"/tmp/msgfile/rmsg.data"           /* send by uartd and received by SIP/CGI */
#define  RMSG_FILE 	"/tmp/msgfile/smsg.data"		/* send by SIP/CGI and received by uartd */
#define  EMSG_FILE	"/tmp/msgfile/emsg.data"
#define  LOG_FILE	"/tmp/msgfile/log.data"

#endif
