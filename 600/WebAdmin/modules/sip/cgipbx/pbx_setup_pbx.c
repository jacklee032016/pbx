/*
* $Id: pbx_setup_pbx.c,v 1.12 2007/09/21 21:16:02 lizhijie Exp $
*/
#include "_pbx.h"
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
			pbx_send_comcmd((isOn)?CMD_PC_TONE_DIFF:CMD_PC_TONE_NODIFF,NULL,NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_CALLID_MODE))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_CALLID_DTMF);
			pbx_send_comcmd((isOn)?CMD_PC_DISP_FSK:CMD_PC_DISP_DTMF,NULL,NULL);
		}
		else if(strstr(pnode->entry.name, PBX_GLOBAL_NODIAL_ACTION ))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_NODIAL_BROKEN );
			pbx_send_comcmd((isOn)?CMD_PC_NODIAL_PHONE:CMD_PC_NODIAL_OFF,NULL,NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_LINE_2_LINE))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_send_comcmd((isOn)?CMD_PC_L2L_YES:CMD_PC_L2L_NO,NULL,NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_PSTN_PREFIX))
		{
			if(atoi(pnode->entry.value))
				pbx_send_comcmd(CMD_PC_LNET_HEAD,NULL,pnode->entry.value);
			else
				pbx_send_comcmd(CMD_PC_LNET_NOHEAD,NULL,NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_SPECIAL_FIRST))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_send_comcmd((isOn)?CMD_PC_SPECIAL_FIRST:CMD_PC_NO_SPECIAL, NULL, NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_GROUP_CALL))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_send_comcmd((isOn)?CMD_PC_GROUP:CMD_PC_NOGROUP, NULL, NULL);
		}
		else if(strstr(pnode->entry.name,PBX_GLOBAL_IP_PREFIX))
			pbx_send_comcmd(CMD_PC_IP_PREFIX/*0x52*/, NULL, pnode->entry.value);
		else if(strstr(pnode->entry.name,PBX_GLOBAL_SYSTEM_PASSWD))
			pbx_send_comcmd(CMD_PC_SYS_PASS,NULL,pnode->entry.value);
		else if(strstr(pnode->entry.name,PBX_GLOBAL_PASSWORD/*PC05*/) )
			pbx_send_comcmd(CMD_PC_PASSWORD, NULL, pnode->entry.value);
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

	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_EXT_B_PHONE_NUMBER))
			pbx_send_comcmd(0x3F, getendch(pnode->entry.name),pnode->entry.value);
		else if(strstr(pnode->entry.name,PBX_EXT_B_DIALOUT ))
		{
			if(!strcmp(pnode->entry.value,"10") )//""a"))/* PSTN FIRST */
				pbx_send_comcmd(0x42, getendch(pnode->entry.name),"0");
			else if(!strcmp(pnode->entry.value,"11"))//"b"))/* IP First */
				pbx_send_comcmd(0x42, getendch(pnode->entry.name), "1");
			else
				 pbx_send_comcmd(0x41, getendch(pnode->entry.name), pnode->entry.value);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_SCOPE /*BC03*/))
			pbx_send_comcmd(0x2F, getendch(pnode->entry.name), pnode->entry.value);
		else if(strstr(pnode->entry.name,PBX_EXT_B_LENGTH/*BC04*/))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x30, getendch(pnode->entry.name),"0");
			else
			{
				pbx_send_digit_comcmd(0x30, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		else if(strstr(pnode->entry.name,PBX_EXT_B_CALLID/*BC08*/))
		{
			if(!strcmp(pnode->entry.value, CGI_STR_ON))
				pbx_send_comcmd(0x36, getendch(pnode->entry.name),NULL);
			else
				pbx_send_comcmd(0x35, getendch(pnode->entry.name),NULL);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_DELAY2LINE/*BC09*/))
			pbx_send_comcmd(0x3E, getendch(pnode->entry.name),pnode->entry.value);
		else if(strstr(pnode->entry.name, PBX_EXT_B_NODISTURB/*BC22*/))
		{
			if(!strcmp(pnode->entry.value, CGI_STR_ON))
			      pbx_send_comcmd(0x5c, getendch(pnode->entry.name),NULL);
			else
			      pbx_send_comcmd(0x5d, getendch(pnode->entry.name),NULL);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_B_CALLWAIT/*BC26*/ ))
		{
			if(!strcmp(pnode->entry.value, CGI_STR_ON))
			      pbx_send_comcmd(0x4e, getendch(pnode->entry.name),NULL);
			else
			      pbx_send_comcmd(0x4f, getendch(pnode->entry.name),NULL);
		}
#if 0 /*what is BC13 */		
		else if(strstr(pnode->entry.name, /*BC13*/))
		{
			if(!strcmp(pnode->entry.value, "1"))
				pbx_send_comcmd(0x4e, getendch(pnode->entry.name), NULL);
			else
				pbx_send_comcmd(0x4f, getendch(pnode->entry.name), NULL);
		}
#endif		
		else
		{
			AS_MSG_DEBUG(AS_LOG_WARNING, "ext basic handler : error key value '%s'",pnode->entry.name );
		}
		
		pnode = pnode->next;
	}

	return 0;
}

void get_index_by_number(char *phonenumber,llist *l)
{		
	node *curret=l->head;
	char *value=NULL;
	
	if(strlen(phonenumber)<=4)
	{
		while(curret!=NULL)
		{
			if( (!strcmp(curret->entry.value, phonenumber)) 
				&&strstr(curret->entry.name, PBX_EXT_B_PHONE_NUMBER) )
			{
				value=getendch(curret->entry.name);
				break;
			}	
			curret=curret->next;
		}
		
		if(value!=NULL)
		{
//			memset(phonenumber,0, sizeof(phonenumber));
			sprintf(phonenumber,"%s", value);
		}
	}	

	return;
}

static int _pbx_ext_special_handle(llist *extBaseConfig)
{
#if 0
	char *last_str=NULL;
	char namecmd[128],cmd[128];
#endif

	node *pnode = extBaseConfig->head;
//	int	isOn = 0;

	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_EXT_S_BUSY ))
		{
			if(!strcmp(pnode->entry.value, PBX_INVALIDATE_STR) )
				pbx_send_comcmd(0x3C, getendch(pnode->entry.name),NULL);
			else
			{
				get_index_by_number(pnode->entry.value, &pbxentries);
				pbx_send_comcmd(0x3B, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_SECRET ) )
		{
			if(!strcmp(pnode->entry.value, PBX_INVALIDATE_STR) )
				pbx_send_comcmd(0x3A, getendch(pnode->entry.name),"");
			else
			{
				get_index_by_number(pnode->entry.value,&pbxentries);
				pbx_send_comcmd(0x39, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		/* forat of 'Branch_Index|Differ_Ring|Group' is process by CGI '*/
		else if(strstr(pnode->entry.name,PBX_EXT_S_DIFFERTING ))
		{
			pbx_send_comcmd(0x5b, getendch(pnode->entry.name),pnode->entry.value);
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_NORESPONSE ))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x38, getendch(pnode->entry.name),NULL);
			else
			{
				 get_index_by_number(pnode->entry.value,&pbxentries);
				 if(1==strlen(pnode->entry.value))
				 {
					pbx_send_teshu_comcmd(0x37, getendch(pnode->entry.name),pnode->entry.value);
				 }
				 else
				 	pbx_send_do_spec_comcmd(0x37, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_NIGHT))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x4B, getendch(pnode->entry.name),NULL);
			else
			{
				get_index_by_number(pnode->entry.value,&pbxentries);
				if(1==strlen(pnode->entry.value))
					pbx_send_teshu_comcmd(0x4A, getendch(pnode->entry.name),pnode->entry.value);
				else	
					pbx_send_do_spec_comcmd(0x4A, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
#if 0	/* BCS12 is serect ??? */		
		else if(strstr(pnode->entry.name, BCS12))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x4D, getendch(pnode->entry.name),NULL);
			else
			{
				get_index_by_number(pnode->entry.value,&pbxentries);
				pbx_send_do_spec_comcmd(0x4C, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
#endif
		else if(strstr(pnode->entry.name,PBX_EXT_S_NOCONDITION /*BCS14*/))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x56, getendch(pnode->entry.name),NULL);
			else
			{
				get_index_by_number(pnode->entry.value,&pbxentries);
				if(1==strlen(pnode->entry.value))
					pbx_send_teshu_comcmd(0x55, getendch(pnode->entry.name),pnode->entry.value);
				else
					pbx_send_do_spec_comcmd(0x55, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		else if(strstr(pnode->entry.name,PBX_EXT_S_DENYCALLIN))
		{
			if(!strcmp(pnode->entry.value,""))
				pbx_send_comcmd(0x4d, getendch(pnode->entry.name),NULL);
			else
			{
				get_index_by_number(pnode->entry.value,&pbxentries);
				if(1==strlen(pnode->entry.value))
					pbx_send_teshu_comcmd(0x4c, getendch(pnode->entry.name),pnode->entry.value);
				else
					pbx_send_do_spec_comcmd(0x4c, getendch(pnode->entry.name),pnode->entry.value);
			}
		}
		else if(strstr(pnode->entry.name, PBX_EXT_S_NORESTIME/*BCS15*/))
		{
			pbx_send_digit_comcmd(0x57, getendch(pnode->entry.name),pnode->entry.value);
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
	int	isOn = 0;
	while(pnode)
	{

		if(strstr(pnode->entry.name,PBX_LINE_STATUS ))
		{
			isOn = strcasecmp(pnode->entry.value, CGI_STR_OFF);
			pbx_send_comcmd((isOn)?0x20/* on */:0x21/* off*/, getendch(pnode->entry.name), NULL);
		}
		 if(strstr(pnode->entry.name, PBX_LINE_CONSOLE ) )
			pbx_send_comcmd(0x2c, getendch(pnode->entry.name), pnode->entry.value);
		 if(strstr(pnode->entry.name, PBX_LINE_GROUP ))
			pbx_send_comcmd(0x5e, getendch(pnode->entry.name), pnode->entry.value);
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
	int	isOn = 0;
	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_TRUNCK_MODE/* CC01*/ ))
		{
			isOn = strcasecmp(pnode->entry.value, PBX_TRUNCK_CONSOLE);
			pbx_send_comcmd((isOn)?0x23/* direct*/:0x22/* console*/, NULL, NULL);
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
#define	ACCOUNT_AUTH_CANCEL	6
	while(pnode)
	{
		if(strstr(pnode->entry.name,PBX_ACCOUNT_PASSWD /*AC02*/))
		{
			pbx_send_comcmd(0x44, getendch(pnode->entry.name), pnode->entry.value);
		}
		
		if(strstr(pnode->entry.name, PBX_ACCOUNT_AUTH/*AC03*/))
		{
#if 1		
			if(ACCOUNT_AUTH_CANCEL==atoi(pnode->entry.value))/* cancel, no authen for call */
				pbx_send_comcmd(0x2e, getendch(pnode->entry.name), pnode->entry.value);
			else
#endif				
				pbx_send_comcmd(0x2d, getendch(pnode->entry.name), pnode->entry.value);
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
	
	while(pnode)
	{
		if(strstr(pnode->entry.name, PBX_SPECIAL_SERVICE /*SC02*/))
		{
			if(! strlen(pnode->entry.value) )
				pbx_send_comcmd(0x54, getendch(pnode->entry.name),NULL);
			else
				pbx_send_comcmd(0x53, getendch(pnode->entry.name), pnode->entry.value);
			//pbx_send_do_spec_comcmd(0x53, getendch(pnode->entry.name), pnode->entry.value);
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
	pbx_send_comcmd(CMD_PC_RESET, NULL, NULL);
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

int	pbx_setup_pbx(llist *cfgs)
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
		pbx_send_comcmd(CMD_PC_GET_CONFIG, NULL, NULL); 
	}

	return res;
}


int	pbx_setup_pbx_when_startup(llist *cfgs)
{
//	int	i;
	int	res = -1;

//	pbx_send_comcmd(0x5e, NULL, NULL);
#if 0
	pbx_send_comcmd(CMD_PC_GET_CONFIG, NULL, NULL);
	{
		PBX_COMMAND *cmd;
		while(1)
		{
			cmd = as_cmd_cgi_from_uart();
			if(cmd && cmd->op == 0xca)
			{
				break;
			}
		}
	}

	sleep(2);
#endif

#if 0
	for(i=0; i< sizeof(pbx_handlers)/sizeof(pbx_sub_handler); i++)
	{
		if( strcasecmp(pbx_handlers[i].pbxCfgElement, PBX_CMDSET_RESET) )/* no reset is needed*/
		{
			AS_MSG_LOG(AS_LOG_INFO, "%s is handled when startup", pbx_handlers[i].pbxCfgElement);
			res = (pbx_handlers[i].handler)(cfgs);
			usleep(100);
		}	
	}
	{
		sleep(2);/* delay some time is needed */
		pbx_send_comcmd(CMD_PC_GET_CONFIG, NULL, NULL); 
	}
#endif
	return res;
}

