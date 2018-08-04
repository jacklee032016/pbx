/*
* $Id: hwCheckMain.c,v 1.3 2007/09/04 20:46:17 lizhijie Exp $
*/
#include "_hwCheck.h"

/*
* enhanced it by added timeout of read msg queue and check cgi has been running 
*/

static HWCHECK_INFO	_hwCheckInfo;

#define	PBX_EXT_NUM		"Ext_Number"

static int __init(HWCHECK_INFO *info)
{
	as_msg_queue_init(0);

	cgi_html_header();
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

#if 0
	list_create(&info->cfgs);
	if (readconfig(FILE_PBX_CONF, ITEM_DBL, NULL,&info->cfgs) != 0 )
	{
		cgi_error_page(NULL, gettext("PBX Operation Failed"), gettext( "Read config info reply be CGIPBX failed" ) );
		return -1;
	}
	info->extsCount = atoi(GET_VALUE(&info->cfgs, PBX_EXT_NUM) );
#endif	
	info->extsCount = 8;

//	cgiPbxReadFromUart();
	return 0;
}


int main()
{
	char 	*cmd=NULL;
	HWCHECK_INFO 	*info = &_hwCheckInfo;
	int res = 0;

	if(__init( info) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	cmd = GET_CGI_OP(&info->cgiVariables );
	if( !cmd || !strlen(cmd) || !strcasecmp(cmd, PBXHW_OP_CHECK_RINGER ) )
	{
		res =  cgiPbxHwCheckRinger(info);
	}
	else	 if(!strcasecmp(cmd, PBXHW_OP_CHECK_PCM) )
	{
//		res = cgiPbxHwCheckPcm(info);
		return cgi_future_page(cmd, "");
	}
	else	 if(!strcasecmp(cmd, PBXHW_OP_CHECK_NVRAM) )
	{
		res = cgiPbxHwCheckNvRam(info);
	}
	else	 if(!strcasecmp(cmd, PBXHW_OP_CHECK_RTC) )
	{
		res = cgiPbxHwCheckRTC(info);
	}
	else	 if(!strcasecmp(cmd, PBXHW_OP_CHECK_CALLID_CHIP) )
	{
//		res = cgiPbxHwCheckDtmf(info);
		return cgi_future_page(cmd, "");
	}
	else	 if(!strcasecmp(cmd, PBXHW_OP_CHECK_ARRAY) )
	{
//		res = cgiPbxHwCheckDtmf(info);
		return cgi_future_page(cmd, "");
	}
	else
	{
		cgi_invalidate_page();
		res = 0;
	}

//	as_msg_queue_destroy();

	return res;	
}

