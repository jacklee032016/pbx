/*
* $Id: cgi_cfg_mail_alias.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

#include "cgi_mail.h"

conf_item_t	default_aliases[] =
{
// Person who should get root's mail. Don't receive mail as root!
		{"root", 						"lizhijie adminstrator"},
// Basic system aliases -- these MUST be present		
		{"MAILER-DAEMON", 			"postmaster"},
		{"postmaster", 				"root"},
// General redirections for pseudo accounts		
		{"bin", 						"root"},
		{"daemon", 					"root"},
		{"named", 					"root"},
		{"nobody", 					"root"},
		{"uucp", 						"root"},
		{"www", 						"root"},
		{"ftp-bugs", 					"root"},
		{"postfix", 					"root"},

// Well-known aliases		
		{"manager", 					"root"},
		{"dumper", 					"root"},
		{"operator", 					"root"},
		{"abuse", 					"postmaster"},
		
// trap decode to catch security attacks		
		{"decode", 					"root"}
};



#define	MAIL_SMTP_CREATE_ALIAS_DB	"/usr/sbin/postalias hash:/etc/postfix/aliases"
#define	MAIL_SMTP_POSTFIX_RELOAD		"/usr/sbin/postfix reload"

static int __mail_group_merge(llist *groups)
{
	node 	*current, *next;
	node	*tmp;

	current = groups->head;
	while(current)
	{/* merge multiple user in the same group into a line */
//		pre = current;
		next = current->next;
		while(next)
		{
			if(!strcmp(current->entry.name, next->entry.name ) )
			{
				sprintf( current->entry.value, "%s  %s", current->entry.value, next->entry.value);
				tmp = next->next;
//				cgitrace;
				list_delete_node(groups, next);
				next = tmp;
			}
			else 
			{
//				cgitrace;
				next = next->next;
			}
//				cgitrace;
		}
		current = current->next;
	}

	current = groups->head;
	while(current)
	{/* remove the group without user */
		if(!strlen(current->entry.value) )
		{
			tmp = current->next;
//			cgitrace;
			list_delete_node(groups, current);
			current = tmp;
		}
		else 
		{
//			cgitrace;
			current = current->next;
		}
	}

	return 0;
}

int cgi_mail_reactivate_account()
{
	llist  		myAliases;
	llist		myGroups;
	llist		newCfgs;
	FILE		*fp;
	char		command[256];
	
	list_create(&myAliases);
	list_create(&myGroups);
	list_create( &newCfgs);
	
	if (readconfig( WEB_ALIASES_DB, ITEM_DBL, NULL, &myAliases) != 0)
	{
		return -1;
	}

	if (readconfig(WEB_GROUP_DB, ITEM_DBL, NULL, &myGroups) != 0)
	{
		return -1;
	}

	list_add_config_items( &newCfgs, default_aliases, sizeof(default_aliases)/ sizeof(conf_item_t));

	if(( fp=fopen(MAIL_SMTP_ALIASES_FILE ,"w"))==NULL)
	{
		cgidebug( "open file %s error : %s\n" , MAIL_SMTP_ALIASES_FILE, strerror(errno) );
		return -1;
	}

	fprintf(fp, "#ALIASES not modified by WebAdmin\n" );
	cgi_write_config(fp, &newCfgs, NULL, cgi_write_config_item_with_collon );
	fprintf(fp, "\n\n#ALIASES Added by WebAdmin\n" );
	cgi_write_config(fp, &myAliases, NULL, cgi_write_config_item_with_collon );

	__mail_group_merge(&myGroups );
	fprintf(fp, "\n\n#GROUP Added by WebAdmin\n" );
	cgi_write_config(fp, &myGroups, NULL, cgi_write_config_item_with_collon );

	fclose(fp);
	
	sprintf(command, MAIL_SMTP_CREATE_ALIAS_DB );
	system(command);
		
	sprintf(command, MAIL_SMTP_CREATE_ALIAS_DB );
	system(command);
	return 0;
}

