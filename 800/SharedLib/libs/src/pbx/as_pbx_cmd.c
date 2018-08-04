/*
 * $Author: lizhijie $
 * $Log: as_pbx_cmd.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.9  2005/12/31 05:51:34  tangr
 * fix for segment and mem leak
 *
 * Revision 1.8  2005/09/07 10:12:32  wangwei
 * no message
 *
 * Revision 1.7  2005/08/05 03:29:47  lijie
 * no message
 *
 * Revision 1.6  2005/08/05 01:23:45  lijie
 * no message
 *
 * Revision 1.5  2005/08/04 03:15:18  lijie
 * 1.串口读bug修正
 * 2.添加发送dtmf音的接口
 *
 * Revision 1.4  2005/07/08 01:24:26  lijie
 * no message
 *
 * Revision 1.3  2005/07/01 08:24:53  wangwei
 * 更改串口通讯方式为硬件握手
 *
 * Revision 1.2  2005/06/20 09:36:15  lijie
 * 串口bug修正,消息队列封装到共享库
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_pbx_cmd.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/
#include "as_uart.h"
#include "as_pbx_cmd.h"
#include "aslib.h"

#include <stdlib.h>
#include <assert.h>


static uart_t  serial_pbx = 
{
	name	:	"PBX UART Comm",
#if  __ARM_IXP__
	port		:	UART_TTYS0,
#else
	port		:	UART_TTYS1,
#endif

#if  !__USE_SOFTWARE__
	type		:	HANDSHANK_HARDWARE,
#else	
	type		:	HANDSHANK_SOFTWARE,
#endif

	baudrate	:	B9600,
	databits	:	CS8,
	stopbits	:	'A',
	parity	:	'N',

	fd		:	-1,
	
	open	:	uart_open,
	close	:	uart_close,
	read		:	uart_read,
	write	:	uart_write
};


/* send command to PBX through UART */
int  as_cmd_to_uart(PBX_COMMAND *cmd)
{
	uart_t *uart = &serial_pbx;
	int i;
	int res;

	for(i=0; i<CMD_VALUE_MAX; i++)
	{
		if( cmd->value[i]==0xff)
		{
			break;
		}
	}
		
	do
	{
		res = (uart->write)(uart, (void *)cmd, i+3 );
	}while(res==0);

	as_error_msg("IP-->PBX : sip/cgi send command to serial port :LEN=%d  Op=%x\r\n",res, cmd->op);
	
	return res;
}


/* receive command from PBX through UART */
PBX_COMMAND *as_cmd_from_uart(void )
{
	uart_t *uart = &(serial_pbx);
	char  buf[sizeof(PBX_COMMAND)];

	int iIndex;
	int iLeft;
	int iPrev;
	int res;
	PBX_COMMAND  *cmd =NULL;

#define _BUF_LEN_	1024

	static int iBegin = 0;
	static int iEnd = 0;
	static char bufRead[_BUF_LEN_];
		
	memset(buf, 0, sizeof(PBX_COMMAND));
	
	res = (uart->read)(uart,  buf, sizeof( PBX_COMMAND) );

		
	if( res>0 || iBegin!=iEnd)
	{
#if 0	
		as_error_msg("read content is %:\r\n");
		for(i=0; i<res; i++)
		{
			as_error_msg("0x%x ", buf[i]);
		}
		as_error_msg("\r\n");
#endif
		iLeft = (_BUF_LEN_-1+iBegin-iEnd)%_BUF_LEN_;
#if 0
		printf("before buf content is: \n");
		if(iEnd>iBegin)
		{
			for(i = iBegin; i<iEnd; i++)
			{
				printf("%02x ", bufRead[i]); 
			}
		
		}

		if(iEnd<iBegin)
		{
			for(i = iBegin; i<_BUF_LEN_; i++)
			{
				printf("%02x ", bufRead[i]);
			}
			for(i = 0; i<iEnd; i++)
			{
				printf("%02x ", bufRead[i]);
			}
		}
		printf("\n");

		printf("before ibegin is %d iEnd is %d\n", iBegin, iEnd);
#endif

		if(iLeft<res)
		{
			while(iLeft<res && iBegin!=iEnd)
			{
				iLeft++;
				iBegin = (iBegin+1) % _BUF_LEN_;
			}

			if(iBegin!=iEnd)
			{
				iPrev = (iBegin+_BUF_LEN_-1)%_BUF_LEN_;

				if(bufRead[iPrev] != 0xFF)
				{
					while(bufRead[iBegin]!=0xFF && iBegin!=iEnd)
					{
						iBegin = (iBegin+1)%_BUF_LEN_;
					}

					if(iBegin!=iEnd)
						iBegin = (iBegin+1)%_BUF_LEN_;
				}
			}	
			
		}	
			
		if(iEnd+res<_BUF_LEN_)			
		{
			memcpy(bufRead+iEnd, buf, res);
			iEnd = iEnd+res;
		}
		else/***/			
		{				
			memcpy(bufRead+iEnd, buf, _BUF_LEN_-iEnd-1);				
			memcpy(bufRead, buf, iEnd+1+res-_BUF_LEN_);				
			iEnd = iEnd+res-_BUF_LEN_;			
		}

#if 0
		printf("after ibegin is %d iEnd is %d\n", iBegin, iEnd);

		printf("before buf content is: \n");
		if(iEnd>iBegin)
		{
			for(i = iBegin; i<iEnd; i++)
			{
				printf("%02x ", bufRead[i]); 
			}
		
		}

		if(iEnd<iBegin)
		{
			for(i = iBegin; i<_BUF_LEN_; i++)
			{
				printf("%02x ", bufRead[i]);
			}
			for(i = 0; i<iEnd; i++)
			{
				printf("%02x ", bufRead[i]);
			}
		}
		printf("\n");
		
#endif

		if(iEnd>iBegin)
		{
			iPrev = iBegin;
			for(iIndex = iPrev; iIndex<iEnd; iIndex++)
			{
				if(bufRead[iIndex] == 0xFF)
					break;
			}
			if(iIndex<iEnd)
			{
				if(iIndex+1-iPrev>CMD_VALUE_MAX)
				{
					as_error_msg("line %d too big packet recvied ,throw off it\r\n", __LINE__);
					iBegin = iIndex+1;
					return NULL;
				}
				
				cmd = (PBX_COMMAND *)malloc(sizeof(PBX_COMMAND) );
				assert( cmd);
				
				memcpy(cmd, bufRead+iPrev, iIndex+1-iPrev);
				iBegin = iIndex+1;
				as_error_msg("line %d PBX-->IP : serial port send command to sip/cgi :LEN=%d  Op=0x%x id=0x%x\r\n", __LINE__, iIndex+1-iPrev, cmd->op, cmd->id);

				res = 0;
				while(cmd->value[res]!=0xFF)
				{
					as_error_msg("value %d is 0x%x\r\n", res, cmd->value[res]);
					res++;
				}
				return cmd;
			}
			iBegin = iEnd;
			return NULL;
		}
		else if(iEnd<iBegin)
		{
			iPrev = iBegin;
			for(iIndex = iPrev; iIndex<_BUF_LEN_; iIndex++)
			{
				if(bufRead[iIndex] == 0xFF)
					break;
			}
			if(iIndex<_BUF_LEN_)
			{
				if(iIndex+1-iPrev>CMD_VALUE_MAX)
				{
					as_error_msg("line %d too big packet recvied ,throw off it\r\n", __LINE__);
					iBegin = iIndex+1;
					return NULL;
				}
				
				cmd = (PBX_COMMAND *)malloc(sizeof(PBX_COMMAND) );
				assert( cmd);
									
				memcpy(cmd, bufRead+iPrev, iIndex+1-iPrev);
				iBegin = iIndex+1;

				as_error_msg("line %d PBX-->IP : serial port send command to sip/cgi :LEN=%d  Op=0x%x id=0x%x\r\n", __LINE__, iIndex+1-iPrev, cmd->op, cmd->id);

				res = 0;
				while(cmd->value[res]!=0xFF)
				{
					as_error_msg("value %d is 0x%x\r\n", res, cmd->value[res]);
					res++;
				}
					
				return cmd;
			}
			else
			{
				for(iIndex = 0; iIndex<iEnd; iIndex++)
				{
					if(bufRead[iIndex] == 0xFF)
						break;
				}

				if(iIndex<iEnd)
				{
					if(iIndex+_BUF_LEN_+1-iPrev>CMD_VALUE_MAX)
					{
						iBegin = iIndex+1;
						as_error_msg("line %d too big packet recvied ,throw off it\r\n", __LINE__);
						return NULL;
					}
					
					cmd = (PBX_COMMAND *)malloc(sizeof(PBX_COMMAND) );
					assert( cmd);
					
					memcpy(cmd, bufRead+iPrev, _BUF_LEN_-iPrev);
					memcpy(cmd, bufRead, iIndex+1);

					iBegin = iIndex+1;

					as_error_msg("line %d PBX-->IP : serial port send command to sip/cgi :LEN=%d  Op=0x%x id=0x%x\r\n", __LINE__, iIndex+1-iPrev, cmd->op, cmd->id);

					res = 0;
					while(cmd->value[res]!=0xFF)
					{
						as_error_msg("value %d is 0x%x\r\n", res, cmd->value[res]);
						res++;
					}
					return cmd;
				}					
				iBegin = iEnd;

				return NULL;
			}
				
			return NULL;
		}
	}

	return NULL;
	
}

/* parameter : timeout value, 0 : forever  */
int as_cmd_uart_init(int seconds, int millseconds)
{
	int res;
	uart_t *uart = &serial_pbx; 

	uart->tv_timeout.tv_sec = seconds;
	uart->tv_timeout.tv_usec = millseconds/1000;
	
	PDEBUG("%s\r\n" , uart->name  );
	res = ( uart->open)( uart );
	if( AS_FAIL== res)
	{
		PDEBUG("Error in open PBX serial commu port : %s\r\n", uart->name );
	}	
	return res;
}

void as_cmd_uart_destroy()
{
	uart_t *uart = &serial_pbx;
	(uart->close)(uart);
}


