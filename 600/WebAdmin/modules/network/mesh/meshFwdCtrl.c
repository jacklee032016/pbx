/*
* $Id: meshFwdCtrl.c,v 1.5 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_mesh.h"

static int	__mesh_fwd_ctrl(WIFI_INFO *info)
{
	char	*action;
	char	*dest;
	char	command[512];
	
	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	dest  = GET_VALUE( &info->cgiVariables, MESH_KEYWORD_FWD_MAC);
	if(!dest||!strlen(dest) )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Destination is not provided") );
		return -1;
	}
	
	if(!strcasecmp(action, CGI_ACTION_ADD)  )
	{/*  add */
		char *meshDev = GET_VALUE( &info->cgiVariables, MESH_KEYWORD_PORTAL_DEV);
		sprintf(command, "%s 0 fwd add mac %s dev %s ", MESH_BIN_MGR_CONFIG, dest, meshDev );
	}
	else
	{
		sprintf(command, "%s 0 fwd del mac %s", MESH_BIN_MGR_CONFIG, dest);
	}

	SYSTEM_CALL(command);

	return 0;
}

static char *__mesh_fwd_table_info( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		dest[64],dev[64],isLocal[64],isStatic[64],ageTime[64], t1[64],t2[64];
	char		target[512];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n"
		"<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n", 
		gettext("Destination"), gettext(CGI_STR_DEVICE), gettext("isLocal"), gettext("isStatic"),  gettext("Age Timer"), gettext(CGI_STR_MANAGEMENT) );

	while(fgets(line,1024,fp))
	{
		int local = 0;
		count = sscanf(line,"%s %s %s %s %s %s %s", dest, dev, isLocal, isStatic, ageTime, t1, t2);
		if(count !=5)
			continue;

		if(!strcasecmp(isLocal, "Yes") )
		{
			local = 1;
		}
		else
			sprintf(target, WEB_URL_MESH"?"CGI_KEYWORD_OPERATOR"="CGI_MESH_OP_CTRL_FWD"&"CGI_ACTION"=%s&"MESH_KEYWORD_FWD_MAC"=%s", CGI_ACTION_DEL, dest);

		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD>"
			"<TD align=\"right\">%s</TD></TR>\r\n", 
		dest, dev, isLocal, isStatic, ageTime, (local==0)? cgi_button(gettext(CGI_STR_DELETE), target):"" );

	}
	
	return strdup(buf);
}

char *__mesh_fwd_buttons(WIFI_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	node 		*current ;
	
	current = info->meshDevs.head;

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("MESH Portal Parameters") );
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_MESH );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_MESH_OP_CTRL_FWD);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ADD);
/*bgcolor=\"#cccccc\" */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s:</strong></TD>\r\n<TD align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Destination(MAC Address)"),MESH_KEYWORD_FWD_MAC );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
		gettext(CGI_STR_MESH_DEVICE), MESH_KEYWORD_PORTAL_DEV );

	while(current)
	{
		if( strcasecmp(current->entry.name, WIFI_KEYWORD_MESH_NUM) )
		{
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\">%s</option>\n", 
				current->entry.name, current->entry.name );
		}
		current = current->next;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ADD) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MESH_PORTAL) );

	return strdup(buf);
}

int	mesh_fwd_tbl_ctrl(WIFI_INFO *info)
{
	char	 *cmd;
	char *action;
	FILE		*fp;

	if((fp = fopen(MESH_TBL_FWD, "r") )==NULL)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Can not open MESH forward table") );
		return -1;
	}

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if(!strcasecmp(cmd, CGI_MESH_OP_STAT_FWD ) )
	{
		cgi_info_page(gettext("MESH Forward Table"), gettext("Forward Items"), __mesh_fwd_table_info(fp));
		fclose( fp);
		return 0;
	}
	
	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);
	if(!strcasecmp(action, CGI_ACTION_ADD) || !strcasecmp(action, CGI_ACTION_DEL) )
	{/*  save */
		__mesh_fwd_ctrl(info);
	}
	
	cgi_info_page(gettext("Portal Configuration of MESH"), __mesh_fwd_table_info(fp), __mesh_fwd_buttons(info));

	fclose(fp);
	
	return 0;
}

