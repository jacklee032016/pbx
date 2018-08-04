/*
* $Id: videoView.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgiLib.h"
#include "cgi_video.h"
#include "_video.h"

#if 0
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
#define	PLUGIN_APPLET  \
	"<TR><TD ><td align=\"center\" width=\"%d\" height=\"%d\"> \
	<OBJECT width=%d height=%d ID=\"PlayCtl\" CLASSID=\"CLSID:8647B51D-776B-461E-89E0-AEB23DD08DB5\" \
	codebase=\"/HttpCtl.cab#version=VIDEO_ACTIVEX_VERSION\"></OBJECT></TD></TR>\n"



static char *_video_applet_page(VIDEO_INFO *info)
{
	char 	buf[8192*20];
	int 		length = 0;
	int		count= 0;
	
	int	width, height;
	char *separateur;
	char		*value = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_NUM);
	if( value && strlen(value) )
		count = atoi(value);

	value = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_RESOLUTION);

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
		
	length += CGI_SPRINTF(buf,length, PLUGIN_APPLET, width+10, height+10, width, height );

	return strdup(buf);
}

#endif	
int	videoView(VIDEO_INFO *info)
{/*
	printf("<html><body onLoad=\"PlayCtl.MyMethod(window.location.hostname)\">");
	printf("<table border=\"0\" cellspacing=\"0\">");
	printf("%s\n", _video_applet_page(info) );
	printf("</table></body></html>");
*/
	printf("<HTML><HEAD>	<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\"><TITLE></TITLE>\r\n");
	printf("<META HTTP-EQUIV=\"Expires\" CONTENT=\"now\"><META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n"); 
	printf("<SCRIPT SRC=\"/include/help.js\"></SCRIPT>\r\n");
	printf("</HEAD>\r\n");
	printf("<body >\r\n");
	printf("<script> open_page_view('/cgi-bin/videoview.cgi')</script>\r\n");	
	printf("</body></html>");
#if 0	
	cgi_info_page(gettext("View of Media Stream"), gettext("IP Camera"), _video_applet_page(info));
#endif	
	return 0;
}

