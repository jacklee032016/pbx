/*
* $Id: cgiDataMain.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "aslib.h"
#include "cgi_data.h"

DATA_INFO		*_dataInfo;

char		dataFilename[256] = "\0";

static int __cgiData_init()
{
	_dataInfo = (DATA_INFO *)malloc( sizeof(DATA_INFO) );
	if(!_dataInfo)
		return -1;
	memset(_dataInfo, 0, sizeof(DATA_INFO) );
	
	cgi_html_header();
	list_create(&_dataInfo->cgiVariables);
	read_cgi_input(&_dataInfo->cgiVariables, NULL, NULL);

	_dataInfo->netInfo = cgi_get_network_info( 1);
	
	list_create(&_dataInfo->globalCfgs);
	if (readconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &_dataInfo->globalCfgs) != 0)
	{
		return -1;
	}

	return 0;
}


int cgiData_RestoreInit(DATA_INFO *info)
{
	char  	target[128];
	char		msg[256];
	int		length = 0;

 	as_beep_short(1);
	cgidata_write_config_file(info, CGIDATA_KEYWORD_RESET_FLAG, CGIDATA_KEYWORD_RESET_FLASH);
	
	sprintf(target, "%s", WEB_URL_CONTENT  );
	length += sprintf(msg+ length, "%s,",gettext("Restore From Init will cost 1 minuts or more") );
	length += sprintf(msg+ length, "%s!",gettext("it is determined by the services you selected"));
	cgi_refresh_page(60, target, gettext("Rebooting....."), msg ) ;
	
	system("reboot");
	return 0;	
}


int main()
{
	char 	*cmd=NULL;

	if(__cgiData_init() ==-1 )
	{
		cgi_error_page(NULL, gettext("Data&Software Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_VALUE( &_dataInfo->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGIDATA_OP_BACK_SERVER) )
	{
		return cgiData_BackupServer(_dataInfo);
	}
	if(!strcasecmp(cmd, CGIDATA_OP_BACK_LOCAL) )
	{
		return cgiData_BackupLocal(_dataInfo);
	}
	if(!strcasecmp(cmd, CGIDATA_OP_RESTORE_SERVER) )
	{
		return cgiData_RestoreServer(_dataInfo);
	}
	if(!strcasecmp(cmd, CGIDATA_OP_RESTORE_INIT) )
	{
		return cgiData_RestoreInit(_dataInfo);
	}

	if(!strcasecmp(cmd, CGIDATA_OP_AUTO_UPDATE)  )
	{
		return cgi_future_page(cmd, gettext("Data&Software") );
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}


