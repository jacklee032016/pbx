/*
* $Id: videoView.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_video.h"

#define	JAVA_APPLET  \
	"<TR><TD ><applet codebase=\"/\" archive=\"/sources/JWebcamPlayer.jar\" code=\"JWebcamPlayer.class\" \
		name=\"JWebcamPlayer\" ID=\"JWebcamPlayer\" align=\"center\" width=\"%d\" \
		height=\"%d\" MAYSCRIPT>\n \
		<param name=\"Color\" value=\"#ffffff\">\n \
		<param name=\"Server\" value=\"127.0.0.1\">\n \
		<param name=\"Port\" value=\"%d\">\n \
		<param name=\"ImagePath\" value=\"/sources\">\n \
		<PARAM NAME=\"scriptable\" value=\"true\">\n \
		<PARAM NAME=\"mayscript\" value=\"true\">\n \
		<strong>You need to download Java.<br>\n \
		Click <a href=\"http://www.java.com/en/download/manual.jsp\">\n \
		here:http://www.java.com/en/download/manual.jsp</a></strong></applet></TD>\n</TR>\r\n"


static char *_video_applet_page(VIDEO_INFO *info)
{
	char 	buf[8192*20];
	int 		length = 0;
	int		i;
	int		count= 0;
	char		key[256];
	
	char		*value = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_NUM);
	if( value && strlen(value) )
		count = atoi(value);

	for(i=0; i< count; i++ )
	{
		int	port;
		int	width, height;
		char *separateur;
		
		sprintf(key, VIDEO_KEYWORD_CAMERA_ONBOOT"_%d", i);
		if(strcasecmp(GET_VALUE(&info->globalCfgs, key),VIDEO_VALUE_ON) )
			continue;

		sprintf(key, VIDEO_KEYWORD_CAMERA_NETWORK_PORT"_%d", i);
		value = GET_VALUE(&info->globalCfgs, key);
		port = atoi(value);
		
		sprintf(key, VIDEO_KEYWORD_CAMERA_RESOLUTION"_%d", i);
		value = GET_VALUE(&info->globalCfgs, key);

			width = strtoul (value, &separateur, 10);
			if (*separateur != 'x')
			{
				printf ("Error in size use -s widthxheight \n");
				exit (1);
			}
			else
			{
				++separateur;
				height = strtoul (separateur, &separateur, 10);
				if (*separateur != 0)
					printf ("hmm.. dont like that!! trying this height \n");
//				printf (" size width: %d height: %d \n", width, height);
			}
		
		length += CGI_SPRINTF(buf,length, JAVA_APPLET, width, height, port );

		
	}

	length += CGI_SPRINTF(buf,length, "<TR><TD ><strong>%s:</strong><p>%s<br>%s</TD></TR>\r\n",
		gettext(CGI_STR_NOTE),gettext("click on the left-upper corner to set brightness and contrast"),
		gettext("when done, click other are to remove the control box."));

	return strdup(buf);
}

int	videoView(VIDEO_INFO *info)
{
	cgi_info_page(gettext("View of Web Camera"), gettext("Web Camera"), _video_applet_page(info));
	
	return 0;
}

