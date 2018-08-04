/*
* $Id: cgiPbxBranchSpec.c,v 1.7 2007/09/01 21:23:42 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_branch_special_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
//	char		target[128];
	int 		i, j;
	char		key[32];
	char		*value;
	int		isOn;

	llist 	*pbxCfgs = &info->cfgs;
	length += CGI_SPRINTF(buf, length, "<form name=\"branchSpecialForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGIPBX_OP_BRANCH_SPEC);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BRANCH_BASIC);

	length += CGI_SPRINTF(buf,length, "<table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
#if WITH_EXT_DIFFER_RINGING
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Branch Index"),gettext("Differ Ringing"), gettext("Group Call"), gettext("Delay to Line"),gettext("Call Wait"),gettext("No Disturb") );
#else
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Branch Index"),gettext("Group Call"), gettext("Delay to Line"),gettext("Call Wait"),gettext("No Disturb") );
#endif
	for( j = 0;j< info->extsCount/*PBX_BRANCH_MAX*/; j++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s-%d</TD>\n", gettext("branch"), j+1 );

#if WITH_EXT_DIFFER_RINGING
		/* Differ Dial : first Digit is Differ Ring */
		sprintf(key, "%s_%d", PBX_EXT_S_DIFFERTING, j );

		isOn = (!strcasecmp(GET_VALUE(pbxCfgs, key), CGI_STR_ON) )?1:0;
		
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:55px;'>\n", key );
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_ON, (isOn)?"selected":"", gettext(CGI_STR_ENABLE));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_OFF, (isOn)?"":"selected", gettext(CGI_STR_DISABLE));
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");
#endif

		/* Group : group is the second digit in Differ Ring */
		sprintf(key, "%s_%d", PBX_EXT_S_GROUP, j );
		isOn = atoi(GET_VALUE(pbxCfgs, key));
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:65px;'>\n", key );
		for(i= 0; i< 4; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s%d</option>\n", i,(i==isOn)?"selected":"", gettext("Group Call"), i+1 );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");


		/* Delay */
		sprintf(key, "%s_%d", PBX_EXT_B_DELAY2LINE, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:50px;'>\n", key );

		value = GET_VALUE(pbxCfgs, key);
		isOn = atoi(value);
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"0\" %s>%s</option>\n",(isOn==0)?"selected":"", gettext(CGI_STR_DISABLE));
		for(i= 1;i< (info->linesCount +1)/*i< 10*/; i++)
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i,(isOn==i)?"selected":"", i );
		}
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");
		
		/* Call Wait */
		sprintf(key, "%s_%d", PBX_EXT_B_CALLWAIT, j );
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:50px;'>\n", key );
		isOn = (!strcasecmp(GET_VALUE(pbxCfgs, key), CGI_STR_ON) )?1:0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_ON, (isOn)?"selected":"", gettext(CGI_STR_ENABLE));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_OFF, (isOn)?"":"selected", gettext(CGI_STR_DISABLE));
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

		/* No Disturb */
		sprintf(key, "%s_%d", PBX_EXT_B_NODISTURB, j );

		isOn = (!strcasecmp(GET_VALUE(pbxCfgs, key), CGI_STR_ON) )?1:0;
		length += CGI_SPRINTF(buf, length, "\t<TD align=\"center\">\n\t<select name=\"%s\" style='width:50px;'>\n", key );
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_ON, (isOn)?"selected":"", gettext(CGI_STR_ENABLE));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n",CGI_STR_OFF, (isOn)?"":"selected", gettext(CGI_STR_DISABLE));
		length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
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
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_BRANCH_SPEC) );

	return strdup(buf);
}


int	cgiPbx_BranchSpecial(PBX_INFO *info)
{
	return cgi_info_page(gettext("Branch Special Configuration of PBX"), gettext("Branch Special parameters of PBX are set here"), __pbx_branch_special_status(info) );
}

