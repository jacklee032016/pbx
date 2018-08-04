/*
* $Id: sysInfoMain.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgi_sysinfo.h"

SYS_INFO		*_sysInfo;

char		dataFilename[256] = "\0";

static int __cgiSysInfo_init()
{
	_sysInfo = (SYS_INFO *)malloc( sizeof(SYS_INFO) );
	if(!_sysInfo)
		return -1;
	memset(_sysInfo, 0 ,sizeof(SYS_INFO) );
	
	cgi_html_header();
	read_cgi_input(&_sysInfo->cgiVariables, NULL, NULL);

#if 0
	_sysInfo->netInfo = cgi_get_network_info( 1);
#endif

	return 0;
}


int main(int argc, char *argv[])
{
	char 	*cmd=NULL;
	int 		ret = __cgiSysInfo_init();

	if( ret )
	{
		if( ret ==-1)
			cgi_error_page(NULL, gettext("System Information Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_CGI_OP( &_sysInfo->cgiVariables );
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGI_SYSINFO_OP_SYS_PROC) )
	{
		return cgiSysInfo_Tasks(_sysInfo);
	}
	if(!strcasecmp(cmd, CGI_SYSINFO_OP_VERSION) )
	{
		return cgiSysInfo_Versions(_sysInfo);
	}
	if(!strcasecmp(cmd, CGI_SYSINFO_OP_LOCALE) )
	{
		return cgiSysInfo_Locale(_sysInfo);
	}
	if(!strcasecmp(cmd, CGI_SYSINFO_OP_TIME) )
	{
		return cgiSysInfo_Time(_sysInfo);
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


