/*
* $Id: servicesMiscUpnp.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "servicesMisc.h"
#include "cgi_services.h"

#define	UPNP_CFG_KEYWORD_DEBUG			"debug_mode"
#define	UPNP_CFG_KEYWORD_UPSTREAM		"upstream_bitrate"
#define	UPNP_CFG_KEYWORD_DOWNSTREAM	"downstream_bitrate"

#define	UPNP_DEBUG_MODE_NO				_("No Debug")
#define	UPNP_DEBUG_MODE_SYSLOG			_("SysLog")

struct _upnp_debug
{
	int		mode;
	char		*name;
};

struct _upnp_debug _debugModes[] =
{
	{
		0,
		UPNP_DEBUG_MODE_NO
	},
	{
		1,
		UPNP_DEBUG_MODE_SYSLOG
	}
};

static llist  	upnpCfgs;

#define 		UPNP_CONFIG_FILE						"/etc/upnpd.conf"

static char *__servicesMisc_upnp_status()
{
	char 	buf[4096];
	int 		length = 0;
	
//	length += CGI_SPRINTF(buf,length, "<strong>%s %s</strong>\n", 
//		gettext("All Parameters of DHCP"), gettext(CGI_STR_SET_ON_PAGAE) );
	length += CGI_SPRINTF(buf, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td></tr>", 
		gettext(CGI_STR_NAME), gettext(CGI_STR_STATUS), gettext(CGI_STR_MANAGEMENT) );

	length += CGI_SPRINTF(buf,length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\">%s</td><td align=\"center\">%s</td></TR>",
		gettext("UPNP Server Status"), cgi_service_get_state_name(SERVICE_NAME_UPNP), cgi_button(gettext(CGI_STR_MODIFY), WEB_URL_SERVICES ) );

	return strdup(buf);
}

static char *__servicesMisc_upnp_buttons()
{
	char 	buf[4096];
	int 		length = 0;
	int		i;
	int		mode = atoi(GET_VALUE(&upnpCfgs, UPNP_CFG_KEYWORD_DEBUG) );
	struct	_upnp_debug *_debug = _debugModes;

	length += CGI_SPRINTF(buf, length, "<form name=\"upnpForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SERVICES );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_UPNP);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf, length, "<TD align=\"right\"><strong>%s:</strong></TD>\t<TD>\n\t<select name=\"%s\"  maxlength=\"30\">\n", 
		gettext("Debug Mode"), UPNP_CFG_KEYWORD_DEBUG );

	for(i=0; i< 2; i++)
	{
		
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", _debug->mode, (mode==_debug->mode)?"selected":"", gettext(_debug->name) );
		_debug++;
	}	

	length += CGI_SPRINTF(buf,length, "</TR><TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\">bps</TD></TR>\r\n", 
		gettext("Up Bit Rate"),UPNP_CFG_KEYWORD_UPSTREAM, GET_VALUE( &upnpCfgs, UPNP_CFG_KEYWORD_UPSTREAM));
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\">bps</TD></TR>\r\n", 
		gettext("Down Bit Rate"),UPNP_CFG_KEYWORD_DOWNSTREAM, GET_VALUE( &upnpCfgs, UPNP_CFG_KEYWORD_DOWNSTREAM));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 >%s %s %s</TD></TR>\r\n", 
		cgi_submit_button( gettext(CGI_STR_SAVE) ) , cgi_submit_button(gettext(CGI_STR_SUBMIT)), cgi_help_button(CGI_HELP_MSG_SERVICES_UPNP));

	return strdup(buf);
}

int	servicesMisc_Upnp(SERVICE_INFO *info)
{
	char		*action;
	list_create(&upnpCfgs);

	if (readconfig(UPNP_CONFIG_FILE, ITEM_EQU, NULL, &upnpCfgs) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext(CGI_STR_WITHOUT_CONF_FILE) );
		return -1;
	}

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("UPNP(Internet Gateway Device) Configuration"), __servicesMisc_upnp_status(),__servicesMisc_upnp_buttons() );
		return 0;
	}
	else// if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{
		char	target[256];
		
		list_replace_with_new_list(&upnpCfgs, &info->cgiVariables );

		sprintf(target, "%s?%s=%s", WEB_URL_SERVICES, CGI_KEYWORD_OPERATOR, SERVICES_MISC_OP_UPNP );
		cgi_refresh_page(1, target, gettext(CGI_STR_SAVE_CONFIG ), gettext(CGI_STR_PLEASE_WAIT) );

		writeconfig(UPNP_CONFIG_FILE, ITEM_EQU, NULL, &upnpCfgs);
	}

	return 0;
}

