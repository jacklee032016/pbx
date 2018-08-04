/*
* $Id: as_msg_queue.h,v 1.7 2007/03/26 08:53:00 lizhijie Exp $
*/

#ifndef  __AS_MSG_QUEUE_H__
#define __AS_MSG_QUEUE_H__

typedef struct
{
	int	uartdMsgId; 			/* handler which uartd monior */
	int	cgiMsgId; 			/* handler which CGI monitor */
	int	sipMsgId; 			/* handler which SIP monitor */
	int	recordMsgId;		/* handler which Record monitor */
	int	selfMsgId;			/* handler which SlefTest monitor */
	
	int	pid_msgid;
	int	uart_fd;
}AS_MSG_Q_HANDLER;

typedef enum
{
	AS_MSG_TYPE_CGI		=	1,
	AS_MSG_TYPE_SELFTEST	=	2,
	AS_MSG_TYPE_SIP		=	3,
	AS_MSG_TYPE_RECORD	=	4
}AS_MESSAGE_TYPE;

#define  CMD_VALUE_MAX   14

struct _ip_pbx_cmd
{
	unsigned char		op;
	unsigned char		id;
	unsigned char		value[CMD_VALUE_MAX];
}__attribute__ ((packed));

/* message (command ) commnicate between PBX and IXP422 */
typedef struct _ip_pbx_cmd PBX_COMMAND;

/* assist message exchanged between uartd and SIP/CGI */
typedef struct _msg_type
{
	long				type;
	PBX_COMMAND 	cmd;	/* command commu between PBX and IXP */
}AS_MESSAGE;

/* command for SelfTest */
#define	PCM_SELFTEST_REQUEST				0x18
#define	PCM_CHAN_TEST						0x98
#define	PCM_PBX_BUSY						0x99

/* record command */
#define	PCM_RECORDER_QEQUEST				0x19	/* IP-->PBX */
#define	PCM_RECORDER_RESPONSE			0x9A	/* PBX-->IP */
#define	PCM_RECORDER_END					0x1A	/* IP-->PBX */

#define	PCM_RECORDER_CGI_TIMEOUT		0xFE	/* CGI(record)-->RECORD */
#define	PCM_RECORDER_CGI_BEGIN			0xFD	/* CGI(record)-->RECORD */
#define	PCM_RECORDER_CGI_END				0xFC	/* CGI(record)-->RECORD */

#define	SIP_CMD_MAX1						0x88 /*modified from 85 to 88 by lizhijie*/
#define	SIP_CMD_MIN1						0x80

#define	SIP_CMD_MAX2						0x95  /*modified from 94 to 95 by lizhijie*/
#define	SIP_CMD_MIN2						0x90

#define	SIP_CMD_STATE_CHANGED			0xFE	/* cmd only used in IP */

#define	SYS_MSG_QUEUE_DIR				"/var/mq/"


#define	MSGQ_FILE_UARTD 					SYS_MSG_QUEUE_DIR"uartd.mq"		/* SIP/CGI-->UARTD */
#define	MSGQ_FILE_CGI 						SYS_MSG_QUEUE_DIR"cgi.mq"           /* UARTD-->CGI */
#define	MSGQ_FILE_SIP						SYS_MSG_QUEUE_DIR"sip.mq"		/* UARTD--> SIP */
#define	MSGQ_FILE_RECORD					SYS_MSG_QUEUE_DIR"record.mq"	/* UARTD--> record */
#define	MSGQ_FILE_SELFTEST				SYS_MSG_QUEUE_DIR"selftest.mq"	/* UARTD--> selftest */

/* return length in variable 'i' */
#define	PBX_MSG_LENGTH(data,  i)		\
do{	\
	for(i=2; i<sizeof(PBX_COMMAND); i++){  \
		if( *(data+i) == 0xff) {  \
			break; \
		} \
	} \
}while(0)

#endif

