/*
* $Id: cgiPbxBlackList.c,v 1.2 2007/05/31 20:38:06 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_blacklist_status(PBX_INFO *info)
{
	char 	buf[8192*10];
	int 		length = 0;
	int 		j;
	char		key[32];

	llist 	*pbxCfgs = &info->cfgs;
	length += CGI_SPRINTF(buf, length, "<form name=\"branchSpecialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_PHONE_NUMBER) );

	for( j = 0;j< STC_BLACKLIST_COUNT; j++)
	{
		/* Deny Call In */
		sprintf(key, "%s_%d", PBX_BLACKLIST, j );
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%d</TD><TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"18\" value=\"%s\" maxlength=\"18\"></TD></TR>\r\n", 
			j+1, key, GET_VALUE(pbxCfgs, key));
	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM></TABLE>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_BRANCH_SPEC) );

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s.\r\n",
		gettext(CGI_STR_NOTE), gettext("BlackList in IP,PSTN Phone Number which will by denied call-in"), 
		gettext("Normally it is more than 5 and less than 18 digits"));
	length += CGI_SPRINTF(buf,length, "</TD></TR>\r\n" );

	return strdup(buf);
}


int	cgiPbx_BlackList(PBX_INFO *info)
{
	return cgi_info_page(gettext("Black List Configuration of PBX"), gettext("Black List of PBX are set here"), __pbx_blacklist_status(info) );
}

