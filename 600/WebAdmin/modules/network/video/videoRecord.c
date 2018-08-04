/*
* $Id: videoRecord.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_video.h"

#include "cgi_services.h"
#include "aslib.h"

#define	CAM_RECORD_INDEX		"index"

#if 0
static char *_get_avi_files(char *dirName)
{
	struct dirent	**namelist;
	struct stat 	fstats;
	char			filename[256];
	int n, i = 0;
	char		buf[4096*5];
	int		length = 0;
	
	n = scandir( dirName, &namelist, NULL, alphasort);
	while(n--)
	{
		if( !strcasecmp(namelist[n]->d_name, ".") ||!strcasecmp(namelist[n]->d_name, "..") )
			continue;
		sprintf(filename, "%s/%s", dirName, namelist[n]->d_name );
		stat(filename, &fstats);
		
		length += CGI_SPRINTF(buf,length, "<TR><TD >%s</TD><TD >%ld KB</TD></TR>\r\n", 
			namelist[n]->d_name, KB(fstats.st_size) );
		
		free(namelist[n]);
		i++;
	}
		
	free(namelist);
	return strdup(buf);
}
#endif

static int	_video_command_activate(VIDEO_INFO *info, char *action, int index)
{
	char	cmd[1024];
	char	key[128], *value;
	int 	length = 0;
	char	target[256];
	sprintf(target, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS );

	length += CGI_SPRINTF(cmd, length, VIDEO_RPOCESS_RECORDER);
	
	sprintf(key, VIDEO_KEYWORD_CAMERA_NETWORK_PORT"_%d", index);
	value = GET_VALUE( &info->globalCfgs, key);
	if(value && strlen(value) )
		length += CGI_SPRINTF(cmd, length, " -w 127.0.0.1:%s", value);
	
	sprintf(key, VIDEO_KEYWORD_CAMERA_FILE_DURATION"_%d", index);
	value = GET_VALUE( &info->globalCfgs, key);
	if(value && strlen(value) )
		length += CGI_SPRINTF(cmd, length, " -p %s", value);
	
	value = GET_VALUE( &info->globalCfgs, VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR);
	if(value && strlen(value) )
		length += CGI_SPRINTF(cmd, length, " -o %s/cam%d", value, index );

//	length += CGI_SPRINTF(cmd, length, " > /dev/null  2>&1 &");
	length += CGI_SPRINTF(cmd, length, " > /dev/null  &");
//	printf("%s\n", cmd);

	cgi_refresh_page(3, target,gettext(CGI_STR_WAIT), gettext("Camera Record is starting.....") );

#if 0
	fork_cmd(cmd);
#else

	system(cmd);
#endif

	return 0;
}

static int	_video_command_deactivate(VIDEO_INFO *info, char *action, int pid)
{
	char	cmd[128];
	char	target[256];
	sprintf(target, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS );
	
	sprintf(cmd,"kill -9 %d",pid);
	system(cmd);

	cgi_refresh_page(3, target,gettext(CGI_STR_WAIT), gettext("Camera Record is stopping.....") );

	return 0;
}

static char *_video_record_status_page(VIDEO_INFO *info)
{
	char 	buf[8192*20];
	int 		length = 0;
	int		i;
	int		count= 0;
#if 0	
	char		key[256];
	char		*homeDir;
#endif	
	int		pid;
	service_state_t	state = SERVICE_STATE_SLEEPING;
	
	char		*value = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_NUM);
	if( value && strlen(value) )
		count = atoi(value);

	for(i=0; i< count; i++ )
	{
		char	target[512];
		cgi_camera_t *camera = glist_get_data(info->mgr.cameras, i);
		if(!camera)
		{
			fprintf(stderr, "No camera is found with index = %d\n", i);
			continue;
		}
		
		value = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_NETWORK_PORT);
		pid = cgi_get_program_pid_with_param(VIDEO_RPOCESS_RECORDER, value);
		if(pid>0)
		{
			state = SERVICE_STATE_RUNNING;
			sprintf(target, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS"&"CGI_ACTION"=%s&"CAM_RECORD_INDEX"=%d",
				CGI_ACTION_DEACTIVATE, pid );
		}
		else
		{
			if( strstr(camera->status, "yes") )
			{
				sprintf(target, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"="CGI_VIDEO_OP_STATUS"&"CGI_ACTION"=%s&"CAM_RECORD_INDEX"=%d", CGI_ACTION_ACTIVATE, i );
			}
			else
				target[0] = '\0';
			
		}
		
		length += CGI_SPRINTF(buf,length, "<TR><TD><table width=\"100%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(buf,length, "<TR><TD ><strong>%s %d</strong></TD><TD align=\"center\">%s</TD>\r\n", 
			gettext("Camera Record"), i+1, cgi_service_state_name(state) );

		if(strlen(target) )
		{
			length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n", 
				cgi_button((pid>0)?gettext(CGI_ACTION_DEACTIVATE):gettext(CGI_ACTION_ACTIVATE), target) );
		}
		else
		{
			length += CGI_SPRINTF(buf,length, "<TD align=\"center\">%s</TD></TR>\r\n" ,gettext("can not startup because of Camera Server is not running now") );
		}
#if 1
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3 align=\"center\"><img height=10 src=\"/sp.gif\" width=10><br><a href='/%s/cam%d/'  target='mainframe'>%s</a><IMG SRC='/pd.gif' BORDER=0></TD></TR>\r\n</table></TD></TR>\n", 
			VIDEO_KEYWORD_CAMERA_DATA_HOME, i, gettext("Recorded Movie Files") );
#else		
		homeDir = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR);
		sprintf(key, "%s/cam%d", homeDir, i);
		length += CGI_SPRINTF(buf,length, "%s\r\n", _get_avi_files(key) );
#endif

	}

	return strdup(buf);
}

int	videoRecordStatus(VIDEO_INFO *info)
{
	char *action;
	int	index;

	action = GET_CGI_ACTION( &info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Status of Web Camera Record"), gettext("Web Camera Record"), _video_record_status_page(info));
		return 0;
	}	

	index = atoi(GET_VALUE( &info->cgiVariables, CAM_RECORD_INDEX) );

	if(!strcasecmp(action, CGI_ACTION_DEACTIVATE) )
		return _video_command_deactivate( info,action, index);

	return _video_command_activate(info, action, index);
}

