/*
* $Id: cgiPbxTrunck.c,v 1.9 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"
/*
* Optional value is 'direct dial'(1) or 'virtual console mode'(0), default is direct dial(1)
*/

static char *__pbx_trunck_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	char		target[128];
	int		isDirect;
	llist 		*pbxCfgs = &info->cfgs;

	isDirect = (!strcasecmp(GET_VALUE(pbxCfgs,PBX_TRUNCK_MODE), PBX_TRUNCK_DIRECT)  )?1:0;

	length += CGI_SPRINTF(buf, length, "<form name=\"trunckForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_TRUNCK);

	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"65%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Trunck Configuration of PBX") );
	length += CGI_SPRINTF(buf,length, "<TR><TD ><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR><TR><TD ><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		PBX_TRUNCK_MODE, PBX_TRUNCK_DIRECT, (isDirect)?"checked":"", gettext("Direct Dial(DID)"), PBX_TRUNCK_MODE, PBX_TRUNCK_CONSOLE, (isDirect)?"":"checked", gettext("Virtual Console") );

#if __WITH_NEW_STC__
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"65%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("PSTN Console Agent") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s:</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
		gettext("Agent Phone Number"), PBX_AGENT_PHONE_NUMBER, GET_VALUE(pbxCfgs,PBX_AGENT_PHONE_NUMBER) );

	isDirect = atoi( GET_VALUE(pbxCfgs,PBX_AGENT_BRANCH_INDEX)) +1;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s:</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%d\" maxlength=\"50\"></TD></TR>\n", 
		gettext("Agent Branch Index"), PBX_AGENT_BRANCH_INDEX, isDirect );
#endif

	sprintf(target, "%s?%s=%s&%s=%s",WEB_URL_PBX, CGI_KEYWORD_OPERATOR, CGIPBX_OP_TRUNCK, CGI_ACTION, CGI_ACTION_DEFAULT );

	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s%s\r\n</TD></TR></FORM>\r\n",
		cgi_submit_button( gettext(CGI_STR_ACTIVATE) ), cgi_help_button(CGI_HELP_MSG_PBX_TRUNCK) );

#if __WITH_NEW_STC__
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* <strong>%s</strong> : (Direct-Inward Dial), %s<br>* <strong>%s</strong> : %s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), 
		gettext("Direct Dial(DID)"),
		gettext("After dialing in, extensions or extension group will ringing one by one"),
		gettext("Virtual Console"),
		gettext("After dialing in, the alert voice will be heard"), 
		gettext("then you can redial a extension, PSTN or IP as need"));
#endif
	
	return strdup(buf);
}

int	cgiPbx_Trunck(PBX_INFO *info)
{
	return cgi_info_page(gettext("Trunck Configuration of PBX"), "", __pbx_trunck_status(info) );
}

