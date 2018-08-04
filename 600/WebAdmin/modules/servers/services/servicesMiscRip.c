/*
* $Id: servicesMiscRip.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"

#define	RIP_CFG_KEYWORD_PASSWD			"password"
#define	RIP_CFG_KEYWORD_SUPER			"enable"
#define	RIP_CFG_KEYWORD_INTFACE			"network"
#define	RIP_CFG_KEYWORD_NEIGHBOR		"neighbor"
#define	RIP_CFG_KEYWORD_SEND				"send"
#define	RIP_CFG_KEYWORD_RECV				"receive"

#define	RIP_VERSION_1						_("Version 1")
#define	RIP_VERSION_2						_("Version 2")

/*
* ip rip receive/send version 1/2
*/
struct _rip_version
{
	int		version;
	char		*descriptor;
};

struct _rip_version _versions[] =
{
	{
		1,
		RIP_VERSION_1
	},
	{
		2,
		RIP_VERSION_2
	}
};

static llist  	ripCfgs;

#define 		RIP_CONFIG_FILE						"/etc/ripd/ripd.conf"
#define 		ZEBRA_CONFIG_FILE						"/etc/ripd/zebra.conf"

static char *__servicesMisc_rip_status(void)
{
	char 	buf[4096];
	int 		length = 0;
	
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_NAME), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("RIP Daemon Status"), cgi_service_get_state_name(SERVICE_NAME_RIP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("Router Mgr Status"), cgi_service_get_state_name(SERVICE_NAME_ZEBRA), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *__servicesMisc_rip_buttons(SERVICE_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	int		i;
	char		*passwd;
	int		mode = atoi(GET_VALUE(&ripCfgs, RIP_CFG_KEYWORD_SEND) );
	struct	_rip_version *_version = _versions;

	length += CGI_SPRINTF(buf, length, "<form name=\"ripForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf,length, "</TR><TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Normal Pass word"),RIP_CFG_KEYWORD_PASSWD, GET_VALUE( &ripCfgs, RIP_CFG_KEYWORD_PASSWD));
	passwd = GET_VALUE(&ripCfgs, RIP_CFG_KEYWORD_SUPER);
	passwd = strchr(passwd, ' ');
	length += CGI_SPRINTF(buf,length, "</TR><TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"password\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Super Pass word"), RIP_CFG_KEYWORD_SUPER, (passwd!=NULL)?passwd+1:"" );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext("RIP RX Version"), RIP_CFG_KEYWORD_RECV );
	for(i=0; i< 2; i++)
	{
		
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _version->version, (mode==_version->version)?"selected":"", gettext(_version->descriptor) );
		_version++;
	}	

	_version = _versions;
	length += CGI_SPRINTF(buf, length, "</TR><TR><TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext("RIP TX Version"), RIP_CFG_KEYWORD_SEND );
	for(i=0; i< 2; i++)
	{
		
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _version->version, (mode==_version->version)?"selected":"", gettext(_version->descriptor) );
		_version++;
	}	

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\" disabled></TD></TR>\r\n", 
		gettext("Interface"), RIP_CFG_KEYWORD_INTFACE, gettext(CGI_STR_LAN_PORT));
#if 0
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\"  onChange=\"checkIP(%s.value,this)\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Network IP Address"),RIP_CFG_KEYWORD_INTFACE, RIP_CFG_KEYWORD_INTFACE, GET_VALUE( &ripCfgs, RIP_CFG_KEYWORD_INTFACE));
#endif	
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Neighbor"), RIP_CFG_KEYWORD_NEIGHBOR, GET_VALUE( &ripCfgs, RIP_CFG_KEYWORD_NEIGHBOR));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 >%s %s %s</TD></TR>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_submit_button(gettext(CGI_STR_SUBMIT)), cgi_help_button(CGI_HELP_MSG_SERVICES_RIP));


	return strdup(buf);
}

int	servicesMisc_Rip(SERVICE_INFO *info)
{
	char		*action;
	list_create(&ripCfgs);

	if (readconfig(RIP_CONFIG_FILE, ITEM_DBL, NULL, &ripCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("RIP(Route Information Protocol) Configuration"), __servicesMisc_rip_status(),__servicesMisc_rip_buttons(info) );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char	target[256];
		
		list_replace_with_new_list(&ripCfgs, &info->cgiVariables );
		sprintf(target, "password %s", GET_VALUE(&info->cgiVariables, RIP_CFG_KEYWORD_SUPER) );
		list_replace_value_by_key(&ripCfgs, RIP_CFG_KEYWORD_SUPER, target);

		sprintf(target, "%s?%s=%s", WEB_URL_SERVICES, CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
		cgi_refresh_page(1, target, gettext("RIP Config File"), gettext(CGI_STR_PLEASE_WAIT) );

		writeconfig(RIP_CONFIG_FILE, ITEM_DBL, NULL, &ripCfgs);
	}

	return 0;
}

