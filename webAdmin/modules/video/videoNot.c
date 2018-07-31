
int		*resolutions[3] =
{
	160, 120,
	"320x240",
	"640x480",
};



char *__video_mode_status(VIDEO_INFO *info)
{
	char 	buf[8192*10];
	int 		length = 0;
	int		i, j;
	char		*value;

//	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD>\n", gettext("Select Video Mode") );
	length += CGI_SPRINTF(buf, length, "<form name=\"videoForm\" method=\"post\" action=\"%s\">\n",WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_SETUP);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);

	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%d\">\n", VIDEO_KEYWORD_CAMERA_NUM, info->mgr.count );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR, 
		GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR));

	length += CGI_SPRINTF(buf,length, "<TR><TD><table border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space width=400>\r\n");
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
			gettext("Load Camera System when boot"), VIDEO_KEYWORD_ONBOOT );
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		VIDEO_VALUE_ON, (!strcasecmp(GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_ONBOOT),VIDEO_VALUE_ON) )?"selected":"",gettext(VIDEO_VALUE_ON));
	length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
		VIDEO_VALUE_OFF, (!strcasecmp(GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_ONBOOT),VIDEO_VALUE_OFF) )?"selected":"",gettext(VIDEO_VALUE_OFF));
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR></table>\r\n");

	if( info->isDriverLoad)
	{

		for(i=0; i< info->mgr.count; i++)
		{
			char	key[32];
			int	port;
			char	*tmp;
			
			cgi_camera_t *camera = glist_get_data(info->mgr.cameras, i);
			if(!camera)
			{
				fprintf(stderr, "No camera is found with index = %d\n", i);
				continue;
			}	

			length += sprintf(buf+length, "<table border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space width=400>\r\n");
			length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s %d</strong></TD><TD>%s</TD>\r\n", 
				gettext("Camera"), i, camera->name);
			length += sprintf(buf+length, "<TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD></TR>\r\n", 
				gettext("Sensor"), camera->sensor );
			length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD>\r\n", 
				gettext("Image Format"), camera->format );

			sprintf(key, VIDEO_KEYWORD_CAMERA_RESOLUTION"_%d", i);
			length += CGI_SPRINTF(buf, length, "\t<TD align=\"right\"><strong>%s</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
				gettext(VIDEO_DEV_KEY_RESOLUTION), key );

			tmp = GET_VALUE(&info->globalCfgs, key);
#if 0			
			for(j=0; j< camera->resolutionCount; j++ )
			{
				sprintf(key, VIDEO_KEYWORD_CAMERA_RESOLUTION"_%d", j);
				
//				sprintf(key, VIDEO_DEV_KEY_RESOLUTION"%d", j);
				value = GET_VALUE(&camera->resolutions, key);
//				length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s:</strong></TD><TD>%s</TD></TR>\r\n", 
//					gettext(VIDEO_DEV_KEY_RESOLUTION), value);
				length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
					value,  (!strcasecmp(tmp, value))?"selected":"",value);
				
			}
#else
			for(j=0; j< 2; j++ )
			{
				length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
					resulutions[j],  (!strcasecmp(tmp, resulutions[j]))?"selected":"",resulutions[j]);
			};
#endif
			length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD><TD>%s</TD><TD></TD><TD></TD>\r\n", 
				gettext("Grabbing"), camera->status);
			
			sprintf(key, VIDEO_KEYWORD_CAMERA_ONBOOT"_%d", i);
			
			length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
				gettext(CGI_STR_START_WHEN_BOOT), key );
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				VIDEO_VALUE_ON, (!strcasecmp(GET_VALUE(&info->globalCfgs, key),VIDEO_VALUE_ON) )?"selected":"",gettext(VIDEO_VALUE_ON));
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				VIDEO_VALUE_OFF, (!strcasecmp(GET_VALUE(&info->globalCfgs, key),VIDEO_VALUE_OFF) )?"selected":"",gettext(VIDEO_VALUE_OFF));
			length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

			sprintf(key, VIDEO_KEYWORD_CAMERA_NETWORK_PORT"_%d", i);
			value = GET_VALUE(&info->globalCfgs, key);
			if(!value || strlen(value)==0)
				port = ASCAM_SERVER_PORT +i;
			else
				port = atoi(value);
			
			length += sprintf(buf+length, "<td align=\"right\"><strong>%s</strong></td><td><input name=\"%s\" onKeyPress=\"checkIntegerNumber(this)\" type=\"text\" size=\"15\" value=\"%d\" maxlength=\"50\"></td></TR>",
				gettext("Port Number"), key, port );

			sprintf(key, VIDEO_KEYWORD_CAMERA_RECORD"_%d", i);
			
			length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"><strong>%s</strong></TD><TD>\n\t<select name=\"%s\" style='width:120px;'>\n", 
				gettext("Need Record"), key );
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				VIDEO_VALUE_ON, (!strcasecmp(GET_VALUE(&info->globalCfgs, key),VIDEO_VALUE_ON) )?"selected":"",gettext(VIDEO_VALUE_ON));
			length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
				VIDEO_VALUE_OFF, (!strcasecmp(GET_VALUE(&info->globalCfgs, key),VIDEO_VALUE_OFF) )?"selected":"",gettext(VIDEO_VALUE_OFF));
			length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>\r\n");

			sprintf(key, VIDEO_KEYWORD_CAMERA_FILE_DURATION"_%d", i);
			value = GET_VALUE(&info->globalCfgs, key);
			if(!value || strlen(value)==0)
				port = 120;
			else
				port = atoi(value);
			
			length += sprintf(buf+length, "<td align=\"right\"><strong>%s</strong></td><td><input name=\"%s\" onKeyPress=\"checkIntegerNumber(this)\" type=\"text\" size=\"15\" value=\"%d\" maxlength=\"50\">%s</td></TR>",
				gettext("File Length"), key, port, gettext("Minutes") );
		
			length += sprintf(buf+length, "</TABLE>\r\n");
		
		}

		length += CGI_SPRINTF(buf,length, "<table align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space width=400>\r\n \
			<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

		length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></table></TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_PBX_ACCOUNT) );

	}
	return strdup(buf);
}


int videoConfig(VIDEO_INFO *info)
{
	char *action;
	
	action = GET_VALUE( &info->cgiVariables, CGI_ACTION);

	if(!strcasecmp(action, CGI_ACTION_DEFAULT) )
	{/* mode save and display basic parameters */
		char		msg[2048];
		int 		len = 0;
		cgi_camera_save_config(info);

		len += sprintf(msg+len, "<strong>%s</strong><br>%s", gettext(CGI_STR_NOTE), gettext("When hardware has been changed, you should reconfig it in this page!") );
		cgi_info_page( gettext("Basic Parameters of Video"), gettext("Camera Configuration will be take effective when next boot"), msg);
		return 0;
	}

	cgi_info_page(gettext("Basic Parameters of Video"), __video_mode_info(info), __video_mode_status(info));
	return 0;
}


int	_cgi_camera_dev_init(VIDEO_INFO *info)
{
	char		line[PARSE_FILE_MAX_LENGTH];
	FILE  	*fp;
	int		ret = 0;
	char		t1[32], t2[32];
	int		i;
	
	if((fp=fopen(CGI_VIDEO_PROC_ENGINE_NAME, "r"))==NULL)
	{
		perror( "Open Video SubSystem proc file failure!" );
		return ret;
	}

	fgets(line,PARSE_FILE_MAX_LENGTH,fp);
	int __count = sscanf(line, "%s %s", t1, t2);

	fprintf(stderr, "%s", t2);
	if(__count == 2)
	{
		info->mgr.count = atoi(t2);
	}
	fprintf(stderr, "%s, count =%d\n", t2, info->mgr.count);

	fgets(line,PARSE_FILE_MAX_LENGTH,fp);
//	line[ret] = '\0';
	strncpy(info->mgr.version, line, (strlen(line)>sizeof(info->mgr.version))?sizeof(info->mgr.version):strlen(line) );

	fclose(fp);

	info->mgr.cameras = NULL;
	for(i=0; i< info->mgr.count; i++)
	{
		cgi_camera_t *camera;
		char		resolution[32];
		char		fileName[128];
		sprintf(fileName, CGI_VIDEO_PROC_DEVICE_NAME"%d", i);
		if((fp=fopen(fileName, "r"))==NULL)
		{
			perror( "Open Video SubSystem proc file failure!" );
			info->mgr.count = i;
			return -1;
		}

		camera = (cgi_camera_t *)malloc(sizeof(cgi_camera_t) );
		if(!camera )
		{
			info->mgr.count = i;
			return -1;
		}
		memset(camera, 0 ,sizeof(cgi_camera_t) );
		LIST_CREATE( &camera->resolutions);

		info->mgr.cameras = glist_add( info->mgr.cameras, i, camera );
		
		while( fgets(line,PARSE_FILE_MAX_LENGTH,fp) )
		{
//			line[ret] = '\0';
			if(strstr(line, "Device" ) )
			{
				ret = __get_device_info( camera->name, ASCAM_NAME_LENGTH, line );
			}
			if(strstr(line, "Sensor" ) )
			{
				ret = __get_device_info( camera->sensor, ASCAM_NAME_LENGTH, line );
			}
			if(strstr(line, "Format" ) )
			{
				ret = __get_device_info( camera->format, ASCAM_NAME_LENGTH, line );
			}
			if(strstr(line, "Streaming" ) )
			{
				ret = __get_device_info( camera->status, ASCAM_NAME_LENGTH, line );
			}
			if(strstr(line, VIDEO_DEV_KEY_RESOLUTION ) )
			{
				char	key[32];
				ret = __get_device_info( resolution, sizeof(resolution), line );
				sprintf(key, VIDEO_KEYWORD_CAMERA_RESOLUTION"_%d", camera->resolutionCount );
				list_append_raw(&camera->resolutions, key, resolution, NULL, NULL);
				camera->resolutionCount++;
			}
		}

		fclose(fp);
		
	}

	return 0;
}


int	__get_device_info(char *dest, int destSize, char *src )
{
	char	*p = strchr(src, ':');
	if(!p)
		return -1;
	p++;
	p++;

	strncpy(dest, p, (destSize>strlen(p) )?strlen(p):destSize );

	return 0;
}

