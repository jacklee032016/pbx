/*
* $Id: fwButtons.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/
#include "cgi_lib.h"
#include "cgi_iptables.h"

/* input rule name and desciption */
char *fw_rule_name_buttons(FW_INFO *info )
{
	char 	buf[4096];
	int 		length = 0;
	char		*op = GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR);

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2>%s:%s</TD></TR>\r\n", gettext(op), gettext("Please Input Rule Name") );

	length += CGI_SPRINTF(buf, length, "<form name=\"natSelectForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, op);
	
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SELECT );

	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_TYPE, op);//(!strcasecmp(op,FIREWALL_OP_SNAT))?IPT_CFG_NAT_RULE:IPT_CFG_OTHER_RULE );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(IPT_CFG_RULE_NAME) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_CFG_RULE_NAME );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(IPT_CFG_RULE_DESCRIPTOR) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_CFG_RULE_DESCRIPTOR);

	length += CGI_SPRINTF(buf,length, "</TD></TR><TR><TD align=\"center\" colspan=2>%s\r\n", cgi_submit_button( gettext(CGI_STR_NEXT) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>", cgi_help_button(CGI_HELP_MSG_SNAT) );

	return strdup(buf);
}

