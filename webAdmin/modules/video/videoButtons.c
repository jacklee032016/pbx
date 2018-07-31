/*
* $Id$
*/

#include "_video.h"


typedef	struct	
{
	int							msgOpType;
	VIDEO_BUTTON_TYPE_T		type;
	char							*key;
	char							*description;
}button_config_t;

static button_config_t	_buttonConfigs[] =
{
	{
		CMN_CMD2SERVER_IMG_SHARPNESS,
		VIDEO_BUTTON_TYPE_SHARPNESS,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Sharpness"
	},
	{
		CMN_CMD2SERVER_IMG_HUE,
		VIDEO_BUTTON_TYPE_HUE,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Hue"
	},
	{
		CMN_CMD2SERVER_IMG_SATURATION,
		VIDEO_BUTTON_TYPE_SATURATION,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Saturation"
	},
	{
		CMN_CMD2SERVER_IMG_CONTRAST,
		VIDEO_BUTTON_TYPE_CONTRAST,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Contrast"
	},
	{
		CMN_CMD2SERVER_IMG_BRIGHTNESS,
		VIDEO_BUTTON_TYPE_BRIGHTNESS,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Brightness"
	},
	{
		-1,
		VIDEO_BUTTON_TYPE_MAX,
		NULL,
		NULL
	}
	
};

static button_config_t *__findButtonConfigs(VIDEO_BUTTON_TYPE_T buttonType)
{
	button_config_t *config = _buttonConfigs;

	while(config->type < VIDEO_BUTTON_TYPE_MAX)
	{
		if(config->type == buttonType)
		{
			return config;
		}
		
		config = config+1;
	};

	return NULL;
}

static char *__video_buttons_status(VIDEO_INFO *info, VIDEO_BUTTON_TYPE_T buttonType)
{
	char 	buf[8192*5];
	int 		length = 0;
	
	button_config_t *config = __findButtonConfigs(buttonType);
	if(config == NULL)
		return strdup("");
	
//	length += CGI_SPRINTF(buf,length, "\t\n<script src=\"/include/video.js\"></script>\r\n" );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>'%s' %s</strong><p></TD></TR>\n", config->description , gettext(CGI_STR_PARAMETER));
	length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", config->description, WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"center\"></TD></TR><TR><TD align=\"center\">\t\n" );

	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"button\" value=\"-\" name=\"B3\" onClick=\"dec(%s);\">\n", config->description);
	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"button\" name=\"%s\" onClick=\"mid(%s);\">\n", config->description, config->description);
	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"button\" value=\"+\" name=\"B5\" onClick=\"inc(%s);\">\n", config->description);
//	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"hidden\" name=\"%s\">\n", config->description);
	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"hidden\" name=\"data\" value=\"5\">\n" );
	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"hidden\" name=\"%s\" value=\"%d\">\n" , CGI_VIDEO_MSG_OP_TYPE, config->msgOpType );
	
	length += CGI_SPRINTF(buf,length, "\t\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\">  </TD></TR>" );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MEDIA_CONFIG) );

	return strdup(buf);
}


int	videoButtonsForm(VIDEO_INFO *info, VIDEO_BUTTON_TYPE_T buttonType)
{
	char *action;
	int	msgOpType = -1;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgidebug("webPages");
		cgi_info_page(gettext("Media Configuration"), gettext("Basic parameters of Media are set here"), __video_buttons_status(info, buttonType));
		cgidebug("webPages Ended!");
		return 0;
	}	
	else if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
	{
		msgOpType = atoi(GET_VALUE(&info->cgiVariables,CGI_VIDEO_MSG_OP_TYPE ) );
		videoActionButton(info, msgOpType );
		return cgi_camera_save_config(info );
	}
	else
	{
		cgi_invalidate_page();
	}
	
	return 0;
}

