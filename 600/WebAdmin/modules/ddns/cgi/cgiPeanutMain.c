/*
* $Id: cgiPeanutMain.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgiDdns.h"

int main()
{
	char 	*cmd=NULL;
	PEANUT_INFO _info, *info;
	info = &_info;
	char buf[4096];

	if(PeanutInitConfig(info) ==-1 )
	{
		CGI_SPRINTF(buf, 0, "%s %s", gettext(CGI_STR_PEANUT_NAME), gettext("Operation Failed"));
		cgi_error_page(NULL, buf, gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}
	
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	cmd = GET_VALUE( &info->cgiVariables,  CGI_KEYWORD_OPERATOR );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, PEANUT_OP_CONFIG) )
	{
		return cgiPeanutConfig(info);
	}
	if( !strcasecmp(cmd, PEANUT_OP_TEST) )
	{
		return cgiPeanutTest(info);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


