/*
* $Id: scanMain.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_syscfg.h"
#include "cgi_scan.h"
#include "_scan.h"

int cgi_scan_save_config(SCAN_INFO *info)
{
	list_replace_with_new_list(&info->globalCfgs, &info->cgiVariables);

	if(writeconfig(SCAN_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		return -1;
	}
	return 0;
}

static int __scan_bus_parse(SCAN_INFO *info)
{
	int		res = 0;
	char *p, *p1, *p2, *p3;
	p = (char *)info->scan.devname ;

	if(!p || strlen(p)==0 )
		return -1;
	cgidebug("scan init %s\n", p);

	p1 =	 strchr(p, ':' );		/* libusb*/
	p2 =	 strchr(p1+1, ':' );	/* 001*/

	if(p1 && p2 && p3 )
	{
		strncpy(info->scanner.name, p, ((p1-p)>sizeof(info->scanner.name))? sizeof(info->scanner.name): (p1-p));
		strncpy(info->scanner.bus, p1+1, ((p2-p1 -1)>sizeof(info->scanner.bus))? sizeof(info->scanner.bus): (p2-p1 -1));
		strncpy(info->scanner.device, p2+1, ((strlen(p)-(p2-p))>sizeof(info->scanner.device))? sizeof(info->scanner.device): (strlen(p)-(p2-p)) );
	}

	return res;	
}

SCAN_INFO *cgi_scan_init()
{
	SCAN_INFO	*info;
	char			*p;
	
	info = (SCAN_INFO *)malloc( sizeof(SCAN_INFO) );
	if(!info)
		return NULL;
	memset(info, 0, sizeof(SCAN_INFO) );
	
	list_create(&info->cgiVariables);
	read_cgi_input(&info->cgiVariables, NULL, NULL);

	cgidebug("read config\n");
	list_create(&info->globalCfgs);
	if (readconfig(SCAN_CONFIG_FILE, ITEM_EQU, NULL, &info->globalCfgs) )
	{
		cgi_html_header();
		cgi_error_page(NULL, gettext("Configuration File is not found"), gettext( CGI_STR_SOME_ERROR ) );
		free(info);
		return NULL;
	}

	/* set here, just set for backend */
	p = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT);
	if(!strcasecmp( p, SCAN_KEYWORD_FORMAT_TIFF) )
	{
		info->scan.format = SCAN_FORMAT_TIFF;
	}
	else
	{
		info->scan.format = SCAN_FORMAT_PNM;
	}
	
	if( aslib_scan_init(&info->scan) )
	{
		cgi_html_header();
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No USB Scanner Hardware is found") );
		free(info);
		return NULL;
	}
	info->isPlugged = 1;
	cgidebug("scan init\n");
	__scan_bus_parse(info);
	
	cgidebug("init OK\n");

	return info;
}


int main()
{
	char 	*cmd=NULL;
	SCAN_INFO		*info = cgi_scan_init();
	int	res = 0;

	if( !info )
		return 1;

	cmd = GET_VALUE( &info->cgiVariables, CGI_KEYWORD_OPERATOR);
	if( !cmd || !strlen(cmd) ||!strcasecmp(cmd, CGI_SCAN_OP_SETUP) )
	{
		res = scanSetup(info);
	}
	else if(!strcasecmp(cmd, CGI_SCAN_OP_IMAGE) )
	{/* load image in client preview window */
		res = scanLoad(info);
	}
	else
	{
		cgi_html_header();
		if(!strcasecmp(cmd, CGI_SCAN_OP_SCAN) )
		{
			res = scanScan(info);
		}
		else if(!strcasecmp(cmd, CGI_SCAN_OP_PREVIEW) )
		{/* save image to client system */
			res = scanPreview(info);
		}
		else if(!strcasecmp(cmd, CGI_SCAN_OP_CONVERT) )
		{/* change from pnm to jpeg */
			res = scanConvert(info);
		}
		else if(!strcasecmp(cmd, CGI_SCAN_OP_SAVE) )
		{/* save image to IXP shared/scan directory */
			
			res = cgi_refresh_page(3, WEB_URL_SCANNER, gettext("Write Configuration File"), gettext(CGI_STR_PLEASE_WAIT) );
		}
		else
		{
			cgi_invalidate_page();
			res = 0;
		}
	}	

	aslib_scan_exit(&info->scan);
	
	return res;
}

