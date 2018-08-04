/*
* $Id: help_cgi.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

static llist		helpCgiVariables;

char *cgi_help_getLocaleHeader(cgi_help_type_t type);
char *cgi_help_getLocaleMsg(cgi_help_type_t type);


static int _help_cgi_init()
{
	cgi_html_header();
	list_create(&helpCgiVariables);
	read_cgi_input(&helpCgiVariables, NULL, NULL);
	
	return 0;
}

int main()
{
	char *cmd=NULL;
	char	*header, *msg;
	int	msgType;

	if(_help_cgi_init() )
	{
		cgi_error_page(NULL, gettext("Help Failed"), gettext( "Some Errors in your releases for HELP function" ) );
		return 1;
	}

	cmd = GET_VALUE( &helpCgiVariables,  "msg");
	if( !cmd)
	{
		cgi_invalidate_page();
		return 0;
	}

	msgType = atoi(cmd);
	header = cgi_help_getLocaleHeader(msgType);
	msg = cgi_help_getLocaleMsg( msgType);
	
	if( header && msg && strlen(header) && strlen(msg) )
	{
		cgi_info_page(gettext("Help"), header, msg );
		return 0;
	}
	else
	{
		cgidebug("mail command '%s' is not found\n", cmd );
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Help information for this menu") );
		return 0;
	}
	
	return 0;	
}

