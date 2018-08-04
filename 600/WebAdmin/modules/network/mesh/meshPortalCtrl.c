/*
* $Id: meshPortalCtrl.c,v 1.4 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_mesh.h"

struct dev_t
{
	char		name[32];
	char		description[64];
};

struct dev_t _eths[] =
{
	{
		ETH0,
		_("WAN")
	},
	{
		ETH1,
		_("LAN")
	}
};

char *__mesh_portal_status(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
#if 0	
	int 		i;
	char		target[512];
#endif

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s\n</TD></TR>\r\n", 
		gettext("Ethernet Device"), GET_VALUE(&info->globalCfgs, MESH_KEYWORD_PORTAL_DEV) );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s\n</TD></TR>\r\n", 
		gettext(CGI_STR_START_WHEN_BOOT), GET_VALUE(&info->globalCfgs, MESH_KEYWORD_PORTAL) );
#if 0
	sprintf(target, WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_PORTAL"&"CGI_ACTION"=%s", 
		CGI_ACTION_ACTIVATE:CGI_ACTION_DEACTIVATE);
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>", 
		gettext(CGI_STR_START_AODV) );
#endif

	return strdup(buf);
}


char *__mesh_portal_buttons(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int 		i;
	int		isChecked = 0;
	node 	*current ;
	
	current = info->meshDevs.head;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("MESH Portal Parameters") );
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_MESH_OP_PORTAL);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext("Ethernet Device"), MESH_KEYWORD_PORTAL_DEV );
	for(i= 0; i< sizeof(_eths)/sizeof(_eths[0]); i++)
	{
		isChecked = !(strcasecmp(GET_VALUE(&info->globalCfgs, MESH_KEYWORD_PORTAL_DEV), _eths[i].name ));
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
			_eths[i].name, (isChecked) ?"selected":"",  _eths[i].description );
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

#if 0
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_MESH_DEVICE), MESH_KEYWORD_PORTAL_MESH );
	while(current)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
			current->entry.name, current->entry.name );
		current = current->next;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
#endif

	length += CGI_SPRINTF(buf, length, "\t<TR><TD  align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_START_WHEN_BOOT), MESH_KEYWORD_PORTAL );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_ON, (!strcasecmp(GET_VALUE(&info->globalCfgs, MESH_KEYWORD_PORTAL),WIFI_VALUE_ON) )?"selected":"",gettext(WIFI_VALUE_ON));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		WIFI_VALUE_OFF, (!strcasecmp(GET_VALUE(&info->globalCfgs, MESH_KEYWORD_PORTAL),WIFI_VALUE_OFF) )?"selected":"",gettext(WIFI_VALUE_OFF));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MESH_PORTAL) );

	return strdup(buf);
}

int	mesh_portal_ctrl(WIFI_INFO *info)
{
	char *action;

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!strcasecmp(action, CGI_ACTION_ADD) )
	{/*  save */
		cgi_wireless_save_config(info);

		if(!strcasecmp(GET_VALUE(&info->cgiVariables, MESH_KEYWORD_PORTAL), WIFI_VALUE_ON) )
			return cgi_lan_setup(&info->cgiVariables, CGI_LAN_MESH, CGI_LAN_WHERE_MESH);
	}
	
//	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Portal Configuration of MESH"), __mesh_portal_status(info), __mesh_portal_buttons(info));
//		return 0;
	}	

	return 0;
}

