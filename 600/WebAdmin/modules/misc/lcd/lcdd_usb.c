/*
* $Id: lcdd_usb.c,v 1.9 2007/09/29 07:59:25 lizhijie Exp $
*/

#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <stdio.h> 
#include <string.h> 

#include "cgi_services.h"
#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "lcdd.h"

#define	WITH_SHARED_LIB		1

static int lcdSeqence = 0;

#if		LCDD_DEBUG
static void _debug_frame(char *title, unsigned char *buf, int len)
{
	int	i;
	
	AS_MSG_LOG(AS_LOG_ERR, "%s dump : Length is %d" , title, len);
	if (len > 0)
	{
		for (i = 0; i < len; i++)
		{
//			if ((i & 1) == 0)
//				AS_MSG_LOG(AS_LOG_ERR, " ");
			AS_MSG_LOG(AS_LOG_ERR, "%02x", buf[i]);
		}
		AS_MSG_LOG(AS_LOG_ERR, "");
	}
}
#else
#define	_debug_frame(title,buf, len)		
#endif


LCD_UART_REQUEST *lcdd_read_command(lcdd_t *lcdd)
{
	int	pos;
	int	length;
	int	i;
	int	found = 0;

	pos = (lcdd->uart->read)( lcdd->uart,  lcdd->buf + lcdd->inIndex, (sizeof(lcdd->buf)-lcdd->inIndex) );
//	pos = (lcdd->uart->read)( lcdd->uart, (void *)&request, sizeof(LCD_UART_REQUEST));
	if(pos<= 0)
	{
		return NULL;
	}
	
	_debug_frame("UART RX",(unsigned char *)lcdd->buf + lcdd->inIndex, pos);
	
	lcdd->inIndex += pos;
	if(lcdd->outIndex> sizeof(lcdd->buf) )
	{
		lcdd->outIndex = lcdd->inIndex = 0;
	}

	_debug_frame("LOOKUP",(unsigned char *)lcdd->buf + lcdd->outIndex, lcdd->inIndex-lcdd->outIndex);
	for( i= lcdd->outIndex; i< lcdd->inIndex; i++)
	{
		if(lcdd->buf[i] == LCD_UART_CMD_END )
		{
			found = 1;

			length = i -lcdd->outIndex +1;
			memcpy((void *)&lcdd->request, lcdd->buf + lcdd->outIndex, length );

			if(i==lcdd->inIndex)
			{
				lcdd->inIndex = lcdd->outIndex;
			}
			else
			{
				memcpy(lcdd->buf+lcdd->outIndex, lcdd->buf+i+1, lcdd->inIndex-i );
				lcdd->inIndex = lcdd->inIndex - length ;
			}

			break;
		}
	}
	
	if(found == 0)
	{
		return NULL;
	}
	
	_debug_frame("READ LCD Request is ",(unsigned char *)&lcdd->request, length );

	return &lcdd->request;

}

int lcdd_write_msg(lcdd_t *lcdd, lcd_item_t *item )
{
	LCD_UART_RESPONSE response;
	int length = strlen(item->name);

	response.op = LCD_UART_CMD_STRING;
	response.service = item->type;
	response.state = item->state;
	response.drawMode = 0;
	response.sequence = lcdSeqence++;

	if(lcdSeqence == LCD_UART_CMD_END )
		lcdSeqence = 0;

	if(length>=LCD_UART_RESPONSE_DATA_LENGTH)
		length = LCD_UART_RESPONSE_DATA_LENGTH-1;
	
	if(length>0)
		memcpy(response.loadData, item->name, length);
	response.loadData[length] = LCD_UART_CMD_END;

	response.length = length;

	_debug_frame("SEND OUT : ",(unsigned char *)&response, 6+length+1 );

	return (lcdd->uart->write)(lcdd->uart, (void *)&response, 6+ length+1 );
}


int lcdd_write_reboot_cmd(lcdd_t *lcdd, lcd_item_t *item )
{
	sprintf(item->name, "   IP    Rebooting" );
//	if(item->type != LCD_SERVICE_NET_MODE )
//		item->type = LCD_SERVICE_NET_MODE;
	item->type = LCD_SERVICE_WEB;
	
	lcdd_write_msg(lcdd, item);

	AS_MSG_LOG(AS_LOG_ERR, "rebooting.....");
	sleep(2);
	system("reboot");

	return 0;
}

int lcdd_get_ver_info(lcdd_t *lcdd )
{
	LCD_UART_RESPONSE 	response;
	int res;

	response.op = LCD_UART_CMD_GET_VER_INFO;
	response.service = LCD_SERVICE_VERSION;
	response.state = LCD_SERVICE_STATE_NULL;
	response.drawMode = 0;
	response.sequence = lcdSeqence++;
	if(lcdSeqence == LCD_UART_CMD_END )
		lcdSeqence = 0;

	response.loadData[0] = LCD_UART_CMD_END;

	response.length = 0;

	_debug_frame("SEND OUT",(unsigned char *)&response, 6+1 );

	res = (lcdd->uart->write)(lcdd->uart, (void *)&response, 6+ 1 );
	if(res < 0 )
	{
		AS_MSG_LOG(AS_LOG_ERR, "Error in communicate with Serial Port : %s", lcdd->uart->name );
		return -1;
	}

	return 0;
}

void lcdd_write_ver_info(lcdd_t *lcdd)
{
	LCD_UART_REQUEST 	*verInfo;
#if 0	
	lcd_ver_t				*ver;
#else
	char					*ver;
#endif

	verInfo = &lcdd->request;
	if(verInfo==NULL)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Error in communicate with Serial Port : %s", lcdd->uart->name );
		return ;
	}

#if 0
	ver = (lcd_ver_t *)verInfo->loadData;
	AS_MSG_LOG(AS_LOG_WARNING, "LCD Version : %c%c.%c%c %s", 
		ver->major1, ver->major2, ver->minor1, ver->minor2, ver->dateBuild );
#else
	ver = (char *)verInfo->loadData;
	AS_MSG_LOG(AS_LOG_WARNING, "LCD Version : %s", ver );
#endif

	return ;
}

