/*
* $Id: cgiPbxBranchExtTransfer.c,v 1.2 2007/11/15 08:18:50 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__ext_transfer_button(PBX_INFO *info, int extIndex, char *transferType)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i, index;
	char		key[32], *value;

	llist 	*pbxCfgs = &info->cfgs;
	
	sprintf(key, "%s_%d", transferType, extIndex );
	value = GET_VALUE(pbxCfgs,key);
	
	length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:65px;'>\n", key );
	if( !value || strlen(value)==0)
		index = 0xFF;
	else
		index = atoi(value);
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"0xFF\" %s>%s</option>\n", (index==0xFF)?"selected":"", gettext(CGI_STR_NO) );
	for(i= 0; i< info->extsCount; i++)
	{
		if(i != extIndex)
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s%d</option>\n", i,(i==index)?"selected":"", gettext("branch"), i+1 );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

	return strdup(buf);
}

static char *__pbx_branch_ext_transfer_status(PBX_INFO *info)
{
	char 	buf[8192*10];
	int 		length = 0;
	int 		j;

	length += CGI_SPRINTF(buf, length, "<form name=\"branchSpecialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Branch Index"), gettext("Secret"), gettext("Busy") );

	for( j = 0;j< info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\n", gettext("branch"), j+1 );

		/* Secret */
		length += CGI_SPRINTF(buf,length, "%s", __ext_transfer_button(info, j, PBX_EXT_S_SECRET) );

		/* Busy */
		length += CGI_SPRINTF(buf,length, "%s", __ext_transfer_button(info, j, PBX_EXT_S_BUSY) );

	}
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM></TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"95%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>* '%s'</strong>,<strong>'%s'</strong> %s.</TD></TR>\r\n",
		gettext("Secret"), gettext("Busy"), gettext("can only use local branch") );

	return strdup(buf);
}


int	cgiPbx_BranchExtTransfer(PBX_INFO *info)
{
	return cgi_info_page(gettext("Branch Transfer Configuration of PBX"), gettext("Branch Transfer of PBX are set here"), __pbx_branch_ext_transfer_status(info) );
}

