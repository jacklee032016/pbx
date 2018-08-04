/*
* $Id: mailMain.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_mail.h"

MAIL_INFO		*_mailInfo;

static int __cgiMail_init()
{
	_mailInfo = (MAIL_INFO *)malloc( sizeof(MAIL_INFO) );
	if(!_mailInfo)
		return -1;
	memset(_mailInfo, 0 ,sizeof(MAIL_INFO) );
	
	cgi_html_header();
	list_create(&_mailInfo->cgiVariables);
	read_cgi_input(&_mailInfo->cgiVariables, NULL, NULL);

	_mailInfo->netInfo = cgi_get_network_info(1);

	return 0;
}

int main()
{
	char 	*cmd=NULL;
	int 		ret = __cgiMail_init();

	if( ret )
	{
		if( ret ==-1)
			cgi_error_page(NULL, gettext("Mail Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE( &_mailInfo->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGI_MAIL_OP_SMTP) )
	{
		return mail_Smtp(_mailInfo);
	}
	if(!strcasecmp(cmd, CGI_MAIL_OP_POPIMAP) )
	{
		return mail_PopImap(_mailInfo);
	}
	if(!strcasecmp(cmd, CGI_MAIL_OP_ALIAS) )
	{
		return mail_Alias(_mailInfo);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


