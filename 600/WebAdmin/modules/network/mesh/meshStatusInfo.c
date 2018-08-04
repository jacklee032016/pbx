/*
* $Id: meshStatusInfo.c,v 1.5 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_mesh.h"

typedef char *(*mesh_read_func_t)(FILE *fp);

static char *__mesh_devs_info( FILE *fp)
{
	char 	buf[8192*5];
	int 		length = 0;
	char 	line[1024];
	int		count;
	char		dev[64], mac[64], type[64], t1[64],t2[64];

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n \
		<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n \
		<TD align=\"right\" bgcolor=\"#cccccc\"><strong>%s</strong></TD>\n</TR>\r\n", 
		gettext(CGI_STR_DEVICE), gettext("MAC"), gettext(CGI_STR_TYPE) );

	while(fgets(line,1024,fp))
	{
		count = sscanf(line,"%s %s %s %s %s", dev, mac, type, t1, t2);
		if(count != 3 )
			continue;
		length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\">%s</TD>\n \
			<TD align=\"right\">%s</TD> \
			<TD align=\"right\">%s</TD></TR>\r\n", 
		dev, mac, type);
	}
	
	return strdup(buf);
}

int	mesh_status_infos(WIFI_INFO *info)
{
	char 	*cmd=NULL;
	char		*title;
	char		*msg;
	FILE		*fp;
	mesh_read_func_t readFunc;

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}
	
	if(!strcasecmp(cmd, CGI_MESH_OP_STAT_DEVICE) )
	{
		fp=fopen(MESH_TBL_MGR, "r");
		title = "MESH Devices Information";
		readFunc = __mesh_devs_info;
	}
	
	if(!fp)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Mesh File Open Failed!Maybe no mesh driver is loaded") );
		return 0;
	}
	msg = (*readFunc)(fp);
	
	cgi_info_page(gettext("Status Information of MESH"), title, msg );

	fclose(fp);
	
	return 0;
}

