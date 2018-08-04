/*
* $Id: index_cgi.c,v 1.2 2007/09/06 09:19:58 lizhijie Exp $
*/

#include <string.h>
#include "cgi_lib.h"

typedef struct
{
	llist	cgiVariables;
}INDEX_INFO;

#define	ASSIST_LOG_PAGE		"/log.html"
#define	ASSIST_TOP_PAGE		"/top.html"

static char *__index_pages(INDEX_INFO *info)
{
	char 	buf[4096*4];
	int 		length = 0;

	char		*cmd;
	cmd = GET_VALUE(&info->cgiVariables, CGI_KEYWORD_OPERATOR );
#if 0	
	if( !cmd || !strlen(cmd) )
	{
		sprintf(title, "%s : %s" ,gettext(CGI_STR_QUICK_INSTALL), gettext("Select mode of WAN") ); 
		cgi_info_page(title, "", __cgi_wan_protocol_select_buttons(&__quickInfo) );
		return 0;
	}
#endif

	/* header */
	length += CGI_SPRINTF(buf,length, "<html><head><title>%s</title>\n", gettext("NINGBO ASSIST, Inc."));
	length += CGI_SPRINTF(buf,length, "<meta content=\"text/html; charset=utf-8\" http-equiv=content-type>");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\">\n");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"Cache-Control\" CONTENT=\"no-cache, must-revalidate\">\n");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"expires\" CONTENT=\"0\">\n</head>\n");

	/* frameset  */
	/* title frameset */
	length += CGI_SPRINTF(buf,length, "<frameset border=0 cols=* frameBorder=no framespacing=0 rows=94,*>\n" );
	length += CGI_SPRINTF(buf,length, "<frameset border=0 cols=185,* frameBorder=no frameSpacing=0 rows=*>\n" );  
	length += CGI_SPRINTF(buf,length, "<frame name=topLeftframe noresize scrolling=no src=\"%s\">" , ASSIST_LOG_PAGE);
	length += CGI_SPRINTF(buf,length, "<frame name=toprightframe noresize scrolling=no src=\"%s\"></frameset>\n", ASSIST_TOP_PAGE);

	/* content frameset */
	length += CGI_SPRINTF(buf,length, "<frameset border=0 cols=155,* frameborder=no framespacing=0 rows=*>\n" );
	length += CGI_SPRINTF(buf,length, "<frame name=bottomleftframe noresize  src=\"%s?%s=%s\">", WEB_URL_NAVIGATE, CGI_KEYWORD_OPERATOR, cmd);
	length += CGI_SPRINTF(buf,length, "<frame name=mainframe src=\"%s?%s=%s\">\n", WEB_URL_CONTENT, CGI_KEYWORD_OPERATOR, cmd);

	length += CGI_SPRINTF(buf,length, "</frameset></frameset><noframes></noframes>\n");

	length += CGI_SPRINTF(buf,length, "</html>\n");


	return strdup(buf);
}

static int __index_page_init(INDEX_INFO *info)
{
	list_create( &info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);
	
	cgi_html_header();

	return 0;
}

int main()
{
	INDEX_INFO	info;

	__index_page_init(&info);
	
#if 1	
	printf("%s", __index_pages( &info) );
#else
	cgi_info_page("", __content_status(), "" );
#endif
	return 0;
}

