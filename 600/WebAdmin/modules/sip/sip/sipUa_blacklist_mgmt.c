/*
* $Id: sipUa_blacklist_mgmt.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "sipUa.h"

#define	BLACKLIST_PHONE_NUMBER				"Phone_Number"

static llist  	blackListCfgs;

#define	PXB_BLACKLIST_CONF					"/etc/sip/enemy_list.conf"


static char *__sipUa_black_view_status()
{
	char 	buf[4096];
	int 		length = 0;
	int		index =0;
	char		target[256];
	node	*current;
	
	current = blackListCfgs.head;
	if(!current )
		return "";
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\n\t</TR>\n", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_PHONE_NUMBER), gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
		sprintf(target, "%s?%s=%s&%s=%s&%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_BLACKLIST_MGMT, CGI_ACTION,  CGI_ACTION_DEL,
			BLACKLIST_PHONE_NUMBER, current->entry.name );
		length += CGI_SPRINTF(buf,length, "<TR>\n\t<TD align=\"center\">%d</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n\n\t<TD align=\"center\">%s</TD>\r\n</TR>", 
			++index, current->entry.name,cgi_button_alert(gettext(CGI_STR_DELETE), target, gettext(CGI_STR_ARE_YOU_SURE)) );
		current = current->next;
	}
	return strdup(buf);

}

static char *__sipUa_black_view_buttons()
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"blackListForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SIPUA_OP_BLACKLIST_MGMT);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n", gettext(CGI_STR_PHONE_NUMBER) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		BLACKLIST_PHONE_NUMBER );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD>\r\n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", cgi_help_button(CGI_HELP_MSG_BLACKLIST_MGMT) );

	return strdup(buf);
}


int	sipUa_backlist_mgmt(llist *cgiVariables)
{
	char		*action;

	list_create(&blackListCfgs);

	if (readconfig(PXB_BLACKLIST_CONF, ITEM_BLCK_LIST, NULL, &blackListCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("BlackList Management"), __sipUa_black_view_status(),__sipUa_black_view_buttons() );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char		target[256];
		char		*phoneNumber = GET_VALUE(cgiVariables, BLACKLIST_PHONE_NUMBER);
		node	*current = list_find_node_by_key(&blackListCfgs, phoneNumber);

		if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
			(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
		{
			sprintf(target, "\"%s\" %s", phoneNumber, (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), target );
			return -1;
		}

		if(current)
		{/* delete */
			list_delete_node(&blackListCfgs, current);
		}
		else
		{/* add */
			list_append_raw(&blackListCfgs, phoneNumber, NULL, NULL, NULL );
		}
		writeconfig(PXB_BLACKLIST_CONF, ITEM_BLCK_LIST, NULL, &blackListCfgs);
		
		sprintf(target, "%s?%s=%s", WEB_URL_SIP_UA, CGI_KEYWORD_OPERATOR, SIPUA_OP_BLACKLIST_MGMT);
		cgi_refresh_page(1, target, gettext("Save BlackList File"), gettext(CGI_STR_PLEASE_WAIT) );
	}

	return 0;
}

