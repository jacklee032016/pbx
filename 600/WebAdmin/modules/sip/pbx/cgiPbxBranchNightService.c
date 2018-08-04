/*
* $Id: cgiPbxBranchNightService.c,v 1.3 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_branch_night_service_status(PBX_INFO *info)
{
	char 	buf[8192*10];
	int 		length = 0;
	int 		j;
	char		*value;
	int startHour = 0, startMinute = 0, endHour = 0, endMinute = 0;

	llist 	*pbxCfgs = &info->cfgs;
	length += CGI_SPRINTF(buf, length, "<form name=\"branchSpecialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>\n", 
		gettext("Branch Index"), gettext(CGI_STR_PHONE_NUMBER), gettext(CGI_STR_TYPE) );

	for( j = 0;j< info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" title=\"%s\">%s-%d</TD>\n", 
			gettext("Please Input IP or PSTN Phone Number"),gettext("branch"), j+1 );

#if __WITH_NEW_STC__
		/* Night Service */
		length += CGI_SPRINTF(buf,length, "%s</TR>", transfer_button( info, j, PBX_EXT_S_NIGHT) );
#endif
	}

	length += CGI_SPRINTF(buf,length, "</TABLE><br>\r\n" );

	value = GET_VALUE(pbxCfgs, PBX_GLOBAL_NIGHT_TIME);
	if(value && (strlen(value)>0) )
	{
		char *p =strrchr(value, ':' );
		endMinute = atoi(p+1);
		*p = 0;
		
		p =strrchr(value, ':' );
		endHour = atoi(p+1);
		*p = 0;
		
		p =strrchr(value, ':' );
		startMinute = atoi(p+1);
		*p = 0;
		
		startHour = atoi(value);
	}
	
	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>\n", 
		gettext("Night Service Time") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"2\" value=\"%d\" maxlength=\"2\">%s<input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"2\" value=\"%d\" maxlength=\"2\">%s</TD>\n", 
		gettext("Start Time"), START_HOUR, startHour, gettext("Hour"), START_MINUTE, startMinute, gettext("Minute") );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s</strong></TD<TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"2\" value=\"%d\" maxlength=\"2\">%s<input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"2\" value=\"%d\" maxlength=\"2\">%s\n", 
		gettext("End Time"),END_HOUR, endHour, gettext("Hour"), END_MINUTE, endMinute, gettext("Minute") );
	length += CGI_SPRINTF(buf,length, "</TD></TR></TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM></TABLE>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_BRANCH_SPEC) );

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s!<br>* %s, %s.<br>* %s, %s, %s\r\n",
		gettext(CGI_STR_NOTE), gettext("Destination of Call Transfered can be Branch,IP,PSTN Phone Number"), 
		gettext("If want to transfer to another local branch"), gettext("Please input branch Index"), 
		gettext("If want to transfer to IP or PSTN"), gettext("Please input phone number as requirement of Dial Plan"), 
		gettext("Normally it is more than 5 and less than 18 digits"));
	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n" );

	return strdup(buf);
}


int	cgiPbx_BranchNightService(PBX_INFO *info)
{
	return cgi_info_page(gettext("Call Transfer Configuration of PBX"), gettext("Call Transfer of PBX are set here"), __pbx_branch_night_service_status(info) );
}

