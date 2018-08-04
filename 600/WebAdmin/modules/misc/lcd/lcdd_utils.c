/*
* $Id: lcdd_utils.c,v 1.12 2007/09/27 17:57:03 lizhijie Exp $
*/

#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/ioctl.h>

#include "cgi_services.h"
#include "cgi_lib.h"
#include "cgi_syscfg.h"

#include "lcdd.h"

#define	LCD_IP_LENGTH			13

lcd_item_t lcd_items[] =
{
	{
		LCD_SERVICE_VERSION,
		LCD_SERVICE_STATE_NULL,	
			
		NULL,
		NULL,
		""
	},
	
	{
		LCD_SERVICE_WEB,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_WWWSERVER,
		"/sbin/www.sh",
		LCD_DEFAULT_NAME_WEB
	},
	{
		LCD_SERVICE_FILE,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_FILESERVER,
		"/sbin/smbd",
		LCD_DEFAULT_NAME_FILE,
	},
	{
		LCD_SERVICE_MAIL,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_MAILSERVER,
		"/sbin/smtpd.sh",
		LCD_DEFAULT_NAME_MAIL
	},
	{
		LCD_SERVICE_PRINT,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_PRINTSERVER,
		"/usr/sbin/cupsd",
		LCD_DEFAULT_NAME_PRINTER
	},
#if 0
	{
		LCD_SERVICE_DHCP,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_DHCPSERVER,
		"/usr/sbin/udhcpd",
		LCD_DEFAULT_NAME_DHCP
	},
#else
	{
		LCD_SERVICE_DHCP,
		LCD_SERVICE_STATE_DISABLE,	
		
		LCD_CMD_NAME_CAMERA,
		"/sbin/camera.sh",
		LCD_DEFAULT_NAME_CAMERA
	},
#endif	
	{
		LCD_SERVICE_LAN,
		LCD_SERVICE_STATE_DISABLE,	
		
		NULL,
		NULL,
		LCD_DEFAULT_NAME_LAN
	},
	{
		LCD_SERVICE_WAN,
		LCD_SERVICE_STATE_DISABLE,	
		
		NULL,
		NULL,
		LCD_DEFAULT_NAME_WAN
	},
	{
		LCD_SERVICE_NET_MODE,
		LCD_SERVICE_STATE_DISABLE,	
		
		NULL,
		NULL,
		LCD_DEFAULT_NAME_PROTOCOL
	},
	{
		LCD_SERVICE_TIME,
		LCD_SERVICE_STATE_NULL,	
			
		NULL,
		NULL,
		""
	},
	
	{
		-1,
		LCD_SERVICE_STATE_NULL,	
			
		NULL,
		NULL,
		""
	}
};

static lcd_item_t *__find_item(LCD_SERVICE_TYPE_T serverType)
{
	lcd_item_t	*item = lcd_items;
	while(item->type != -1 )
	{
		if(item->type == serverType)
			return item;
		
		item++;
	}

	return NULL;
}

char *_data_processing(char *data)
{
	char srcIpAddress[17];
	char *destIpAddress;
	int i=0,j=0,destInt=0;
	int length = 0;
	if(data == NULL)
		return NULL;

	destIpAddress = malloc(LCD_IP_LENGTH);
	if(destIpAddress== NULL)
		return NULL;
	memset(destIpAddress, '0' , LCD_IP_LENGTH-1);
	destIpAddress[LCD_IP_LENGTH-1] = 0;
	
	memset(&srcIpAddress[0],	0,	sizeof(srcIpAddress) );
	strncpy(srcIpAddress,	data, 	(sizeof(srcIpAddress)>strlen(data))?strlen(data):sizeof(srcIpAddress) );
	srcIpAddress[strlen(data)] = '.';

	AS_MSG_DEBUG(AS_LOG_ERR, "IP Address Before process : '%s', '%s'", data, srcIpAddress );
	while(1)
	{
		if(srcIpAddress[i] == '\0')
			break;
		
		if(srcIpAddress[i] == '.')
		{
#if 0		
			for(j=0;j<3;j++)
			{
				if((i-j-1) <0)
					goto wto1;
				
				if(srcIpAddress[i-j-1] != '.')
				{
					destIpAddress[destInt+2-j] = srcIpAddress[i-j-1];
				}
				else
					goto wto1;
			}
wto1:	
#else
			length = i-j;
			destIpAddress[destInt+2] = srcIpAddress[i-1];
			if( length == 1)
			{
			}
			else if(length == 2)
			{
				destIpAddress[destInt+1] = srcIpAddress[i-2];
			}
			else
			{
				destIpAddress[destInt+1] = srcIpAddress[i-2];
				destIpAddress[destInt] = srcIpAddress[i-3];
			}
#endif
			i++;
			j = i;
			destInt = destInt+3;

			continue;
		}

		i++;

		if((destInt >= LCD_IP_LENGTH ) ||(i >= sizeof(srcIpAddress)) )
			break;
	}
	
	AS_MSG_DEBUG(AS_LOG_ERR, "IP Address after process : '%s'", destIpAddress);
	return destIpAddress;
}

LCD_NET_MODE_T _ip_assign(lcd_item_t *item)
{
	llist	wanCfgs;
	LCD_NET_MODE_T result = LCD_NET_MODE_PPPOE;

	list_create( &wanCfgs);
	
	if(readconfig(ETH0_CONFIG_FILE, ITEM_EQU, NULL, &wanCfgs))
	{
		AS_MSG_LOG(AS_LOG_ERR, "Without WAN configuration file" );
		sprintf(item->name+ 9,  " (%s)", NIC_KEYWORD_PROTOCOL_STATIC);
		result =  LCD_NET_MODE_STATIC;
	}
	else
	{
		sprintf(item->name+ 9, " (%s)", GET_VALUE(&wanCfgs, NIC_KEYWORD_PROTOCOL ) );
		if(!strcasecmp(item->name, NIC_KEYWORD_PROTOCOL_DHCP) )
		{
			result = LCD_NET_MODE_DHCP;
		}
	}

	list_clear( &wanCfgs);
	return result;
}


static int __str_leftzero(char *dest, char *src)
{
 	int i=0,j=0;

	for(i=0; i<3;i++)
 	{
 	 	if(*(src+i) != '0' )
			break;
 	}
	
	for(j=i; j<3; j++)
	{
		dest[j-i] = src[j];
	}

	return 0;
}


static void __as_lcd_ip_addr_deal(char *destIP,char *srcIp)
{
	char tmp1[4];
	char tmp2[4];
	char tmp3[4];
	char tmp4[4];
	
	memset(tmp1, 0, 4);
	memset(tmp2, 0, 4);
	memset(tmp3, 0, 4);
	memset(tmp4, 0, 4);

	__str_leftzero(tmp1, srcIp);
	__str_leftzero(tmp2, srcIp+3);
	__str_leftzero(tmp3, srcIp+6);
	__str_leftzero(tmp4, srcIp+9);

	sprintf(destIP,"%s.%s.%s.%s",tmp1,tmp2,tmp3,tmp4);

	AS_MSG_LOG(AS_LOG_DEBUG, "IP Address : %s\n", destIP );

	return ;
}

static void __as_lcd_fix_com_deal(	lcd_item_t  *item, char *value)
{
	int pid;
	char		cmd[256];

#if 1//LCDD_DEBUG
	unsigned char fixValue = value[0];
	AS_MSG_LOG(AS_LOG_DEBUG, "LCD Command : %s(0x%x)\n", ((int)fixValue==1)?"Start":"Stop", (int)fixValue);
#endif
	pid = cgi_get_program_pid_with_param(item->command, NULL);
	if(pid > 0)
	{
		sprintf(cmd, "kill -9 %d > /dev/null ", pid);
		AS_MSG_LOG(AS_LOG_DEBUG, "Stopping %s : %s", item->command, cmd);
		system(cmd);
		sleep(2);
	}
	else
	{
		sprintf(cmd, "%s > /dev/null ", item->startScript );
		AS_MSG_LOG(AS_LOG_DEBUG, "Starting %s : %s", item->command, cmd );
		fork_cmd(cmd );
		sleep(2);
	}

	return ;
}

int __get_lcd_item_status(lcd_item_t *item)
{
	network_info	*netInfo;
	int		result = 0;
	int pid = 0;
	LCD_NET_MODE_T netMode;
	char		*ip;
	
	switch( item->type)
	{
		case LCD_SERVICE_WAN:
			netInfo = cgi_get_network_info( 0);
			if(netInfo== NULL)
			{
				break;
			}
			ip = _data_processing(netInfo->wanInfo.addr);
			if((ip!= NULL) &&( strcasecmp(ip, item->name) ) )
			{
				memcpy(item->name, ip, strlen(ip));
				result = 1;
			}
			if(ip)
				free(ip);
			
			free(netInfo);
			break;
			
		case LCD_SERVICE_LAN:
			netInfo = cgi_get_network_info( 0);
			if(netInfo== NULL)
			{
				break;
			}
			ip = _data_processing(netInfo->lanInfo.addr);
			if((ip!= NULL)&&( strcasecmp(netInfo->lanInfo.addr, item->name) ) )
			{
				memcpy(item->name, ip, strlen(ip));
				result = 1;
			}
			if(ip)
				free(ip);
			
			free(netInfo);
			break;
			
		case LCD_SERVICE_NET_MODE:/* protocol type */
			netMode = _ip_assign(item);
			if(netMode!= item->state)
			{
				item->state = netMode;
				result = 1;
			}
			break;
			
		case LCD_SERVICE_TIME:
		{
			current_time_value_t *value = systime_get_current_time();
			if(value!= NULL)
			{
				sprintf(item->name, "%s-%s-%s %s", value->year, value->month, value->day, value->current_time);
				free(value);
			}
			break;
		}	
		case LCD_SERVICE_VERSION:
			break;
		default:
			pid = cgi_get_program_pid_with_param(item->command, NULL);
			if(pid > 0 )
			{
				if(item->state !=  LCD_SERVICE_STATE_ENABLE)
				{
					item->state = LCD_SERVICE_STATE_ENABLE;
					result = 1;
				}
			}
			else
			{
				if(item->state !=  LCD_SERVICE_STATE_DISABLE)
				{
					item->state = LCD_SERVICE_STATE_DISABLE;
					result = 1;
				}
			}
			break;
	}

	return result;
}

void  __update_lcd_item(lcdd_t *lcdd, lcd_item_t  *item, LCD_UART_REQUEST *request)
{
	char dealCom[256];

	memset(dealCom, 0, sizeof(dealCom) );
	if(request->op == LCD_UART_CMD_STRING)
	{
		if( (request->service == LCD_SERVICE_LAN)||(request->service == LCD_SERVICE_WAN))
		{
			char  *filename, *dev;
			char destIP[16];
			llist	nicCfgs;
			
			if(request->service == LCD_SERVICE_WAN)
			{
				filename = ETH0_CONFIG_FILE;
				dev = ETH0;
			}
			else
			{
				filename = ETH1_CONFIG_FILE;
				dev = ETH1;
			}
			

			if(strcasecmp(request->loadData, item->name) )
			{
				AS_MSG_DEBUG(AS_LOG_DEBUG, "com data : %s; item->name", request->loadData, item->name );
				__as_lcd_ip_addr_deal(destIP, request->loadData );
				list_create(&nicCfgs);
				readconfig(filename, ITEM_EQU, NULL, &nicCfgs);
				if( !strcasecmp(GET_VALUE(&nicCfgs, NIC_KEYWORD_PROTOCOL), NIC_KEYWORD_PROTOCOL_STATIC) )
				{/* only when protocol is static, IP Address can be set */
					sprintf(dealCom, "ifconfig %s %s >> /dev/null  2>&1 ", dev, destIP );
					system(dealCom);
				}
				AS_MSG_LOG(AS_LOG_DEBUG, "New IP Address is %s", destIP);
				list_replace_value_by_key(&nicCfgs, NIC_KEYWORD_IP_ADDRESS, destIP);
				
				cgi_nic_refresh_net_and_broad_address(&nicCfgs);
				writeconfig(filename, ITEM_EQU, NULL, &nicCfgs );
				list_clear( &nicCfgs);

				lcdd_write_reboot_cmd(lcdd, item);
			}

		}
	}
	else if(request->op == LCD_UART_CMD_CONFIG )
	{
		int server =request->service;
		if( server >= LCD_SERVICE_WEB && server <= LCD_SERVICE_DHCP )
		{
			__as_lcd_fix_com_deal(item, request->loadData );
		}
		else if(request->service == LCD_SERVICE_NET_MODE )
		{
			unsigned char protocol = request->state;

#if LCDD_DEBUG
			AS_MSG_LOG(AS_LOG_NOTICE, "PROTOCOL : %d item->state : %d\n", protocol, item->state );
#endif

			if(protocol!= item->state)
			{
				char	*mode;
				if(protocol== LCD_NET_MODE_DHCP )
					mode = NIC_KEYWORD_PROTOCOL_DHCP;
				else if(protocol == LCD_NET_MODE_PPPOE )
					mode = NIC_KEYWORD_PROTOCOL_PPPOE;
				else
					mode = NIC_KEYWORD_PROTOCOL_STATIC;
				
				config_file_replace_field(ETH0_CONFIG_FILE, ITEM_EQU, NIC_KEYWORD_PROTOCOL, mode);
				lcdd_write_reboot_cmd(lcdd, item);
			}
		}
		else
		{/* no command for IP Protocol and System Time */
		}
	}

}


/* process command from LCD */
void *lcdd_read_thread(void *data)
{
	lcdd_t *lcdd = (lcdd_t *)data;
	lcd_item_t  *item;
	LCD_UART_REQUEST	*request = NULL;
	
	AS_MSG_DEBUG(AS_LOG_ERR, "LCD UART READ Thread");
	while(1)
	{
		AS_MSG_DEBUG(AS_LOG_ERR, "Reading LCD UART msg.....");
		request = lcdd_read_command(lcdd);
		if(request != NULL)
		{
			item = __find_item( request->service );
			if( item != NULL)
			{
				if(request->service == LCD_SERVICE_VERSION)
				{
					lcdd_write_ver_info(lcdd);
				}
				else
				{
					lcdd->currentService = request->service;

					AS_MSG_LOG(AS_LOG_DEBUG, "service : %d %s", item->type, item->name );

					if(request->op == LCD_UART_CMD_CONFIG || request->op == LCD_UART_CMD_STRING)
						__update_lcd_item(lcdd, item, request);

					__get_lcd_item_status(item);

					lcdd_write_msg(lcdd,  item );

				}
			}
			else
			{
				AS_MSG_LOG(AS_LOG_WARNING, "'%x' is not validate item which can be configed by LCD", request->service );
			}
		}
	};

	
//	return;
}


void *lcdd_monitor_thread(void *data)
{
	lcdd_t *lcdd = (lcdd_t *)data;

	AS_MSG_DEBUG(AS_LOG_DEBUG, "LCD Task Monitor Thread");
	while(1)
	{
		LCD_SERVICE_TYPE_T		type = LCD_SERVICE_WEB;;
		lcd_item_t	*item = __find_item(type );
		do
		{

			if(!item || item->type != -1 )
			{
				AS_MSG_DEBUG(AS_LOG_DEBUG, "LCD Task Monitor Thread : get status of %s", item->name );
				if(__get_lcd_item_status( item))
				{
					lcdd_write_msg(lcdd,  item );
				}
			}

			type++;
			item = __find_item(type );

		}while(item);
		
		sleep(10);
	};

//	return;
}

void initPrintLcd(lcdd_t *lcdd)
{
	lcd_item_t	*item = lcd_items;
	
	while(item->type != -1 )
	{
		__get_lcd_item_status(item);
		lcdd_write_msg(lcdd, item );
//		sleep(1);
		usleep(500*1000);
		item++;
	}

	return ;
}


