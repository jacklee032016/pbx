/*
* $Author: lizhijie $
* $Id: syscfg_user.c,v 1.5 2007/09/06 09:19:58 lizhijie Exp $
*/

//#include "sysuser_conf.h"

#include "sysconf.h"

static void __sysuser_system_add(char *name,char *password)
{
	char cmd[256];

#if 1
	sprintf(cmd, "/bin/adduser -p0 %s", name);
	system(cmd);
	sprintf(cmd, "/usr/bin/passwd -p %s %s",password,name);
	system(cmd);
#else
	sprintf(cmd, "/usr/sbin/useradd  -g 1 -p %s %s", password, name);
	system(cmd);
#endif
}

static void __sysuser_system_delete(char *name)
{
	char cmd[256];

/* check the command in busybox */
#if 1	/* BUSYBOX */
	sprintf(cmd,"/bin/deluser %s",name);
	system(cmd);
	sprintf(cmd,"rm -rf /home/%s",name);
	system(cmd);
	sprintf(cmd,"rm -rf /var/spool/smtp/%s", name);
	system(cmd);
	sprintf(cmd,"rm -rf /var/spool/mail/%s", name);
	system(cmd);
#else	/* normal PC environments */
	sprintf(cmd, "/usr/sbin/userdel  -r %s",  name);
	system(cmd);
#endif
}

char *__sysuser_list_info(SYSCFG_INFO *sinfo, char *errMsg)
{
	char info[4096*10];
	int 	length = 0;
	node *current;
	int count = 0;
	
	if(errMsg)
	{
		length += sprintf(info+length, "<tr><td colspan=2 bgcolor=\"#ffffff\"><font color=\"#ff0000\"><strong>%s</strong></font></td></tr>", errMsg );
	}
	
	current = sinfo->usersCfgs.head;
	if(current== NULL)
		return strdup(info);

	length += sprintf(info+length, "<tr><td bgcolor=\"#cccccc\" align=\"center\"><strong>%s</strong></td><td bgcolor=\"#cccccc\"align=\"center\"><strong>%s</strong></td><td bgcolor=\"#cccccc\" align=\"center\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_INDEX), gettext(CGI_STR_USER_NAME), gettext(CGI_STR_MANAGEMENT) );

	while (current != 0) 
	{
		count++;
		length += sprintf(info+length, "<tr><td align=\"center\">%d</td><td align=\"center\">%s</td>\r\n",count, current->entry.name);

		if(strcmp(current->entry.name, "root") )
		{
			char target[128];

			sprintf(target, "%s?%s=%s&%s=%s&UserName=%s", WEB_URL_SYSCONFIG,
				CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_USER, CGI_ACTION, CGI_ACTION_DEL, current->entry.name );
			length += sprintf(info+length, "<td align=\"center\">%s</TD>", cgi_button_alert(gettext(CGI_STR_DELETE), target, gettext(CGI_STR_ARE_YOU_SURE) ) );
		}
		length += sprintf(info+length, "</tr>");
		current = current->next;
	}

	return strdup(info);
}

static char *__sysuser_button()
{
	char buf[2048];
	int 	length = 0;

	length += sprintf(buf+length, "<form name=\"smtpForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSCONFIG);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_USER );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">",CGI_ACTION,  CGI_ACTION_ADD );

	length += sprintf(buf+length, "<TR><TD  align=\"center\"><strong>%s</strong></TD>", gettext(CGI_STR_USER_NAME) );
	length += sprintf(buf+length, "<TD  align=\"center\"><input name=\"UserName\" onKeyPress=\"\" onChange=\"checkIsNotNULL(UserName,'%s')\"  type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("User Name can not be NULL") );

	length += sprintf(buf+length, "<TR><TD  align=\"center\"><strong>%s</strong></TD>", gettext(CGI_STR_PASSWORD) );
	length += sprintf(buf+length, "<TD  align=\"center\"><input name=\"Password\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Password,'%s')\"  type=\"password\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n",
		gettext("Password can not be NULL"));
	
	length += sprintf(buf+length, "<TR><TD  align=\"center\"><strong>%s</strong></TD>", gettext("RePassword") );
	length += sprintf(buf+length, "<TD  align=\"center\"><input name=\"RePassword\" onKeyPress=\"\" onChange=\"checkIsNotNULL(RePassword,'%s')\"  type=\"password\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("RePassword can not be NULL") );

	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext(CGI_STR_SAVE) ) );
	length += sprintf(buf+length, "</TD>\r\n");

	length += sprintf(buf+length, "<TD  align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_SYSTEM_USER ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");

	return strdup(buf);
}


int	sysconfig_user(SYSCFG_INFO *info)
{
	char *action= NULL, *user = NULL,*passwd=NULL, *rePasswd=NULL;
	char 	msg[256];
	int		isError = 0;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	user = GET_VALUE(&info->cgiVariables, "UserName" );
	passwd = GET_VALUE(&info->cgiVariables, CGI_STR_PASSWORD );
	rePasswd = GET_VALUE(&info->cgiVariables, "RePassword" );

	cgitrace;
	if( action && (! strcasecmp(action, CGI_ACTION_DEL) ||
		!strcasecmp(action, CGI_ACTION_ADD ) ) )
	{
		if(!user||!strlen(user))
		{
			sprintf(msg, gettext("No System User Name is provided") );
			isError = 1;
		}
		else if(! strcasecmp(action, CGI_ACTION_DEL) )
		{
			if( ! GET_VALUE(&info->usersCfgs, user) )
			{
				isError = 1;
				sprintf(msg, "%s '%s' %s", gettext(CGI_STR_SYSTEM_USER), user, gettext(CGI_STR_IS_NOT_EXIST) );
			}
			else
			{
				list_delete_by_name(&info->usersCfgs, user);
				__sysuser_system_delete( user);
			}	
		}
		else
		{
			if(!passwd||!strlen(passwd) ||!rePasswd||!strlen( rePasswd))
			{
				isError = 1;
				sprintf(msg, gettext("Password can not be null when add user") );
			}
			else if(strcmp(passwd, rePasswd) )
			{
				isError = 1;
				sprintf(msg, gettext("Twice Password is not the same") );
			}
			else if( strlen(GET_VALUE(&info->usersCfgs, user) ) )
			{
				isError = 1;
				sprintf(msg, "%s '%s' %s", gettext(CGI_STR_SYSTEM_USER), user, gettext(CGI_STR_HAS_EXIST) );
			}
			else
			{
				list_append_raw( &info->usersCfgs,  user, passwd, NULL, NULL);
				__sysuser_system_add(user,  passwd);
			}
		}

		writeconfig(SYSUSER_CONF_FILE, ITEM_DBL, NULL, &info->usersCfgs);
	}

	cgi_info_page(gettext("System User Management"), __sysuser_list_info(info, (isError==1)?msg:NULL), __sysuser_button());

	return 0;
}

