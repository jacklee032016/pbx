/*
* $Id: cgiPbxLines.c,v 1.6 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_lines_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i, j;
	char		key[32];
	char		*value;
	int		isOn;
	llist *pbxCfgs = &info->cfgs;
	
	length += CGI_SPRINTF(buf, length, "<form name=\"linesForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_LINES);

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Line"), gettext(CGI_STR_STATUS), gettext("Console"), gettext("Bind Group") );

	for( j = 0;j< info->linesCount/*PBX_LINE_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\"><strong>%s-%d</strong></TD>\n", gettext("Line"), j+1 );

		/* Status */
		sprintf(key, "%s_%d", PBX_LINE_STATUS, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:70px;'>\n", key );

		value = GET_VALUE(pbxCfgs, key);
		isOn = (!strcasecmp(value, CGI_STR_ON) )?1:0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_ON, (isOn)?"selected":"", gettext(CGI_STR_ENABLE));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_OFF, (isOn)?"":"selected", gettext(CGI_STR_DISABLE));
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

		/* Console */
		sprintf(key, "%s_%d", PBX_LINE_CONSOLE, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:70px;'>\n", key );

		value = GET_VALUE(pbxCfgs, key);
		isOn = atoi(value);
		for(i= 0; i< PBX_BRANCH_MAX; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i,(isOn==i)?"selected":"", i+1 );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

		/* Bind Group */
		sprintf(key, "%s_%d", PBX_LINE_GROUP, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:70px;'>\n", key );
		value = GET_VALUE(pbxCfgs, key);
		isOn = atoi(value);
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"0\" %s>%s</option>\n",(isOn==0)?"selected":"", gettext(CGI_STR_DISABLE) );
		for(i= 0; i< info->groupCount/*PBX_GROUP_MAX*/; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i+1,(isOn==(i+1) )?"selected":"", i+1 );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
#if 0
		length += CGI_SPRINTF(buf,length, "<TD align=\"center\" ><strong>%s</strong></TR>\r\n", 
			cgi_button(gettext(CGI_STR_RESTORE_DEFAULT), "") );
#endif
	}
	
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );
	
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=10 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_LINES) );

	return strdup(buf);
}


int	cgiPbx_Line(PBX_INFO *info)
{
	cgi_info_page(gettext("External Lines Configuration of PBX"), gettext("External Lines parameters of PBX are set here"), __pbx_lines_status(info) );

	return 0;
}

