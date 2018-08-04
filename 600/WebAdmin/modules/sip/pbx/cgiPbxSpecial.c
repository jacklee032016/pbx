/*
* $Id: cgiPbxSpecial.c,v 1.6 2007/03/15 16:54:21 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_special_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		j;
	char		key[32];
	llist 		*pbxCfgs = &info->cfgs;

	length += CGI_SPRINTF(buf, length, "<form name=\"specialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_SPECIAL);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_PHONE_NUMBER) );

	for( j = 0;j< PBX_SPECIAL_MAX; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%d</TD>\n", j+1 );

		/* special services */
		sprintf(key, "%s_%d", PBX_SPECIAL_SERVICE, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
			key, GET_VALUE(pbxCfgs,key) );
	}
	
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_SPECIAL_SERVICE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s, %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext("Special Service are telephone numbers that routed to PSTN directly"),
		gettext("eg. PSTN Prefix is not required"));

	return strdup(buf);
}


int	cgiPbx_SpecialService(PBX_INFO *info)
{
	return cgi_info_page(gettext("Special Service Configuration of PBX"), gettext("Special Service parameters of PBX are set here"), __pbx_special_status(info) );
}

