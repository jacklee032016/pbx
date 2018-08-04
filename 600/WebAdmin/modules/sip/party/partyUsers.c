/*
* $Id: partyUsers.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"

char *__party_users_info(PARTY_INFO *info)
{
	char 		buf[4096*10];
	int 			length = 0;
	node 		*current = info->userCfgs.head;
	char			target[256];

	if(!current)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<tr>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n\t<td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\n", 
		gettext(CGI_STR_SIP_URL), gettext(CGI_STR_USER_NAME), gettext(CGI_STR_PASSWORD), gettext(CGI_STR_MANAGEMENT) );

	while(current )
	{
		length += CGI_SPRINTF(buf, length,"<tr><td align=\"center\">%s &nbsp;(%s)</td>",current->entry.type,current->entry.name);
		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.value );

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td>", current->entry.rule );

		sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_PARTY, CGI_KEYWORD_OPERATOR, PARTY_OP_USER, CGI_ACTION, CGI_ACTION_DEL, 
			PARTY_CFG_USER_URL, current->entry.type, PARTY_CFG_USER_NAME, current->entry.value );

		length += CGI_SPRINTF(buf, length,"<td align=\"center\">%s &nbsp;</td></TR>", cgi_button_alert(gettext(CGI_ACTION_DEL), target, gettext(CGI_STR_ARE_YOU_SURE) ));

		current = current->next;
	}

	return strdup(buf);
}

char *__party_users_buttons(PARTY_INFO *info )
{
	char 		buf[4096];
	int 			length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"partyUserForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PARTY);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, PARTY_OP_USER );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD );

#if 0	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_SIP_URL) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\">%s</TD></TR>\r\n", 
		PARTY_CFG_USER_URL, gettext("Prefix 'sip:' must be added") );
#endif

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_USER_NAME) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		PARTY_CFG_USER_NAME );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_PASSWORD) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"password\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		PARTY_CFG_USER_PASSWD );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=4>%s  \n", cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "%s</TD></TR>\r\n</FORM></TABLE>\r\n", cgi_help_button(CGI_HELP_MSG_PARTY_GLOBAL) );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* <strong>%s</strong>%s, %s.<br>* %s, %s, %s!<br>* %s, %s, %s, %s.</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), gettext(CGI_STR_USER_NAME), 
		gettext("must be digit which can be dialed with phone"),
		gettext("and its' length is same as dial plan defined"), 
		gettext("If the first digit is same as that of local dial plan and device number"), 
		gettext("then this user name is used by our AS600 device"),
		gettext("Other is used by software phone"),
		gettext("For example, with our predefined dialed plan"),
		gettext("user name must be length of 4"), 
		gettext("and which begin with '6','7','8' or '9' is used by AS600"),
		gettext("other user name such as '3456' is used by softphone"));

	return strdup(buf);
}

	
int __party_user_save(PARTY_INFO *info)
{
#if 0
	char		*userUrl;
#else
	char 	userUrl[256];
#endif
	char		*action;
	char 	msg[256];
	node 		*current ;
	
	
	current = info->userCfgs.head;
#if 0
	userUrl = GET_VALUE(&info->cgiVariables, PARTY_CFG_USER_URL) ;
#else
	sprintf(userUrl, "sip:%s@%s", GET_VALUE(&info->cgiVariables,PARTY_CFG_USER_NAME),
		GET_VALUE(&info->globalCfgs, PARTY_CFG_SERVER_REALM) );
#endif

	while(current)
	{
		if( !strcasecmp(current->entry.type, userUrl)  ) 
		{
			break;
		}
		current = current->next;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION ) ;
	if((!current && !strcasecmp(action, CGI_ACTION_DEL) ) ||
		(current && !strcasecmp(action, CGI_ACTION_ADD) )   )
	{
		sprintf(msg, "\"%s('%s')\" %s", gettext(CGI_STR_SIP_URL), userUrl, 
			 (current)?gettext(CGI_STR_HAS_EXIST):gettext(CGI_STR_IS_NOT_EXIST) );

		cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg );
		return -1;
	}

	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{
		list_append_raw(&info->userCfgs, "user", GET_VALUE(&info->cgiVariables,PARTY_CFG_USER_NAME), 
			userUrl, GET_VALUE(&info->cgiVariables,PARTY_CFG_USER_PASSWD) );
	}
	else
	{/* delete */
		current = info->userCfgs.head;

		while(current)
		{
			if( !strcasecmp(current->entry.type, userUrl)  ) 
			{
				list_delete_node(&info->userCfgs, current);
				break;
			}
			current = current->next;
		}
	}

	return 0;
}

int cgiParty_Users(PARTY_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if( action && strlen(action) )
	{
		if(__party_user_save( info) )
			return -1;
		party_save_config( info);
		
	}
	
	return cgi_info_page(gettext("SIP Server Users Configuration"), __party_users_info(info), __party_users_buttons(info) );
 	
//	return cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP Server Users Configuration")  );
}

