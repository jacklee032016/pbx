/*
* $Id: fwMiddleButtons.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "fwMiddle.h"

#define	_HTML_SERVICES(len, buf, service, list)	\
	do{	\
		len += CGI_SPRINTF(buf, len, "<tr>\n\t<td align=\"center\" colspan=2>%s</TD>\n\n\t<td align=\"center\">%s</TD>\n\t<td align=\"center\">%s</TD>\n</TR>\n",   \
		 	gettext(service), gettext(GET_VALUE(list, service)), gettext(CGI_STR_MANAGEMENT) );	\
	}while(0)
		
char *fwMiddle_info_buttons(iptables_item *header, llist *serviceCfg)
{
		
	char 		buf[4096];
	int 			length = 0;
	iptables_item *current = header;
	char			target[256];

	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\" colspan=2><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n</TR>\n", 
		gettext("Service Name"), gettext(CGI_STR_AUTHORITY), gettext(CGI_STR_MANAGEMENT) );

	_HTML_SERVICES(length, buf,IPT_SERVICE_WWW, serviceCfg);
	_HTML_SERVICES(length, buf,IPT_SERVICE_FTP, serviceCfg);
	_HTML_SERVICES(length, buf,IPT_SERVICE_EMAIL, serviceCfg);
	_HTML_SERVICES(length, buf,IPT_SERVICE_QQ, serviceCfg);

	if(!current)
		return strdup(buf);
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_IP_ADDRESS), gettext(CGI_STR_MAC_ADDRESS), gettext(CGI_STR_AUTHORITY), gettext(CGI_STR_MANAGEMENT) );
	while(current )
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s &nbsp;</td>", current->interAddr);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", gettext(current->mac) );
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s</td>",current->action);

		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_SNAT, CGI_ACTION, CGI_ACTION_DEL);

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

		current = current->next;
	}

	return strdup(buf);
}


char *fwMiddle_services_buttons(FW_INFO *info )
{
	char 		buf[4096];
	int 			length = 0;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2>%s:%s</TD></TR>\r\n", gettext(CGI_STR_FW_MIDDLE_CONFIG), gettext("Please Select the Authority of Services") );

	length += CGI_SPRINTF(buf, length, "<form name=\"middle1Form\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_MIDDLE);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, IPT_MIDDLE_SERVICES );
	
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_NAME, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_TYPE, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_TYPE) );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n\t<TD align=\"center\"><strong>%s</strong></TD>\n\t</TR>\n", 
		gettext("Services"), gettext(CGI_STR_AUTHORITY) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n\t<TD align=\"center\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext(IPT_SERVICE_WWW), IPT_SERVICE_WWW, IPTABLES_KEYWORD_RULE_ACCEPT, "checked", gettext(IPTABLES_KEYWORD_RULE_ACCEPT), IPT_SERVICE_WWW, IPTABLES_KEYWORD_RULE_DROP, "", gettext(IPTABLES_KEYWORD_RULE_DROP));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n\t<TD align=\"center\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext(IPT_SERVICE_FTP),  IPT_SERVICE_FTP, IPTABLES_KEYWORD_RULE_ACCEPT, "checked", gettext(IPTABLES_KEYWORD_RULE_ACCEPT), IPT_SERVICE_FTP, IPTABLES_KEYWORD_RULE_DROP, "", gettext(IPTABLES_KEYWORD_RULE_DROP));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n\t<TD align=\"center\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext(IPT_SERVICE_EMAIL),  IPT_SERVICE_EMAIL, IPTABLES_KEYWORD_RULE_ACCEPT, "checked", gettext(IPTABLES_KEYWORD_RULE_ACCEPT), IPT_SERVICE_EMAIL, IPTABLES_KEYWORD_RULE_DROP, "", gettext(IPTABLES_KEYWORD_RULE_DROP));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n\t<TD align=\"center\"><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext(IPT_SERVICE_QQ), IPT_SERVICE_QQ, IPTABLES_KEYWORD_RULE_ACCEPT, "checked", gettext(IPTABLES_KEYWORD_RULE_ACCEPT), IPT_SERVICE_QQ, IPTABLES_KEYWORD_RULE_DROP, "", gettext(IPTABLES_KEYWORD_RULE_DROP));

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><br></TD></TR><TR><TD align=\"center\" colspan=2>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_SNAT) );

	return strdup(buf);
}


#define	SERVICES_HTML_WRITE(len, buf, list, service)	\
	do{	\
		len += CGI_SPRINTF(buf, len, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", service, GET_VALUE(list, service)); \
		}while(0)

char *fwMiddle_address_buttons(FW_INFO *info, llist *middleCfgs )
{
	char 		buf[4096];
	int 			length = 0;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2>%s:%s</TD></TR>\r\n", gettext(CGI_STR_FW_MIDDLE_CONFIG), gettext("Add host with this configuration") );

	length += CGI_SPRINTF(buf, length, "<form name=\"middle2Form\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_MIDDLE );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	SERVICES_HTML_WRITE(length, buf, middleCfgs, IPT_SERVICE_WWW);
	SERVICES_HTML_WRITE(length, buf, middleCfgs, IPT_SERVICE_FTP);
	SERVICES_HTML_WRITE(length, buf, middleCfgs, IPT_SERVICE_EMAIL);
	SERVICES_HTML_WRITE(length, buf, middleCfgs, IPT_SERVICE_QQ);
	
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_NAME, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_TYPE, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_TYPE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong>\r\n", gettext(CGI_STR_IP_ADDRESS) );
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkIP(%s.value,this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_KW_InterAddress, IPT_KW_InterAddress);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong>\r\n", gettext(CGI_STR_MAC_ADDRESS) );
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\" colspan=3><input name=\"%s\" onKeyPress=\"isDigit(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_KW_MAC_ADDRESS );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext(CGI_STR_AUTHORITY), IPT_KW_TARGET, IPTABLES_KEYWORD_RULE_ACCEPT, "checked", gettext(IPTABLES_KEYWORD_RULE_ACCEPT), IPT_KW_TARGET, IPTABLES_KEYWORD_RULE_DROP, "", gettext(IPTABLES_KEYWORD_RULE_DROP));


	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_SNAT) );

	return strdup(buf);
}

