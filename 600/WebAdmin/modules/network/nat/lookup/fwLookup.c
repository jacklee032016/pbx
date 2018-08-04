/*
* $Id: fwLookup.c,v 1.4 2007/02/07 19:04:16 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_iptables.h"

iptables_item	natCgi;		/* NAT config read from CGI */


char *__lookup_status_info(iptables_item *ativeNats )
{
	char		buf[8192];
	int		length = 0;
	char		target[256];
	char 	*ptmp=NULL;

	if(!ativeNats)
		return strdup("");

	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_MODE), gettext(CGI_STR_PROTOCOL),gettext(CGI_STR_DEVICE),gettext(CGI_STR_SOURCE_ADDRESS), gettext("Source Port"),gettext("External Address"),gettext("External Port"), gettext(CGI_STR_MANAGEMENT) );

	while(ativeNats)
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s</td>",ativeNats->action);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", gettext(ativeNats->protocol) );

		if(!strcasecmp(ativeNats->extDevice,"eth0")||!strcasecmp(ativeNats->extDevice,"ppp0"))
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">WAN</td>");
		else
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">LAN</td>");	

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", ativeNats->interAddr);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", ativeNats->interPortBegin);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", ativeNats->extAddrBegin);
		
		if(strlen(ativeNats->extPortBegin))
		{
			ptmp=strchr(ativeNats->extPortBegin,':');
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>",ptmp+1);
		}
		else
		{
			length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>",ativeNats->extPortBegin);
		}
		//printf("<td align=\"center\">%s</td>", piptables->description);
		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_SNAT, CGI_ACTION, CGI_ACTION_DEL);

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

#if 0
		printf("<td align=\"center\"><a href=\"/cgi-bin/iptables.cgi?command=delete&action=%s"
		"&protocol=%s&extDevice=%s&localadd=%s&interPortBegin=%s&extAddrBegin=%s&extPortBegin=%s&ct=%s\">",
		piptables->action,piptables->protocol, piptables->extDevice, piptables->interAddr,piptables->interPortBegin,piptables->extAddrBegin,piptables->extPortBegin,ct);
		printf("Delete");
		printf("</a></tr>");
#endif		
		ativeNats = ativeNats->next;
	}
	return strdup(buf);
}

char *__lookup_select_buttons(llist *_ruleCfgs)
{
#define	LOOKUP_BUTTON(target, node, subOp) \
	do{	\
		sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_FIREWALL, 	\
			CGI_KEYWORD_OPERATOR, node->entry.type, CGI_ACTION, subOp, \
			IPT_CFG_RULE_NAME, node->entry.name, IPT_CFG_RULE_TYPE, node->entry.type );	\
		}while(0)

	char 		buf[8192];
	int 			length = 0;
	node		*current = _ruleCfgs->head;
	char			targetModify[256];
	char			targetDelete[256];
	char			targetActivate[256];
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n</TR>\n", 
		gettext("Rule Name"),gettext("Rule Type"),gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s</TD>\n</TR>\n", 
		gettext(CGI_STR_DEFAULT),gettext("MASQURADE"), gettext("Active"), gettext(CGI_STR_MANAGEMENT) );

	while(current)
	{
#if 0	
		sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_FIREWALL, 
			CGI_KEYWORD_OPERATOR, current->entry.type, CGI_ACTION, CGI_ACTION_MODIFY,
			IPT_CFG_RULE_NAME, current->entry.name, IPT_CFG_RULE_TYPE, current->entry.type );
#endif
		LOOKUP_BUTTON(targetModify, current, CGI_ACTION_MODIFY);
		LOOKUP_BUTTON(targetDelete, current, FIREWALL_SUB_OP_DEL_RULE);
		LOOKUP_BUTTON(targetActivate, current, CGI_ACTION_ACTIVATE);
		length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s</TD>\n\n\t<td align=\"center\">%s%s%s</TD>\n</TR>\n", 
			current->entry.name, gettext(current->entry.type), gettext("Active"), 
			cgi_button(gettext(CGI_ACTION_ACTIVATE), targetActivate), 
			cgi_button(gettext(CGI_STR_MODIFY), targetModify), 
			cgi_button_alert(gettext(CGI_ACTION_DEL), targetDelete, gettext(CGI_STR_ARE_YOU_SURE) )
			);

		current = current->next;
	}

	return strdup(buf);
}


int cgiFwLookup(FW_INFO *fwInfo)
{
	iptables_item	*natActive;	/* active NAT read from kernel */

	natActive = cgifw_read_iptables_actives();

	cgi_info_page(gettext("Firewall Configuration and Current Status"), __lookup_status_info(natActive ), __lookup_select_buttons(&fwInfo->ruleCfgs) );

	return 0;	
}

