/*
* $Id$
*/

#include "_video.h"


typedef	struct	
{
	int							msgOpType;
	VIDEO_SIMPLE_BUTTON_TYPE_T	type;
	char							*key;
	char							*description;
}button_config_t;

static button_config_t	_buttonConfigs[] =
{
	{
		CMN_CMD2SERVER_MIRROR,
		VIDEO_S_BUTTON_TYPE_MIRROR,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Mirror"
	},
	{
		CMN_CMD2SERVER_VFLIP,
		VIDEO_S_BUTTON_TYPE_VFLIP,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Vertical Flip"
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

static char *__video_simple_buttons_status(VIDEO_INFO *info, VIDEO_SIMPLE_BUTTON_TYPE_T simpleButtonType)
{
	char 	buf[8192*5];
	int 		length = 0;
	
	button_config_t *config = __findButtonConfigs(simpleButtonType);
	if(config == NULL)
		return strdup("");
	
//	length += CGI_SPRINTF(buf,length, "\t\n<script src=\"/include/video.js\"></script>\r\n" );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>%s</strong><p></TD></TR>\n", config->description );
	length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", config->description, WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"hidden\" name=\"%s\" value=\"%d\">\n" , CGI_VIDEO_MSG_OP_TYPE, config->msgOpType );

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\">  </TD></TR>" );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MEDIA_CONFIG) );

	return strdup(buf);
}


int	videoSimpleButtonsForm(VIDEO_INFO *info, VIDEO_SIMPLE_BUTTON_TYPE_T simpleButtonType)
{
	char *action;
	int	msgOpType = -1;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Media Configuration"), gettext("Dynamic Configuration of Media are set here"), __video_simple_buttons_status(info, simpleButtonType));
		return 0;
	}	
	else if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
	{
		msgOpType = atoi(GET_VALUE(&info->cgiVariables,CGI_VIDEO_MSG_OP_TYPE ) );
		videoActionButton(info, msgOpType );
		return cgi_camera_save_config(info);;
	}
	else
	{
		cgi_invalidate_page();
	}
	
	return 0;
}

