/*
* $Id: pbx_serial_thread.c,v 1.15 2007/09/21 21:16:02 lizhijie Exp $
* process command from Serial Port
*/

#include "_pbx.h"

char i2c(int i)
{
	if((i<10)&(i>-1))
	{
		return i+'0';		
	}
	else
	{
		return -1;
	}
}

unsigned short str_16_to_int(char * str_16, int length_str_16)
{
	char * p=NULL;
	int i=length_str_16-1;
	unsigned short sum_10=0;
	unsigned short power=1;
	for(p=&str_16[i]; i>=0; i--)
	{
		sum_10 +=str_16[i]*power;
		power*=16;
	}
	return sum_10;
}


char *getendch(char *str)
{
	//get string's last character;
	for(; *str!='\0';)
		str++;
	return --str;
}

/* used in lookup transfered EXT phone number of EXT special configuration */
int  get_ext_phonenumber_by_index(char *index, llist *l)
{	
	node * curret = l->head;
	char tmp[32];
	int res = -1;
	int length = strlen(index);

	if( length >4)
		return res;
	
	if(2== length )
	{/* remove first '0' when in format of '01' */
		sprintf(tmp, "%s", index);
//		memset(index,0,sizeof(index));
		sprintf(index,"%s", tmp+1);
	}
	
	while(curret!=NULL)
	{
		if( strstr(curret->entry.name, PBX_EXT_B_PHONE_NUMBER )
			&&(!strcmp(getendch(curret->entry.name), index)) )
		{
			cgidebug("Transfer Phone Number of EXT %s has been set as %s\n", index, curret->entry.value);
			sprintf(index, "%s", curret->entry.value);
			res = 0;
			break;
		}
		
		curret=curret->next;
	}
	if(res )
	{/* not found, so clear it */
		sprintf(index, "%s", "");
	}
	return res;
}

int __recv_spec_comcmd(char *dest, char *src, int length, int flag)
{
	int i=0,r=0;

	if(!dest || !src || (length <= 0 ))
		return -1;

	for(r=0; r< length; r++)
	{
		cgidebug(" 0x%x", src[r]);
	}
	cgidebug("\n");
	
	switch(flag)
	{
		case 1:	//str1 had not been compressed with more charactor
			for(i=0;i<length;i++)
			{
				dest[i]= src[i]+'0';
			}
			dest[i]='\0';
			break;
		case 4:	//str1 had not been compressed with more charactor
			for(i=0;i<length;i++)
			{
				src +=1;
				dest[i] = src[i]+'0';
			}
			dest[i]='\0';
			break;
		case 2:	//str1 had been compressed
			for(i=0,r=0;i<length;i++,r++)
			{
				dest[r]= (src[i]>>4)+'0';
				r=r+1;
				dest[r]=(src[i]&0x0f)+'0';
			}                            

			if( 0x0f== (src[length-1]&0x0f) )// if last 4 bit is 0xf, then number is odd
				dest[r-1]='\0';

			dest[r]='\0';
			break;
		case 3:
			for(i=0,r=0;i<length;i++,r++)
			{
				dest[r] = (src[i]>>4);
				r=r+1;
				dest[r] = (src[i]&0x0f);
			}
			dest[r]=0xff;
			break;
		default:
			cgidebug("pbx_recv_spec_comcmd() unrecongnized code\n");
			return -1;
	}

	return 1;
}

int __recv_digit_comcmd(char *dest, char *src, int length )
{
	unsigned short sum=0;
	
	if(!dest || !src )
		return -1;

	if(length==0)	//len=0 value is a charactor
	{
		strcpy(dest, src);
		return 1;
	}
	
	sum=str_16_to_int(src, length);
	sprintf(dest, "%d", sum);

	return 1;
}


int __recv_dialiout_ippstn_prefix_comcmd(char *dest, char *src, int length)
{
	int i;

	if(!dest || !src || (length<= 0) )
		return -1;

	if(length==0)	//len=0 value is a charactor
	{
		strcpy(dest, src);
		return 1;
	}

	for(i=0; i<length; i++)
	{
#if 0	
		if(src[i]== 10)
			dest[i] = '0';
		else

		if(src[i]== 0x0b )
			dest[i] = '*';
		else if(src[i]==0x0c )
			dest[i] = '#';
		else
#endif			
			dest[i] = src[i]+'0';
		
	}
	
	dest[i]='\0';

	if(!strcmp(dest, "1" ))
		strcpy(dest, "11");//PREFIX_IP);
	if(!strcmp(dest,"0"))
		strcpy(dest, "10");//PREFIX_PSTN); 

	return 1;
}

int __recv_wuli_comcmd(entrytype *pentry,unsigned char *name, unsigned char *value, int length)
{
	int i;
	
	if(!(pentry&&name))
		return -1;
	strcpy(pentry->name,name);
	if(length==0)	//len=0 value is a charactor
	{
		strcpy(pentry->value, value);
		return 1;
	}

	//pbuf=pentry->value;
	for(i=0; i<length; i++)
	{
		value[i]=value[i]+'0';
	}

	value[i]='\0';
	strcpy(pentry->value,value);
	return 1;
}

int __recv_comcmd(char *dest, char *src, int length)
{
	int i;
	
	if(!dest || !src || (length <= 0) )
		return -1;
#if 0
	if(length==0)	//len=0 value is a charactor
	{
		strcpy(pentry->value, value);
		cgidebug("name: %s\n", name);
		return 1;
	}
#endif

	for(i=0; i<length; i++)
	{
		dest[i]= src[i]+'0';
	}
	dest[i]='\0';

	return 1;
}


/* read serial command into this list */
pbx_cfg_t *__pbx_read_from_pbx(void)
{
	PBX_COMMAND *cmdbuf;
	pbx_cfg_t *cfg = NULL;

	unsigned char *value;
	int len;	//save the lenght of receive value;
	int i;
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

	cfg = pbx_find_cfg_item(cmdbuf);
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

	if( (cfg->type & PBX_CFG_TYPE_RELOAD_KEY) )
	{
		sprintf(cfg->indexKey, "%s_%c", cfg->configKey, i2c(cmdbuf->value[0]) );
		len--;
		value++;
	}
		
	if((cfg->type & PBX_CFG_TYPE_RELOAD_VALUE) )
	{

		if(len == 0 )
		{
			AS_MSG_LOG(AS_LOG_WARNING, "%s(): recv cmd: op 0x%x key %s, len is 0", __FUNCTION__, cmdbuf->op, cfg->configKey);
			return NULL;
		}

		memset(cfg->configValue, 0 , CMD_VALUE_MAX);
		if((cfg->type & PBX_CFG_TYPE_RELOAD_VALUE_DIGIT))
		{
			__recv_digit_comcmd(cfg->configValue, value, len);
		}
		else if((cfg->type & PBX_CFG_TYPE_RELOAD_VALUE_IP_PREFIX))
		{
			__recv_dialiout_ippstn_prefix_comcmd(cfg->configValue, value, len);
		}
		else if((cfg->type & PBX_CFG_TYPE_RELOAD_VALUE_COMPRESSED))
		{
			__recv_spec_comcmd(cfg->configValue, value, len, 2/* flags, eg. format*/);
		}
		else if((cfg->type & PBX_CFG_TYPE_RELOAD_VALUE_INTEGER))
		{
			sprintf(cfg->configValue, "%c", value[0] );
		}
		else
		{
			__recv_comcmd(cfg->configValue, value, len);
		}

		/* more process */
		if((cfg->type & PBX_CFG_TYPE_TRANSFER_EXT) )
		{
			get_ext_phonenumber_by_index(cfg->configValue, &pbxentries);
		}
	}
	
	AS_MSG_LOG(AS_LOG_INFO, "recv cmd: op 0x%x value(%s=%s)", cmdbuf->op, 
		((cfg->type&PBX_CFG_TYPE_RELOAD_KEY))?cfg->indexKey:cfg->configKey, cfg->configValue );

	/* must free it, lizhijie, 2007.03.20 */
	free(cmdbuf);
	
	return cfg;
}

void __pbx_handler_comsig(void )
{
	pbx_cfg_t *cfg;

	cfg = __pbx_read_from_pbx();
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
			if(list_replace_value_by_key(&pbxentries, ((cfg->type&PBX_CFG_TYPE_RELOAD_KEY))?cfg->indexKey:cfg->configKey, cfg->configValue) )
			{
				if(writeconfig(FILE_PBX_CONF, ITEM_DBL, NULL, &pbxentries))
					return;
			}
			else
				AS_MSG_LOG(AS_LOG_WARNING, "PBX Configuration %s is not found", cfg->configKey);
		}

		cfg = __pbx_read_from_pbx();
	}
	
	return;
}

void  read_serial_cmd_thread(void )
{	
	while(1)
	{
		__pbx_handler_comsig();
#if WITH_MSG_QUEUE_NOWAIT		
		sleep(2);
#endif
	}	
}

