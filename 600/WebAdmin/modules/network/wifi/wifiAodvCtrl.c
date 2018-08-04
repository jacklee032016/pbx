/*
* $Id: wifiAodvCtrl.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_wifi.h"

static int __wifi_aodv_get_debug_state()
{
	FILE 	*fp;
	char 	line[1024];
	int 		state;

	if((fp=fopen(WIFI_AODV_CTRL_DEBUG, "r"))==NULL)
		return -1;
	fgets(line,1024,fp);
	state = atoi(line);
	fclose(fp);

	return state;
}

char *__wifi_aodv_ctrl_status()
{
	char 	buf[8192*5];
	int 		length = 0;
	char		target[512];

	sprintf(target, WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_CTRL"&"CGI_ACTION"=%s", (__wifi_aodv_get_debug_state()==0)?CGI_ACTION_ACTIVATE:CGI_ACTION_DEACTIVATE);
	length += CGI_SPRINTF(buf, length, "\t<TR><TD colspan=2><strong>%s:</strong><br>%s<br></TD></TR>\n\t\n", 
		gettext("AODV Debug"), gettext("Click button below, then AODV debug info will be Open/Close") );
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"center\" colspan=2>%s</TD></TR>\n\n", 
		cgi_button( (__wifi_aodv_get_debug_state()==0)?gettext("Open AODV Debug"):gettext("Close AODV Debug"), target ) );

	sprintf(target, WEB_URL_WLAN"?"CGI_KEYWORD_OPERATOR"="CGI_WIFI_OP_AODV_CTRL"&"CGI_ACTION"=%s", CGI_ACTION_DEL);
	length += CGI_SPRINTF(buf, length, "\t<TR><TD colspan=2><p><strong>%s</strong><br>%s</TD></TR>\n\t\n", 
			gettext("AODV Route Management"), gettext("Click button below, then all AODV route will be deleted") );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s</TD></TR>\r\n",cgi_button(gettext("Clear All Route Items"), target)  );

	return strdup(buf);
}

int	wifi_Aodv_Ctrl(WIFI_INFO *info)
{
	char		command[256];
	int		cmd = 0;
	char *action;

	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);

	if(!strcasecmp(action, CGI_ACTION_ACTIVATE) ||!strcasecmp(action, CGI_ACTION_DEACTIVATE) )
	{
		if(__wifi_aodv_get_debug_state()==0)
			cmd = 1;

		sprintf(command, "echo %d > %s", cmd, WIFI_AODV_CTRL_DEBUG );
		system(command);
		cgidebug("AODV command : %s\n", command);
	}
	if(!strcasecmp(action, CGI_ACTION_DEL) )
	{
		sprintf(command, "echo 1 > %s", WIFI_AODV_CTRL_CLEAR );
		system(command);
		cgidebug("AODV command : %s\n", command);
	}

	cgi_info_page(gettext("Wireless AODV Control"), gettext("Debug AODV or Clear all AODV Routes"), __wifi_aodv_ctrl_status());
	return 0;
}

