/*
* $Id: scanSetup.c,v 1.5 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_scan.h"

#include "cgi_services.h"
#include "aslib.h"
#include "_scan.h"

static char		*__scan_modes[3] =
{
	SCAN_KEYWORD_MODE_COLOR,
	SCAN_KEYWORD_MODE_GRAY,
	SCAN_KEYWORD_MODE_BINARY
};

static char		*__scan_formats[4] =
{
	SCAN_KEYWORD_FORMAT_TIFF,
	SCAN_KEYWORD_FORMAT_JPEG,
	SCAN_KEYWORD_FORMAT_GIF,
	SCAN_KEYWORD_FORMAT_PNG
};

scan_page_t	__page_styles[] = 
{
	{
		"A5 portrait",
		148,
		210,
	},
	{
		"A5 landscape",
		210,
		148,
	},
	{
		"Letter",
		215,
		279
	},
	{
		"A4",
		210,
		297,
	},
	{
		NULL,
		-1,
		-1
	}
};

int	__depths[] = { 1, 8 , -1};

int	__resolutions[] = {100, 300, 600, 1200, -1 };
	
#define	SCAN_FORM_NAME	"scanForm"
	
char *scan_dev_info(SCAN_INFO *info)
{
	char 	buf[8192*5];
	int 		length = 0;

	length += CGI_SPRINTF(buf,length, "<table width=\"65%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" bgcolor=\"#cccccc\" colspan=6><strong>%s</strong></TD></TR>\n", gettext("Scanner Information") );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD>\n", 
		gettext("Vendor"), info->scan.vendor );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD>\n", 
		gettext("Model"), info->scan.model );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD></TR>\n", 
		gettext(CGI_STR_TYPE), info->scan.type );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD>\n", 
		gettext(CGI_STR_DEVICE), "USB"/*info->scanner.name*/ );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD>\n", 
		gettext("Bus"), info->scanner.bus );
	length += CGI_SPRINTF(buf,length, "<TD align=\"right\"><strong>%s : </strong></TD><TD>%s</TD></TR>\n", 
		gettext("Device No."), info->scanner.device );

	return strdup(buf);
}

static char *_scan_view_setup_page(SCAN_INFO *info)
{
	char 	buf[8192*20];
	int 		length = 0;
	char		target[256];
	int		i;
	char		*value, *p;
	int		*intvalue, index;
	scan_page_t *page = __page_styles;

	length += CGI_SPRINTF(buf, length, "<form name=\"%s\" method=\"post\" action=\"%s\">\n", SCAN_FORM_NAME, WEB_URL_SCANNER);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_SCAN_OP_SCAN);
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_DEFAULT);

	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", 
		SCAN_KEYWORD_DATA_DIR, GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_DATA_DIR) );

	/* javascripts */
	length += CGI_SPRINTF(buf, length,  "<script language=\"JavaScript\">\n<!--\n");
	length += CGI_SPRINTF(buf, length,  "\tfunction setGeometry(l, t,x,y)\n\t{\n");
	length += CGI_SPRINTF(buf, length,  "\t\tdocument.%s.%s.value = l;\n", SCAN_FORM_NAME, SCAN_KEYWORD_LEFT );
	length += CGI_SPRINTF(buf, length,  "\t\tdocument.%s.%s.value = t;\n", SCAN_FORM_NAME, SCAN_KEYWORD_TOP );
	length += CGI_SPRINTF(buf, length,  "\t\tdocument.%s.%s.value = x;\n", SCAN_FORM_NAME, SCAN_KEYWORD_RIGHT );
	length += CGI_SPRINTF(buf, length,  "\t\tdocument.%s.%s.value = y;\n", SCAN_FORM_NAME, SCAN_KEYWORD_BOTTOM );
	length += CGI_SPRINTF(buf, length,  "\t}\n" );
	length += CGI_SPRINTF(buf, length,  "//-->\n</script>\n");


/* TABLE 1 */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("Basic Parameters") );

	value = GET_VALUE( &info->globalCfgs, SCAN_KEYWORD_MODE);
	length += CGI_SPRINTF(buf, length, "\t<TR><TD  width=\"45%%\" align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext(CGI_STR_MODE), SCAN_KEYWORD_MODE);
#if 1	
	for(i=0;i<3;i++)
	{
		p = __scan_modes[i];
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
			p, (!strcasecmp(p,value) ) ?"selected":"",  gettext(p) );
	}
#else
	p = __scan_modes;
	while(!p)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
			p, (!strcasecmp(p,value) ) ?"selected":"",  gettext(p) );
		p++;
	}
#endif
	length += CGI_SPRINTF(buf, length, "</TD>\n" );

	value = GET_VALUE( &info->globalCfgs, SCAN_KEYWORD_FORMAT);
	length += CGI_SPRINTF(buf, length, "\t<TD width=\"45%%\" align=\"right\"><strong>%s:</strong></TD><TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("Image Format"), SCAN_KEYWORD_FORMAT);
	for(i=0;i<2;i++)
	{
		p = __scan_formats[i];
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%s\" %s>%s</option>\n", 
			p, (!strcasecmp(p,value) ) ?"selected":"",  gettext(p) );
	}
	length += CGI_SPRINTF(buf, length, "</TD></TR>\n" );

	/* resolution and depth */
	length += sprintf(buf+length, "<TR><TD width=\"45%%\" align=\"right\"><strong>%s :</strong></TD><TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("Resolution(DPI)"), SCAN_KEYWORD_RESOLUTION );

	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_RESOLUTION );
	index =atoi(value);
	intvalue = __resolutions;
	while( *intvalue != -1 )
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", 
			*intvalue, (index==*intvalue)?"selected":"",  *intvalue );
		intvalue ++;
	}
	length += CGI_SPRINTF(buf, length, "</TD>\n" );

	/* depth */
	length += sprintf(buf+length, "<TD width=\"45%%\" align=\"right\"><strong>%s :</strong></TD><TD>\n\t<select name=\"%s\" style='width:90px;'>\n", 
		gettext("Bits Per Pixel"), SCAN_KEYWORD_DEPTH );

	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_DEPTH );
	index =atoi(value);
	intvalue = __depths;
	while( *intvalue != -1 )
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%d</option>\n", 
			*intvalue, (index==*intvalue)?"selected":"",  *intvalue );
		intvalue ++;
	}
	length += CGI_SPRINTF(buf, length, "</TD></TR>\n" );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* TABLE 2 */
	/* scan area */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=4 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("Scanning Area") );
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD>", gettext("Left Point") );
	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_LEFT );
	length += sprintf(buf+length, "<TD ><input name=\"%s\" type=\"text\" size=\"5\" value=\"%s\" maxlength=\"32\"></TD>\r\n" ,
		SCAN_KEYWORD_LEFT, value);
	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_TOP );
	length += sprintf(buf+length, "<TD align=\"right\"><strong>%s</strong></TD><TD><input name=\"%s\" type=\"text\" size=\"5\" value=\"%s\" maxlength=\"32\">%s</TD></TR>\r\n" ,
		gettext("Top Point"), SCAN_KEYWORD_TOP, value, gettext("mm"));

	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s</strong></TD>", gettext("Right Point") );
	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_RIGHT );
	length += sprintf(buf+length, "<TD ><input name=\"%s\" type=\"text\" size=\"5\" value=\"%s\" maxlength=\"32\"></TD>\r\n" ,
		SCAN_KEYWORD_RIGHT, value);
	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_BOTTOM);
	length += sprintf(buf+length, "<TD align=\"right\"><strong>%s</strong></TD><TD><input name=\"%s\" type=\"text\" size=\"5\" value=\"%s\" maxlength=\"32\">%s</TD></TR>\r\n" ,
		gettext("Bottom Point"), SCAN_KEYWORD_BOTTOM, value, gettext("mm"));

	/* page buttons */
	length += sprintf(buf+length, "<TR><TD align=\"right\"><strong>%s </strong></TD><TD align=\"center\" colspan=2>", gettext("Set As") );
	while(page->title )
	{
		length += sprintf(buf+length, "<input type=\"button\" name=\"WriteToConf\" value=\"%s\" class=\"button\" onclick=\"setGeometry(0,0, %d, %d)\">\r\n",
			page->title, page->width, page->height );
		
		page++;
	}
	length += sprintf(buf+length, "</TD></TR>\r\n");
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );

/* TABLE 3 */
	/* file name */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("Other Parameters") );
	length += sprintf(buf+length, "<TR><TD ><strong>%s </strong></TD>", gettext("File Name") );
	value = GET_VALUE(&info->globalCfgs, SCAN_KEYWORD_FILENAME );
	length += sprintf(buf+length, "<TD ><input name=\"%s\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"32\"></TD></TR>\r\n" ,
		SCAN_KEYWORD_FILENAME, value);

	/* checkbox SAVE */
	length += sprintf(buf+length, "<TR><TD ><strong>%s :</strong></TD>", gettext("Save Scanning Parameters") );
	length += sprintf(buf+length, "<TD><INPUT type=\"checkbox\" name=\"%s\" value=\"%s\" checked></TD></TR>\r\n",
			SCAN_KEYWORD_SVAE_CONFIG, CGI_STR_YES );
	length += CGI_SPRINTF(buf, length, "</TABLE>\n" );


	sprintf(target, WEB_URL_SCANNER"?"CGI_KEYWORD_OPERATOR"="CGI_SCAN_OP_SAVE);
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\">%s</TD></TR>\r\n", cgi_submit_button( gettext("Scan") ) );
	length += CGI_SPRINTF(buf, length, "</table></FORM>\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br>%s!%s, %s!</TD></TR>\r\n",
		gettext(CGI_STR_NOTE), gettext("Resolution 100 DPI(Dot Per Inch) can satisfied all applications"), 
		gettext("Otherwise the raw image from Scanner will be very big"), 
		gettext("Data Access maybe timeout") );

	return strdup(buf);
}

int	scanSetup(SCAN_INFO *info)
{
	cgi_html_header();
	cgi_info_page(gettext("Web Scanner Setup"), scan_dev_info(info), _scan_view_setup_page(info));

	return 0;
}

