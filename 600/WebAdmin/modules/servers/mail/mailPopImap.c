/*
* $Id: mailPopImap.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

#include "cgi_mail.h"

static llist  	dovecotCfgs;

#define	PROGRAM_NAME_POP3		"pop3-login"
#define	PROGRAM_NAME_IMAP		"imap-login"

static char *pop_status_list( )
{
	char buf[4096];
	int length = 0;

	if(cgi_service_get_state(SERVICE_NAME_POP_IMAP) != SERVICE_STATE_RUNNING )
	{
		length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong><td align=\"center\">%s</td><td align=\"center\">%s</td></tr>", 
			gettext("POP3/IMAP Server"), gettext(CGI_STR_STOP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES)  );

//		return strdup(buf);
	}
	
	length += sprintf(buf+length, "<form name=\"smtpForm\" method=\"post\" action=\"%s\">", WEB_URL_MAIL);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_POPIMAP );
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">", CGI_ACTION, CGI_ACTION_MODIFY);

		
	length += sprintf(buf+length, "<tr><td colspan=3 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext("POP3 Server") );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td>",
		gettext("POP3 Server Status"), cgi_get_program_state_name(PROGRAM_NAME_POP3), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES) );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td><input name=\"Pop3Startup\" value=\"on\" type=\"radio\" checked>%s</td><td><input name=\"Pop3Startup\" value=\"off\" type=\"radio\">%s</td>",
		gettext(CGI_STR_START_WHEN_BOOT), gettext(CGI_STR_YES), gettext(CGI_STR_NO) );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td colspan=2><input name=\"Pop3Port\" onKeyPress=\"checkIntegerNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"  disabled></td>",
		gettext("Port Number"), "110" );

	length += sprintf(buf+length, "<tr><td colspan=3 align=\"center\" ></td></tr>");
	length += sprintf(buf+length, "<tr><td colspan=3 align=\"center\" ></td></tr>");

	length += sprintf(buf+length, "<tr><td colspan=3 align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></td></tr>", 
		gettext("IMAP Server") );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td  align=\"center\">%s</td>",
		gettext("IMAP Server Status"), cgi_get_program_state_name(PROGRAM_NAME_IMAP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td><input name=\"ImapStartup\" value=\"on\" type=\"radio\" checked>%s</td><td><input name=\"ImapStartup\" value=\"off\" type=\"radio\">%s</td>",
		gettext(CGI_STR_START_WHEN_BOOT), gettext(CGI_STR_YES), gettext(CGI_STR_NO) );

	length += sprintf(buf+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td colspan=2><input name=\"ImapPort\" onKeyPress=\"checkIntegerNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"  disabled></td>",
		gettext("Port Number"), "143" );
	
	
	return strdup(buf);
}

static char *pop_add_buttons()
{
	char buf[4096];
	int length = 0;
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext(CGI_STR_SAVE)));
	length += sprintf(buf+length, "</TD>\r\n");

	length += sprintf(buf+length, "<TD  align=\"center\">\r\n");
//	length += sprintf(buf+length, cgi_help_button("/mail/pop_imap_help.html" ) );
	length += sprintf(buf+length, cgi_help_button(CGI_HELP_MSG_POP_IMAP ) );

	length += sprintf(buf+length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}


static int _pop_init()
{
	
	list_create(&dovecotCfgs);
//	list_add_config_items(&postfixCfgs,  postfix_conf, sizeof(postfix_conf)/sizeof(conf_item_t) );

/*	
	if (readconfig( WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases) != 0)
	{
		return -1;
	}
*/

	return 0;
}

int dovecot_modify_config(MAIL_INFO *info )
{
	char *popPort = GET_VALUE( &info->cgiVariables,  "Pop3Port");
	char *imapPort = GET_VALUE( &info->cgiVariables,  "ImapPort");
	char *popStart = GET_VALUE( &info->cgiVariables,  "Pop3Startup");
	char *imapStart = GET_VALUE( &info->cgiVariables,  "ImapStartup");
	char msg[1024];
	int isError = 0;
	int length = 0;

	if(!popPort )
	{
		isError = 1;
		length += sprintf(msg+length, " '%s' ", "POP3 Port Number" );
	}
	if(!popStart )
	{
		isError = 1;
		length += sprintf(msg+length, " '%s' ", "POP3 Startup" );
	}
	if(!imapPort )
	{
		isError = 1;
		length += sprintf(msg+length, " '%s' ", "IMAP Port Number" );
	}
	if(!imapStart )
	{
		isError = 1;
		length += sprintf(msg+length, " '%s' ", "IMAP Startup" );
	}
	
	if( isError )
	{
		length += sprintf(msg + length, "is not provided in Web Page" );
		cgi_error_page(NULL, gettext("POP3/IMAP Error"), msg );
		return 0;
	}

//	cgidebug("\nPOP3/IMAP Parameters : POP3 Port : %s\tPOP3 Startup : %s\tIMAP Port : %s\tIMAP Startup : %s\n", 
//		popPort, popStart, imapPort, imapStart );
/*
	list_clear( &aliases);
*/	

#if 0
	writeconfig(WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases);
	readconfig( WEB_ALIASES_DB, ITEM_DBL, NULL, &aliases);
	cgi_info_page(gettext("Aliases Database"), mail_aliases_list(MAIL_DB_TYPE_ALIASES, &aliases), mail_add_buttons(MAIL_DB_TYPE_ALIASES)  );
#else
	cgi_info_page(gettext("POP3/IMAP Configuration"), pop_status_list(), pop_add_buttons()  );
#endif
	return 0;

}

int mail_PopImap(MAIL_INFO *info)
{
	char *action = NULL;

	if(_pop_init() )
	{
		cgi_error_page(NULL, gettext("Mail Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION );
	if( !action ||! strlen(action) )
	{
		cgi_info_page(gettext("POP3/IMAP Configuration"), pop_status_list(), pop_add_buttons()  );
		return 0;
	}

	else if(!strcasecmp(action,CGI_ACTION_MODIFY))
	{
		dovecot_modify_config(info);
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

