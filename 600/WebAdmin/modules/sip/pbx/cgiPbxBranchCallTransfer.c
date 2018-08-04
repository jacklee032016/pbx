/*
* $Id: cgiPbxBranchCallTransfer.c,v 1.10 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"

#if __WITH_NEW_STC__
char *transfer_button(PBX_INFO *info, int extIndex, char *transferType)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i, index;
	char		key[32], *value, type[32];

	llist 	*pbxCfgs = &info->cfgs;
	
	sprintf(key, "%s_%d", transferType, extIndex );
	value = GET_VALUE(pbxCfgs,key);
	
	sprintf(type, "%s", (strchr(value, '_' )+1));
	index = strchr(value, '_') -value;
	value[index] = 0;
	
	length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"18\"></TD>\r\n", 
		key, value );

	sprintf(key, "%s_Type_%d", transferType, extIndex );
	length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\"><select name=\"%s\" style='width:65px;'>\n", key );
	for(i= 0; i< sizeof(cgi_transfer_types)/sizeof(cgi_transfer_type_t); i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", cgi_transfer_types[i].descriptor, (!strcasecmp(cgi_transfer_types[i].descriptor, type))?"selected":"", gettext(cgi_transfer_types[i].descriptor ));
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

	return strdup(buf);
}


static char *__pbx_branch_special_status(PBX_INFO *info)
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
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\" rowspan=2><strong>%s</strong></TD>\n\t<td colspan=2 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td colspan=3 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Branch Index"), gettext("Unconditional"), gettext("Left"));

	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext(CGI_STR_PHONE_NUMBER), gettext(CGI_STR_TYPE ),gettext(CGI_STR_PHONE_NUMBER), gettext("Type"),gettext("Left Time"));

	for( j = 0;j< info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" title=\"%s\">%s-%d</TD>\n", 
			gettext("Please Input Branch, IP or PSTN Phone Number"),gettext("branch"), j+1 );

		/* Unconditional */
		length += CGI_SPRINTF(buf,length, "%s", transfer_button( info, j, PBX_EXT_S_NOCONDITION) );

		/* Left */
		length += CGI_SPRINTF(buf,length, "%s", transfer_button( info, j, PBX_EXT_S_NORESPONSE) );

		/* Left time */
		sprintf(key, "%s_%d", PBX_EXT_S_NORESTIME, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"3\"></TD></TR>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
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

#else
static char *__pbx_branch_special_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
//	char		target[128];
	int 		j;
	char		key[32];

	llist 	*pbxCfgs = &info->cfgs;
	length += CGI_SPRINTF(buf, length, "<form name=\"branchSpecialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Branch Index"), gettext("Unconditional"), gettext("Night Service"), gettext("Deny CallIn") , gettext("Left"),gettext("Left Time"));

	for( j = 0;j< info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" title=\"%s\">%s-%d</TD>\n", 
			gettext("Please Input Branch, IP or PSTN Phone Number"),gettext("branch"), j+1 );

		/* Unconditional */
		sprintf(key, "%s_%d", PBX_EXT_S_NOCONDITION, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"18\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* Night Service */
		sprintf(key, "%s_%d", PBX_EXT_S_NIGHT, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"18\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* Deny Call In */
		sprintf(key, "%s_%d", PBX_EXT_S_DENYCALLIN, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"18\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* Left */
		sprintf(key, "%s_%d", PBX_EXT_S_NORESPONSE, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"18\"></TD>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );

		/* Left time */
		sprintf(key, "%s_%d", PBX_EXT_S_NORESTIME, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"3\" value=\"%s\" maxlength=\"3\"></TD></TR>\r\n", 
			key, GET_VALUE(pbxCfgs,key) );
#if 0
		sprintf(target, "%s?%s=%s&%s=%s&%s=%d",WEB_URL_PBX, CGI_KEYWORD_OPERATOR, CGIPBX_OP_BRANCH_SPEC, CGI_ACTION, CGI_ACTION_DEFAULT, 
			CGIPBX_SUB_OP_PHONE_INDEX, j );
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</strong></TR>\r\n", 
			cgi_button(gettext(CGI_STR_RESTORE_DEFAULT), target) );
#endif		

	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM></TABLE>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_BRANCH_SPEC) );

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s!<br>* %s, %s.<br>* %s, %s, %s\r\n",
		gettext(CGI_STR_NOTE), gettext("Destination of Call Transfered can be Branch,IP,PSTN Phone Number"), 
		gettext("If want to transfer to another local branch"), gettext("Please input branch phone number which length is less than 5 digits"), 
		gettext("If want to transfer to IP or PSTN"), gettext("Please input phone number as requirement of Dial Plan"), 
		gettext("Normally it is more than 5 and less than 18 digits"));
	length += CGI_SPRINTF(buf,length, "<br><strong>* '%s'</strong>,<strong>'%s'</strong> %s.</TD></TR>\r\n",
		gettext("Secret"), gettext("Busy"), gettext("can only use local branch") );

	return strdup(buf);
}
#endif

int	cgiPbx_BranchCallTransfer(PBX_INFO *info)
{
	return cgi_info_page(gettext("Call Transfer Configuration of PBX"), gettext("Call Transfer of PBX are set here"), __pbx_branch_special_status(info) );
}

