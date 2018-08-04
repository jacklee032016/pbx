/*
* $Id: cgiPbxBranchBasic.c,v 1.10 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"

#if __WITH_NEW_STC__
#else
enum
{
	EXT_AUTHOR_ALL				=	1,
	EXT_AUTHOR_LONG_DISTANCE	=	2,
	EXT_AUTHOR_LOCAL				=	3,
	EXT_AUTHOR_PSTN				=	4,
	EXT_AUTHOR_INTERNAL			=	5,
	EXT_AUTHOR_DENY_ALL			=	6	/* Deny all call */
};

#endif

typedef struct
{
	int		level;
	char		*description;
}branch_authority_t;


branch_authority_t _authorities [] =
{
	{EXT_AUTHOR_ALL, 				CGI_STR_ALL	},
	{EXT_AUTHOR_LONG_DISTANCE,		_("Long Distance")	},
	{EXT_AUTHOR_LOCAL, 				CGI_STR_LOCAL},
	{EXT_AUTHOR_PSTN,				_("PSTN")},
	{EXT_AUTHOR_INTERNAL, 			_("Branch")},
	{EXT_AUTHOR_DENY_ALL, 			"Cancel"},
};

static char *__pbx_branch_basic_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
//	char		target[128];
	int 		i, j;
	char		key[32];
	int		isOn;
	llist 		*pbxCfgs = &info->cfgs;
	
	length += CGI_SPRINTF(buf, length, "<form name=\"branchBasicForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Branch Index"), gettext(CGI_STR_PHONE_NUMBER), gettext("Timeout(minute)"), gettext(CGI_STR_AUTHORITY), gettext("Call ID") );

	for( j = 0;j<info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\n", gettext("branch"), j+1 );

		/* phone number */
		sprintf(key, "%s_%d", PBX_EXT_B_PHONE_NUMBER, j);
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"3\" disabled></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* TIMEOUT */
		sprintf(key, "%s_%d", PBX_EXT_B_LENGTH, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"2\" value=\"%s\" maxlength=\"2\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* AUTHORITY */
		sprintf(key, "%s_%d", PBX_EXT_B_SCOPE, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:50px;'>\n", key );
		isOn = atoi(GET_VALUE(pbxCfgs, key));
		for(i= 0; i< 6; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _authorities[i].level, (isOn==_authorities[i].level)?"selected":"",gettext(_authorities[i].description) );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

		/* CallId */
		sprintf(key, "%s_%d", PBX_EXT_B_CALLID, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:50px;'>\n", key );
		isOn = (!strcasecmp(GET_VALUE(pbxCfgs, key), CGI_STR_ON) )?1:0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_ON, (isOn)?"selected":"",  gettext(CGI_STR_ENABLE));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_OFF, (isOn)?"":"selected",  gettext(CGI_STR_DISABLE));
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
#if 0
		sprintf(target, "%s?%s=%s&%s=%s&%s=%d",WEB_URL_PBX, CGI_KEYWORD_OPERATOR, CGIPBX_OP_BRANCH_BASE, CGI_ACTION, CGI_ACTION_DEFAULT, 
			CGIPBX_SUB_OP_PHONE_INDEX, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><strong>%s</strong></TD></TR>\r\n", 
			cgi_button(gettext(CGI_STR_RESTORE_DEFAULT), target ) );
#endif		

	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM></TABLE>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_BRANCH_BASIC) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext("Timeout(minute)"),
		gettext("is between 1~99 and 0 is disabled"));

	return strdup(buf);
}


int	cgiPbx_BranchBasic(PBX_INFO *info)
{
	return cgi_info_page(gettext("Branch Basic Configuration of PBX"), gettext("Branch Basic parameters of PBX are set here"), __pbx_branch_basic_status(info) );
}

