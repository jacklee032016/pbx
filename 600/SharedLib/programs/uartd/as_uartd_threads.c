/*
* $Id: as_uartd_threads.c,v 1.19 2007/06/19 09:31:51 lizhijie Exp $
*/

#include <stdlib.h>
#include "aslib.h"

#if __WITH_NEW_STC__
#include "uart_cmd.h"
#endif

int  as_cmd_to_uart(char *data, void *th);
int as_cmd_from_uart(char *buf, int size);
int as_cmd_uart_init(int seconds, int millseconds);
void as_cmd_uart_destroy();

PBX_COMMAND *as_cmd_uart_from_apps( );
int as_cmd_uart_to_apps( PBX_COMMAND *cmd, AS_MESSAGE_TYPE type);

extern	uart_t  serial_pbx;

static int	_uartSequence = 0;

typedef struct
{
	int	pbx2Sip;
	int	pbx2Record;
	int	pbx2Cgi;
	int	pbx2Selftest;
	int	digit2Sip;
	int	digit2Pbx;
	
	int	apps2Pbx;

	int	errFromMQ;
	int	errToMQ;

	int	errFromUart;
	int	errToUart;
}uart_stats;


enum
{
	UARTD_PEER_SIP = 0,
	UARTD_PEER_RECORD,
	UARTD_PEER_INVLIDATE
};

typedef	struct
{
	uart_t		*uart;
	
	uart_stats 	stats;

	int			status;	/* peer is SIPUA|record */

	as_thread_t	*hwThread;
	as_thread_t	*mqThread;
}UARTD;

UARTD	uartd;

/* this thread recive command from msg queue and send it to the uart */
static void  *ixp2pbx_thread(void *data)
{    	
	UARTD *uart = (UARTD  *)data;
	as_thread_t *th = uart->mqThread;
//	UARTD *_uartd = (UARTD *)th->private;
	PBX_COMMAND *cmd;
	
	while(1)
	{       
		AS_MSG_DEBUG(AS_LOG_INFO, "%s waiting Message Queue.....", th->name);
		cmd = as_cmd_uart_from_apps( );
		if(cmd)
		{
#if 0		
			if(cmd->op==SIP_CMD_STATE_CHANGED)
			{
				if(_uartd->status == UARTD_PEER_SIP)
				{
					AS_MSG_LOG(AS_LOG_INFO, "UARTD service for RECORD" );
					_uartd->status = UARTD_PEER_RECORD;
				}
				else
				{
					AS_MSG_LOG(AS_LOG_INFO, "UARTD service for SIP" );
					_uartd->status = UARTD_PEER_SIP;
				}	
			}
			else
#endif				
			{
				cmd->id = _uartSequence++;
				if(_uartSequence==0XFF)
					_uartSequence = 0;
				
				char	*p = (char *)cmd;
				AS_MSG_DEBUG(AS_LOG_INFO, "%s : Apps(CGI/SIP) %d MSGs", th->name, ++uartd.stats.apps2Pbx );
				as_cmd_to_uart(p, th);
				free(cmd);
			}	
		}
	}	                      
}

#if 0
int	__as_buf_2_cmd(as_thread_t *th, char *buf, int length)
{
#define	AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT 			0x92	/* PBX send caller number to IP */
	PBX_COMMAND  cmd;
	int i = 0;
	int len;

	if(!buf || length < 2 || length > sizeof(PBX_COMMAND) )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Data Read from PBX Error");
		return -1;
	}
	
	for(i=0; i< length; i++)
	{
//		AS_MSG_LOG(AS_LOG_NOTICE,"0x%x", buf[i]);
		if(buf[i]==0xff)
			break;
	}
	len = i+1;/* add '0xff' byte */
	
	if(len <2 || len > sizeof(PBX_COMMAND) )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Data Format Error");
		return -2;
	}
	
	memset(&cmd, 0, sizeof(PBX_COMMAND) );
	memcpy(&cmd, buf, len);

	if( (SIP_CMD_MIN1 < cmd.op && cmd.op < SIP_CMD_MAX1) 
		||(SIP_CMD_MIN2 < cmd.op && cmd.op < SIP_CMD_MAX2) )
	{

		AS_MSG_LOG(AS_LOG_INFO, "%s : SIP CMDs %d", th->name, ++_stats.pbx2Sip);
		if(cmd.op == AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT)
		{
			AS_MSG_LOG(AS_LOG_NOTICE, "%s : SIP %d PBX Dialed Digits", th->name, ++_stats.digit2Sip);
		}
		as_cmd_uart_to_apps(&cmd, AS_MSG_TYPE_SIP);
	}
	else
	{
		AS_MSG_LOG(AS_LOG_INFO, "%s : CGI CMDs %d", th->name, ++_stats.pbx2Cgi );
		as_cmd_uart_to_apps(&cmd, AS_MSG_TYPE_CGI);
	}
	
	return len;
}
#else
int	__as_buf_uartd_2_apps(UARTD *_uartd, PBX_COMMAND *cmd)
{
#if __WITH_NEW_STC__
	if(cmd->op >= CMD_CFG_TRACK_MODE)
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : CGI CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Cgi );
		return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_CGI);
	}
	else if(cmd->op == CMD_RECORD_RESPONSE )
	{
		AS_MSG_LOG(AS_LOG_INFO, "%s : Record CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Sip);
		return as_cmd_uart_to_apps( cmd, AS_MSG_TYPE_RECORD );
	}
	else if(cmd->op == CMD_SELFTEST_RESULT || cmd->op == CMD_SELFTEST_BUSY)
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : SelfTest CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Selftest);
		return as_cmd_uart_to_apps( cmd, AS_MSG_TYPE_SELFTEST );

	}
	else if( (CMD_CALL_DIALING <= cmd->op && cmd->op <= CMD_CALL_RESET_ALL)  )
	{
		if(cmd->op == CMD_CALL_PHONE_DIGITS)
		{
			AS_MSG_LOG(AS_LOG_NOTICE, "%s : SIP %d PBX Dialed Digits : %c", _uartd->hwThread->name, ++_uartd->stats.digit2Sip, cmd->value[0] );
		}
#if 0		
		if(_uartd->status == UARTD_PEER_RECORD)
		{
			return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_RECORD );
		}
		else
#endif			
		{
			AS_MSG_DEBUG(AS_LOG_INFO, "%s : SIP CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Sip);
			return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_SIP);
		}	
	}
	else
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : CGI CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Cgi );
		return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_CGI);
	}
#else

#define	AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT 			0x92	/* PBX send caller number to IP */

	if(cmd->op == PCM_CHAN_TEST || cmd->op == PCM_PBX_BUSY)
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : SelfTest CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Selftest);
		return as_cmd_uart_to_apps( cmd, AS_MSG_TYPE_SELFTEST );
	}
	else if(cmd->op == PCM_RECORDER_RESPONSE )//|| cmd->op == PCM_RECORDER_END )
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : Record CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Sip);
		return as_cmd_uart_to_apps( cmd, AS_MSG_TYPE_RECORD );
	}
	else if( (SIP_CMD_MIN1 < cmd->op && cmd->op < SIP_CMD_MAX1) 
		||(SIP_CMD_MIN2 < cmd->op && cmd->op < SIP_CMD_MAX2) )
	{
		if(cmd->op == AS_PBX_CMD_TYPE_CALLER_DIAL_DIGIT)
		{
			AS_MSG_LOG(AS_LOG_NOTICE, "%s : SIP %d PBX Dialed Digits", _uartd->hwThread->name, ++_uartd->stats.digit2Sip);
		}
#if 0		
		if(_uartd->status == UARTD_PEER_RECORD)
		{
			return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_RECORD );
		}
		else
#endif			
		{
			AS_MSG_DEBUG(AS_LOG_INFO, "%s : SIP CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Sip);
			return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_SIP);
		}	
	}
	else
	{
		AS_MSG_DEBUG(AS_LOG_INFO, "%s : CGI CMDs %d", _uartd->hwThread->name, ++_uartd->stats.pbx2Cgi );
		return as_cmd_uart_to_apps(cmd, AS_MSG_TYPE_CGI);
	}
#endif

	return 0;
}

#endif

/* thread  reading from Serial Port and send it to msg queue */
static void  *pbx2ixp_thread(void *data)
{
	UARTD *uart = (UARTD  *)data;
	as_thread_t *th = uart->hwThread;
#if 0	
	uart_t *uart = &(serial_pbx);
	char  buf[sizeof(PBX_COMMAND)], *p;
	int	res;
	int	length;
	while(1)
	{
		memset(buf, 0, sizeof(PBX_COMMAND));
		res = (uart->read)(uart, buf, sizeof( PBX_COMMAND) );

		AS_MSG_LOG(AS_LOG_NOTICE,"read from uart : %d bytes", res);
			
		length = res;
		res = 0;
		p = buf;
		while( length > 0)
		{
			res = __as_buf_2_cmd(th, p, length);
			if(res < 0)
			{
				int i = 0;
				AS_MSG_LOG(AS_LOG_ERR,"rx PBX Command Error, as following : ");
				for(i=0; i< length ; i++)
				{
					AS_MSG_LOG(AS_LOG_ERR,"0x%x", *(p+i) );
				}
				break;
			}
			p = p + res;
			length -= res;
		}

	}
#else
#define _BUF_LEN_	1024

	UARTD *_uartd = (UARTD *)th->priv;
	char  buf[sizeof(PBX_COMMAND)];
	int res;
	
	int iBegin = 0;
	int iEnd = 0;
	char bufRead[_BUF_LEN_];

	while(1)
	{
		res = as_cmd_from_uart( buf, sizeof(buf) );
		if(res <= 0)
		{
			AS_MSG_LOG(AS_LOG_ERR, "read from UART %d errors length=%d", ++_uartd->stats.errFromUart,  res);
			continue;
		}
		
#if 	DEBUG_RAW_RW	
		{
			int	i;
			AS_MSG_LOG(AS_LOG_EMERG, "Data Read from UART" );
			for(i=0; i< res; i++)
			{
				AS_MSG_LOG(AS_LOG_EMERG, "0x%x", buf[i] );
			}
			AS_MSG_LOG(AS_LOG_EMERG, "\n" );
		}
#endif

		if(iEnd==iBegin)
			iBegin = iEnd = 0;
		
		/* data copy */
		if(iEnd+res<_BUF_LEN_)			
		{
			memcpy(bufRead+iEnd, buf, res);
			iEnd = iEnd+res;
		}
		else
		{
			AS_MSG_LOG(AS_LOG_ERR, "Buffer is not enough, ignore cmd");
		}

		/* iterate all CMDs in buffer */
		while( iBegin < iEnd)
		{
			int 	i;
			int	count = 0;
			PBX_COMMAND  cmd;
			
			for(i = iBegin; i<iEnd; i++)
			{
				if(bufRead[i] == 0xFF)
				{
					count = (i-iBegin) + 1;/*add byte of 'oxff' */
					break;
				}	
			}
			
			if( count==0 )
			{/* no 0xFF found. read UART again */
				AS_MSG_LOG(AS_LOG_NOTICE, "rx data too short(no packet end ID found), need More Data" );
				break;
			}
			else if(count < 3)
			{/* found 0xFF, but it is too short */
				AS_MSG_LOG(AS_LOG_ERR, "rx Packet is too short(length is %d), Error Data, Ignore it", count );
				iBegin += count ;
				break;
			}
			
			if(( iBegin+ count) <= iEnd )
			{
				if(count > sizeof(PBX_COMMAND) )
				{
					AS_MSG_LOG(AS_LOG_ERR, "too big packet recvied, throw off it" );
					iBegin += count;
					continue;
				}
				
				memcpy(&cmd, bufRead+iBegin, count );
				__as_buf_uartd_2_apps(_uartd, &cmd);
				
				iBegin += count;
			}
			else
			{
				AS_MSG_LOG(AS_LOG_ERR, "parse error %d too big packet recvied, throw off it", __LINE__);
				iBegin = iEnd = 0;
			}
		}

	}
	
#endif
	return NULL;
}


static as_thread_t rxThread =
{
	id		:	-1,
	name	:	"IP-->Analog",
	priority	:	3,
	handler	:	ixp2pbx_thread,
	log		:	printf,
	priv		:	&uartd
};

static as_thread_t sendThread =
{
	id		:	-1,
	name	:	"Analog-->IP",
	priority	:	3,
	handler	:	pbx2ixp_thread,
	log		:	printf,
	priv		:	&uartd
};

int as_uartd_threads_init()
{
	int res;

	res = as_cmd_uart_init(0, 0);
	if(res < 0)
		exit(1);
	memset(&uartd, 0, sizeof(UARTD) );
	uartd.uart = & serial_pbx;
	uartd.status = UARTD_PEER_SIP;
	uartd.hwThread = &sendThread;
	uartd.mqThread = &rxThread;
	
	as_msg_queue_init(1);
	
	as_thread_create( &sendThread);/* must init it firstly*/
	as_thread_create( &rxThread );

	as_thread_join( &rxThread);
	as_thread_join( &sendThread);

	return 0;
}

void as_uartd_threads_destory()
{
	as_cmd_uart_destroy();
	as_msg_queue_destroy();
}

