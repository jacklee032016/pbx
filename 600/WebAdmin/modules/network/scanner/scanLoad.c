/*
* $Id: scanLoad.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_scan.h"

#include "cgi_services.h"
#include "aslib.h"
#include "_scan.h"

/* implement SAVE AS page : load converted image into client */
int	scanLoad(SCAN_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;
	int		fd;
	int		total = 0;
#if 0
	struct stat 	fstats;
#endif
	char		filename[256];
	char		*file;
	char		*format = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT);

	file = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FILENAME);
	printf("Content-type: image/%s\r\n\r\n", format );
//	sprintf(filename, "%s/test.tiff", SCAN_DEFAULT_DATA_DIR);
	
	sprintf(filename, "%s/%s.%s", GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_DATA_DIR),
		file, format );
#if 0
//	if(strcasecmp(format, SCAN_KEYWORD_FORMAT_TIFF ) )
	if(stat(filename, &fstats))
	{
		__format_convert(info);
	}
#endif

	cgidebug("ScanImage : FORMAT : '%s'\tFileName : '%s'\n", format, filename );
	fd = open(filename,O_RDONLY);
	if(fd < 0 )
	{
		fprintf(stderr, "file error %s : %s\n", filename, strerror(errno) );
		return -1;
	}
	
	do
	{
		length  = read(fd, buf, sizeof(buf) );
		if(length > 0 )
			total += fwrite(buf, length, 1, stdout);
	}while(length> 0 );

//	cgidebug("total %d bytes write\n", total);
	close(fd);
	
	return 1;
}


int	scanPreview(SCAN_INFO *info)
{
	char 	buf[8192*5];
	char		button[4096*2];
	int 		length = 0;
	char		target[1024];
	char		*filename;
	
	char		*format = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FORMAT);
	filename = GET_VALUE(&info->cgiVariables, SCAN_KEYWORD_FILENAME );

	cgidebug("ScanPreview : FORMAT : %s\tFileName : %s\n", format, filename );

	length += CGI_SPRINTF(buf,length, "<table width=\"65%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	sprintf(target, WEB_URL_SCANNER"?"CGI_KEYWORD_OPERATOR"="CGI_SCAN_OP_IMAGE"&"SCAN_KEYWORD_FORMAT"=%s&"SCAN_KEYWORD_FILENAME"=%s",
		format, filename );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><img src=\"%s\"><p></TR>\n", 
		target );

	length = 0;
	length += CGI_SPRINTF(button, length, "<TR><TD><table width=\"100%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(button,length, "<TR><TD align=\"center\"><input class=\"button\" type=\"button\" name=\"close\" value=\"%s\" onClick=\"javascript:window.close();\"></TD></TR>\r\n",
		gettext("Close")  );
	length += CGI_SPRINTF(button, length, "</table></TD></TR>\n" );

	return cgi_info_page(gettext("Scanned Image"), buf, button);
}

