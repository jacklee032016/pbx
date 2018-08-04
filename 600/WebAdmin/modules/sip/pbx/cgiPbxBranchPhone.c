/*
* $Id: cgiPbxBranchPhone.c,v 1.8 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"


typedef struct
{
	int		mode;
	char		*description;
}dial_mode_t;

dial_mode_t  _dialOutMode [] =
{
#if __WITH_NEW_STC__
	{EXT_FIRST_PSTN, 	_("PSTN First")},
	{EXT_FIRST_IP,		_("IP First")},
	{DIAL_DIGIT_ZERO, 	_("Dial 0")},
#else
	{0x0a, 				_("PSTN First")	},
	{0x0b,				_("IP First")	},
	{0, 					_("Dial 0")},
#endif	
	{1,					_("Dial 1")},
	{2, 					_("Dial 2")},
	{3,					_("Dial 3")},
	{4, 					_("Dial 4")},
	{5,					_("Dial 5")},
	{6, 					_("Dial 6")},
	{7,					_("Dial 7")},
	{8, 					_("Dial 8")},
	{9,					_("Dial 9")},
};

static char *__pbx_branch_basic_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i, j;
	char		key[32];
	llist 		*pbxCfgs = &info->cfgs;
	
	length += CGI_SPRINTF(buf, length, "<form name=\"branchBasicForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t\n</TR>\n", 
		gettext("Branch Index"), gettext(CGI_STR_PHONE_NUMBER), gettext("Out Mode") );

	for( j = 0;j<info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		int dialoutMode;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\n", gettext("branch"), j+1 );

		/* phone number */
		sprintf(key, "%s_%d", PBX_EXT_B_PHONE_NUMBER, j);
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"3\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* Dial Out Mode */
		sprintf(key, "%s_%d", PBX_EXT_B_DIALOUT, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:70px;'>\n", key );

		dialoutMode = atoi(GET_VALUE(pbxCfgs, key));
		for(i= 0; i< 12; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _dialOutMode[i].mode,(_dialOutMode[i].mode==dialoutMode )?"selected":"" , gettext(_dialOutMode[i].description) );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\">\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM></TABLE>\r\n"  );


	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s.<br>* %s, %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext(CGI_STR_PHONE_NUMBER),
		gettext("can only used when local dialing and be no more than 3 digits"), gettext("Telephone Number will be displayed when CallID is enabled"),
		gettext("But only last digit is used when dialing with '*'"));

	return strdup(buf);
}


int	cgiPbx_BranchPhoneNumber(PBX_INFO *info)
{
	return cgi_info_page(gettext("Branch Telephone Number Configuration of PBX"), gettext("Branch Basic parameters of PBX are set here"), __pbx_branch_basic_status(info) );
}

