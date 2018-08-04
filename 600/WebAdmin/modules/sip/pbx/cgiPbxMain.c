/*
* $Id: cgiPbxMain.c,v 1.13 2007/09/04 20:46:17 lizhijie Exp $
*/

#include "cgi_pbx.h"

typedef struct 
{
	int 	sigCode;
	int	opCode;
}cgi_cmd_t;


PBX_INFO _info;

void sighandler(int signo, siginfo_t *info,void * val)
{
	_info.pbxReplyCode = info->si_int;
}

int	sendDefaultValue(default_value_t *values, int sizeofValue)
{
	FILE *fp;
	int	i;

	if((fp=fopen(FILE_PBX_TMP,"w"))==NULL)
	{
		cgidebug( "writeconfig(): open file %s error : %s\n", FILE_PBX_TMP, strerror(errno) );
		return -1;
	}
	
	for(i=0; i< sizeofValue; i++)
	{
		fprintf(fp, "%s\t\t%s\n", values->key, values->value);
		values++;
	}

	fclose(fp);

	return 0;
}

static int __cgiPbx_init(PBX_INFO *info)
{
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	info->pid=(pid_t)cgi_get_pid_by_name(PROC_PBX);
	if(info->pid<0)
	{
		cgi_error_page(NULL,gettext(CGI_STR_WARN), gettext("Task of Communication Command is not running") );
		return -1;
	}
	cgidebug("CGIPBX pid is %d\n", info->pid);
	cgi_signal_register_handler(SIG_PBX_CGI, sighandler);
	info->pbxReplyCode = -1;

	return 0;
}

int main(int argc, char *argv[])
{
	char 	*cmd=NULL;
	char		*action;
//	char		*cmdSet;
	cgi_cmd_t	cgiCmd;
	llist			pbxCfgs;
	char			msg[256];
	char 		target[256];
	int			count = 0;
	PBX_INFO	*info;
	info = &_info;

	list_create(&pbxCfgs);
	
	if(__cgiPbx_init(info) )
	{
//		cgi_error_page(NULL, gettext("PBX Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}
	
	cmd = GET_CGI_OP(&info->cgiVariables );
	if(!strcasecmp(cmd, CGIPBX_OP_BILL) || !strcasecmp(cmd, CGIPBX_OP_CONFERENCE)  )
	{
		return cgi_future_page(cmd, gettext("PBX") );
	}


	cgiCmd.sigCode = SIG_PBX_CGI;
	action = GET_CGI_ACTION(&info->cgiVariables );
	if(!action || !strlen(action))
	{
		cgiCmd.opCode = OP_PBX_READ_REQ;
	}
	else
	{/* action is activate */
		cgiCmd.opCode = OP_PBX_WRITE_REQ;

		if( !strcasecmp(action, CGI_ACTION_ACTIVATE) || !strcasecmp(action, CGI_ACTION_RESET) )
		{
			/* for Night Service Configuration */
			if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_NIGHT_SERVICE))
			{
				char	v[64];
				char *sh, *sm, *eh, *em;
				sh = GET_VALUE(&info->cgiVariables, START_HOUR);
				sm = GET_VALUE(&info->cgiVariables, START_MINUTE);
				eh = GET_VALUE(&info->cgiVariables, END_HOUR);
				em = GET_VALUE(&info->cgiVariables, END_MINUTE);
				
				sprintf(v, "%s:%s:%s:%s", sh, sm, eh, em);
				list_append_raw(&info->cgiVariables, PBX_GLOBAL_NIGHT_TIME, v, NULL, NULL);
			}
#if 0		

			/* for Differ Ring and Group in Branch Special Configuration */
			if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_SPEC))
			{
				int	i;
				char	key[64];
				node *_node;
				for(i=0; i< 8/* info->extsCount*/; i++)
				{
					sprintf(key, "%s_%d", PBX_EXT_S_DIFFERTING, i);
					_node = list_find_node_by_key(&info->cgiVariables, key);
					sprintf(key, "%s_%d", PBX_EXT_S_GROUP, i);
					sprintf(_node->entry.value, "%s%s", _node->entry.value, GET_VALUE(&info->cgiVariables, key) );
					cgidebug("'%s' is '%s'\n", _node->entry.name, _node->entry.value);
				}
			}
			cmdSet = GET_VALUE(&info->cgiVariables, PBX_CMDSET );
			cgidebug("Config PBX : '%s'\n", cmdSet);
			
			cmdSet = GET_VALUE(&info->cgiVariables, CGI_ACTION );
#endif			
			if(writeconfig(FILE_PBX_TMP, ITEM_DBL, NULL, &info->cgiVariables))
			{
				cgi_error_page(NULL, gettext("PBX Operation Failed"), gettext( "Write Config Info Errors in your releases" ) );
				return -1;
			}
		}
		else
		{
			sprintf(msg, "'%s' %s", action, gettext("is not validate command in PBX configuration") );
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
			return 1;
		}

			{
				cgi_sig_send(info->pid, cgiCmd.sigCode, cgiCmd.opCode );
				
			//	cgidebug("send signal %d(opcode %d)",sigcode,opcode);
			//	cgidebug("to process %d\n",pid);
				while(info->pbxReplyCode< 0 )
				{
					usleep(1000);	/* microsecond */
					count ++;
					if(count> 1000 )
						break;
				};
				
				switch( info->pbxReplyCode)
				{		
					case OP_PBX_READ_RPL:
						cgiCmd.opCode = OP_PBX_READ_OK;
						break;
					case OP_PBX_WRITE_RPL:
						cgiCmd.opCode = OP_PBX_WRITE_OK;
						cgi_sig_send(info->pid, cgiCmd.sigCode, cgiCmd.opCode );
						
						sprintf(target, "%s?%s=%s", WEB_URL_PBX, CGI_KEYWORD_OPERATOR, cmd);
						sprintf(msg, "%s. %s, %s! <br>%s.....", 
							gettext("Communicating with back-end"), gettext("New configuration will take effect as once"),
							gettext("So take more patiently"), 
							gettext(CGI_STR_PLEASE_WAIT) );
						return cgi_refresh_page(9, target, gettext("PBX Configuration"), msg );
						break;
					case -1:
						return cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("CGI communication task has no response!Please try later!")  );
						break;
					default:
						return cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("unknown response code"));
				}
				
				cgi_sig_send(info->pid, cgiCmd.sigCode, cgiCmd.opCode );

			}
		
	}

	if (readconfig(FILE_PBX_CONF, ITEM_DBL, NULL,&info->cfgs) != 0 )
	{
		cgi_error_page(NULL, gettext("PBX Operation Failed"), gettext( "Read config info reply be CGIPBX failed" ) );
		return -1;
	}
	info->extsCount = atoi(GET_VALUE(&info->cfgs, PBX_WEBADMIN_EXT_NUM) );
	info->linesCount = atoi(GET_VALUE(&info->cfgs, PBX_WEBADMIN_LINE_NUM) );
	info->specialServiceCount = atoi(GET_VALUE(&info->cfgs, PBX_WEBADMIN_SPECIAL_NUM) );
	info->groupCount = atoi(GET_VALUE(&info->cfgs, PBX_WEBADMIN_GROUP_NUM) );

	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, CGIPBX_OP_BASE_CONFIG ) )
	{
		return cgiPbx_BasicConfig(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_BASE) )
	{
		return cgiPbx_BranchBasic(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_SPEC) )
	{
		return cgiPbx_BranchSpecial(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_EXT_TRANSFER) )
	{
		return cgiPbx_BranchExtTransfer(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_CALL_TRANSFER) )
	{
		return cgiPbx_BranchCallTransfer(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_NIGHT_SERVICE) )
	{
		return cgiPbx_BranchNightService(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_LINES) )
	{
		return cgiPbx_Line(info );
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_TRUNCK) )
	{
		return cgiPbx_Trunck(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_ACCOUNT) )
	{
		return cgiPbx_Account(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_SERVICES_SPEC) )
	{
		return cgiPbx_SpecialService(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_GLOBAL_NUMBER) )
	{
		return cgiPbx_GlobalDialPlan(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BRANCH_NUMBER) )
	{
		return cgiPbx_BranchPhoneNumber(info);
	}
	else	 if(!strcasecmp(cmd, CGIPBX_OP_BLACKLIST) )
	{
		return cgiPbx_BlackList(info);
	}
	else
	{
		cgidebug("PBX command '%s' is not found\n", cmd );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}

