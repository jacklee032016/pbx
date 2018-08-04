/*
* $Id: wizardUtils.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "wizard.h"
#include "as_version.h"

char	*wizard_page_begin(char *title, WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;
	char		*op = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	char		*nextOp;

	/* html header */
	length += CGI_SPRINTF(buf,length, "<html><head><title>%s</title>\n", gettext("NINGBO ASSIST, Inc."));
	length += CGI_SPRINTF(buf,length, "<meta content=\"text/html; charset=utf-8\" http-equiv=content-type>");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"pragma\" CONTENT=\"no-cache\">\n");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"Cache-Control\" CONTENT=\"no-cache, must-revalidate\">\n");
	length += CGI_SPRINTF(buf,length, "<META HTTP-EQUIV=\"expires\" CONTENT=\"0\">\n</head>\n");

	length += CGI_SPRINTF(buf,length,"\n<script language=\"JavaScript\">\n");
	length += CGI_SPRINTF(buf,length,"<!--\nfunction open_page( page_url){\n" );
	length += CGI_SPRINTF(buf, length,"window.location.replace(page_url );}//-->\n</script>\n");

//	length += CGI_SPRINTF(buf,length, "<link media=screen href=\"/weblog.css\" type=text/css rel=stylesheet>\n");
//	length += CGI_SPRINTF(buf,length, "<link media=screen href=\"/soph.css\" type=text/css rel=stylesheet>\n");
//	length += CGI_SPRINTF(buf,length, "<link media=print href=\"/printer.css\" type=text/css rel=stylesheet>\n");
	length += CGI_SPRINTF(buf,length, "<link href=\"/nb.css\" type=text/css rel=stylesheet>\n");

	/* title */
	length += CGI_SPRINTF(buf,length, "</head>\n<body>\n");
//	length += CGI_SPRINTF(buf,length, "<div id=topnavbar>\n<ul id=topnav>\n");
	length += CGI_SPRINTF(buf,length, "<ul id=topnav>\n");
	length += CGI_SPRINTF(buf,length, "<li>%s\n", gettext("NingBo Assist Technology Co.,Ltd"));
	length += CGI_SPRINTF(buf,length, "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp %s : %s\n",
		gettext("Version"),AS_VERSION_INFO("Wizard") );
	length += CGI_SPRINTF(buf,length, "</li></ul>\n");

	/* title msg */
	length += CGI_SPRINTF(buf,length, "<h1 align=\"center\">%s: %s</h1></div>\n", gettext(WIZARD_STR_TITLE), title);

	/* form */
	if( !strcasecmp(op, WIZARD_OP_FORMAT) )
	{
		length += sprintf(buf+length, "<form name=\"upfiles\" method=\"post\" enctype=\"multipart/form-data\" action=\"%s\">\r\n", WEB_URL_WIZARD);
//		length += CGI_SPRINTF(buf,length, "<form name=\"wanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WIZARD );
	}
	else
		length += CGI_SPRINTF(buf,length, "<form name=\"wanForm\" method=\"post\" action=\"%s\">\n", WEB_URL_WIZARD );

	if( !op || !strlen(op) )
	{
		nextOp = WIZARD_OP_HARDWARE;
	}
	else if( !strcasecmp(op, WIZARD_OP_WANCONFIG) )
	{
		nextOp = WIZARD_OP_WANCONFIG;
	}
	else if( !strcasecmp(op, WIZARD_OP_HARDWARE) )
	{
		nextOp = WIZARD_OP_PARTITION;
	}
	else if( !strcasecmp(op, WIZARD_OP_PARTITION) )
	{
		nextOp = WIZARD_OP_FORMAT;
	}
	else if( !strcasecmp(op, WIZARD_OP_FORMAT) )
	{
		nextOp = WIZARD_OP_UPLOAD;
	}
	else if( !strcasecmp(op, WIZARD_OP_UPLOAD) )
	{
		nextOp = WIZARD_OP_UPDATE;
	}
	else if( !strcasecmp(op, WIZARD_OP_UPDATE) )
	{
		nextOp = WIZARD_OP_REBOOT;
	}
	else if( !strcasecmp(op, WIZARD_OP_REBOOT) )
	{
		nextOp = WIZARD_OP_REBOOT;
	}
	
	length += CGI_SPRINTF(buf,length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, nextOp );

	return strdup(buf);
}

char *wizard_page_alert(char *status, WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;

	/* table */
	length += CGI_SPRINTF(buf,length, "<table width=\"520\" border=\"1\" align=\"center\" cellpadding=\"6\" cellspacing=\"6\" bgcolor=\"#FFFFFF\" >\n");
	/* status info */
	length += CGI_SPRINTF(buf,length, "<tr><td colspan=6 height=\"50\">%s</td></tr>\n", status );
	length += CGI_SPRINTF(buf,length, "</table>\n");

	return strdup(buf);
}

char *wizard_page_status_begin( WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;

	length += CGI_SPRINTF(buf,length, "<table width=\"520\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	/* status info */
	return strdup(buf);
}

char *wizard_page_status(char *status, WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;

	/* status info */
	length += CGI_SPRINTF(buf,length, "<tr><td width=\"100\"></td><td colspan=5>%s</td></tr>\n" , status );
	return strdup(buf);
}

char *wizard_page_status_end( WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;

	length += CGI_SPRINTF(buf,length, "</table>\n");
	/* status info */
	return strdup(buf);
}


char *wizard_page_end(char *leftButton, char *rightButton, WIZARD_INFO *info)
{
	char		buf[16*1024];
	int		length = 0;
	
	/* table */
	length += CGI_SPRINTF(buf,length, "<table width=\"520\" border=\"1\" align=\"center\" cellpadding=\"6\" cellspacing=\"6\" bgcolor=\"#FFFFFF\" height=\"50\">\n");
	length += CGI_SPRINTF(buf,length, "<tr><td colspan=3 width=\"50%%\" align=\"center\">%s</td>\n", leftButton);
	length += CGI_SPRINTF(buf,length, "<td colspan=3 width=\"50%%\" align=\"center\">%s</td></tr>\n", rightButton );

	length += CGI_SPRINTF(buf,length, "</table>\n");
	length += CGI_SPRINTF(buf,length, "</body></html>\n");

	return strdup(buf);
}

int	wizard_page(char *title, char *status, char *alert, char *leftButton, char *rightButton, WIZARD_INFO *info)
{
	printf("%s", wizard_page_begin(title, info) );

	printf("%s", wizard_page_alert(status, info) );
	printf("%s", wizard_page_alert(alert,  info) );

	printf("%s", wizard_page_end(leftButton, rightButton, info) );
	
	return 0;
}

