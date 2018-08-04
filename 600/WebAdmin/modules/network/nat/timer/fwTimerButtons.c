/*
* $Id: fwTimerButtons.c,v 1.2 2006/12/19 19:13:17 lizhijie Exp $
*/
#include "fwTimer.h"

char *fwTimer_info(llist *timers )
{
	char 		buf[4096];
	int 			length = 0;
	node		*current = timers->head;
	char			target[256];

	if(!current)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Time"), gettext("Firewall Rule"), gettext(CGI_STR_MANAGEMENT)  );

	while(current )
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s</td>", current->entry.name);
//		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.type );
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.value );

		sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_FIREWALL, CGI_KEYWORD_OPERATOR, FIREWALL_OP_TIMER, CGI_ACTION, CGI_ACTION_DEL);

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

		current = current->next;
	}

	return strdup(buf);
}

char *fwTimer_buttons(FW_INFO *info )
{
	char 		buf[8192*2];
	int 			length = 0;
	int			i;
	node		*current = info->ruleCfgs.head;

	if(!current)
	{
		length += CGI_SPRINTF(buf,length, "\n<TR><TD ><strong>%s:</strong><br>%s,%s</TD>\n", 
			gettext(CGI_STR_NOTE), gettext("No Firewall Rule is available"), gettext("So Timer can not be used now") );
		return strdup(buf);
	}

	length += CGI_SPRINTF(buf, length, "<form name=\"fwTimerForm\" method=\"post\" action=\"%s\">\n", WEB_URL_FIREWALL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, FIREWALL_OP_TIMER);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );
	
	/* minutes and hour row */
	length += CGI_SPRINTF(buf,length, "\n<TR><TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Minute"), IPT_TIMER_MINUTE );
//	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", IPT_TIMER_ALL, gettext(CGI_STR_ALL) );
	for(i=0; i<60; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}
	/* hours  */
	length += CGI_SPRINTF(buf,length, "\n</TD>\n<TD align=\"right\"><strong>%s</strong></TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Hour"), IPT_TIMER_HOUR );
//	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", IPT_TIMER_ALL, gettext(CGI_STR_ALL) );
	for(i=0; i<24; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}

	/* Day row */
	length += CGI_SPRINTF(buf,length, "\n</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Day"), gettext(CGI_STR_BEGIN), IPT_TIMER_DAY_BEGIN );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", IPT_TIMER_ALL, gettext(CGI_STR_ALL) );
	for(i=1; i<32; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}
	length += CGI_SPRINTF(buf,length, "</TD>\n<TD align=\"left\">(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_END), IPT_TIMER_DAY_END );
	for(i=2; i<32; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}

	/* Week row */
	length += CGI_SPRINTF(buf,length, "\n</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Week"), gettext(CGI_STR_BEGIN), IPT_TIMER_WEEK_BEGIN );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", IPT_TIMER_ALL, gettext(CGI_STR_ALL) );
	for(i=0; i<7; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}
	length += CGI_SPRINTF(buf,length, "</TD>\n<TD align=\"left\">(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_END), IPT_TIMER_WEEK_END );
	for(i=1; i<7; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}

	/* Months row */
	length += CGI_SPRINTF(buf,length, "\n</TD></TR>\n<TR><TD align=\"right\"><strong>%s</strong>(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext("Month"), gettext(CGI_STR_BEGIN), IPT_TIMER_MONTH_BEGIN );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", IPT_TIMER_ALL, gettext(CGI_STR_ALL) );
	for(i=1; i<13; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}
	length += CGI_SPRINTF(buf,length, "</TD>\n<TD align=\"left\">(%s)</TD>\n\t<TD><select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_END), IPT_TIMER_MONTH_END );
	for(i=2; i<13; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\">%d</option>\n", i, i );
	}

	length += CGI_SPRINTF(buf,length, "\n<TR><TD align=\"right\" colspan=2><strong>%s</strong></TD>\n\t<TD colspan=2><select name=\"%s\" style='width:180px;'>\n", 
		gettext("Execute Firewall Rule"), IPT_TIMER_TASK );
	while(current)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", current->entry.name, current->entry.name );

		current = current->next;
	}
	
	length += CGI_SPRINTF(buf,length, "</TD></TR>\n<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_SNAT) );

	return strdup(buf);
}

