/*
* $Id: partyLookup.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"
#include "cgi_services.h"

#include "asPartyFifo.h"
#include <ppl.h>

char *__getUsersInfo(as_fifo_agent_t *clientAgent)
{
	char		buf[8192*10];
	int		length;
	char  	target[512];
	as_fifo_cmd_t cmd;
	sip_user_info_t *fifoUser;
	user_debug_t *user ;
	int res;
	int index = 0;
	
	cmd.type = AS_FIFO_CMD_LOOKUP;
	cmd.length = 0;
	asFifoClientSendCommand(clientAgent, &cmd);
	while(1)
	{
		res = asFifoClientRecvResponse(clientAgent, &cmd );
		if(res == 0 )
			break;

		index++;
		if(index == 1)
		{
			length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\" bgcolor=\"#FFFFFF\" >\n");
			length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
				gettext(CGI_STR_INDEX), gettext("SIP URL"), gettext(CGI_STR_STATUS),gettext(CGI_STR_PROTOCOL), gettext(CGI_STR_PORT),gettext(CGI_STR_USER_NAME), gettext("Location"), gettext(CGI_STR_MANAGEMENT) );
		}
		
		fifoUser  = (sip_user_info_t *)cmd.fifoData;
		user = asFifiGetUserDebug(fifoUser);

		if(user->status == UINFO_STATUS_AWAY)
		{
//			unescape_url(user->url);
			sprintf(target, WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_LOOKUP"&"CGI_ACTION"=%s&"PARTY_CFG_USER_URL"=%s", CGI_ACTION_DEL, user->url );
			length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%d</strong></TD><TD >%s</TD><TD>%s</TD><TD colspan=4></TD><TD >%s</TD></TR>\n\r\n", 
				fifoUser->Id, user->url, asFifoGetUserStatus(user->status) , cgi_button(gettext(CGI_STR_DELETE), target) );
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%d</strong></TD><TD >%s</TD><TD >%s</TD><TD >%s</TD><TD >%d</TD><TD >%s</TD><TD >%s</TD><TD ></TD></TR>\n\r\n", 
				fifoUser->Id, user->url, asFifoGetUserStatus(user->status), asFifoGetProtocol(user->protocol), user->port, user->name, user->contact );
		}
	};

	if(index == 0)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TR>\n\r\n", 
			gettext("No user has been registered until now"));
	}
	else
	{
		length += CGI_SPRINTF(buf, length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n\n" );
	}

	return strdup(buf);
}


int __party_user_delete(PARTY_INFO *info, as_fifo_agent_t *clientAgent)
{
	char		target[512];
	char		msg[512];
	as_fifo_cmd_t cmd;
	
	char *userUrl = GET_VALUE(&info->cgiVariables, PARTY_CFG_USER_URL );
	if(userUrl == NULL || strlen(userUrl) == 0 )
	{
		return cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("No User URL Information")  );
	}

	cmd.type = AS_FIFO_CMD_DELETE;
	cmd.length = strlen(userUrl);
	sprintf(cmd.fifoData, "%s", userUrl);

//	printf("url = '%s'", userUrl);
	asFifoClientSendCommand(clientAgent, &cmd);

	sprintf(target, WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_LOOKUP );
	sprintf(msg, "'%s' %s.....", userUrl, gettext("is deleting"));

	cgi_refresh_page(3, target, gettext(CGI_STR_WAIT), msg);
	
	return 0;
}

int cgiParty_Lookup(PARTY_INFO *info)
{
	char		title[1024];
	as_fifo_agent_t clientAgent;
	char		*action;

#if 0
	int	pid;
	
	pid = cgi_get_program_pid_with_param(SYS_PROC_SIPSERVER, NULL);
	if(pid <= 0)
#else		
	sprintf(clientAgent.name, "%s", "FIFOClient");
	clientAgent.type = AS_FIFO_CLIENT;
#endif		
	if(asFifoAgentInit(&clientAgent))
	{
		return cgi_error_page(NULL, gettext(CGI_STR_WARN), gettext("SIP Server is not running now!") );
	}


	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if( action && strlen(action) )
	{
		__party_user_delete(info, &clientAgent);
		
		asFifoAgentFree(&clientAgent);
		return 0;
	}


	snprintf(title, sizeof(title), "%s %s", cgi_refresh_script(WEB_URL_PARTY"?"CGI_KEYWORD_OPERATOR"="PARTY_OP_LOOKUP, 10) , gettext("User Agent List"));

	cgi_info_page(gettext("Registered SIP UA Information"), title, __getUsersInfo(&clientAgent) );

	asFifoAgentFree(&clientAgent);
//	cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP UA Information")  );
		
	return 0;	
}

