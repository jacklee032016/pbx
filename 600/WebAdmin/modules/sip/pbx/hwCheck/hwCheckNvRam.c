/*
* $Id: hwCheckNvRam.c,v 1.3 2007/09/04 20:46:17 lizhijie Exp $
*/
#include "_hwCheck.h"

int	cgiPbxHwCheckNvRam(HWCHECK_INFO *info)
{
	PBX_COMMAND *cmdbuf;

	char		statusMsg[1024];
	int		length = 0;

	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", gettext("Begin to NvRAM test"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cgiPbxSendCheckCmd(info, SELF_TEST_TYPE_NVRAM, 0, PBX_VALUE_INVALIDATE);

	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", gettext("Waiting test result"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cmdbuf = as_cmd_selftest_from_uart();
	if(cmdbuf == NULL || cmdbuf->op == CMD_SELFTEST_BUSY )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("NvRam Test Failed"));
	}
	else
	{
		cgi_error_page(NULL, gettext(CGI_STR_SUCCESS), gettext("NvRam Test Success "));
	}

	return 0;
}

int	cgiPbxHwCheckRTC(HWCHECK_INFO *info)
{
	PBX_COMMAND *cmdbuf;
	char		statusMsg[1024];
	int		length = 0;

	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", gettext("Begin to RTC test"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cgiPbxSendCheckCmd(info, SELF_TEST_TYPE_RTC, 0, PBX_VALUE_INVALIDATE);

	length += CGI_SPRINTF(statusMsg, length, "<p>%s.....", gettext("Waiting test result"));
	CGI_INSTANCE_INFO(statusMsg, CGI_COLOR_BLUE);

	cmdbuf = as_cmd_selftest_from_uart();
	if(cmdbuf == NULL || cmdbuf->op == CMD_SELFTEST_BUSY )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("RTC Test Failed"));
	}
	else
	{
		cgi_error_page(NULL, gettext(CGI_STR_SUCCESS), gettext("RTC Test Success"));
	}

	return 0;
}


