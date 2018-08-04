/*
* $Id: cgiDdnsConfig.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgiDdns.h"
#include "cgi_services.h"
#include "cgi_syscfg.h"

static char *__ddns_status_list(void)
{
	char buf[4096];
	int length = 0;
	
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_NAME), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s %s %s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext(CGI_STR_DDNS_NAME), gettext(CGI_STR_DAEMON), gettext(CGI_STR_STATUS),  cgi_service_get_state_name(SERVICE_NAME_DDNSD), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}


static char *__peanut_config_button(DDNS_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	int 		i;
	int 		loglevel;
	char		target[512];

	length += CGI_SPRINTF(buf, length, "<form name=\"partyGlobalForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES_DDNSD );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

/* table1 : network configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("DDNS Server Configuration") );

	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\n\r\n", 
		gettext(CGI_STR_IP_ADDRESS), DDNS_KEYWORD_SERVER, GET_VALUE(&info->configs, DDNS_KEYWORD_SERVER) );
	length += CGI_SPRINTF(buf,length, "<TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\n\r\n", 
		gettext(CGI_STR_PORT), DDNS_KEYWORD_PORT, GET_VALUE(&info->configs, DDNS_KEYWORD_PORT) );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* table 2 : User configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("User Configuration") );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  type=\"text\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_USER_NAME), DDNS_KEYWORD_USERNAME, GET_VALUE(&info->configs, DDNS_KEYWORD_USERNAME) );

	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"password\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_PASSWORD), DDNS_KEYWORD_PASSWD, GET_VALUE(&info->configs, DDNS_KEYWORD_PASSWD) );

	length += CGI_SPRINTF(buf,length, "<TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\n\r\n", 
		gettext("Local UDP Port"), DDNS_KEYWORD_LOCAL_UDP, GET_VALUE(&info->configs, DDNS_KEYWORD_LOCAL_UDP) );
#if 0
	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"25\" value=\"%s\" maxlength=\"50\"></TD></TR>\n\r\n", 
		gettext("Registered FQDN"), DDNS_KEYWORD_FQDN, GET_VALUE(&info->configs, DDNS_KEYWORD_FQDN) );
#else
	sprintf(target, WEB_URL_SYSCONFIG"?"CGI_KEYWORD_OPERATOR"="CGI_SYSCFG_OP_HOSTNAME);
	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">%s %s</TD></TR>\n\r\n", 
		gettext("Registered FQDN"), info->fqdn, cgi_button(gettext(CGI_STR_MODIFY), target));
#endif
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* table 3 : Log configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext(CGI_STR_LOG_CONFIGURATION) );
	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"25\" disabled></TD></TR>\r\n", 
		gettext(CGI_STR_LOG_FILE), DDNS_KEYWORD_LOG_FILE, GET_VALUE(&info->configs, DDNS_KEYWORD_LOG_FILE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD width=150 align=\"right\" ><strong>%s</strong></TD>\r\n", gettext(CGI_STR_LOG_LEVEL) );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", DDNS_KEYWORD_LOG_LEVEL );
	loglevel = atoi(GET_VALUE(&info->configs, DDNS_KEYWORD_LOG_LEVEL) );
	
	for(i= 1; i<=8; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i, (loglevel==i)?"selected":"", i );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );


/* table 4 : Button */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM>\r\n</TABLE>\n"  );

/* table 5 : Notes */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s.%s!\r\n",
		gettext(CGI_STR_NOTE), gettext("Multiple FQDNs can be registered by any user"), gettext("Normally, only one FQDN can be activated by any device") );

	length += CGI_SPRINTF(buf,length, "<br>* '%s' %s <A href=\"%s\">'%s'</A>!</TD></TR>\r\n",
		gettext("Registered FQDN"), gettext("only can modified in page of"), target, gettext("Host Name") );

	return strdup(buf);

}


int cgiDdnsConfig(DDNS_INFO *info)
{
	char *action;
	char		buf[1024];

	CGI_SPRINTF(buf, 0, "%s %s", gettext(CGI_STR_DDNS_NAME), gettext(CGI_STR_CONFIGURATION) );
	action = GET_CGI_ACTION(&info->cgiVariables );
	if(!strcasecmp(action, CGI_ACTION_MODIFY ) )
	{
		list_replace_with_new_list(&info->configs, &info->cgiVariables );
		if(writeconfig(DDNS_CONFIG_FILE, ITEM_DBL, NULL, &info->configs ) )
		{
			cgi_error_page(NULL,gettext(CGI_STR_ERROR), buf  );
			return 1;
		}
	}
	
	cgi_info_page(buf, __ddns_status_list(), __peanut_config_button(info) );
		
	return 0;	
}

