/*
* $Id: fwNatButtons.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/
#include "fwNat.h"

char *fwNat_info_buttons(iptables_item *header, NAT_enum_t natType)
{
	char 		buf[4096];
	int 			length = 0;
	iptables_item *current = header;
	char			target[256];

	if(!current)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_MODE), gettext(CGI_STR_PROTOCOL), gettext(CGI_STR_SOURCE_ADDRESS), gettext("Source Port"),gettext("External Address"),gettext("External Port"), gettext(CGI_STR_MANAGEMENT) );

	while(current )
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s</td>",current->action);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", gettext(current->protocol) );


		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->interAddr);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->interPortBegin);
		
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->extAddrBegin);
		
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>",current->extPortBegin);

		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_SNAT, CGI_ACTION, CGI_ACTION_DEL);

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

		current = current->next;
	}

	return strdup(buf);
}

char *fwNat_snat_buttons(FW_INFO *info, NAT_enum_t natType)
{
	char 		buf[4096];
	int 			length = 0;
	int			i;
	ipt_protocol 	*prot;

	length += CGI_SPRINTF(buf, length, "<form name=\"snatForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_SNAT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_NAME, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_NAME) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_CFG_RULE_TYPE, GET_VALUE(&info->cgiVariables, IPT_CFG_RULE_TYPE) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		IPT_KW_TARGET,(natType==NAT_SNAT)?IPTABLES_KEYWORD_SNAT:IPTABLES_KEYWORD_MASQUERADE);

	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD colspan=3>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext(CGI_STR_PROTOCOL), IPT_KW_PROTOCOL );

	prot = iptables_protocols;
	for(i=0; i< 4/*sizeof(iptables_protocols)/sizeof(iptables_protocols[0])*/ ; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%s</option>\n", prot->type, gettext(prot->descriptor) );
		prot++;
	}	

	length += CGI_SPRINTF(buf, length, "</TD></TR>\n<TR><TD align=\"center\" colspan=4><strong>%s</strong></TD></TR>\n", 
		gettext("External Interface") );
	if(natType== NAT_MASQURADE)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_WAN_PORT) );
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\" colspan=3><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\r\n", 
			IPT_KW_ExtDevice, info->netInfo->wanInfo.name );
	}
	else
	{
#if 0	
		length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("Range of Address") );
		length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
			gettext(CGI_STR_BEGIN), IPT_KW_ExtAddressBegin );
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext(CGI_STR_END), IPT_KW_ExtAddressEnd );
#else
		length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("External IP Address") );
		length += CGI_SPRINTF(buf,length, "</TD><TD align=\"left\" colspan=3><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
			IPT_KW_ExtAddress );
#endif
		length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("Range of Port") );
		length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
			gettext(CGI_STR_BEGIN), IPT_KW_ExtPortBegin );
		length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
			gettext(CGI_STR_END), IPT_KW_ExtPortEnd );
	}

	length += CGI_SPRINTF(buf, length, "<TD align=\"center\" colspan=4><strong>%s</strong></TD></TR>\n", 
		gettext("Internal Interface") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext("Internal IP Address") );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\" colspan=3><input name=\"%s\" type=\"text\" size=\"15\"  onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		IPT_KW_InterAddress, IPT_KW_InterAddress );
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>\r\n", gettext("Range of Port") );
	length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
		gettext(CGI_STR_BEGIN), IPT_KW_InterPortBegin );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"isDigit(this)\" onChange=\"checkPortNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_END), IPT_KW_InterPortEnd );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_SNAT) );

	return strdup(buf);
}

