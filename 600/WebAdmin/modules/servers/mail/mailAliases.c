/*
* $Id: mailAliases.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

#include "cgi_mail.h"
#include "cgi_syscfg.h"

static llist		sysUsers;
static llist  	aliases;
static llist		groups;

static char *mail_aliases_list(mail_db_type_t type, llist *aliasesOrGroups )
{
	node 	*current, *next = NULL;
	char 	buf[4096];
	int 		length = 0;
	char 	target[128];
	int		hasSysUsers;	/* has system user in current system */
	
	current = aliasesOrGroups->head;
	if(current== NULL)
	{
		if(type == MAIL_DB_TYPE_ALIASES)
			length += CGI_SPRINTF(buf, length,"<br><br>%s",  gettext("No Aliases in your system now") );
		length += CGI_SPRINTF(buf, length,"<br><br>%s",  gettext("No Aliases Group in your System now") );
		
		return strdup(buf);
	}
	
	hasSysUsers = list_length(&sysUsers);
	
	length +=CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td %s align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>\n", 
		type==MAIL_DB_TYPE_ALIASES?gettext("Alias"):gettext("Alias Group"), gettext("System User Name"), type==MAIL_DB_TYPE_ALIASES?"":"colspan=2", gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
	
		if(type==MAIL_DB_TYPE_GROUP )
		{
			if( current->entry.value )
			{
				CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s",  WEB_URL_MAIL,CGI_KEYWORD_OPERATOR,CGI_MAIL_OP_ALIAS,CGI_ACTION, MAIL_OP_DEL_ALIAS_IN_GROUP, 
					MAIL_OP_TYPE, MAIL_OP_TYPE_GROUP, MAIL_ALIAS_KEYWORD_SYS_USER, current->entry.value, MAIL_ALIAS_KEYWORD_GROUP, current->entry.name );
				length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" >%s</td>\n<td align=\"center\" ><a href=\"%s\">%s</a>",
					current->entry.name, target, current->entry.value);
			}
			else
			{
				length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" ></td>\n<td align=\"center\">");
			}
		}
		else
		{
			length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" >%s</td><td align=\"center\">%s", current->entry.name, current->entry.value);
		}

		next = current->next;
		if( next && (!strcmp(next->entry.name, current->entry.name)  ) )
		{
			CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s&%s=%s&%s=%s&%s=%s",  WEB_URL_MAIL,CGI_KEYWORD_OPERATOR,CGI_MAIL_OP_ALIAS,CGI_ACTION, MAIL_OP_DEL_ALIAS_IN_GROUP, 
				MAIL_OP_TYPE, MAIL_OP_TYPE_GROUP, MAIL_ALIAS_KEYWORD_GROUP, current->entry.name, MAIL_ALIAS_KEYWORD_SYS_USER, current->entry.value );
		
			length += CGI_SPRINTF(buf, length, "<br><A HREF=\"%s\">", target) ;
			length += CGI_SPRINTF(buf, length, "%s</A>",  next->entry.value) ;
			current = next;
		}	
		
		length += CGI_SPRINTF(buf, length, "</td>"); 
		sprintf(target, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_MAIL, CGI_KEYWORD_OPERATOR,CGI_MAIL_OP_ALIAS, CGI_ACTION, type==MAIL_DB_TYPE_ALIASES?MAIL_OP_DEL_ALIAS:MAIL_OP_DEL_GROUP,
			MAIL_OP_TYPE, type==MAIL_DB_TYPE_ALIASES?MAIL_OP_TYPE_ALIAS:MAIL_OP_TYPE_GROUP,  
			type==MAIL_DB_TYPE_ALIASES?MAIL_ALIAS_KEYWORD_ALIAS:MAIL_ALIAS_KEYWORD_GROUP, current->entry.name);
		length += CGI_SPRINTF(buf, length, "<td align=\"center\" %s>%s", (hasSysUsers==0)?"colspan=2":"",  cgi_button_alert(type==MAIL_DB_TYPE_ALIASES?gettext("Delete Aliases"):gettext("Delete Group"), target, gettext(CGI_STR_ARE_YOU_SURE)  ) );
		if(type==MAIL_DB_TYPE_GROUP && hasSysUsers )
		{

			CGI_SPRINTF(target,0, "%s?%s=%s&%s=%s&%s=%s&%s=%s", WEB_URL_MAIL, CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_ALIAS, CGI_ACTION, MAIL_OP_ADD_ALIAS_IN_GROUP, 
				MAIL_OP_TYPE, MAIL_OP_TYPE_GROUP, 
				MAIL_ALIAS_KEYWORD_GROUP, current->entry.name );
			length += CGI_SPRINTF(buf, length, "<td align=\"center\">%s", cgi_button(gettext("Add System User"), target) );

		}
		length += CGI_SPRINTF(buf, length, "</TD>\n");
		current = current->next;
	}

	return strdup(buf);
}

static char *mail_add_buttons(mail_db_type_t type, int addGroup, MAIL_INFO *info )
{
	char buf[4096];
	int length = 0;
	char target[128];
	node *current = NULL;

	if( list_length(&sysUsers) == 0)
	{
		length += CGI_SPRINTF(buf, length, "<tr><td colspan=2><font color=\"#ff0000\"><strong>%s</strong></font></td></tr>\n", 
			gettext("No Systsm User in current system") );
		CGI_SPRINTF(target,0, "%s?%s=%s", WEB_URL_SYSCONFIG, CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_USER );
		length +=CGI_SPRINTF(buf, length,"<TR><td><br></TD></TR>\n<TR><td align=\"center\">%s</TD></TR>\n", cgi_button( gettext("System User Management"), target) );
		
		return strdup(buf);
	}
	
	length += CGI_SPRINTF(buf, length, "<form name=\"aliasForm\" method=\"post\" action=\"%s\">\n", WEB_URL_MAIL );

	if( type==MAIL_DB_TYPE_ALIASES)
	{
		length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_ALIAS);
		length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, MAIL_OP_ADD_ALIAS);

		current = sysUsers.head;
		length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\"><strong>%s</strong></TD><TD  align=\"center\"><strong>%s</strong></TD></TR>\r\n", 
			gettext(CGI_STR_SYSTEM_USER), gettext("Alias") );
		length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\"><select name=\"%s\"  maxlength=\"30\">\n", MAIL_ALIAS_KEYWORD_SYS_USER);
		while(current)
		{

			length += CGI_SPRINTF(buf, length, "<option value=\"%s\">%s</option>", current->entry.name, current->entry.name );

			current = current->next;
		}
		length += CGI_SPRINTF(buf, length, "</select></TD>\r\n");
		length += CGI_SPRINTF(buf, length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"num(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\" ></TD></TR>\r\n", MAIL_ALIAS_KEYWORD_ALIAS);
	}
	else 
	{/* group */

		char *group = GET_VALUE( &info->cgiVariables,  MAIL_ALIAS_KEYWORD_GROUP);
		if(addGroup )//!group || !strlen(group) )
		{
			length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_ALIAS);
			length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, MAIL_OP_ADD_GROUP );

			length += CGI_SPRINTF(buf, length, "<TR><TD colspan=2 >%s :<br> %s</TD></TR>\r\n", 
				gettext("Add Mail Group"), gettext("Please input new Mail Group name you want to create") );
			
			length +=CGI_SPRINTF(buf, length, "<TR><TD align=\"center\" colspan=2><input name=\"%s\" onKeyPress=\"num(this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\" ></TD>\r\n", MAIL_ALIAS_KEYWORD_GROUP );
			length += CGI_SPRINTF(buf, length, "</TD></TR\r\n");
		
		}
		else
		{
			length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_ALIAS);
			length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, MAIL_OP_ADD_ALIAS_IN_GROUP );

			current = sysUsers.head;
			length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"center\" colspan=2>%s '%s'%s :</TD></TR>\r\n", 
				gettext("in Alias Group"), group,gettext("to Add System User"));
			
			length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"center\">%s</TD><TD  align=\"center\">%s</TD></TR>\r\n", 
				gettext("Alias Group"), gettext(CGI_STR_SYSTEM_USER) );

			length += CGI_SPRINTF(buf, length, "<input name=\"%s\" type=\"hidden\" value=\"%s\">\n", 
				MAIL_ALIAS_KEYWORD_GROUP, group);
			length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"center\">%s</TD>\r\n",  group);
			length += CGI_SPRINTF(buf, length, "<TD align=\"center\"><select name=\"%s\"  maxlength=\"30\">\n", MAIL_ALIAS_KEYWORD_SYS_USER);
			while(current)
			{

				length += CGI_SPRINTF(buf, length, "<option value=\"%s\">%s</option>\n", current->entry.name, current->entry.name );

				current = current->next;
			}
			length += CGI_SPRINTF(buf, length, "</select></TD></TR\r\n");
		}

	}
	
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\">\r\n");

	length += CGI_SPRINTF(buf, length, cgi_submit_button(gettext(CGI_ACTION_ADD)) );
	length += CGI_SPRINTF(buf, length, "</TD>\r\n");

	length += CGI_SPRINTF(buf, length, "<TD  align=\"center\">\r\n");
	length += CGI_SPRINTF(buf, length, cgi_help_button(CGI_HELP_MSG_MAIL_ALIAS) );

	length += CGI_SPRINTF(buf, length, "</TD></TR></form>\r\n");

	return strdup(buf);
}


static int _mail_init()
{
	list_create(&aliases);
	list_create(&groups);
	
	if (readconfig( WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases) != 0)
	{
		return -1;
	}

	if (readconfig(WEB_GROUP_DB, ITEM_DBL, NULL, &groups) != 0)
	{
		return -1;
	}

	list_create(&sysUsers);
	if(readconfig(SYSUSER_CONF_FILE, ITEM_DBL, NULL, &sysUsers ) != 0)
	{
		return -1;
	}

	return 0;
}


int _mail_cmd_lookup(int addGroup, MAIL_INFO *info)
{
	char *type;
	
	type = GET_VALUE(&info->cgiVariables, MAIL_OP_TYPE );
	if(!strcasecmp(type, MAIL_OP_TYPE_ALIAS ) )
	{
		cgi_info_page(gettext("Aliases Database"), mail_aliases_list(MAIL_DB_TYPE_ALIASES, &aliases), mail_add_buttons(MAIL_DB_TYPE_ALIASES, 0, info)  );
		return 0;
	}
	else if(!strcasecmp(type,MAIL_OP_TYPE_GROUP ))
	{
		cgi_info_page(gettext("Group Database"), mail_aliases_list(MAIL_DB_TYPE_GROUP, &groups), mail_add_buttons(MAIL_DB_TYPE_GROUP, addGroup, info) );
		return 0;
	}
	else
	{
		cgi_invalidate_page();
		return 0;
	}

}

static int _mail_cmd_alias(mail_op_t opType, MAIL_INFO *info)
{
	char *name = GET_VALUE(&info->cgiVariables, MAIL_ALIAS_KEYWORD_ALIAS );
	char *sysUser = NULL;
	char msg[1024];
	
	if(!name)
	{
		CGI_SPRINTF(msg,0, "%s %s", gettext("No Alias you want to"), (opType == MAIL_OP_DEL)?gettext(CGI_STR_DELETE):gettext(CGI_STR_ADD) ); 
		cgi_error_page(NULL, gettext("Aliases Error"), msg );
		return 0;
	}

	if(opType == MAIL_OP_DEL)
	{
//		cgidebug("deleting alias %s\n", name );
		if (! list_delete_by_name(&aliases, name) )
		{
			cgi_error_page(NULL, gettext("Aliases Error"), gettext("Alias you provide is not exist") );
			return 0;
		}
	}	
	else
	{
		sysUser = GET_VALUE(&info->cgiVariables, "sysUser");
		if(!sysUser || !strlen(sysUser) )
		{
			cgi_error_page(NULL, gettext(CGI_STR_WARN), gettext("System User is not provided"));
			return 0;
		}

		if(strlen(GET_VALUE(&aliases, name) ) )
		{
			CGI_SPRINTF(msg, 0, "%s '%s' %s", gettext("Alias"), name, gettext(CGI_STR_HAS_EXIST) );
			cgi_error_page(NULL, gettext(CGI_STR_WARN), msg );
			return 0;
		}

		list_append_raw(&aliases, name, sysUser, NULL, NULL);

	}

	writeconfig(WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases);

	readconfig( WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases);
	cgi_info_page(gettext("Aliases Database"), mail_aliases_list(MAIL_DB_TYPE_ALIASES, &aliases), mail_add_buttons(MAIL_DB_TYPE_ALIASES, 0, info)  );

	return 0;
}

/* add/delete group */
static int _mail_cmd_group(mail_op_t opType, MAIL_INFO *info)
{
	char *group = GET_VALUE(&info->cgiVariables, MAIL_ALIAS_KEYWORD_GROUP);
	char msg[1024];
	
	if(!group)
	{
		CGI_SPRINTF(msg,0, "%s %s %s", gettext("No Group you want to"), (opType == MAIL_OP_DEL)?gettext(CGI_STR_DELETE):gettext(CGI_STR_ADD), gettext("exist") ); 
		cgi_error_page(NULL, gettext(CGI_STR_WARN), msg );
		return 0;
	}

	if(opType == MAIL_OP_DEL)
	{
//		cgidebug("deleting group %s\n", group );
		if (! list_delete_by_name(&groups, group) )
		{
			cgi_error_page(NULL, gettext("Mail Group Error"), gettext("Mail group you provide is not exist") );
			return 0;
		}
	}	
	else
	{
//		cgidebug("adding group %s\n", group );
		if(strlen(GET_VALUE(&groups, group ) ) )
		{
			CGI_SPRINTF(msg,0, "%s '%s' %s", gettext("Group"), group, gettext(CGI_STR_HAS_EXIST) );
			cgi_error_page(NULL, gettext(CGI_STR_WARN), msg );
			return 0;
		}

		list_append_raw(&groups, group, NULL, NULL, NULL);

	}

	writeconfig(WEB_GROUP_DB, ITEM_DBL, NULL, &groups);

	readconfig( WEB_GROUP_DB, ITEM_DBL, NULL, &groups);
	cgi_info_page(gettext("Group Database"), mail_aliases_list(MAIL_DB_TYPE_GROUP, &groups), mail_add_buttons(MAIL_DB_TYPE_GROUP, 1, info)  );

	return 0;
}


/* add/delete system user of a group */
static int _mail_cmd_sysuser_of_group(mail_op_t opType, MAIL_INFO *info)
{
	char *group = GET_VALUE(&info->cgiVariables, MAIL_ALIAS_KEYWORD_GROUP);
	char *sysUser = NULL;
	char msg[1024];
	node *current;
	
	if(!group)
	{
		CGI_SPRINTF(msg,0, "%s %s %s", gettext("No Group you want to"), (opType == MAIL_OP_DEL)?gettext(CGI_STR_DELETE):gettext(CGI_STR_ADD), gettext("exist") ); 
		cgi_error_page(NULL, gettext(CGI_STR_WARN), msg );
		return 0;
	}

	if(opType == MAIL_OP_DEL)
	{
		sysUser = GET_VALUE(&info->cgiVariables,  MAIL_ALIAS_KEYWORD_SYS_USER );
//		cgidebug("deleting system user %s in group %s\n", sysUser, group );

		current = groups.head;
		while(current)
		{
			if( !strcmp(current->entry.name, group)&& current->entry.value && !strcmp(current->entry.value, sysUser) )
			{
				list_delete_node(&groups, current);
				break;
			}
			current = current->next;
		}
	}	
	else
	{
		int isAdded = 0;
		sysUser = GET_VALUE(&info->cgiVariables, MAIL_ALIAS_KEYWORD_SYS_USER );
		if(!sysUser || !strlen(sysUser) )
		{
			_mail_cmd_lookup(0, info);
			return 0;
		}

		current = groups.head;
		while(current)
		{
			if( !strcasecmp(current->entry.name, group) )
			{
				if( current->entry.value )
				{
					if( !strlen(current->entry.value) )
					{
						sprintf(current->entry.value, "%s", sysUser );
						isAdded = 1;
						break;
					}
					else if( !strcmp(current->entry.value, sysUser) )
					{
						CGI_SPRINTF(msg,0, "%s '%s' %s '%s'%s", gettext(CGI_STR_SYSTEM_USER), sysUser, gettext("has been a member"), group,gettext("of group"));
						cgi_error_page(NULL, gettext(CGI_STR_ERROR), msg);
						return 0;
					}	
				}
			}
			current = current->next;
		}
//		cgitrace;
		if(!isAdded)
		{
//			cgitrace;
			list_append_raw(&groups, group, sysUser, NULL, NULL);
//			cgitrace;
		}	

	}

	writeconfig(WEB_GROUP_DB, ITEM_DBL, NULL, &groups);

	readconfig( WEB_GROUP_DB, ITEM_DBL, NULL, &groups);
	cgi_info_page(gettext("Group Database"), mail_aliases_list(MAIL_DB_TYPE_GROUP, &groups), mail_add_buttons(MAIL_DB_TYPE_GROUP, 1, info)  );

	return 0;
}

int mail_Alias(MAIL_INFO *info)
{
	char *action=NULL;

	if(_mail_init() )
	{
		cgi_error_page(NULL, "Mail Operation Failed", CGI_STR_SOME_ERROR);
		return 1;
	}

	action = GET_VALUE( &info->cgiVariables,  CGI_ACTION );
	if( !action || !strlen(action) )// || !strcasecmp(cmd,"lookup")  )
	{
//		cgidebug("mail command is %s\n", action);
		_mail_cmd_lookup( 1, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_DEL_ALIAS))
	{
		_mail_cmd_alias(MAIL_OP_DEL, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_ADD_ALIAS ))
	{
		_mail_cmd_alias(MAIL_OP_ADD, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_DEL_GROUP ))
	{
		_mail_cmd_group(MAIL_OP_DEL, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_ADD_GROUP ))
	{
		_mail_cmd_group(MAIL_OP_ADD, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_DEL_ALIAS_IN_GROUP))
	{
		_mail_cmd_sysuser_of_group(MAIL_OP_DEL, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_ADD_ALIAS_IN_GROUP))
	{
		_mail_cmd_sysuser_of_group(MAIL_OP_ADD, info);
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_ACTIVATE))
	{
		char	target[512];
		int	len = 0;
		
		len += CGI_SPRINTF(target,len, "%s %s.", gettext("Mail Aliases and Groups"), gettext("is activating"));
		len += CGI_SPRINTF(target,len, "%s!", gettext("Try the new account with your mail client"));
		len += CGI_SPRINTF(target,len, "<br>%s.....", gettext(CGI_STR_PLEASE_WAIT));
		cgi_refresh_page(3, WEB_URL_MAIL"?"CGI_KEYWORD_OPERATOR"="CGI_MAIL_OP_SMTP, gettext("Mail Accounts Activating"), target);

		cgi_mail_reactivate_account();
		return 0;
	}
	else
	{
//		cgidebug("mail command '%s' is not found\n", action );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}

