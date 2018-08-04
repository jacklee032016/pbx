/*
* $Id: cgiPbxBasicConfig.c,v 1.13 2007/09/15 19:45:14 lizhijie Exp $
*/

#include "cgi_pbx.h"

static char *__pbx_global_status(PBX_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	char		target[128];
	int 		isFsk, isOn;
	llist 		*cfgs = &info->cfgs;
	char		*value;

	length += CGI_SPRINTF(buf, length, "<form name=\"pbxBasicForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PBX );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", PBX_CMDSET, PBX_CMDSET_BASIC);

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	value = GET_VALUE(cfgs, PBX_GLOBAL_VERSION_RELEASE);
	if(!value || strlen(value)>0 )
	{
		char		major[64], minor[64];
		sprintf(major, "%s", value);
		major[2] = 0;
		sprintf(minor, "%s", value+2);
		minor[2] = 0;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"right\"><strong>%s</strong>:%s.%s <strong>%s</strong>:%s</TD></TR>\n", 
			gettext("Firmware") ,gettext("Version"), major, minor, gettext("Date"), value+4 );
	}
	
#if __WITH_NEW_STC__
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\n", 
		gettext("Signal Type")  );

	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:50px;'>\n", PBX_GLOBAL_SIGNAL_TYPE );
	isOn = (!strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_SIGNAL_TYPE), CGI_STR_CHINESE) )?1:0;
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n",SIGNAL_TYPE_JAPANESE, (isOn)?"":"selected",  gettext(CGI_STR_JAPANESE));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n",SIGNAL_TYPE_CHINESE, (isOn)?"selected":"",  gettext(CGI_STR_CHINESE));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
#endif

	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_DIFFER_RING), CGI_STR_OFF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Differ Ringing"), PBX_GLOBAL_DIFFER_RING, CGI_STR_ON, (isOn)?"checked":"", gettext(CGI_STR_ON), PBX_GLOBAL_DIFFER_RING, CGI_STR_OFF, (isOn)?"":"checked", gettext(CGI_STR_OFF) );

	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_SPECIAL_FIRST), CGI_STR_OFF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Special First"), PBX_GLOBAL_SPECIAL_FIRST, CGI_STR_ON, (isOn)?"checked":"", gettext(CGI_STR_ON), PBX_GLOBAL_SPECIAL_FIRST, CGI_STR_OFF,(isOn)?"":"checked", gettext(CGI_STR_OFF) );

#if __WITH_NEW_STC__
	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_CALLID_MODE), CGI_STR_OFF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Call ID"), PBX_GLOBAL_CALLID_MODE, CGI_STR_ON, (isOn)?"checked":"", gettext(CGI_STR_ON), PBX_GLOBAL_CALLID_MODE, CGI_STR_OFF, (isOn)?"":"checked", gettext(CGI_STR_OFF) );
#else
	isFsk = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_CALLID_MODE), PBX_CALLID_DTMF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Call ID"), PBX_GLOBAL_CALLID_MODE, PBX_CALLID_FSK, (isFsk)?"checked":"", gettext(PBX_CALLID_FSK), PBX_GLOBAL_CALLID_MODE, PBX_CALLID_DTMF, (isFsk)?"":"checked", gettext(PBX_CALLID_DTMF) );
#endif

	isFsk = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_NODIAL_ACTION), PBX_NODIAL_BROKEN );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("No Dialing"), PBX_GLOBAL_NODIAL_ACTION, PBX_NODIAL_AUTO, (isFsk)?"checked":"", gettext(PBX_NODIAL_AUTO), PBX_GLOBAL_NODIAL_ACTION, PBX_NODIAL_BROKEN, (isFsk)?"":"checked", gettext(PBX_NODIAL_BROKEN) );

#if __WITH_NEW_STC__
	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_LINE_2_LINE), PBX_LINE2LINE_FORBIDDEN );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Line to Line"), PBX_GLOBAL_LINE_2_LINE, PBX_LINE2LINE_OPEN, (isOn)?"checked":"", gettext(PBX_LINE2LINE_OPEN), PBX_GLOBAL_LINE_2_LINE, PBX_LINE2LINE_FORBIDDEN, (isOn)?"":"checked", gettext(PBX_LINE2LINE_FORBIDDEN) );
#else
	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_LINE_2_LINE), CGI_STR_OFF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Line to Line"), PBX_GLOBAL_LINE_2_LINE, CGI_STR_ON, (isOn)?"checked":"", gettext(CGI_STR_ON), PBX_GLOBAL_LINE_2_LINE, CGI_STR_OFF, (isOn)?"":"checked", gettext(CGI_STR_OFF) );
#endif

	isOn = strcasecmp(GET_VALUE(cfgs, PBX_GLOBAL_GROUP_CALL), CGI_STR_OFF );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\n", 
		gettext("Group Calling"), PBX_GLOBAL_GROUP_CALL, CGI_STR_ON, (isOn)?"checked":"", gettext(CGI_STR_ON), PBX_GLOBAL_GROUP_CALL, CGI_STR_OFF, (isOn)?"":"checked", gettext(CGI_STR_OFF) );

#if __WITH_NEW_STC__
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
		gettext("PSTN Password"), PBX_GLOBAL_PASSWORD, GET_VALUE(cfgs,PBX_GLOBAL_PASSWORD) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
		gettext("IP Password"), PBX_GLOBAL_SYSTEM_PASSWD, GET_VALUE(cfgs,PBX_GLOBAL_SYSTEM_PASSWD) );
#else
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
		gettext(CGI_STR_PASSWORD), PBX_GLOBAL_PASSWORD, GET_VALUE(cfgs,PBX_GLOBAL_PASSWORD) );
	length += CGI_SPRINTF(buf,length, "<TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n", 
		gettext("System Password"), PBX_GLOBAL_SYSTEM_PASSWD, GET_VALUE(cfgs,PBX_GLOBAL_SYSTEM_PASSWD) );
#endif

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	sprintf(target, "%s?%s=%s&%s=%s&%s=%s",WEB_URL_PBX, CGI_KEYWORD_OPERATOR, CGIPBX_OP_BASE_CONFIG, CGI_ACTION, CGI_ACTION_RESET, PBX_CMDSET, PBX_CMDSET_RESET);
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s %s</TD></TR></FORM>\r\n" , cgi_button_alert(gettext(CGI_STR_RESTORE_DEFAULT), target, gettext(CGI_STR_ARE_YOU_SURE)), cgi_help_button(CGI_HELP_MSG_PBX_BASIC) );

#if 0	
	sprintf(target, "%s?%s=%s&%s=%s",WEB_URL_PBX, CGI_KEYWORD_OPERATOR, CGIPBX_OP_BASE_CONFIG, CGI_ACTION, CGI_ACTION_DEFAULT);
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">\t\t%s\n\t</TD>\n</TR>\r\n" , cgi_button(gettext(), target) );

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">\t\t%s\n\t</TD>\n</TR>\r\n" , cgi_button(gettext("Reset PBX"), target) );
#endif
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s, %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext(CGI_STR_RESTORE_DEFAULT),
		gettext("will reset all PBX parameters as factory default"),gettext("not only global parameters, but also that of Branches and Lines, etc."));

	return strdup(buf);

}


int	cgiPbx_BasicConfig(PBX_INFO *info)
{
	return cgi_info_page(gettext("Global Configuration of PBX"), gettext("Global parameters of PBX are set here"), __pbx_global_status(info) );
}

