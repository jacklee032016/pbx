/*
* $Id: cgiVpnServer.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "_vpn.h"

pppd_item	_pppds[] =
{
	{VPNS_PPPD_AUTH},
	{VPNS_PPPD_LOCK},
	{VPNS_PPPD_DUMP},
//	{VPNS_PPPD_DEBUG},
	{VPNS_PPPD_PROXYARP},
	{VPNS_PPPD_MULTILINK},
	{NULL}
};

auth_protocol_t	_protocols[] =
{
	{	VPNS_PPPD_AUTH_EAP, 		"EAP", 				"require-eap", 		"refuse-eap" 	},
	{	VPNS_PPPD_AUTH_PAP, 		"PAP", 				"require-pap", 		"refuse-pap" 	},
	{	VPNS_PPPD_AUTH_CHAP, 		"CHAP", 				"require-chap", 		"refuse-chap" 	},
	{	VPNS_PPPD_AUTH_MS_CHAP,	"MS CHAP", 			"require-mschap", 		"refuse-mschap" 	},
	{	VPNS_PPPD_AUTH_MS_CHAP_V2,"MS CHAP Version 2", 	"require-mschap-v2", 	"refuse-mschap-v2" 	},
	{	VPNS_PPPD_AUTH_MPPE,		"MS PPP Encryption", 	"require-mppe", 		"nomppe" 	},
	{	NULL,	NULL, 				NULL,				NULL}
};

auth_protocol_t	_msppe[] =
{
	{	"PAP", 	"require-mppe-128", 	"refuse-mppe-128" 	},
	{	"CHAP", 	"require-chap", 	"refuse-chap" 	},
	{	"MS CHAP", 	"require-mschap", 	"refuse-mschap" 	},
	{	"MS CHAP Version2", 	"require-mschap-v2", 	"refuse-mschap-v2" 	},
	{	"EAP", 	"require-eap", 	"refuse-eap" 	},
	{	"PAP", 	"require-pap", 	"refuse-pap" 	},
	{	"PAP", 	"require-pap", 	"refuse-pap" 	},
	{	"PAP", 	"require-pap", 	"refuse-pap" 	},
	{	NULL, 				NULL,				NULL}
};

static int __write_pppd_config(VPN_INFO *info)
{
	FILE *fp;
	pppd_item	*ppp;
	auth_protocol_t	*protocol ;
	
	if(( fp=fopen(VPNS_PPPD_CONFIG_FILE,"w") )==NULL)
	{
		cgidebug( ": open file %s error : %s\n" ,VPNS_PPPD_CONFIG_FILE, strerror(errno) );
		return -1;
	}

	fprintf(fp, "# %s\n\n# %s\n", "pppd configuration for VPN Server" ,"simple configuration" );
	for(ppp = _pppds; ppp->key; ppp++)
	{
		fprintf(fp, "%s\n", ppp->key );
	}
	fprintf(fp, "%s \n", GET_VALUE(&info->cgiVariables, VPNS_PPPD_DEBUG) );

	fprintf(fp, "\n# %s\n", "Authen Protocols" );
	for(protocol = _protocols; protocol->key; protocol++)
	{
		fprintf(fp, "%s\n", GET_VALUE(&info->cgiVariables, protocol->key) );
	}
	
	fprintf(fp, "\n# %s\n", "Misc Options" );
	fprintf(fp, "%s %s\n",VPNS_PPPD_AUTH_SERVER_NAME, GET_VALUE(&info->cgiVariables, PPPOE_KEYWORD_SERVER_NAME) );
#if 0	
	fprintf(fp, "%s\t%s\n",VPNS_PPPD_IP_PARAM, GET_VALUE(&info->cgiVariables, VPNS_PPPD_IP_PARAM) );
#else
	fprintf(fp, "%s\t%s\n",VPNS_PPPD_IP_PARAM, VPN_SERVER_DEFAULT_IP_PARAM );
#endif

	fprintf(fp, "%s\t%s\n",VPNS_PPPD_LOG_FILE, GET_VALUE(&info->cgiVariables, VPNS_PPPD_LOG_FILE) );
	fprintf(fp, "%s\t%s\n",VPNS_PPPD_MS_WINS,GET_VALUE(&info->cgiVariables, VPNS_PPPD_MS_WINS) );
	fprintf(fp, "%s\t%s\n",VPNS_PPPD_MS_DNS, GET_VALUE(&info->cgiVariables, VPNS_PPPD_MS_DNS) );
	
	fclose(fp);
	return 0;
}

static int __write_vpn_server_confg(VPN_INFO *info)
{
	char	target[256];
	
	list_replace_with_new_list(&info->pptpdConfig, &info->cgiVariables );

	sprintf(target, "%s?%s=%s", WEB_URL_VPN, CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	cgi_refresh_page(1, target, gettext(CGI_STR_SAVE_CONFIG), gettext(CGI_STR_PLEASE_WAIT) );

	writeconfig(VPNS_PPTPD_CONFIG_FILE, ITEM_DBL, NULL, &info->pptpdConfig);

	__write_pppd_config(info);
	return 0;	
}

static char *__pppd_redio_button(auth_protocol_t *proto, VPN_INFO *info)
{
	char 	buf[1024];
	int 		length = 0;
	int 		yes = (strlen(GET_VALUE(&info->pppdConfig, proto->yes ))== 0)  ? 0: 1;
	
	length += CGI_SPRINTF(buf,length, "\t<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s\r\n", 
		proto->key, proto->yes, (yes==1)?"checked":"", gettext(CGI_STR_YES) );
	length += CGI_SPRINTF(buf,length, "\t<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s\r\n", 
		proto->key, proto->no, (yes==0)?"checked":"", gettext(CGI_STR_NO) );

	return strdup(buf);	
}

static char *__vpn_server_buttons(VPN_INFO *info)
{
	char 	buf[8192*2];
	int 		length = 0;
	auth_protocol_t	*protocol;

	length += CGI_SPRINTF(buf, length, "<form name=\"syslogForm\" method=\"post\" action=\"%s\">\n", WEB_URL_VPN );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables));
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf,length, "<table width=\"75%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TD></TR>\n", 
		gettext("PPTP Server Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("IP Address Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_LOCAL) );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\"  onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"  value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		VPNS_PPTPD_LOCAL_IP, VPNS_PPTPD_LOCAL_IP, GET_VALUE(&info->pptpdConfig, VPNS_PPTPD_LOCAL_IP));
	length += CGI_SPRINTF(buf,length, "\n<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n", gettext(CGI_STR_REMOTE) );
#if 1
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		VPNS_PPTPD_REMOTE_IP, VPNS_PPTPD_REMOTE_IP,GET_VALUE(&info->pptpdConfig, VPNS_PPTPD_REMOTE_IP) );
#else
	length += CGI_SPRINTF(buf,length, "(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD>\r\n", 
		gettext(CGI_STR_BEGIN), VPNS_PPTPD_REMOTE_IP_BEGIN, VPNS_PPTPD_REMOTE_IP_BEGIN );
	length += CGI_SPRINTF(buf,length, "<TD align=\"left\">(%s)</TD><TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_END), VPNS_PPTPD_REMOTE_IP_END, VPNS_PPTPD_REMOTE_IP_END );
#endif

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"center\" colspan=2><strong>%s</strong></TD></TR>\n", 
		gettext("PPP Configuration") );
	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Authentication Configuration") );
	for(protocol = _protocols; protocol->key; protocol++)
	{
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD><TD align=\"left\" colspan=3>%s</TD></TR>\r\n", 
			gettext(protocol->description), __pppd_redio_button(protocol, info) );
	}

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD>\r\n</TR>\r\n", 
		gettext("Others Configuration") );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("MS WINS"),VPNS_PPPD_MS_WINS, info->ms_wins);
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("MS DNS"),VPNS_PPPD_MS_DNS, info->ms_dns);
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\r\n", 
		gettext("IP PARAM"),VPNS_PPPD_IP_PARAM, info->ipParam);
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" ></TD></TR>\r\n", 
		gettext("Server Name"),PPPOE_KEYWORD_SERVER_NAME, info->serverName);

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n", gettext("Debug") );
{
	int 		yes = (strlen(GET_VALUE(&info->pppdConfig, VPNS_PPPD_DEBUG))== 0)  ? 0: 1;
	length += CGI_SPRINTF(buf,length, "\n\t<TD><input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s\r\n", 
		VPNS_PPPD_DEBUG, VPNS_PPPD_DEBUG, (yes==1)?"checked":"", gettext(CGI_STR_YES) );
	length += CGI_SPRINTF(buf,length, "\t<input name=\"%s\" type=\"Radio\" value=\"%s\" %s>%s</TD></TR>\r\n", 
		VPNS_PPPD_DEBUG, "", (yes==0)?"checked":"", gettext(CGI_STR_NO) );
}

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=1 align=\"right\"  bgcolor=\"#cccccc\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext(CGI_STR_LOG_FILE),VPNS_PPPD_LOG_FILE, info->logFile);
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"75%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><br><p></TD></TR><TR><TD align=\"center\" colspan=2>%s %s</TD></TR></TABLE></FORM>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_help_button(CGI_HELP_MSG_SERVICES_SYSLOG));

/*
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
*/
	
	return strdup(buf);
}

int	vpn_server_config(VPN_INFO *info)
{
	char		*action;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("VPN Server Configuration"), "",__vpn_server_buttons(info) );
		return 0;
	}
	else
	{
		__write_vpn_server_confg(info);
		return 0;
	}
	
	return 0;
}

