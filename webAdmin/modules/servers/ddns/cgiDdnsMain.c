/*
* $Id: cgiDdnsMain.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgiDdns.h"

int main(int argc,  char *argv[])
{
	char 	*cmd=NULL;
	DDNS_INFO _info, *info;
	char buf[4096];

	info = &_info;
	memset(info, 0 , sizeof(DDNS_INFO));

	if(DdnsInitConfig(info) ==-1 )
	{
		CGI_SPRINTF(buf, 0, "%s %s", gettext(CGI_STR_DDNS_NAME), gettext("Operation Failed"));
		cgi_error_page(NULL, buf, gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}
	
	cgi_html_header();
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	cmd = GET_CGI_OP( &info->cgiVariables );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, DDNS_OP_CONFIG) )
	{
		return cgiDdnsConfig(info);
	}
	if( !strcasecmp(cmd, DDNS_OP_TEST) )
	{
		return cgiDdnsTest(info);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


