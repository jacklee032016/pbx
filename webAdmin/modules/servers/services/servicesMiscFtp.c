/*
* $Id: servicesMiscFtp.c,v 1.7 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"

#define	FTP_CFG_KEYWORD_CLIENTS							"max_clients"
#define	FTP_CFG_KEYWORD_IP_CLIENTS						"max_per_ip"
#define	FTP_CFG_KEYWORD_TIMEOUT							"idle_session_timeout"

#define	FTP_CFG_KEYWORD_LOCAL_ENABLE					"local_enable"
#define	FTP_CFG_KEYWORD_LOCAL_CHANGE					"chroot_local_user"

#define	FTP_CFG_KEYWORD_ANON_ENABLE					"anonymous_enable"
#define	FTP_CFG_KEYWORD_ANON_PASSWD_DISABLE			"no_anon_password"


#define	FTP_KEYWORD_YES									CGI_STR_YES_CASE
#define	FTP_KEYWORD_NO									CGI_STR_NO_CASE

const char	*ftp_options[]=
{
	FTP_KEYWORD_YES,
	FTP_KEYWORD_NO
};

static llist  	ftpCfgs;

#define 		FTP_CONF_FILE									"/etc/vsftpd.conf"

static char *__servicesMisc_ftp_status()
{
	char 	buf[4096];
	int 		length = 0;
	
	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("FTP Server Status"), cgi_service_get_state_name(SERVICE_NAME_FTP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *__ftp_redio_button(char *keyword)
{
	char 	buf[256];
	int 		length = 0;

	length += CGI_SPRINTF(buf,length, "\t<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s\r\n", 
		keyword, FTP_KEYWORD_YES, (!strcasecmp(GET_VALUE(&ftpCfgs, keyword),FTP_KEYWORD_YES))?"checked":"", gettext(FTP_KEYWORD_YES) );
	length += CGI_SPRINTF(buf,length, "\t<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s\r\n", 
		keyword, FTP_KEYWORD_NO, (!strcasecmp(GET_VALUE(&ftpCfgs, keyword),FTP_KEYWORD_NO))?"checked":"", gettext(FTP_KEYWORD_NO) );

	return strdup(buf);	
}

static char *__servicesMisc_ftp_buttons()
{
	char 	buf[4096];
	int 		length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"ftpForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_FTP);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Global Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("MAX Clients"),FTP_CFG_KEYWORD_CLIENTS, GET_VALUE( &ftpCfgs, FTP_CFG_KEYWORD_CLIENTS));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("MAX Clients Per IP"),FTP_CFG_KEYWORD_IP_CLIENTS, GET_VALUE( &ftpCfgs, FTP_CFG_KEYWORD_IP_CLIENTS));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\">%s</TD></TR>\r\n", 
		gettext("Timeout after Idle"),FTP_CFG_KEYWORD_TIMEOUT, GET_VALUE( &ftpCfgs, FTP_CFG_KEYWORD_TIMEOUT), gettext(CGI_STR_SECONDS) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Local User Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD></TR>\r\n", 
		gettext("Local User Login"), __ftp_redio_button(FTP_CFG_KEYWORD_LOCAL_ENABLE) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD></TR>\r\n", 
		gettext("Change Root After Local Login"), __ftp_redio_button(FTP_CFG_KEYWORD_LOCAL_CHANGE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2  bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Anonymous User Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD></TR>\r\n", 
		gettext("Anonymous Login"), __ftp_redio_button(FTP_CFG_KEYWORD_ANON_ENABLE) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD><TD align=\"left\">%s</TD></TR>\r\n", 
		gettext("No Password for Anonynous"), __ftp_redio_button(FTP_CFG_KEYWORD_ANON_PASSWD_DISABLE) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 >%s %s %s</TD></TR>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_submit_button(gettext(CGI_STR_SUBMIT)), cgi_help_button(CGI_HELP_MSG_SERVICES_DHCP));

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s:<p></strong>%s,%s<br>%s,%s</TD></TR>\r\n", 
		gettext(CGI_STR_NOTE), gettext("In order to take effective after change config"), gettext("FTP Server must be reboot"),
		gettext("In order to terminate FTP server"), gettext("all FTP clients must be quit firstly") );

	return strdup(buf);
}

int	servicesMisc_Ftp(SERVICE_INFO *info)
{
	char		*action;

	if (readconfig(FTP_CONF_FILE, ITEM_EQU, NULL, &ftpCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("FTP Configuration"), __servicesMisc_ftp_status(),__servicesMisc_ftp_buttons() );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char	target[256];
		
		list_replace_with_new_list(&ftpCfgs, &info->cgiVariables );

		sprintf(target, "%s?%s=%s", WEB_URL_SERVICES, CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_FTP );
		cgi_refresh_page(1, target, gettext("Save FTP Config File"), gettext(CGI_STR_PLEASE_WAIT) );

		writeconfig(FTP_CONF_FILE, ITEM_EQU, NULL, &ftpCfgs);
	}

	return 0;
}

