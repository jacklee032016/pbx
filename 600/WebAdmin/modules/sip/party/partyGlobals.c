/*
* $Id: partyGlobals.c,v 1.9 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"

char		*_proxy407Modes[] = 
{
	CGI_STR_OFF,
	CGI_STR_ON,
	_("detect"),
	NULL
};

#define	GET_P_G_VALUE(info, key)		\
	GET_VALUE( (&info->globalCfgs), (key) )

static char *__party_globals_button(PARTY_INFO *info)
{
	char 	buf[8192];
	int 		length = 0;
	int 		i;
	int 		loglevel;
	char		*value, *key;
//	char		target[128];

	length += CGI_SPRINTF(buf, length, "<form name=\"partyGlobalForm\" method=\"post\" action=\"%s\">\n", WEB_URL_PARTY );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, PARTY_OP_GLOBAL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

/* table1 : network configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Network Configuration") );
/*	
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\n</TR>\r\n", 
		gettext("Server IP Address"), PARTY_CFG_SERVER_IP, GET_P_G_VALUE(info, PARTY_CFG_SERVER_IP) );
*/

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD>\r\n", 
		gettext("Reload IP"), PARTY_CFG_DYNAMIC_IP, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_DYNAMIC_IP)?"checked":"", gettext(CGI_STR_ON), PARTY_CFG_DYNAMIC_IP, PARTY_VALUE_OFF, IS_ON_P_G_VALUE(info, PARTY_CFG_DYNAMIC_IP)?"":"checked", gettext(CGI_STR_OFF) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		gettext("IPv6 Support"), PARTY_CFG_ENABLE_IPV6, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_ENABLE_IPV6)?"checked":"", gettext(CGI_STR_ON), PARTY_CFG_ENABLE_IPV6, PARTY_VALUE_OFF, IS_ON_P_G_VALUE(info, PARTY_CFG_ENABLE_IPV6)?"":"checked", gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD>\r\n", 
		gettext("Mulicast Support"), PARTY_CFG_MULTICAST, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_MULTICAST)?"checked":"", gettext(CGI_STR_ON), PARTY_CFG_MULTICAST, PARTY_VALUE_OFF, IS_ON_P_G_VALUE(info, PARTY_CFG_MULTICAST)?"":"checked", gettext(CGI_STR_OFF) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD>\n</TR>\r\n", 
		gettext("Multicast Device"), PARTY_CFG_MULTICAST_DEV, GET_P_G_VALUE(info, PARTY_CFG_MULTICAST_DEV) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s</strong></TD>\r\n", gettext("Transport Protocol") );
#if 1
	length += CGI_SPRINTF(buf, length, "\t<TD>UDP+TCP</TD>\r\n" );
#else
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' disabled>\n", 
		PARTY_CFG_TRANSFORT );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"udp\" selected>UDP</option>\n");
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"tcp\">TCP</option>\n");
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"sctp\">SCTP</option>\n");
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");
#endif	
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\n\r\n", 
		gettext("SIP Port"), PARTY_CFG_SERVER_PORT, GET_P_G_VALUE(info, PARTY_CFG_SERVER_PORT) );

	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* table 2 : network configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Authentication Configuration") );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\">	<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD>\r\n", 
		gettext(CGI_STR_AUTHENTICATION), PARTY_CFG_AUTHEN, PARTY_VALUE_ON, IS_ON_P_G_VALUE(info, PARTY_CFG_AUTHEN)?"checked":"", gettext(CGI_STR_ON), PARTY_CFG_AUTHEN, PARTY_VALUE_OFF, IS_ON_P_G_VALUE(info, PARTY_CFG_AUTHEN)?"":"checked", gettext(CGI_STR_OFF) );

	value = info->proxyAuth;
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" ><strong>%s</strong></TD>\r\n", gettext("Proxy Authen Required") );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' >\n", PARTY_CFG_AUTHEN_407 );
	for(i=0; i< 3; i++)
	{
		key = _proxy407Modes[i];
		length += CGI_SPRINTF(buf, length, "\t\t<option value=\"%s\" %s>%s</option>\n", key,(!strcasecmp(key, value))?"selected":"", gettext(key) );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD>\n\r\n", 
		gettext("Server Realm"), PARTY_CFG_SERVER_REALM, GET_P_G_VALUE(info, PARTY_CFG_SERVER_REALM) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" title=\"%s\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD>\n</TR>\r\n", 
		gettext("support multi FQDN which separated by commas"), gettext("Host(FQDN)"), PARTY_CFG_SERVER_NAME, GET_P_G_VALUE(info, PARTY_CFG_SERVER_NAME) );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* table 3 : Log configuration */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext(CGI_STR_LOG_CONFIGURATION) );
	length += CGI_SPRINTF(buf,length, "<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\" disabled></TD>\r\n", 
		gettext(CGI_STR_LOG_FILE), PARTY_CFG_LOG_FILE, GET_P_G_VALUE(info, PARTY_CFG_LOG_FILE) );

	length += CGI_SPRINTF(buf,length, "<TD align=\"right\" ><strong>%s</strong></TD>\r\n", gettext(CGI_STR_LOG_LEVEL) );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;'>\n", PARTY_CFG_LOG_LEVEL );
	loglevel = atoi(GET_P_G_VALUE(info, PARTY_CFG_LOG_LEVEL) );
	
	for(i= PARTY_LOG_MIN; i<=PARTY_LOG_MAX; i++)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", i, (loglevel==i)?"selected":"", i );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );


/* table 4 : Button */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PARTY_GLOBAL) );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* table 5 : Notes */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>* %s,%s!%s!\r\n",
		gettext(CGI_STR_NOTE), gettext("In order to startup SIP server"), gettext("default route must be activated"), gettext("Normally, it is executed by PPPOE client") );
	length += CGI_SPRINTF(buf,length, "<br><strong>* %s : </strong>%s!%s, %s. %s!\r\n",
		gettext("Proxy Authen Required"), gettext("Use SIP 407 status code to begin Proxy Authen negotiation process"), 
		gettext("Where, 'Off' is disabled, 'On' indicate 'always on'"),
		gettext("and 'detect' indicate auto detect the feature of SIP UA"),
		gettext("Suggest make it 'always on' when authen is enabled"));
	length += CGI_SPRINTF(buf,length, "<br><strong>* %s : </strong>%s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_PROTOCOL), gettext("Our SIP Server support TCP and UDP UA simultaneously"), 
		gettext("translated SIP signals between UAs with different transport protocol"));

	return strdup(buf);

}


int cgiParty_Globals(PARTY_INFO *info)
{
	char *action;

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!strcasecmp(action, CGI_ACTION_MODIFY ) )
	{
		list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables );
		if(party_save_config(info) )
		{
			cgi_error_page(NULL,gettext(CGI_STR_ERROR), gettext("SIP Server Global Configuration")  );
			return 1;
		}
	}
	
//	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("SIP Server Global Configuration"), "", __party_globals_button(info) );
		return 0;
	}
		
	return 0;	
}

