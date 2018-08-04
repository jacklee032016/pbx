/*
* $Id: scanScan.c,v 1.8 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_scan.h"

#include "cgi_services.h"
#include "aslib.h"
#include "_scan.h"
#include "aslib_cjpeg.h"

typedef	struct
{
	char		*key;
	char		*value;
}scan_item_t;


scan_item_t		__epson_cfg_keys[] = 
{
	{
		SCAN_KEYWORD_MODE,
		NULL	
	},
	{
		SCAN_KEYWORD_LEFT,
		NULL
	},
	{
		SCAN_KEYWORD_TOP,
		NULL
	},
	{
		SCAN_KEYWORD_RIGHT,
		NULL
	},
	{
		SCAN_KEYWORD_BOTTOM,
		NULL
	},
	{
		SCAN_KEYWORD_RESOLUTION,
		NULL,
	},
	{
		NULL,
		NULL
	}
};


static int	__format_convert(SCAN_INFO *info)
{
	char		*filename, *datadir;
	char		infilename[256], outfilename[256];
	char		*format = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT);

	filename = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FILENAME);
	datadir = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_DATA_DIR);
	sprintf( infilename, "%s/%s.%s", datadir, filename, SCAN_KEYWORD_FORMAT_PNM );

	sprintf( outfilename, "%s/%s.%s", datadir, filename, format );

	cgidebug("ScanImage : from  '%s' to '%s'\n", infilename, outfilename);
	aslib_jpeg_to_jpeg(infilename, outfilename);

//	if(unlink(infilename) )
	if(remove(infilename) )
	{
		fprintf(stderr, "unlink file %s failed : %s\n", infilename, strerror(errno) );
	}
	cgidebug("ScanImage : remove  '%s' \n", infilename);
	
	return 0;
}

static char	*__base_url(SCAN_INFO *info, char *scanOp)
{
	char		url[4096];
	int		len = 0;
	scan_item_t	*item = __epson_cfg_keys;

	len = 0;
	len += CGI_SPRINTF(url, len, WEB_URL_SCANNER"?"SCAN_KEYWORD_FILENAME"=%s&"SCAN_KEYWORD_FORMAT"=%s", 
		GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FILENAME), 
		GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT) );
	item = __epson_cfg_keys;
	while(item->key)
	{
		len += CGI_SPRINTF(url, len, "&%s=%s", item->key, GET_VALUE(&info->cgiVariables, item->key) );
		item++;
	}
	len += CGI_SPRINTF(url, len, "&%s=%s", CGI_KEYWORD_OPERATOR, scanOp);

	return strdup(url);
}

static char *__preview_save_buttons(SCAN_INFO *info)
{
	char		button[4096];
	int		len = 0;
	char		*target;

	target = __base_url(info, CGI_SCAN_OP_PREVIEW);
	len += CGI_SPRINTF(button, len, "<input type=\"button\" name=\"cdef\" value=\"%s\" class=\"button\" onclick=\"open_scan_preview_window('%s')\">\r\n", 
		gettext("Preview"), target);
	free(target);

	target = __base_url(info, CGI_SCAN_OP_IMAGE);
	len += CGI_SPRINTF(button, len, "<input type=\"button\" name=\"cdef\" value=\"%s\" class=\"button\" onclick=\"open_scan_preview_window('%s')\">\r\n", 
		gettext(CGI_STR_SAVE), target);
	free(target);
	
	target = __base_url(info, CGI_SCAN_OP_SCAN);
	len += CGI_SPRINTF(button, len, "%s\r\n", cgi_button(gettext("ReScan"), target) );
	free(target);

	return strdup(button);
}

int	scanScan(SCAN_INFO *info)
{
	char		status[4096];
	char		button[4096];
	int		length = 0, len = 0;
	char		msg[2048];
	scan_item_t	*item = __epson_cfg_keys;
	char		*target;
	char		*datadir;
	struct stat st;
	
	char		*format = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT);
	if(!strcasecmp(format, SCAN_KEYWORD_FORMAT_JPEG) )
	{
		info->scan.format = SCAN_FORMAT_JPEG;
	}
#if 0	
	else if(!strcasecmp(format, SCAN_KEYWORD_FORMAT_GIF) )
	{
		info->scan.format = SCAN_FORMAT_GIF;
	}
	else if(!strcasecmp(format, SCAN_KEYWORD_FORMAT_PNG) )
	{
		info->scan.format = SCAN_FORMAT_PNG;
	}
#endif
	else //if(!strcasecmp(format, SCAN_KEYWORD_FORMAT_TIFF) )
	{
		info->scan.format = SCAN_FORMAT_TIFF;
	}

	target = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_SVAE_CONFIG);
	cgidebug("%s = %s\n",SCAN_KEYWORD_SVAE_CONFIG , target);
	if(!strcasecmp(target, CGI_STR_YES) )
		cgi_scan_save_config(info);

	datadir = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_DATA_DIR);
	if(stat(datadir, &st) )
	{
		char		cmd[256];
		sprintf(cmd, "mkdir -p %s", datadir);
		system(cmd);
	}
	
	sprintf(info->filename, "%s/%s.%s", datadir,
		GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FILENAME), 
		(info->scan.format == SCAN_FORMAT_TIFF)?format:SCAN_KEYWORD_FORMAT_PNM );
	
	info->scan.filename = info->filename;
	
	CGI_INSTANCE_INFO(gettext("Set Scanning Parameters....."), CGI_COLOR_BLUE);

	while( item->key )
	{
		item->value = GET_VALUE(&info->cgiVariables, item->key);
		if(aslib_scan_config(&info->scan, item->key, item->value) )
		{
			sprintf(msg, "set EPSON option : key(%s)|value(%s)", item->key, item->value);
			cgidebug("%s\n", msg);
			return cgi_info_page(CGI_STR_ERROR, gettext("Scanner Setup Failed"), msg);
		}

		item++;
	}


	CGI_INSTANCE_INFO(gettext("Scanning....."), CGI_COLOR_BLUE);
	if( (aslib_scan_scan(&info->scan) != SCAN_STATUS_GOOD) )
	{
		sprintf(msg, "%s!", gettext("Scanning Failed") );
		return cgi_info_page(CGI_STR_ERROR, gettext("Scanning Failed"), msg);
	}

	if(info->scan.format == SCAN_FORMAT_TIFF)
	{
	
		length = 0;
		length += CGI_SPRINTF(button, length, "<TR><TD><table width=\"100%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
		length += CGI_SPRINTF(button, length, "<TR><TD align=\"center\">\r\n");

		target = __preview_save_buttons(info);
		length += CGI_SPRINTF(button, length, "%s</TD></TR></table></TD></TR>\n", target );
		free(target);
		cgi_info_page(gettext("Scan OK!"), status, button);
	}
	else
	{

		target = __base_url(info, CGI_SCAN_OP_CONVERT);
		len += CGI_SPRINTF(msg, len, "%s '%s'!", gettext("Image is converting into format"), format );
		cgi_refresh_page(10, target, msg, gettext(CGI_STR_PLEASE_WAIT) );
		free(target);
		__format_convert(info);
	}
	
	return 0;

}


/* change raw image from pnm to jpeg  */
int	scanConvert(SCAN_INFO *info)
{
	char 	button[1024];
	int 		length = 0;
	char		*target;
	int		res = 0;

	length = 0;
	length += CGI_SPRINTF(button, length, "<TR><TD><table width=\"100%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(button, length, "<TR><TD align=\"center\">\r\n");

	target = __preview_save_buttons(info);
	length += CGI_SPRINTF(button, length, "%s</TD></TR></table></TD></TR>\n", target );
	free(target);
	cgi_info_page(gettext("Scan OK!"), "", button);
		
	return res;
}

