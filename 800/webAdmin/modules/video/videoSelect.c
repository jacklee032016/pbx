/*
* $Id$
*/

//#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
//#include <string.h>
#include "cgiLib.h"
#include	<ctype.h>

#include "_video.h"
#define	NIC_KEYWORD_IP_ADDRESS					"IPADDR"


struct video_select_t
{
	int		value;
	char		*description;
};

typedef	struct	
{
	VIDEO_SELECT_TYPE_T			type;
	int							msgOpType;
	struct video_select_t			*selects;
	char							*key;
	char							*description;
}select_config_t;


static struct video_select_t _qualities[] =
{
	{
		0,
		"Low"
	},
	{
		15,
		"Middle"
	},
	{
		30,
		"High"
	},
	{
		-1,
		NULL	
	}
		
};

static struct video_select_t _flickers[] =
{
	{
		0,
		"50HZ"
	},
	{
		1,
		"60HZ"
	},
	{
		-1,
		NULL	
	}
	
};


static struct video_select_t _resolutions[] =
{
	{
		640,
		"640x480"
	},
	{
		320,
		"320x240"
	},
	{
		160,
		"160x120"
	},
	{
		-1,
		NULL	
	}
	
};


static struct video_select_t _frameRates[] =
{
	{
		5,
		"5"
	},
	{
		10,
		"10"
	},
	{
		15,
		"15"
	},
	{
		20,
		"20"
	},
	{
		25,
		"25"
	},
	{
		30,
		"30"
	},
	{
		-1,
		NULL	
	}
	
};

static struct video_select_t _bitRates[] =
{
	{
		128,
		"128"
	},
	{
		256,
		"256"
	},
	{
		384,
		"384"
	},
	{
		512,
		"512"
	},
	{
		640,
		"640"
	},
	{
		768,
		"768"
	},
	{
		896,
		"896"
	},
	{
		1024,
		"1024"
	},
	{
		-1,
		NULL	
	}
};

static struct video_select_t _audio[] =
{
	{
		0,
		"Disable"
	},
	{
		1,
		"Enable"
	},
	{
		-1,
		NULL	
	}
		
};
/*add by wyb*/
static struct video_select_t _audiosr[] =
{

	{
		44100,
		"44100"
	},
	{
		22050,
		"22050"
	},
	{
		11025,
		"11025"
	},

	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _audioch[] =
{
	{
		1,
		"1"
	},
	{
		2,
		"2"
	},

	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _enauth[] =
{
	{
		0,
		"Disable"
	},
	{
		1,
		"Enable"
	},

	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _watchdog[] =
{
	{
		0,
		"Disable"
	},
	{
		1,
		"Enable"
	},

	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _mediaserverlog[] =
{
	{
		0,
		"LOG_EMERG"
	},
	{
		1,
		"LOG_ALERT"
	},
	{
		2,
		"LOG_CRIT"
	},
	{
		3,
		"LOG_ERR"
	},
	{
		4,
		"LOG_WARNING"
	},
	{
		5,
		"LOG_NOTICE"
	},
	{
		6,
		"LOG_INFO"
	},
	{
		7,
		"LOG_DEBUG"
	},
	
	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _ACL[] =
{
	{
		0,
		"DENY"
	},
	{
		1,
		"ALLOW"
	},

	{
		-1,
		NULL	
	}
		
};
static struct video_select_t _streamselect[] =
{
	{
		1,
		"Stream1"
	},
	{
		2,
		"Stream2"
	},
	{
		3,
		"Stream3"
	},
	{
		-1,
		NULL	
	}
		
};


static select_config_t	_selectConfigs[] =
{
	{
		VIDEO_SELECT_TYPE_QUALITY,
		CMN_CMD2SERVER_VIDEO_QUALITY,	
		_qualities,
		VIDEO_KEYWORD_CAMERA_QUALITY,
		"Quality"
	},
	{
		VIDEO_SELECT_TYPE_FLICKER,
		CMN_CMD2SERVER_FLICKER,
		_flickers,
		VIDEO_KEYWORD_CAMERA_FLICKER,
		"Flickers"
	},
	{
		VIDEO_SELECT_TYPE_RESOLUTION,
		CMN_CMD2SERVER_IMG_RESOLUTION,	
		_resolutions,
		VIDEO_KEYWORD_CAMERA_RESOLUTION,
		"Resolution"
	},
	{
		VIDEO_SELECT_TYPE_BITRATE,
		0,/* note : no bitrate is defined */	
		_bitRates,
		VIDEO_KEYWORD_CAMERA_BITRATE,
		"BitRate"
	},
	{
		VIDEO_SELECT_TYPE_FRAMERATE,
		CMN_CMD2SERVER_IMG_FRAMERATE,
		_frameRates,
		VIDEO_KEYWORD_CAMERA_FRAMERATE,
		"FrameRate"
	},
	{
		VIDEO_SELECT_TYPE_AUDIO,
		CMN_CMD2SERVER_A_MUTE,	
		_audio,
		VIDEO_KEYWORD_AUDIO_ENABLE,
		"on/off"
	},
	{
		VIDEO_SELECT_TYPE_AUDIOSR,	
		CMN_CMD2SERVER_A_SAMPLE_RATE,
		_audiosr,
		VIDEO_KEYWORD_AUDIO_SAMLPERATE,
		"Audio samplerate"	
	},
	{
		VIDEO_SELECT_TYPE_AUDIOCH,	
		CMN_CMD2SERVER_A_CHANNELS,
		_audioch,
		VIDEO_KEYWORD_AUDIO_CHANNELS,
		"Audio Channels"
	},
	{	VIDEO_SELECT_TYPE_ENAUTH,
		0,
		_enauth,
		VIDEO_KEYWORD_AUDIO_ENAUTH,
		"enauth"
	},
	{	VIDEO_SELECT_TYPE_WATCHDOG,
		0,
		_watchdog,
		VIDEO_KEYWORD_AUDIO_WATCHDOG,
		"watchdog"
	},
	{	VIDEO_SELECT_TYPE_MEDIASERVER_LOG,
		0,
		_mediaserverlog,
		VIDEO_KEYWORD_MEDIASERVER_LOG,
		"mediaserverlog"
	},
	
	{	VIDEO_SELECT_TYPE_ACL,
		0,
		_ACL,
		VIDEO_KEYWORD_ACL,
		"ACL"			
	},
	
	{	VIDEO_SELECT_TYPE_STREAMSELECT,
		0,
		_streamselect,
		VIDEO_KEYWORD_ACL,   
		"streamselect"
	},
	
	{
		VIDEO_SELECT_TYPE_MAX,
		-1,	
		NULL,
		NULL,
		NULL
	}
	
};

static select_config_t *__findSelectConfigs(VIDEO_SELECT_TYPE_T selectType)
{
	select_config_t *config = _selectConfigs;

	while(config->type < VIDEO_SELECT_TYPE_MAX)
	{
		if(config->type == selectType)
			return config;
		
		config++;
	};

	return NULL;
}

static char *__video_select_status(VIDEO_INFO *info, VIDEO_SELECT_TYPE_T selectType)
{
	char 	buf[8192*5];
	int 		length = 0;
	struct video_select_t *content;
	
	select_config_t *config = __findSelectConfigs(selectType);
	if(config == NULL)
		return strdup("");
	
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2><strong>'%s' %s</strong><p></TD>\n", config->description, gettext(CGI_STR_PARAMETER) );
	length += CGI_SPRINTF(buf, length, "<form name=\"wifiForm\" method=\"post\" action=\"%s\">\n", WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += CGI_SPRINTF(buf, length, "\t\n\t<input type=\"hidden\" name=\"%s\" value=\"%d\">\n" , CGI_VIDEO_MSG_OP_TYPE, config->msgOpType );

	content  = config->selects;
	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"center\"><select name=\"%s\" style='width:80px;'>\n", 
		config->key );
	while(content->value != -1 )
	{
		int phy = 0;//atoi(GET_VALUE(&info->globalCfgs, WIFI_KEYWORD_PHY_MODE) );
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			content->value, (phy==content->value) ?"selected":"", content->description);

		content++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"></TD></TR>" );
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MEDIA_CONFIG) );

	return strdup(buf);
}


int	videoSelectForm(VIDEO_INFO *info, VIDEO_SELECT_TYPE_T selectType)
{
	char *action;
	int msgOpType;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Media Configuration"), gettext("Basic parameters of Media are set here"), __video_select_status(info, selectType));
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


int video_streamdesc(VIDEO_INFO *info)
{
	char* 	action;
	
	action = GET_CGI_ACTION(&info->cgiVariables);
       list_replace_with_new_list(&info->global2Cfgs, &info->cgiVariables);

	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("stream description")," ",video_streamdesc_buttons(info));
		return 0;
	}	
	else if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
	{
		cgitrace;
		 list_replace_with_new_list(&info->global2Cfgs, &info->cgiVariables);
		 if(writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) )
	
		return -1;
		 cgi_info_page(gettext("stream description")," ",video_streamdesc_buttons(info));
		return 0;
	
	}
	else
	{
		cgi_invalidate_page();
	}
	return 0;
}

char *video_streamdesc_buttons(VIDEO_INFO *info)
{
	char buf[2048];
	int 	length = 0;

	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_VIDEO);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_STREAM_DESC);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ACTIVATE );


	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("author  ") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Author,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"Author",gettext("Author Name can not be NULL"), GET_VALUE(&info->global2Cfgs, "Author"));

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", gettext("comment  ") );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Comment,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"Comment",gettext("comment can not be NULL"), GET_VALUE(&info->global2Cfgs, "Comment"));

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", gettext("Copyright") );
	length += sprintf(buf+length, "<TD  align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Copyright,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"Copyright",gettext("Copyright can not be NULL") , GET_VALUE(&info->global2Cfgs, "Copyright" ));

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD>", gettext("Title") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Title,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"Title",gettext("Title Name can not be NULL")  , GET_VALUE(&info->global2Cfgs, "Title" ));
     
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext("save") ) );
	length += sprintf(buf+length, "</TD>\r\n");



	return strdup(buf);
}


int video_streamcfg(VIDEO_INFO *info)
{	char* 	action;	
	char		temp1[128]="0";
	char		temp2[128]="0";
	char		temp3[128]="0";
	int		t=1;
	char*	temp=NULL;
	
	
	action = GET_CGI_ACTION(&info->cgiVariables);
    	list_replace_with_new_list(&info->global2Cfgs, &info->cgiVariables);

		 if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
		{	
		 	while(t<4)
			{ 
				write_list_by_key(&info->global2Cfgs,KEY_FORMAT,getvalue_list_by_key(&info->cgiVariables,KEY_FORMAT,t),t);
				t++;
		 	} 

			temp=getvalue_list_by_key(&info->cgiVariables,KEY_STREAM,1);				
	  		sprintf(temp1,"%s>",temp);	
			write_list_by_key(&info->global2Cfgs,KEY_STREAM,temp1,1);
		
			temp=getvalue_list_by_key(&info->cgiVariables,KEY_STREAM,2);				
	  		sprintf(temp2,"%s>",temp);	
			write_list_by_key(&info->global2Cfgs,KEY_STREAM,temp2,2);
		
			temp=getvalue_list_by_key(&info->cgiVariables,KEY_STREAM,3);				
	  		sprintf(temp3,"%s>",temp);	
			write_list_by_key(&info->global2Cfgs,KEY_STREAM,temp3,3);	
			 writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs);	
	
	
	}
 		cgi_info_page(gettext("stream configuration")," ",video_streamcfg_buttons(info));
	return 0;
	
}
 char *video_streamcfg_buttons(VIDEO_INFO *info)
{
	char 	buf[4096];
	int 		length = 0;
	char*	p=NULL;
	char		q1[32]="0";
	char		q2[32]="0";
	char		q3[32]="0";
	
	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_VIDEO);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_STREAM_CFG);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ACTIVATE );
	
	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,1);
       memcpy(q1,p,(strlen(p)-1));
        length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream1:") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(<Stream,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_STREAM,gettext("stream name can not be NULL"), q1);		
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext(KEY_FORMAT) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Format,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_FORMAT,gettext("Format can not be NULL"), getvalue_list_by_key(&info->global2Cfgs,KEY_FORMAT,1));

	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,2);
       memcpy(q2,p,(strlen(p)-1));
   	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream2:") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(<Stream,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_STREAM,gettext("stream name can not be NULL"), q2);   	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext(KEY_FORMAT) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Format,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_FORMAT,gettext("Format can not be NULL"), getvalue_list_by_key(&info->global2Cfgs,KEY_FORMAT,2));

	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,3);	
       memcpy(q3,p,(strlen(p)-1));	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream3:") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(<Stream,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_STREAM,gettext("stream name can not be NULL"), q3);	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext(KEY_FORMAT) );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Format,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		KEY_FORMAT,gettext("Format can not be NULL"), getvalue_list_by_key(&info->global2Cfgs,KEY_FORMAT,3));

	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("port") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(port,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"port",gettext("port can not be NULL"),GET_VALUE(&info->global2Cfgs,"port"));
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("RTSPPort") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(RTSPPort,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"RTSPPort",gettext("RTSPPort can not be NULL"),GET_VALUE(&info->global2Cfgs,"RTSPPort"));
	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("MaxClients") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(MaxClients,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"MaxClients",gettext("MaxClients can not be NULL"),GET_VALUE(&info->global2Cfgs,"MaxClients"));

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("MaxBandwidth") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(MaxBandwidth,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"MaxBandwidth",gettext("MaxBandwidth can not be NULL"),GET_VALUE(&info->global2Cfgs,"MaxBandwidth"));
	
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext("save") ) );
	length += sprintf(buf+length, "</TD>\r\n");




	return strdup(buf);
}

char *getvalue_list_by_key(llist *list, char *key,int x)
{
	node* 	current;
	int		y=0;
	
	if(!key || strlen(key)==0 )
		return  NULL;
	current = list->head;
	while (current != 0) 
	{   
		if(!strcasecmp(current->entry.name, key) )	
		{
			y=y+1;
			if(x==y)
			{		
				return current->entry.value;
			}
		}
		
		current = current->next;
	}

	return NULL;
}
int write_list_by_key(llist *list, char *key,char *value,int x)
{	
	node* 	current;
	int		y=0;
	
	if(!key || strlen(key)==0 )
		return  -1;
	current = list->head;
	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name, key) )	
		{	
			y=y+1;			
			if(x==y)
				{
					current->entry.value=value;
					return 0;
				}
		}
	
		current = current->next;
	}
	
	return -1;
}

char *video_mediaserverlog_buttons(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType)
{	
	char 	buf[4096];
	int 		length = 0;
	struct video_select_t *content;
	
	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_VIDEO);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_MEDIASERVER_LOG);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ACTIVATE );

	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("CustomLog") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(CustomLog,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"CustomLog",gettext("CustomLog can not be NULL"), GET_VALUE(&info->global2Cfgs, "CustomLog"));	
	
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("MaxLogSize") );
	length += sprintf(buf+length, "<TD align=\"left\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(MaxLogSize,'%s')\"  type=\"text\" size=\"15\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		"MaxLogSize",gettext("MaxLogSize can not be NULL"), GET_VALUE(&info->global2Cfgs, "MaxLogSize"));	


	select_config_t *config = __findSelectConfigs(selectType);
	if(config == NULL)
		return strdup("");
	content  = config->selects;
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("log level") );
	length += CGI_SPRINTF(buf, length, "\t<TD align=\"left\"><select name=\"%s\" style='width:80px;'>\n", 
		config->key );
	while(content->value != -1 )
	{
		int phy = 0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			content->value, (phy==content->value) ?"selected":"", content->description);
		content++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");

	length += CGI_SPRINTF(buf, length, "\t<TR><TD align=\"right\"></TD></TR>" );	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext("save") ) );
	length += sprintf(buf+length, "</TD>\r\n");
	
	return strdup(buf);
	
}

int video_mediaserverlog(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType)
{
	char* 	action;	
	
	action = GET_CGI_ACTION(&info->cgiVariables);
      	list_replace_with_new_list(&info->global2Cfgs, &info->cgiVariables);

	if(action ||strlen(action) )
	{		
	
	 	if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
		{
			if(writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) )	
			return -1;		 	
		}
	}
	cgi_info_page(gettext("mediaserverlog")," ",video_mediaserverlog_buttons(info,selectType));
	return	0;
}


char* ACL_list_info(VIDEO_INFO *info)
{
	char 	buf[8192];
	char*	p;
	char		q[32];
	char*	temp;
	int 		length = 0;
	int 		i= 0;
	int		t=0;
	node* 	current;
	

	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,1);
	memset(q, 0, sizeof(q));
       memcpy(q,p,(strlen(p)-1));
       length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream1:") );
	length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD></TR>\r\n", gettext(q));	
	temp=GET_VALUE(&info->cgiVariables, "streamselect");
	llist *list=&info->global2Cfgs;
	current=list->head;
	while(current !=0)
		{	
			if(!strcasecmp(current->entry.name, KEY_STREAM) )
				{	i=i+1;
					if(i==1)
					{
						current=current->next;
						while(strcasecmp(current->entry.name, "</Stream>") )
						{
							if(!strcasecmp(current->entry.name, KEY_ACL) )
			      	 			{
			      	 				t++;
			      	 				length += sprintf(buf+length,"<TR><TD  align=\"right\"><strong>%s</strong></TD>",gettext("ACL:"));
			      	 				length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD>", gettext(current->entry.value));
			      	 				char target[128];
								sprintf(target, "%s?%s=%s&%s=%s&Index=%d", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_ACL, CGI_ACTION, CGI_ACTION_DEL,t);
								length += sprintf(buf+length, "<td align=\"center\">%s</TD>", cgi_button(gettext(CGI_STR_DELETE), target) );	
					 		}
							current=current->next;
						}
					}
				}
			current=current->next;
		}

	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,2);	
	memset(q, 0, sizeof(q));
       memcpy(q,p,(strlen(p)-1));
       length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream2:") );
	length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD></TR>\r\n", gettext(q));

	current=list->head;
	i=0;
	while(current !=0)
		{	
			if(!strcasecmp(current->entry.name, KEY_STREAM) )
				{	i=i+1;
					if(i==2)
					{    
						current=current->next;
						while(strcasecmp(current->entry.name, "</Stream>") )
						{
							if(!strcasecmp(current->entry.name, KEY_ACL) )
			      	 			{	
			      	 				t++;
			      	 				length += sprintf(buf+length,"<TR><TD  align=\"right\"><strong>%s</strong></TD>",gettext("ACL:"));
			      	 				length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD>", gettext(current->entry.value));
								char target[128];
								sprintf(target, "%s?%s=%s&%s=%s&Index=%d", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_ACL, CGI_ACTION, CGI_ACTION_DEL,t);
								length += sprintf(buf+length, "<td align=\"center\">%s</TD>", cgi_button(gettext(CGI_STR_DELETE), target) );			
					 		}
							current=current->next;
						}
					}
				}
			current=current->next;
		}

	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,3);	
	memset(q, 0, sizeof(q));
       memcpy(q,p,(strlen(p)-1));
       length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream3:") );
	length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD></TR>\r\n", gettext(q));

	current=list->head;
	i=0;
	while(current !=0)
		{	
			if(!strcasecmp(current->entry.name, KEY_STREAM) )
				{	i=i+1;
					if(i==3)
					{
						current=current->next;
						while(strcasecmp(current->entry.name, "</Stream>") )
						{
							if(!strcasecmp(current->entry.name, KEY_ACL) )
			      	 			{	  
			      	 				t++;	 		
			      	 				length += sprintf(buf+length,"<TR><TD  align=\"right\"><strong>%s</strong></TD>",gettext("ACL:"));
			      	 				length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD>", gettext(current->entry.value));
								char target[128];
								sprintf(target, "%s?%s=%s&%s=%s&Index=%d", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_ACL, CGI_ACTION, CGI_ACTION_DEL,t);
								length += sprintf(buf+length, "<td align=\"center\">%s</TD>", cgi_button(gettext(CGI_STR_DELETE), target) );			
					 		}
							current=current->next;
						}
					}
				}
			current=current->next;
		}	
	
	return strdup(buf);
}


char *video_ACL_buttons(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType,VIDEO_SELECT_TYPE_T selectType2)
{	
	char 	buf[4096];	
	int 		length = 0;
	struct video_select_t *content;
	
	length += sprintf(buf+length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n",FORM_NAME, WEB_URL_VIDEO);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_ACL);
	length += sprintf(buf+length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n",CGI_ACTION, CGI_ACTION_ACTIVATE );

	select_config_t *config2 = __findSelectConfigs(selectType2);
	if(config2 == NULL)
		return strdup("");
	content  = config2->selects;
	length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Stream select:") );
	length += CGI_SPRINTF(buf, length, "\t<TD align=\"right\"><select name=\"%s\" style='width:80px;'>\n", "streamselect");
	while(content->value != -1 )
	{
		int phy = 0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			content->value, (phy==content->value) ?"selected":"", content->description);
		content++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD>");
	
	select_config_t *config = __findSelectConfigs(selectType);
	if(config == NULL)
		return strdup("");
	content  = config->selects;
	length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Operation:") );
	length += CGI_SPRINTF(buf, length, "\t<TD align=\"right\"><select name=\"%s\" style='width:80px;'>\n", "operation");
	while(content->value != -1 )
	{
		int phy = 0;
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			content->value, (phy==content->value) ?"selected":"", content->description);
		content++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>");
	 length += sprintf(buf+length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("Range:") );
	length += sprintf(buf+length, "<TD align=\"right\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\"   type=\"text\" size=\"15\" value=\"%s\" maxlength=\"100\"></TD>", 
		"Range",gettext("ACL can not be NULL"), "");
	 length += sprintf(buf+length, "<TD  align=\"right\"><strong>%s</strong></TD>\n", gettext("TO") );
	 length += sprintf(buf+length, "<TD align=\"right\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" onChange=\"checkIP(%s.value,this)\" type=\"text\" size=\"15\" value=\"%s\" maxlength=\"100\"></TD></TR>", 
		"to","to", "");
	
	length += sprintf(buf+length, "<TR><TD align=\"center\">\r\n");
	length += sprintf(buf+length, cgi_submit_button(gettext("AddACL") ) );
	length += sprintf(buf+length, "</TD></TR>\r\n");
	
	return strdup(buf);
	
}

int video_ACL(VIDEO_INFO *info,VIDEO_SELECT_TYPE_T selectType,VIDEO_SELECT_TYPE_T selectType2)
{
	char* 	action;	
	char*	temp;
	char*	index;
	node	*cunode;			
	char		ACLvalue[128];
	int 		length = 0;
	int 		indextemp;

	
	cunode=info->cgiVariables.head;	
	action = GET_CGI_ACTION(&info->cgiVariables);
	
	if(action || strlen(action) )
	{
		 	if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
			{	
				 if(check_inputIP(info))
				 	{
				 		cgi_info_page(gettext("ACL Configuration increct :invalid IP Address"),ACL_list_info(info),video_ACL_buttons(info,selectType,selectType2));
						return 0;
			
				 	}
				temp=GET_VALUE(&info->cgiVariables, "operation");
   		 		if(!strcmp(temp,"0"))
 					length += sprintf(ACLvalue+length, "deny " );
				else
					length += sprintf(ACLvalue+length, "allow " );
				if(strlen(GET_VALUE(&info->cgiVariables, "Range")))
					{
						temp=GET_VALUE(&info->cgiVariables, "Range");
						length += sprintf(ACLvalue+length, "\t%s",temp );
					}
				if(strlen(GET_VALUE(&info->cgiVariables, "to")))
					{
						temp=GET_VALUE(&info->cgiVariables, "to");				
						length += sprintf(ACLvalue+length, "\t%s",temp );
					}
				temp=GET_VALUE(&info->cgiVariables, "streamselect");

				if(!strcmp(temp,"1"))
				{
					add_ACL_node(&info->global2Cfgs,KEY_STREAM,ACLvalue,1);
			
				}
				else if(!strcmp(temp,"2"))
				
				{
					add_ACL_node(&info->global2Cfgs,KEY_STREAM,ACLvalue,2);
		
				}
				
				else if(!strcmp(temp,"3"))
				{
					add_ACL_node(&info->global2Cfgs,KEY_STREAM,ACLvalue,3);		
				}

				if(writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) )
				{
					return -1;
				}

			}
			if(!strcasecmp(action,CGI_ACTION_DEL))
				{	
					index=GET_VALUE(&info->cgiVariables, "Index");
					indextemp=atoi(index);
					list_delete_by_index(&info->global2Cfgs,KEY_ACL,indextemp);
			       	writeconfig(SERVER_CONFIG_FILE, ITEM_DBL, NULL, &info->global2Cfgs) ;		
				}
				
	}
	cgi_info_page(gettext("ACL Configuration"),ACL_list_info(info),video_ACL_buttons(info,selectType,selectType2));
			return 0;
	
}
int add_ACL_node(llist *list, char *key,char* aclvalue,int x)
{
	node* 	current;
	int		y=0;
	node* newnode = __list_newnode(KEY_ACL, aclvalue, NULL, NULL);
	if(!key || strlen(key)==0 )
		return  -1;
	current = list->head;
	while (current != 0) 
	{   
		if(!strcasecmp(current->entry.name, key) )	
		{
				y=y+1;
				if(x==y)
				{
					newnode->next=current->next;
					current->next=newnode;	
					return 0;
				}
		}		
		current = current->next;
	}

	return -1;

}


node *__list_newnode(char *key, char *value,char *type, char *rule)
{
	node *newnode = (node *)malloc(sizeof(node));

	if(newnode==NULL)
	{
		printf("%s : malloc error!\n", __FUNCTION__);
		return NULL;
	}

	memset(newnode, 0, sizeof(node) );

	newnode->entry.name = cgi_new_string( key);
	newnode->entry.value = cgi_new_string( value);
	newnode->entry.type = cgi_new_string( type);
	newnode->entry.rule = cgi_new_string( rule);

	return newnode;	
}

int	get_ACLvalue(VIDEO_INFO *info)
{
	FILE 	*fp;
	char		*retptr;
	char		*q;
	char		buf[1024];
	char		ACLvalue[20][128];
	int		i=0;
	
	if((fp=fopen(SERVER_CONFIG_FILE,"r"))==NULL)
		{	
			cgitrace;	
			return	-1;
		}

	while((retptr=fgets(buf, 1024, fp)) != NULL)
	{	
		q=strstr(retptr,KEY_ACL);
		if(q!=NULL)
		{	i++;			
			q++;
			q++;
			q++;
			q++;
			sprintf(ACLvalue[i],"%s",q);	
			evaluate_ACL(info,ACLvalue);	
			continue;
		}		
	}

	fclose(fp);
	return	0;

}
int evaluate_ACL(VIDEO_INFO *info,char ACLvalue[20][128])
{node 	*current;
			int 	y=0;
			llist*	list=&info->global2Cfgs;		

			current = list->head;			
			while (current != 0) 
			{   
				if(!strcasecmp(current->entry.name, KEY_ACL) )	
				{		
					y++;
					current->entry.value=ACLvalue[y];						  
				}		
				current = current->next;
			}
			
			return	0;
}

node *list_find_node_by_index(llist *list, char *key,int x)
{
	node* current;
	int		y=0;

	if(!key || strlen(key)==0 )
		return NULL;
	current = list->head;
	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name, key) )	
		{
			y=y+1;
	
			if(x==y)
				{		
					return current;
				}
		}
		
		current = current->next;
	}
	return NULL;
}

short list_delete_by_index(llist *l, char  *name,int index)
{
	node	*thenode;
	thenode = list_find_node_by_index(l, name,index);
	return list_delete_node(l, thenode);
}

int	media_status(VIDEO_INFO *info)
{	char url[256]="0";
	char*	host;
	char*	port;
	char*	p;
	char		name[32]="0";

	sprintf(host,"%s",get_IPAddress());	
	port=GET_VALUE(&info->global2Cfgs, "Port");
	p=getvalue_list_by_key(&info->global2Cfgs,KEY_STREAM,4);
       memcpy(name,p,(strlen(p)-1));	
	sprintf(url,"http://%s:%s/%s",host,port,name);
	cgidebug("the URL is %s\n",url);
	cgitrace;
	
	printf("<HTML><HEAD>	<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\"><TITLE></TITLE>\r\n");
	printf("<META HTTP-EQUIV=\"Expires\" CONTENT=\"now\"><META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n"); 
	printf("<SCRIPT SRC=\"/include/help.js\"></SCRIPT>\r\n");
	printf("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;URL=%s\">\r\n", 0, url);
	printf("</HEAD><BODY><CENTER>\r\n");
	printf("</CENTER></BODY></HTML>\r\n");
	return 0;

}
char*	get_IPAddress()
{
	FILE*	fp;
	char*	p;
	char*	retptr;
	char		buf[128];
	char		temp[32];
	char		ipaddress[32]="0";
	int		i=0;
	
	system("ifconfig >/tmp/temp");

	if((fp=fopen("/tmp/temp","r"))==NULL)
		{	
			return	NULL;
		}
	while((retptr=fgets(buf, 1024, fp)) != NULL)
	{
		p=strstr(retptr,"addr:");
		if(p!=NULL)
		{	
			p=p+5;
			sprintf(temp,"%s",p);
			break;
		}
		continue;
	}
	fclose(fp);
	
	system("rm /tmp/temp");
	while(!isspace(temp[i]))
	{	
		ipaddress[i]=temp[i];
		i++;
	}
	cgidebug("%s\n",ipaddress);
	cgitrace;
	return strdup(ipaddress);		
}


int check_inputIP(VIDEO_INFO *info)
{     
 	char	*range;
	char	*to;
	
	range=GET_VALUE(&info->cgiVariables, "Range");
	to=GET_VALUE(&info->cgiVariables, "to");
	
	if(!strlen(range)&&!strlen(to))
		return 1;

	 if(strlen(range))
	 	{ 			
	 		if(!isValidateIpAddress(range)) 	

				return 1;
	 	
			 if(!strlen(to))
			 	return 0;
				
	 	}
	 if(!isValidateIpAddress(to))
	 			return 1;	 	
	 if(!strcasecmp(range,to))
	 	return 1;		
	return 0;
}

