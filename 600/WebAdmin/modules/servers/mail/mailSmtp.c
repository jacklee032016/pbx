/*
* $Id: mailSmtp.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>

#include "cgi_lib.h"

#include "cgi_mail.h"

static llist  	postfixCfgs;

static char *smtp_status_list( )
{
	char buf[4096];
	int length = 0;
	
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_NAME), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("SMTP Server Status"), cgi_service_get_state_name(SERVICE_NAME_SMTPD), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *smtp_add_buttons(char *msg)
{
	char buf[4096];
	int length = 0;

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TD></TR>\n", msg);

	length += CGI_SPRINTF(buf,length, "<form name=\"smtpForm\" method=\"post\" action=\"%s\">\n", WEB_URL_MAIL);
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_MAIL_OP_SMTP );
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, MAIL_OP_SMTP_CONFIG);
#if 0
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td>",
		gettext("Mail Host FQDN"), cgi_domain_name_read(), cgi_button(gettext(CGI_STR_MODIFY), "/cgi-bin/sysconf.cgi?vt=1&ct=hostname") );
#endif

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>\n", 
		gettext("Mail Host FQDN") );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		MAIL_SMTP_KEYWORD_FQDN , GET_VALUE(&postfixCfgs, MAIL_SMTP_KEYWORD_FQDN));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\"><strong>%s</strong></TD>", 
		gettext("Port Number") );

	length += CGI_SPRINTF(buf,length, "<TD align=\"center\"><input name=\"%s\" onKeyPress=\"checkIntegerNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"  disabled></TD></TR>\r\n", 
		MAIL_SMTP_KEYWORD_PORTNUM , "25" );

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">\r\n");
	length += CGI_SPRINTF(buf,length, cgi_submit_button(gettext(CGI_STR_SAVE)));

	length += CGI_SPRINTF(buf,length, cgi_help_button(CGI_HELP_MSG_SMTP ) );

	length += CGI_SPRINTF(buf,length, "</TD></TR></FORM>\r\n");

	return strdup(buf);
}


static int _smtp_init()
{
	
	list_create(&postfixCfgs);
//	list_add_config_items(&postfixCfgs,  postfix_conf, sizeof(postfix_conf)/sizeof(conf_item_t) );

	if (readconfig( MAIL_SMTP_POSTFIX_CONFIG, ITEM_EQU, NULL, &postfixCfgs) != 0)
	{
//		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("SMTP Configration Information Error") );
		return -1;
	}

	return 0;
}

static int __postfix_modify_config( MAIL_INFO *info)
{
#if DEBUG
	char *port = GET_VALUE( &info->cgiVariables,  MAIL_SMTP_KEYWORD_PORTNUM );
#endif
	char *fqdn = GET_VALUE( &info->cgiVariables,  MAIL_SMTP_KEYWORD_FQDN );
#if 0
	if(!port || !fqdn )
#else
	if( !fqdn )
#endif
	{
		cgi_error_page(NULL, gettext("SMTP Error"), gettext("Can not get SMTP parameters from web page") );
		return 0;
	}

	cgidebug("SMTP Port Number is %s, FQDN is %s\n", port, fqdn );

	config_file_replace_field(MAIL_SMTP_POSTFIX_CONFIG, ITEM_EQU, MAIL_SMTP_KEYWORD_FQDN, 
		GET_VALUE(&info->cgiVariables, MAIL_SMTP_KEYWORD_FQDN) );
	readconfig( MAIL_SMTP_POSTFIX_CONFIG, ITEM_EQU, NULL, &postfixCfgs);

	cgi_info_page(gettext("SMTP Configuration"), smtp_status_list(), smtp_add_buttons(gettext("New SMTP Configuration"))  );

	return 0;
}

int mail_Smtp(MAIL_INFO *info)
{
	char *action=NULL;

	if(_smtp_init() )
	{
		cgi_error_page(NULL, gettext("Mail Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return 1;
	}

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION );
	if( !action ||!strlen(action) )
	{
		cgi_info_page(gettext("SMTP Configuration"), smtp_status_list(), smtp_add_buttons(gettext("Current SMTP Configuration"))  );
		return 0;
	}
	else if(!strcasecmp(action, MAIL_OP_SMTP_CONFIG))
	{
		__postfix_modify_config(info);
		return 0;
	}
	else
	{
		cgidebug("mail command '%s' is not found\n", action );
		cgi_invalidate_page();
		return 0;
	}
	
	return 0;	
}

