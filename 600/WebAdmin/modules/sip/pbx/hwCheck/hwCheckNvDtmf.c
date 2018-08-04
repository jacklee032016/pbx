/*
* $Id: hwCheckNvDtmf.c,v 1.2 2007/09/04 20:46:17 lizhijie Exp $
*/
#include "_hwCheck.h"

static int	__check_dtmf_channel(HWCHECK_INFO *info, int encoder, int decoder)
{
	PBX_COMMAND *cmdbuf;
	char		statusMsg[1024];
	int		length = 0;
	
	length += CGI_SPRINTF(statusMsg, 0, "<p>%d DTMF Encoder and %d DTMF Decoder Begin to test.....", encoder+1, decoder+1);
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cgiPbxSendCheckCmd(info, SELF_TEST_TYPE_CALLERID, encoder, decoder);

	length += CGI_SPRINTF(statusMsg, 0, "<p>%s.....", gettext("Waiting test result"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cmdbuf = as_cmd_selftest_from_uart();
	if(cmdbuf == NULL || cmdbuf->op == CMD_SELFTEST_BUSY )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Test Failed"));
		return 1;
	}
	else
	{
		cgi_error_page(NULL, gettext(CGI_STR_SUCCESS), gettext("Test Success"));
		return 0;
	}
}

int	cgiPbxHwCheckDtmf(HWCHECK_INFO *info)
{

	if(__check_dtmf_channel(info, 0 , 0) )
		return -1;
	
	if(__check_dtmf_channel(info, 0 , 1) )
		return -1;
	
	if(__check_dtmf_channel(info, 1 , 2) )
		return -1;
	
	if(__check_dtmf_channel(info, 1 , 3) )
		return -1;
	
	return 0;
}

