/*
* $Id: hwCheckPcm.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "_hwCheck.h"

static char *__cgi_pbx_hw_pcm_status(HWCHECK_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		i;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX_HW);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP( &info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);

/* table1 : Recording Parameters */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Select PCM Channel test") );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext(CGI_STR_INDEX)  );

	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:50px;'>\n", PBX_EXT_INDEX );
	for(i=0; i< PBX_PCM_CHANNLE_MAX; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" >%d</option>\n",i, i+1);
	}
	
	length += CGI_SPRINTF(buf, length, "\t</select>\n</TD></TR></TABLE>\n" );

/* TABLE 2 : Button info */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_BEGIN) ) );
	length += CGI_SPRINTF(buf,length, "\t</TD></TR></FORM>\r\n" );

	return strdup(buf);
}


int	cgiPbxHwCheckPcm(HWCHECK_INFO *info)
{
	char		*action;
	char		target[512];

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Hardware Check"), gettext("PCM Channel Test"), __cgi_pbx_hw_pcm_status(info) );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_ACTIVATE)  )
	{
		int index = atoi( GET_VALUE(&info->cgiVariables, PBX_EXT_INDEX) );
		if(index >= 0 && index < PBX_PCM_CHANNLE_MAX)
		{
			cgiPbxSendCheckCmd(info, SELF_TEST_TYPE_PCM_CHANNEL, index, PBX_VALUE_INVALIDATE);
			sleep(1);
			pcm_test_one_step(index);

			sprintf(target, WEB_URL_PBX_HW"?"CGI_KEYWORD_OPERATOR"=%s&%s=%s", GET_CGI_OP(&info->cgiVariables), CGI_ACTION, CGI_ACTION_MODIFY );
			return cgi_refresh_page(3, target, gettext(CGI_STR_WAIT), gettext("Hardware Check"));
		}
		else
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Invalidate parameters"));
		}
	}
	else
	{
		PBX_COMMAND *cmdbuf;

		cmdbuf = as_cmd_selftest_from_uart();
		if(cmdbuf == NULL || cmdbuf->op == CMD_SELFTEST_BUSY )
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("PCM Channel Test Failed"));
		}
		else
		{
			cgi_error_page(NULL, gettext(CGI_STR_SUCCESS), gettext("PCM Channel Test Success"));
		}
	}

	return 0;
}


