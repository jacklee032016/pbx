/*
* $Id: partyFilters.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"

static char *__party_filters_info(PARTY_INFO *info, int filterType)
{
	char 		buf[4096*10];
	int 			length = 0;
	node 		*current ;
	char			target[1024];

	if(filterType==PARTY_FILTER_TYPE_EXT)
		current = info->extFilterCfgs.head;
	else
		current = info->intFilterCfgs.head;
	
	if(!current)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext("Express"), gettext(CGI_STR_SIP_URL), gettext(CGI_STR_MANAGEMENT) );

	while(current )
	{
		if(strcasecmp(current->entry.name, "forward" ) )
		{
			current = current->next;
			continue;
		}

		cgidebug("%s %s %s\n", current->entry.name, current->entry.value, current->entry.type );
		
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s &nbsp;</td>", current->entry.value );
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.type );
	cgitrace;

		snprintf(target, sizeof(target), "%s?%s=%s&%s=%s&%s=%s", WEB_URL_PARTY, CGI_KEYWORD_OPERATOR, 
			(filterType==PARTY_FILTER_TYPE_EXT)?PARTY_OP_FILTER_EXT:PARTY_OP_FILTER_INT, 
			CGI_ACTION, CGI_ACTION_DEL, PARTY_CFG_FILTER_REGULAR_EXP, current->entry.rule );
	cgitrace;

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));
	cgitrace;

		current = current->next;
	}

	cgitrace;
	return strdup(buf);
}

static char *__party_filters_buttons(PARTY_INFO *info , int filterType)
{
	char 		buf[4096*5];
	int 			length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"partyUserForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PARTY);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, 
		(filterType==PARTY_FILTER_TYPE_EXT)?PARTY_OP_FILTER_EXT:PARTY_OP_FILTER_INT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

#if 1
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		PARTY_CFG_FILTER_DIAL_PLAN, (filterType==PARTY_FILTER_TYPE_EXT)?"external":"internal" );
#else
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("Dialing Plan") );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' disabled>\n", 
		PARTY_CFG_FILTER_DIAL_PLAN );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"external\">%s</option>\n", gettext("external"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"internal\">%s</option>\n", gettext("internal"));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
#endif

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("Dialing Mode") );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' disabled>\n", 
		PARTY_CFG_FILTER_MODE );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"statefull\">%s</option>\n", gettext("statefull"));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"stateless\">%s</option>\n", gettext("stateless"));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext("Route Record"), PARTY_CFG_FILTER_ROUTE_RECORD, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_FILTER_ROUTE_RECORD)?"checked":"", gettext(CGI_STR_ON), PARTY_CFG_FILTER_ROUTE_RECORD, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_FILTER_ROUTE_RECORD)?"":"checked", gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"10\" value=\"\" maxlength=\"50\"></TD>\n</TR>\r\n", 
		gettext("Express"), PARTY_CFG_FILTER_REGULAR_EXP );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_SIP_URL) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		PARTY_CFG_USER_URL );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM>\r\n", cgi_help_button(CGI_HELP_MSG_PARTY_GLOBAL) );

	cgitrace;
	return strdup(buf);
}

	
static int __party_filter_save(PARTY_INFO *info, int type)
{
	char		*userUrl, *express;
	char		*action;
	char 	msg[256];
	llist 		*list;
	node 	*current ;
	char		*tmp;

	if(type == PARTY_FILTER_TYPE_EXT)
		list = &info->extFilterCfgs;
	else
		list = &info->intFilterCfgs;
	
	current = list->head;
	
	express = GET_VALUE(&info->cgiVariables, PARTY_CFG_FILTER_REGULAR_EXP ) ;
	userUrl = GET_VALUE(&info->cgiVariables, PARTY_CFG_USER_URL) ;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION ) ;
	while(current)
	{
		if(!strcasecmp(action, CGI_ACTION_DEL) )
			tmp = current->entry.rule;
		else
			tmp = current->entry.value;
		
		if( !strcasecmp(tmp , express)  ) 
		{
			break;
		}
		current = current->next;
	}

	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s' '%s')\" %s", gettext(CGI_STR_SIP_URL),express, userUrl, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );

		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{
		list_append_raw(list, "forward", express,	userUrl, NULL );
	}
	else
	{/* delete */
		current = list->head;

		while(current)
		{
			if( !strcasecmp(current->entry.rule, express)  ) 
			{
				list_delete_node(list, current);
				break;
			}
			current = current->next;
		}
	}

	tmp = GET_VALUE(&info->cgiVariables, PARTY_CFG_FILTER_DIAL_PLAN);
	if(tmp && strlen(tmp))
	{
		list_replace_value_by_key(list, PARTY_CFG_FILTER_DIAL_PLAN, tmp );
	}

	tmp = GET_VALUE(&info->cgiVariables, PARTY_CFG_FILTER_MODE);
	if(tmp && strlen(tmp))
	{
		list_replace_value_by_key(list, PARTY_CFG_FILTER_MODE, tmp);
	}

	tmp = GET_VALUE(&info->cgiVariables, PARTY_CFG_FILTER_ROUTE_RECORD);
	if(tmp && strlen(tmp))
	{
		list_replace_value_by_key(list, PARTY_CFG_FILTER_ROUTE_RECORD, tmp);
	}
	
	return 0;
}


int cgiParty_Filters(PARTY_INFO *info, int filterType )
{
	char *action;
	char	title[256];
	int	len = 0;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(action && strlen(action) )
	{
		if(__party_filter_save(info, filterType) )
			return -1;
		party_save_config( info);
	}

	len += snprintf(title+len, sizeof(title)-len, "%s:", gettext("SIP Server Filters Configuration") );
	if(filterType==PARTY_FILTER_TYPE_EXT)
		len += snprintf(title+len, sizeof(title)-len, "%s", gettext("Externel"));
	else
		len += snprintf(title+len, sizeof(title)-len, "%s", gettext("Internal"));
	cgi_info_page(title, __party_filters_info(info,filterType), __party_filters_buttons(info,filterType) );
	
//	cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP Server Filters Configuration")  );
	return 0;	
}

