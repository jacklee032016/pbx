/*
* $Id: cgiPbxGlobalDialPlan.c,v 1.5 2007/03/15 16:54:21 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_global_dialplan_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	llist 		*cfgs = &info->cfgs;
	int		pstnPrefix;

	length += CGI_SPRINTF(buf, length, "<form name=\"pbxBasicForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BASIC);

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\n", 
		gettext("Branch Count"), PBX_WEBADMIN_EXT_NUM, GET_VALUE(cfgs,PBX_WEBADMIN_EXT_NUM) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\n", 
		gettext("Lines Count"), PBX_WEBADMIN_LINE_NUM, GET_VALUE(cfgs,PBX_WEBADMIN_LINE_NUM) );

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\r\n", 
		gettext("IP Prefix"), PBX_GLOBAL_IP_PREFIX, GET_VALUE(cfgs,PBX_GLOBAL_IP_PREFIX) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", 
		gettext("PSTN Prefix") );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		PBX_GLOBAL_PSTN_PREFIX );
	pstnPrefix = atoi(GET_VALUE(cfgs, PBX_GLOBAL_PSTN_PREFIX));
	for(i= 0; i< 10; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i,(pstnPrefix==i)?"selected":"", i );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");


	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM>\r\n"  );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s.<br><strong>* '%s'</strong> %s.<br>* %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext("IP Prefix"),	gettext("means all number begin with IP Prefix will routed to IP"),
		gettext("PSTN Prefix"),	gettext("means all number begin with PSTN Prefix will routed to PSTN"), 
		gettext("Local Dial is always in the format of '*'+'last digit of Branch Number'"));

	return strdup(buf);
}

int	cgiPbx_GlobalDialPlan(PBX_INFO *info)
{
	return cgi_info_page(gettext("Global Dial Plan of PBX"), gettext("Global parameters of PBX are set here"), __pbx_global_dialplan_status(info) );
}

