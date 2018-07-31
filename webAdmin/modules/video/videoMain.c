/*
* $Id: videoMain.c,v 1.9 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "_video.h"

int cgi_camera_save_config(VIDEO_INFO *info)
{
	char 	target[256];
	char 	cmp1[256];
	char 	cmp2[256];
	char		*temp;
	node	*cunode;

	cunode=info->cgiVariables.head;

	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);


	if(!strcmp(cunode->entry.value,CGI_VIDEO_OP_SETUP_RESOLUTION))
	{
		temp = GET_VALUE( &info->cgiVariables,  VIDEO_KEYWORD_CAMERA_RESOLUTION);
 		sprintf(cmp1,"%s",temp);
		
		 	if(!strcmp(cmp1,"640"))
			list_replace_value_by_key(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_RESOLUTION, "640x480");
		 if(!strcmp(cmp1,"320"))
			list_replace_value_by_key(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_RESOLUTION, "320x240");
	 	if(!strcmp(cmp1,"160"))
			list_replace_value_by_key(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_RESOLUTION, "160x120");
	}

	if(writeconfig(CAMERA_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}

        list_replace_with_new_list(&info->global2Cfgs, &info->cgiVariables);
	if(!strcmp(cunode->entry.value,CGI_VIDEO_OP_SETUP_AUDIO))
		{
			temp = GET_VALUE( &info->cgiVariables,  VIDEO_KEYWORD_AUDIO_ENABLE);	 
			sprintf(cmp2,"%s",temp);
		
   		 	if(!strcmp(cmp2,"0"))
			 	list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_ENABLE, "NO");
		 	if(!strcmp(cmp2,"1"))				
			 	list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_ENABLE, "adpcm_ima_wav");
		}
	if(!strcmp(cunode->entry.value,CGI_VIDEO_OP_SETUP_RESOLUTION))
		{
   		 	if(!strcmp(cmp1,"640"))
				list_replace_value_by_key(&info->global2Cfgs, "VideoSize", "640x480");
			 if(!strcmp(cmp1,"320"))
				list_replace_value_by_key(&info->global2Cfgs, "VideoSize", "320x240");
		 	if(!strcmp(cmp1,"160"))
				list_replace_value_by_key(&info->global2Cfgs, "VideoSize", "160x120");
		}
	if(!strcmp(cunode->entry.value,CGI_VIDEO_OP_ENAUTH))
		{
			temp = GET_VALUE( &info->cgiVariables,  VIDEO_KEYWORD_AUDIO_ENAUTH);
	 		sprintf(cmp1,"%s",temp);
   		 	if(!strcmp(cmp1,"0"))
				list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_ENAUTH, "NO");
			 if(!strcmp(cmp1,"1"))
				list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_ENAUTH, "YES");
		 	
		}
	if(!strcmp(cunode->entry.value,CGI_VIDEO_OP_WATCHDOG))
		{
			temp = GET_VALUE( &info->cgiVariables,  VIDEO_KEYWORD_AUDIO_WATCHDOG);
	 		sprintf(cmp1,"%s",temp);
   		 	if(!strcmp(cmp1,"0"))
				list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_WATCHDOG, "NO");
			 if(!strcmp(cmp1,"1"))
				list_replace_value_by_key(&info->global2Cfgs, VIDEO_KEYWORD_AUDIO_WATCHDOG, "YES");
		 	
		}
	if(writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) )
	{
		return -1;
	}


	list_replace_with_new_list(&info->motionCfgs, &info->cgiVariables);
	if(writeconfig(MOTION_CONFIG_FILE, ITEM_EQU, NULL, &info->motionCfgs) )
	{
		return -1;
	}
	
	CGI_SPRINTF(target, 0, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"=%s", GET_CGI_OP(&info->cgiVariables) );
	cgi_refresh_page(1, target, gettext(CGI_STR_PLEASE_WAIT), gettext("Media setup is saveing!") );

	return 0;
}

static VIDEO_INFO *cgi_camera_init(void)
{
	VIDEO_INFO	*info;
	
	info = (VIDEO_INFO *)malloc( sizeof(VIDEO_INFO) );
	if(!info)
		return NULL;
	memset(info, 0 ,sizeof(VIDEO_INFO) );
	
	cgi_html_header();

	read_cgi_input(&info->cgiVariables, NULL, NULL);

	info->isDriverLoad = 1; //cgi_check_usb_driver( ASCAM_USB_DRIVER_NAME );
	if(! info->isDriverLoad )
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("Video Driver is not loaded now") );
//		return NULL;
	}

	if (readconfig(CAMERA_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		cgi_error_page(NULL, gettext("Video Configuration File is not found"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}

	if (readconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) ) 	
	{
		cgi_error_page(NULL, gettext("server Configuration File is not found"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}
	get_ACLvalue(info);

	if (readconfig(RECORD_CONFIG_FILE, ITEM_DBL, NULL, &info->recordCfgs) ) 	
	{
		cgi_error_page(NULL, gettext("record Configuration File is not found"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	
	}
	
	if (readconfig(MOTION_CONFIG_FILE, ITEM_EQU, NULL, &info->motionCfgs) )
	{
		cgi_error_page(NULL, gettext("Motion Detection Configuration File is not found"), gettext( CGI_STR_SOME_ERROR ) );
		return NULL;
	}
	
#if 0
	if(info->isDriverLoad)
	{
		_cgi_camera_dev_init( info);
//		info->isPlugged = cgi_check_usb_device("Vendor=0ac8", "ProdID=301b");
		info->isPlugged = info->mgr.count ;
		if(! info->isPlugged )
		{
			cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No USB Video Hardware is found") );
	//		return NULL;
		}
	}
#endif	
	return info;
}

char *__video_mode_info(VIDEO_INFO *info)
{
	char 	buf[8192*5];
#if 0
	int 		length = 0;
	if( info->isDriverLoad)
	{
		length += sprintf(buf+length, "<TR><TD align=\"right\" colspan=2><strong>%s</strong></TD><TD  colspan=2>%s</TD></TR>\r\n", 
			gettext("Driver Version"), info->mgr.version );
	}
#endif
	return strdup(buf);
}

int main(int argc, char *argv[])
{
	char 	*cmd=NULL;
	VIDEO_INFO		*info = cgi_camera_init();
	VIDEO_SELECT_TYPE_T		selectType = VIDEO_SELECT_TYPE_MAX;
	VIDEO_BUTTON_TYPE_T	buttonType = VIDEO_BUTTON_TYPE_MAX;
	VIDEO_SIMPLE_BUTTON_TYPE_T simpleButtonType = VIDEO_SIMPLE_BUTTON_TYPE_MAX;
	if( !info )
		return 1;

	cmd = GET_CGI_OP( &info->cgiVariables);
	if( !cmd || !strlen(cmd) )
	{
		cgi_invalidate_page();
		return 0;
	}

	if(!strcasecmp(cmd, CGI_VIDEO_OP_VIEW) )
	{
		return videoView(info);
	}
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_MOTION_DETECT) )
	{
		return videoMotionForm(info);
	}
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_RECORD) )
	{
		return videoRecordStatus(info);
	}

	if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_MIRROR)  )
		simpleButtonType = VIDEO_S_BUTTON_TYPE_MIRROR;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_VFLIP) )
		simpleButtonType = VIDEO_S_BUTTON_TYPE_VFLIP;
	if(simpleButtonType != VIDEO_SIMPLE_BUTTON_TYPE_MAX)
	{
		return videoSimpleButtonsForm(info, simpleButtonType);
	}

	if(!strcasecmp(cmd, CGI_VIDEO_OP_STREAM_DESC) )				
		return	video_streamdesc(info);			
	if(!strcasecmp(cmd, CGI_VIDEO_OP_STREAM_CFG) )		
		return	video_streamcfg(info);	
	if(!strcasecmp(cmd, CGI_VIDEO_OP_MEDIASERVER_LOG) )		
		{			
			selectType = VIDEO_SELECT_TYPE_MEDIASERVER_LOG;  			
			return video_mediaserverlog(info,selectType);		
		}	
	if(!strcasecmp(cmd, CGI_VIDEO_OP_ACL) )		
		{		
			selectType = VIDEO_SELECT_TYPE_ACL;		
			VIDEO_SELECT_TYPE_T		selectType2 =VIDEO_SELECT_TYPE_STREAMSELECT; 		
			return	video_ACL(info,selectType,selectType2);		
		}	
	if(!strcasecmp(cmd, CGI_VIDEO_OP_STATUS) )	
	{		
		return media_status(info);	
	}

	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_QUALITY)  )
		selectType = VIDEO_SELECT_TYPE_QUALITY;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_FLICKER) )
		selectType = VIDEO_SELECT_TYPE_FLICKER;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_RESOLUTION) )
		selectType = VIDEO_SELECT_TYPE_RESOLUTION;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_BITRATE) )
		selectType = VIDEO_SELECT_TYPE_BITRATE;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_FRAMERATE) )
		selectType = VIDEO_SELECT_TYPE_FRAMERATE;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_AUDIO) )
		selectType = VIDEO_SELECT_TYPE_AUDIO;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_AUDIOSR) )
		selectType = VIDEO_SELECT_TYPE_AUDIOSR;                           
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_AUDIOCH) )
		selectType = VIDEO_SELECT_TYPE_AUDIOCH;                           
      
	  else if(!strcasecmp(cmd, CGI_VIDEO_OP_ENAUTH) )
		selectType = VIDEO_SELECT_TYPE_ENAUTH;    
	  else if(!strcasecmp(cmd, CGI_VIDEO_OP_WATCHDOG) )
		selectType = VIDEO_SELECT_TYPE_WATCHDOG;  

	if(selectType != VIDEO_SELECT_TYPE_MAX)
	{
		return videoSelectForm(info, selectType);
	}
	
	if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_SHARPNESS)  )
		buttonType = VIDEO_BUTTON_TYPE_SHARPNESS;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_HUE) )
		buttonType = VIDEO_BUTTON_TYPE_HUE;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_SATURATION) )
		buttonType = VIDEO_BUTTON_TYPE_SATURATION;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_CONTRAST) )
		buttonType = VIDEO_BUTTON_TYPE_CONTRAST;
	else if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP_BRIGHTNESS) )
		buttonType = VIDEO_BUTTON_TYPE_BRIGHTNESS;

	if(buttonType != VIDEO_BUTTON_TYPE_MAX)
	{
		return videoButtonsForm(info, buttonType);
	}
#if 0	
	if(!strcasecmp(cmd, CGI_VIDEO_OP_SETUP) )
	{
		return videoConfig(info);
	}
	if(!strcasecmp(cmd, CGI_VIDEO_OP_STATUS) )
	{
		return videoRecordStatus(info);
	}
#endif	
	else
	{
		return cgi_future_page(cmd, gettext("Media Stream") );
//		cgi_invalidate_page();
//		return 0;
	}
	
	return 0;	
}


