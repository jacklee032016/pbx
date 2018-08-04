/*
* $Id: cgiStcSetup.c,v 1.7 2007/09/13 18:25:00 lizhijie Exp $
*/

#include "_cgi_stc.h"
#include "cgi_pbx.h"

typedef	int (*pbx_sub_cfg_funtion)(llist *list);

typedef	struct
{
	char					*pbxCfgElement;
	pbx_sub_cfg_funtion	handler;
}pbx_sub_handler;

static int _pbx_global_handle(llist *baseConfig)
{
	node *pnode = baseConfig->head;
	int	isOn = 0;

	while(pnode)
	{
		if(strstr(pnode->entry.name, PBX_GLOBAL_DIFFER_RING ))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_tx_int_value(CMD_CFG_DIFFER_RINGING, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_CALLID_MODE))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_tx_int_value(CMD_CFG_CALLER_ID_MODE, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name, PBX_GLOBAL_NODIAL_ACTION))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_NODIAL_BROKEN );
			pbx_tx_int_value(CMD_CFG_NODIAL_ACTION, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_LINE_2_LINE))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_LINE2LINE_FORBIDDEN);
			pbx_tx_int_value(CMD_CFG_LINE2LINE, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_GROUP_CALL))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_tx_int_value(CMD_CFG_GROUP_CALL, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_SPECIAL_FIRST ))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_tx_int_value(CMD_CFG_SPECIAL_FIRST, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_PSTN_PREFIX))
		{
			pbx_tx_digit_value(CMD_CFG_PSTN_PREFIX, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_SIGNAL_TYPE))
		{
			pbx_tx_int_value(CMD_CFG_SIGNAL_TYPE, atoi(pnode->entry.value), PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_IP_PREFIX ))
		{
			pbx_tx_digit_value(CMD_CFG_IP_PREFIX, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_SYSTEM_PASSWD))
		{
			pbx_tx_digit_value(CMD_CFG_SYS_PASSWD, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_PASSWORD) )
		{
			pbx_tx_digit_value(CMD_CFG_LINE2LINE_PASSWD, pnode->entry.value);
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "Global Configration command '%s' is not found",pnode->entry.name );
		}
		pnode = pnode->next;
	}

	return 0;
}

static int _pbx_ext_basic_handle(llist *extBaseConfig)
{
	node *pnode = extBaseConfig->head;
	unsigned char isOn, index;
	
	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_EXT_B_PHONE_NUMBER))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_index_digit_value(CMD_CFG_EXT_PHONENUMER, index, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_EXT_B_DIALOUT))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_DIALOUT_MODE, index, atoi(pnode->entry.value ) );
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_SCOPE ))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_AUTHORITY, index, atoi(pnode->entry.value) );
		}
		else if(strstr(pnode->entry.name,PBX_EXT_B_LENGTH))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_TIMEOUT, index, atoi(pnode->entry.value) );
		}
		else if(strstr(pnode->entry.name,PBX_EXT_B_CALLID))
		{
			index = get_index(pnode->entry.name);
			isOn = !strcmp(pnode->entry.value, CGI_STR_ON);
			pbx_tx_int_value(CMD_CFG_EXT_CALLER_ID, index, isOn);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_DELAY2LINE))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_DELAY_2_LINE, index, atoi(pnode->entry.value));
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_NODISTURB))
		{
			index = get_index(pnode->entry.name);
			isOn = !strcmp(pnode->entry.value, CGI_STR_ON);
			pbx_tx_int_value(CMD_CFG_EXT_NODISTURB, index, isOn);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_CALLWAIT ))
		{
			index = get_index(pnode->entry.name);
			isOn = !strcmp(pnode->entry.value, CGI_STR_ON);
			pbx_tx_int_value(CMD_CFG_EXT_CALLWAIT, index, isOn);
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "ext basic handler : error key value '%s'",pnode->entry.name );
		}
		
		pnode = pnode->next;
	}

	return 0;
}

unsigned char get_index_by_number(char *phonenumber,llist *l)
{		
	node *curret=l->head;
	
	if(strlen(phonenumber)<=4)
	{
		while(curret!=NULL)
		{
			if( (!strcmp(curret->entry.value, phonenumber)) 
				&&strstr(curret->entry.name, PBX_EXT_B_PHONE_NUMBER) )
			{
				return get_index(curret->entry.name);
				break;
			}	
			curret=curret->next;
		}
	}	

	return PBX_VALUE_INVALIDATE;
}

static int _pbx_ext_special_handle(llist *extBaseConfig)
{
	node *pnode = extBaseConfig->head;
	unsigned char index, value, type = 0;
	char	key[64];

	while(pnode)
	{
		
		if(strstr(pnode->entry.name,PBX_EXT_S_BUSY ))
		{
			index = get_index(pnode->entry.name);
			value = atoi(pnode->entry.value);
			pbx_tx_int_value(CMD_CFG_EXT_BUSY_EXT, index, value);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_SECRET ) )
		{
			index = get_index(pnode->entry.name);
			value = atoi(pnode->entry.value);
			pbx_tx_int_value(CMD_CFG_EXT_SECRECT_EXT, index, value);
		}
#if WITH_EXT_DIFFER_RINGING
		else if(strstr(pnode->entry.name, PBX_EXT_S_DIFFERTING))
		{
			index = get_index(pnode->entry.name);
			isOn = !strcmp(pnode->entry.value, CGI_STR_ON);
			pbx_tx_int_value(CMD_CFG_EXT_DIFFER_RINGING, index, isOn );
		}
#endif
		else if(strstr(pnode->entry.name, PBX_EXT_S_GROUP))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_GROUP, index, atoi(pnode->entry.value));
		}
		else if(strstr(pnode->entry.name,PBX_EXT_S_NORESPONSE))
		{
			index = get_index(pnode->entry.name);
			sprintf(key, "%s_Type_%d", PBX_EXT_S_NORESPONSE, index);
			if(strstr(pnode->entry.name, key) )
			{
				pnode = pnode->next;
				continue;
			}
			type = stc_tranfer_type(GET_VALUE(extBaseConfig, key));
			pbx_tx_index_compressed_digit_value(CMD_CFG_EXT_TRANSFER_LEFT, 
				index, type, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_NIGHT))
		{
			index = get_index(pnode->entry.name);
			sprintf(key, "%s_Type_%d", PBX_EXT_S_NIGHT, index);
			if(strstr(pnode->entry.name, key) )
			{
				pnode = pnode->next;
				continue;
			}
			type = stc_tranfer_type(GET_VALUE(extBaseConfig, key));
			pbx_tx_index_compressed_digit_value(CMD_CFG_EXT_TRANSFER_NIGHT, 
				index, type, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_EXT_S_NOCONDITION ))
		{
			index = get_index(pnode->entry.name);
			sprintf(key, "%s_Type_%d", PBX_EXT_S_NOCONDITION, index);
			if(strstr(pnode->entry.name, key) )
			{
				pnode = pnode->next;
				continue;
			}
			type = stc_tranfer_type(GET_VALUE(extBaseConfig, key));
			pbx_tx_index_compressed_digit_value(CMD_CFG_EXT_TRANSFER_UNCONDITIONAL, 
				index, type, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_NIGHT_TIME) )
		{
			PBX_COMMAND cmd;
			int startHour = 0, startMinute = 0, endHour = 0, endMinute = 0;
			char *p =strrchr(pnode->entry.value, ':' );
			endMinute = atoi(p+1);
			*p = 0;
			
			p =strrchr(pnode->entry.value, ':' );
			endHour = atoi(p+1);
			*p = 0;
			
			p =strrchr(pnode->entry.value, ':' );
			startMinute = atoi(p+1);
			*p = 0;
			
			startHour = atoi(pnode->entry.value);

			cmd.op = CMD_CFG_NIGHT_SERVICE_TIME;
			cmd.value[0] = startHour;
			cmd.value[1] = startMinute;
			cmd.value[2] = endHour;
			cmd.value[3] = endMinute;
			cmd.value[4] = PBX_VALUE_INVALIDATE;

			SEND_OUT_PBX(cmd);
		}
		else if(strstr(pnode->entry.name,PBX_BLACKLIST))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_index_compressed_digit_value(CMD_CFG_EXT_TRANSFER_DENY, 
				index, PBX_VALUE_INVALIDATE, pnode->entry.value);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_NORESTIME))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_EXT_LEFT_TIME, index, atoi(pnode->entry.value));
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "ext special handler : error key value '%s'",pnode->entry.name );
		}
		pnode = pnode->next;
	}

	return 0;
}

static int _pbx_lines_handle(llist *baseConfig)
{
	node *pnode = baseConfig->head;
	unsigned char isOn = 0, index;

	while(pnode)
	{

		if(strstr(pnode->entry.name,PBX_LINE_STATUS))
		{
			index = get_index(pnode->entry.name);
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_tx_int_value(CMD_CFG_PSTN_FLAG, index, isOn);
		}
		if(strstr(pnode->entry.name, PBX_LINE_CONSOLE) )
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_PSTN_CONSOLE, index, atoi(pnode->entry.value));
		}
		if(strstr(pnode->entry.name, PBX_LINE_GROUP))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_PSTN_GROUP, index, atoi(pnode->entry.value));
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "Line Configuration failed");
		}
	
		pnode = pnode->next;
	}

	return 0;
}

static int _pbx_trunck_handle(llist *baseConfig)
{
	node *pnode = baseConfig->head;
	unsigned char	isOn = 0;

	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_TRUNCK_MODE))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_TRUNCK_CONSOLE);
			pbx_tx_int_value(CMD_CFG_TRACK_MODE, isOn, PBX_VALUE_INVALIDATE);
		}
		else if(strstr(pnode->entry.name,PBX_AGENT_PHONE_NUMBER ))
		{
			pbx_tx_digit_value(CMD_CFG_CONSOLE_PHONENUMBER, pnode->entry.value);
		}
		if(strstr(pnode->entry.name,PBX_AGENT_BRANCH_INDEX))
		{
			isOn = atoi(pnode->entry.value);
			pbx_tx_int_value(CMD_CFG_CONSOLE_EXT_INDEX, isOn-1, PBX_VALUE_INVALIDATE);
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "Trunck Configuration Failed");
		}
	
		pnode = pnode->next;
	}

	return 0;
}

static int _pbx_account_handle(llist *baseConfig)
{
	node *pnode = baseConfig->head;
	unsigned char index;
#define	ACCOUNT_AUTH_CANCEL	6

	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_ACCOUNT_PASSWD ))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_index_digit_value(CMD_CFG_ACCOUNT_PASSWD,index, pnode->entry.value);
		}
		if(strstr(pnode->entry.name, PBX_ACCOUNT_AUTH))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_int_value(CMD_CFG_ACCOUNT_AUTHOR, index, atoi(pnode->entry.value) );
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "Account Configuration Failed");
		}
	
		pnode = pnode->next;
	}

	return 0;
}

static int _pbx_special_handle(llist *baseConfig)
{
	node *pnode = baseConfig->head;
	unsigned char index;

	while(pnode)
	{

		if(strstr(pnode->entry.name, PBX_SPECIAL_SERVICE))
		{
			index = get_index(pnode->entry.name);
			pbx_tx_index_digit_value(CMD_CFG_SPECIAL_SERVICE, index, pnode->entry.value);
		}
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "Special Service Configuration Failed");
		}
	
		pnode = pnode->next;
	}

	return 0;
}


static int _pbx_reset_handle(llist *baseConfig)
{
	pbx_tx_int_value(CMD_CFG_IP_PBX_FACTORY_DEFAULT, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE);
//	sleep(2);
//	pbx_send_comcmd(CMD_PC_GET_CONFIG, NULL, NULL); 

	return 0;
}

static pbx_sub_handler	pbx_handlers[] = 
{
	{PBX_CMDSET_RESET, 				_pbx_reset_handle},
	{PBX_CMDSET_BASIC, 				_pbx_global_handle},
	{PBX_CMDSET_BRANCH_BASIC,		_pbx_ext_basic_handle},
	{PBX_CMDSET_BRANCH_BASIC,		_pbx_ext_special_handle },
	{PBX_CMDSET_LINES, 				_pbx_lines_handle},
	{PBX_CMDSET_TRUNCK, 			_pbx_trunck_handle},
	{PBX_CMDSET_ACCOUNT,			_pbx_account_handle},
	{PBX_CMDSET_SPECIAL, 			_pbx_special_handle},
};

int	stc_setup_pbx(llist *cfgs)
{
	char	*op;
	int	i;
	int	res = -1;
	int	isWrite = 0;
	
	op = GET_VALUE(cfgs, PBX_CMDSET);

	for(i=0; i< sizeof(pbx_handlers)/sizeof(pbx_sub_handler); i++)
	{
		if(!strcasecmp(pbx_handlers[i].pbxCfgElement, op) )
		{
			if(!isWrite )
			{
//				pbx_send_comcmd(0x5e, NULL, NULL);
			}
			isWrite = 1;
			AS_MSG_LOG(AS_LOG_INFO, "%s is handled Called CGI", pbx_handlers[i].pbxCfgElement);
			res = (pbx_handlers[i].handler)(cfgs);
		
//			break;
		}
	}
	
	if(isWrite)
	{
		sleep(2);
		pbx_tx_int_value(CMD_CFG_IP_PBX_GET_ALL_CFGS, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE); 
	}

	return res;
}


int	stc_setup_pbx_when_startup(llist *cfgs)
{
	pbx_tx_int_value(CMD_CFG_IP_PBX_GET_ALL_CFGS, PBX_VALUE_INVALIDATE, PBX_VALUE_INVALIDATE); 
	return 0;
}

