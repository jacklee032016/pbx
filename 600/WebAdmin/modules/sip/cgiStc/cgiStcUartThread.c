/*
* $Id: cgiStcUartThread.c,v 1.10 2007/09/14 09:11:10 lizhijie Exp $
* process command from Serial Port
*/

#include "_cgi_stc.h"
#include "cgi_pbx.h"

int	__recv_int_value(pbx_cfg_t *cfg, unsigned char value)
{
	if((cfg->type&STC_CFG_TYPE_BOOL) )
	{
		if(value==0)
			sprintf(cfg->configValue, "%s", cfg->offValue);
		else
			sprintf(cfg->configValue, "%s", cfg->onValue);
	}
	else if( (cfg->type & STC_CFG_TYPE_KEY) )
	{
		sprintf(cfg->indexKey, "%s_%d", cfg->configKey, value );
	}
	else if( (cfg->type & STC_CFG_TYPE_INTEGER) )
	{
		sprintf(cfg->configValue, "%d", value);
	}
	else
	{
		cfg->configValue[0] = value;
	}
	
	return 0;
}

/* return real length of digits */
int __recv_compressed_value(char *dest, char *src, int length)
{
	int i=0,r=0;

	if(!dest || !src || (length <= 0 ))
		return 0;
	
	for(i=0,r=0;i<length;i++,r++)
	{
		dest[r]= (src[i]>>4);
		r=r+1;
		if((src[i]&0x0F)!=0x0F)
			dest[r]= src[i]&0x0F;
		else
		{
			dest[r] = 0;
			return 2*length-1;
		}
	}                            

	return length*2;
}


int	__recv_2_int_value(pbx_cfg_t *cfg, unsigned char value1, unsigned char value2)
{
	if( (cfg->type & STC_CFG_TYPE_KEY) )
	{
		sprintf(cfg->indexKey, "%s_%d", cfg->configKey, value1);
	}

	if((cfg->type&STC_CFG_TYPE_BOOL) )
	{
		if(value2==0)
			sprintf(cfg->configValue, "%s", cfg->offValue);
		else
			sprintf(cfg->configValue, "%s", cfg->onValue);
	}
	else if((cfg->type&STC_CFG_TYPE_VALUE_DIGIT) )
	{
		 if((cfg->type&STC_CFG_TYPE_KEY) )
	 	{
			cfg->configValue[0] = value2;
			return 1;
	 	}
		 else
		 {
//		sprintf(cfg->configValue, "%d%d",value1, value2);
			cfg->configValue[0] = value1;
			cfg->configValue[1] = value2;
			return 2;
		 }
	}
	else
	{
		sprintf(cfg->configValue, "%d", value2);
	}
	
	return 1;
}


/* read serial command into this list */
pbx_cfg_t *__stc_read_from_pbx(void)
{
	PBX_COMMAND *cmdbuf;
	pbx_cfg_t *cfg = NULL;

	unsigned char *value;
	int len;	//save the lenght of receive value;
	int i;
	int valueLength = 0;
	static int	count = 0;

	/* this function is implemented as blocked, lizhijie,2007.03.20 */
	AS_MSG_DEBUG(AS_LOG_INFO, "read from UARTD message queue.....");
	cmdbuf = as_cmd_cgi_from_uart();

	if(NULL==cmdbuf)
	{
		AS_MSG_DEBUG(AS_LOG_WARNING, "read from UART message queue Failed");
		return NULL;
	}
	AS_MSG_LOG(AS_LOG_INFO, "read %d MSGs with op 0x%x",++count,  cmdbuf->op );

	cfg = stc_find_cfg_item(cmdbuf);
	if(!cfg)
	{
		AS_MSG_LOG(AS_LOG_WARNING, "CGI Cmd(from PBX) Op '0x%x' is not found", cmdbuf->op);
		return NULL;
	}

	for(i=0; cmdbuf->value[i]!=0xff && i < CMD_VALUE_MAX; i++)
	{
//			cgidebug("%02x",cmdbuf->value[i]);
	}
	len=i;
	value = cmdbuf->value;
	AS_MSG_LOG(AS_LOG_INFO, "%s length is %d", cfg->configKey, len);

	memset(cfg->configValue, 0 , CMD_VALUE_MAX*3);
	switch(len)
	{
		case 0 :
			AS_MSG_LOG(AS_LOG_INFO,"%s is not stored", cfg->configValue);
			if(cmdbuf->op == CMD_CFG_PBX_IP_BEGIN )
				count = 1;
			if(cmdbuf->op == CMD_CFG_PBX_IP_END )
			{
				AS_MSG_LOG(AS_LOG_WARNING, "Total %d Config Items have been received", count);
			}
			break;
		case 1:
			__recv_int_value(cfg, cmdbuf->value[0]);
			if(cfg->type & STC_CFG_TYPE_KEY )
				valueLength = 0;
			else
				valueLength  = 1;
			break;
		case 2:
			if((cfg->type & STC_CFG_TYPE_VALUE_TRANSFER) == 0 && (cfg->type & STC_CFG_TYPE_VALUE_COMPRESSED)==0 )
			{
				valueLength = __recv_2_int_value(cfg, cmdbuf->value[0], cmdbuf->value[1]);
				break;
			}
		default:
			if( (cfg->type & STC_CFG_TYPE_KEY) )
			{
				sprintf(cfg->indexKey, "%s_%d", cfg->configKey, cmdbuf->value[0] );
				len--;
				value++;
			}
				
#if 0
			if(len < 2 )
			{
				AS_MSG_LOG(AS_LOG_WARNING, "%s(): recv cmd: op 0x%x key %s, len is %d", __FUNCTION__, cmdbuf->op, cfg->configKey, len);
	//			return NULL;
			}
#endif

			if((cfg->type & STC_CFG_TYPE_VALUE))
			{
				memcpy(cfg->configValue, value, len);
				valueLength = len  ;
				if((cfg->type & STC_CFG_TYPE_VALUE_TIME))
				{
					int pos = 0;
					for(i=0; i< len; i++)
					{
						if(i!= 0 )
							pos += CGI_SPRINTF(cfg->configValue, pos, ":");
						pos += CGI_SPRINTF(cfg->configValue, pos, "%d",value[i]);
					}
				}

			}
			else if((cfg->type & STC_CFG_TYPE_VALUE_TRANSFER))
			{
				cfg->tranferType = *value;
				value++;
				len--;
				valueLength = __recv_compressed_value(cfg->configValue, value, len );
			}
			else if((cfg->type & STC_CFG_TYPE_VALUE_COMPRESSED))
			{
				valueLength = __recv_compressed_value(cfg->configValue, value, len );
			}

			break;
	}
	

	if((cfg->type&STC_CFG_TYPE_VALUE_DIGIT) )
	{
		for(i=0;i<valueLength; i++)
		{
			if(cfg->configValue[i] == DIAL_DIGIT_ZERO)
				cfg->configValue[i] = '0';
			else if(cfg->configValue[i] == DIAL_DIGIT_ASTREIC)
				cfg->configValue[i] = '*';
			else if(cfg->configValue[i] == DIAL_DIGIT_SPECIAL)
				cfg->configValue[i] = '#';
			else
				cfg->configValue[i] += '0';
		}
	}

	if((cfg->type & STC_CFG_TYPE_VALUE_TRANSFER))
	{
		AS_MSG_LOG(AS_LOG_NOTICE, "value=%s", cfg->configValue );
		AS_MSG_LOG(AS_LOG_NOTICE, "type=%s", stc_tranfer_type_name(cfg->tranferType) );
		sprintf(cfg->configValue, "%s_%s",cfg->configValue, stc_tranfer_type_name(cfg->tranferType) );
	}

	AS_MSG_LOG(AS_LOG_NOTICE, "recv cmd: op 0x%x value(%s=%s)", cmdbuf->op, 
		((cfg->type&STC_CFG_TYPE_KEY))?cfg->indexKey:cfg->configKey, cfg->configValue );

	/* must free it, lizhijie, 2007.03.20 */
	free(cmdbuf);
	
	return cfg;
}

static void _stc_handler_uart_sig(void )
{
	pbx_cfg_t *cfg;

	cfg = __stc_read_from_pbx();
	while(cfg)
	{
#if 0	
		pid_t pid;
		if(action==1) //do with black_list
		{					
			if(writeconfig(FILE_BLCK_LIST, ITEM_BLCK_LIST, NULL, &sipblck_list))
				return;
#if 1
			/* send command to PBX */
			pid=(pid_t)cgi_get_pid_by_name(PROC_SIP);
			if(pid<0)
			{
				printf("'%s' process is not running\n", PROC_SIP );
				return;
			}
 			cgi_sig_send(pid, SIG_BLACKLIST, 0);
#endif
		}	
		else if(action==0)
#endif			
		{
			if(list_replace_value_by_key(&pbxentries, ((cfg->type&STC_CFG_TYPE_KEY))?cfg->indexKey:cfg->configKey, cfg->configValue) )
			{
				if(writeconfig(FILE_PBX_CONF, ITEM_DBL, NULL, &pbxentries))
					return;
			}
			else
				AS_MSG_LOG(AS_LOG_WARNING, "PBX Configuration %s is not found", cfg->configKey);
		}

		cfg = __stc_read_from_pbx();
	}
	
	return;
}

void  stc_read_serial_cmd_thread(void )
{	
	while(1)
	{
		_stc_handler_uart_sig();
#if WITH_MSG_QUEUE_NOWAIT		
		sleep(2);
#endif
	}	
}

