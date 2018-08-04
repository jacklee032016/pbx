/*
* $Id: cgiPbxAccounts.c,v 1.7 2007/09/01 08:48:47 lizhijie Exp $
*/

#include "cgi_pbx.h"

typedef struct
{
	int		level;
	char		*description;
}account_author_t;

account_author_t  _accountAuthors [] =
{
	{1,					_("International")},
	{2, 					_("Long Distance")},
	{3,					CGI_STR_LOCAL},
	{4, 					_("PSTN")},
	{5,					_("Branch")},
	{6, 					_("Cancel")}
};


static char *__pbx_account_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i, j;
	char		key[32];
	llist 		*pbxCfgs= &info->cfgs;

	length += CGI_SPRINTF(buf, length, "<form name=\"accountForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_ACCOUNT);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Account"), gettext(CGI_STR_PASSWORD), gettext(CGI_STR_AUTHORITY) );

	for( j = 0;j< /*info->*/ PBX_ACCOUNT_MAX; j++)
	{

		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\n", gettext("Account"), j+1 );

		/* password */
		sprintf(key, "%s_%d", PBX_ACCOUNT_PASSWD, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* authority */
		sprintf(key, "%s_%d", PBX_ACCOUNT_AUTH, j );
		int index = atoi(GET_VALUE(pbxCfgs, key) );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:100px;'>\n", key );
		for(i= 0; i< 6; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _accountAuthors[i].level, 
				(index==_accountAuthors[i].level)?"selected":"", gettext(_accountAuthors[i].description) );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	}
	
	length += CGI_SPRINTF(buf,length, "</TABLE><table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_ACCOUNT) );

	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext(CGI_STR_PASSWORD),
		gettext("are some dialed digits with this authority"));

	return strdup(buf);
}


int	cgiPbx_Account(PBX_INFO *info)
{
	return cgi_info_page(gettext("Account Configuration of PBX"), gettext("Account parameters of PBX are set here"), __pbx_account_status(info) );
}

