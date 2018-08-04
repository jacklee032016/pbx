/*
* $Id: wifiAuthen.c,v 1.6 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wifi.h"
#include "cgi_syscfg.h"

#define	CGI_WIFI_AUTHEN_OPEN_STR		_("OPEN(None,Default)")
#define	CGI_WIFI_AUTHEN_SHARED_STR		_("SharedKey")
#define	CGI_WIFI_AUTHEN_DOT1X_STR		_("802.1X")
#define	CGI_WIFI_AUTHEN_AUTO_STR		_("AUTO")
#define	CGI_WIFI_AUTHEN_WPA_STR			_("WPA(Wireless Protected Access)")

#define	CGI_WIFI_AUTHEN_WEP_KEY_COUNT	4

struct	wifi_authen_mode
{
	int			type;
	char			*name;
	char			*description;
};

struct wifi_authen_mode _authModes[] =
{
	{
		CGI_WIFI_AUTHEN_OPEN,
		"AUTH_OPEN",
		CGI_WIFI_AUTHEN_OPEN_STR
	},
	{
		CGI_WIFI_AUTHEN_SHARED_KEY,
		"AUTH_SHARE_KEY",
		CGI_WIFI_AUTHEN_SHARED_STR
	},
	{
		CGI_WIFI_AUTHEN_802DOT1X, 
		"AUTH_1X",
		CGI_WIFI_AUTHEN_DOT1X_STR
	},
	{
		CGI_WIFI_AUTHEN_AUTO, 
		"AUTH_AUTO",	
		CGI_WIFI_AUTHEN_AUTO_STR
	},
	{
		CGI_WIFI_AUTHEN_WPA, 
		"AUTH_WPA",
		CGI_WIFI_AUTHEN_WPA_STR
	},
	{
		-1, 
		NULL,	
		NULL
	},
};


//#define	__AUTHEN_MODE_COMMAND		"/sbin/wifipriv %s authmode %d"
#define	__AUTHEN_MODE_COMMAND		"/sbin/wifi/authen.sh"

static int	__authen_save_config(WIFI_INFO *info)
{
	wifi_authen_info_t *authen = &info->authenInfo;
	char	cmd[128];//, *value;
	
//	value = GET_VALUE( &info->cgiVariables, WIFI_KEYWORD_ACL_POLICY );
//	list_replace_value_by_key( &acl->macCfgs, WIFI_KEYWORD_ACL_POLICY, value);
	list_replace_with_new_list(&authen->authenCfgs, &info->cgiVariables );

	if(writeconfig(WIFI_CONFIG_AUTHEN_FILE, ITEM_EQU, NULL, &authen->authenCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return -1;
	}
	
	sprintf(cmd, __AUTHEN_MODE_COMMAND);
	system(cmd);

	return 0;
}

int	__authen_init(WIFI_INFO *info)
{
	wifi_authen_info_t	 *authen = &info->authenInfo;
	
	list_create(&authen->authenCfgs);
	if (readconfig(WIFI_CONFIG_AUTHEN_FILE, ITEM_EQU, NULL, &authen->authenCfgs) )
	{
		cgi_error_page(NULL, gettext("Wireless Operation Failed"), gettext( CGI_STR_SOME_ERROR ) );
		return -1;
	}

	return 0;
}

char *__wifi_authen_button(WIFI_INFO *info)
{
	char 	buf[8192*10];
	int 		length = 0;
	int 		j;
	char		*value;
	struct	wifi_authen_mode *mode = _authModes;
	wifi_authen_info_t				*authen = &info->authenInfo;

	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WLAN );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_WIFI_OP_AUTHEN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);

//	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	value = GET_VALUE( &authen->authenCfgs, WIFI_KEYWORD_AUTHEN_MODE);
	length += CGI_SPRINTF(buf, length, "\t<TR><TD  width=\"45%%\" align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:220px;'>\n", 
		gettext("Authentication Mode"), WIFI_KEYWORD_AUTHEN_MODE);
	while(mode->type != -1)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			mode->type, (mode->type==atoi(value) ) ?"selected":"",  mode->description);
		mode++;
	}
	length += CGI_SPRINTF(buf, length, "</TD></TR></TABLE>\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><br>%s,%s! %s!</TD></TR>\r\n",
		gettext("Please input the key:"),
		gettext("Shared Key are 5 ASCII Charactors"), gettext("used in 64 bits Authentication and Cryption Key"), 
		gettext("128 bits Key can be supported in future") );
	for(j=0 ;j < CGI_WIFI_AUTHEN_WEP_KEY_COUNT; j++)
	{
		char	key[128];
		length += sprintf(buf+length, "<TR><TD width=\"45%%\" align=\"right\"><strong>%s %d :</strong></TD>", 
			gettext("Key") , j+1 );
		sprintf(key, "%s_%d", WIFI_KEYWORD_AUTHEN_KEY, j);
		value = GET_VALUE(&authen->authenCfgs, key);
		length += sprintf(buf+length, "<TD><input name=\"%s\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n",
			key, value );
	}
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );
	length += CGI_SPRINTF(buf, length, "</table></FORM>\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>%s!%s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), gettext("Now only Open and Shared Key can be used"), gettext("More features can be supported in future!") );
//	length += CGI_SPRINTF(buf, length, "</table>\n" );

	return strdup(buf);
}


int	wifi_Authen(WIFI_INFO *info)
{
	char *action;
	char	target[256];
	sprintf(target, WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AUTHEN);
	
	__authen_init(info);

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Authentication Configuration of WiFi"), "", __wifi_authen_button(info));
		return 0;
	}

	if(__authen_save_config( info) )
			return 0;

	return cgi_refresh_page(3, target,gettext(CGI_STR_WAIT), gettext("Authentication Info is saving") );
}

