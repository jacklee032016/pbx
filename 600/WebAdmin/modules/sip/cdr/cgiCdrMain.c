/*
* $Id: cgiCdrMain.c,v 1.3 2007/11/15 08:18:50 lizhijie Exp $
*/

#include "_cdr.h"

static int __cdr_init(CDR_INFO *info)
{
	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

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
	CDR_INFO	_info, *info;
	char 	*cmd=NULL;
	
	info = &_info;

	if(__cdr_init(info) )
	{
		cgi_error_page(NULL, gettext("SIP UA Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}
	cmd = GET_CGI_OP( &info->cgiVariables);
#if 0
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, SIPUA_OP_PROTOCOL ) )
	{
		return sipUa_protocol(&sipUaCgiVariables);
	}
	else if(!strcasecmp(cmd, CDR_OP_CONFIG) || !strcasecmp(cmd, CDR_OP_VIEW)  )
	{
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
#endif	
	{
		char	*content;
		if(strcasecmp(cmd, CDR_OP_CONFIG)==0)
		{
			content = gettext("CDR Configuration");
		}
		else
			content = gettext("CDR View");
		
		return cgi_future_page(content, content );
	}

	return 0;	
}


