/*
* $Id: fwExpertButtons.c,v 1.5 2007/09/06 09:19:58 lizhijie Exp $
*/
#include "fwExpert.h"

char		*expert_rules[] = 
{
	IPTABLES_KEYWORD_CHAIN_INPUT,
	IPTABLES_KEYWORD_CHAIN_OUTPUT,
	IPTABLES_KEYWORD_CHAIN_FORWARD
};

char		*expert_targets[] = 
{
	IPTABLES_KEYWORD_RULE_ACCEPT,
	IPTABLES_KEYWORD_RULE_DROP,
	IPTABLES_KEYWORD_RULE_REJECT
};

char *fwExpert_info(iptables_item *header )
{
	char 		buf[4096];
	int 			length = 0;
	iptables_item *current = header;
	char			target[256];

	if(!current)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(IPT_KW_RULE), gettext(IPT_KW_TARGET), gettext(IPT_KW_PROTOCOL), gettext(CGI_STR_SOURCE_ADDRESS),gettext("Source Port"),gettext(CGI_STR_DESTIN_ADDRESS), gettext("Destination Port"), gettext(CGI_STR_MANAGEMENT)  );

cgitrace;
	while(current )
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s</td>",current->rule );
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", gettext(current->action) );
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", gettext(current->protocol) );

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->interAddr);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s%s%s</td>", current->interPortBegin,
			(strlen(current->interPortEnd))?"--":"", (strlen(current->interPortEnd))?current->interPortEnd:"" );
		
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->extAddrBegin);
		
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s%s%s</td>",current->extPortBegin,
			(strlen(current->extPortEnd))?"--":"", (strlen(current->extPortEnd))?current->extPortEnd:"" );

		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_EXPERT, CGI_ACTION, CGI_ACTION_DEL);

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

		current = current->next;
	}

	return strdup(buf);
}

char *fwExpert_buttons(FW_INFO *info )
{
	char 		buf[8192*2];
	int 			length = 0;
	int			i;
	ipt_protocol 	*prot;

	length += CGI_SPRINTF(buf, length, "<form name=\"expertForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_EXPERT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_NAME, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_TYPE, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_TYPE) );

	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD colspan=3>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_PROTOCOL), IPT_KW_PROTOCOL );

	prot = iptables_protocols;
	for(i=0; i< 4/*sizeof(iptables_protocols)/sizeof(iptables_protocols[0])*/ ; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%s</option>\n", prot->type, gettext(prot->descriptor) );
		prot++;
	}	

	/* rule and target row */
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(IPT_KW_RULE), IPT_KW_RULE );
	for(i=0; i< 3; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", expert_rules[i], gettext(expert_rules[i]) );
	}
	
	length += CGI_SPRINTF(buf,length, "</TD>\n<TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(IPT_KW_TARGET), IPT_KW_TARGET );
	for(i=0; i< 3; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", expert_targets[i], expert_targets[i]);
	}

	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(IPT_KW_InterDevice), IPT_KW_InterDevice );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", info->netInfo->wanInfo.name, gettext(info->netInfo->wanInfo.name) );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", info->netInfo->lanInfo.name, gettext(info->netInfo->lanInfo.name) );

	length += CGI_SPRINTF(buf,length, "</TD>\n<TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(IPT_KW_ExtDevice), IPT_KW_ExtDevice );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", info->netInfo->wanInfo.name, gettext(info->netInfo->wanInfo.name) );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", info->netInfo->lanInfo.name, gettext(info->netInfo->lanInfo.name) );

	length += CGI_SPRINTF(buf, length, "</TD></TR>\n<TD align=\"center\" colspan=4><strong>%s</strong></TD></TR>\n", 
		gettext("Source Interface") );

	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext(CGI_STR_IP_ADDRESS) );
	length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\" colspan=3><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_KW_ExtAddress );

	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext(CGI_STR_PORT) );
	length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
		gettext(CGI_STR_BEGIN), IPT_KW_ExtPortBegin );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_END), IPT_KW_ExtPortEnd );

	length += CGI_SPRINTF(buf, length, "<TD align=\"center\" colspan=4><strong>%s</strong></TD></TR>\n", 
		gettext("Destination Interface") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_IP_ADDRESS) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\" colspan=3><input name=\"%s\" type=\"text\" size=\"15\"  onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_KW_InterAddress, IPT_KW_InterAddress );
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext(CGI_STR_PORT) );
	length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
		gettext(CGI_STR_BEGIN), IPT_KW_InterPortBegin );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_END), IPT_KW_InterPortEnd );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_SNAT) );
cgitrace;

	return strdup(buf);
}

