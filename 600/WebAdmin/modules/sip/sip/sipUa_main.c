/*
* $Id: sipUa_main.c,v 1.12 2007/10/25 08:33:02 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"

//static llist  	sipUaCfgs;
static llist		sipUaCgiVariables;

static int __sipUa_init()
{
	cgi_html_header();
	list_create(&sipUaCgiVariables);
	read_cgi_input(&sipUaCgiVariables, NULL, NULL);

#if 0	
	list_create(&sipUaCfgs);

	if (readconfig(SYS_LOCALE_CONF, ITEM_EQU, NULL, &sipUaCfgs) != 0)
	{
		return -1;
	}
#endif

	return 0;
}

int main()
{
	char 	*cmd=NULL;

	if(__sipUa_init() )
	{
		cgi_error_page(NULL, gettext("SIP UA Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE( &sipUaCgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, SIPUA_OP_PROTOCOL ) )
	{
		return sipUa_protocol(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_IP_DEVICE) )
	{
		return sipUa_global(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_PSTN_LINE) )
	{
		return sipUa_PstnStatus(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_SERVER) )
	{
		return sipUa_Server(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_GAIN_CONTROL) )
	{
		return sipUa_gain_control(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_DIAL_PATTERN) )
	{
		return sipUa_dial_parttern(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_SPEED_DIAL) 
		||!strcasecmp(cmd, SIPUA_OP_LOCAL_DIAL)
 		||!strcasecmp(cmd, SIPUA_OP_CALL_RETURN) )
	{
		return sipUa_dial_rules(&sipUaCgiVariables, cmd);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_DISTURBLIST) )
	{
		return sipUa_disturb_list(&sipUaCgiVariables);
	}
	else	 if(!strcasecmp(cmd, SIPUA_OP_BLACKLIST_MGMT) )
	{
		return sipUa_backlist_mgmt(&sipUaCgiVariables);
	}
	else if(!strcasecmp(cmd, SIPUA_OP_NO_DISTURB_FUNC)   )
	{
		return cgi_future_page(cmd, gettext("No Disturb Function") );
	}
	else if( !strcasecmp(cmd, SIPUA_OP_BLACKLIST)  )
	{
		return cgi_future_page(cmd, gettext("Black List") );
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


