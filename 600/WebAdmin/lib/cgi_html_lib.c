/*
* $Author: lizhijie $
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cgi_lib.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/* HTTP headers */
void cgi_html_header()
{
	printf("Content-type: text/html\r\n\r\n");
}

#if WITH_STATIC_HTML
void cgi_html_output(char *filename,llist *l,void (*specout)())
{
	llist *pentry=l;
	FILE *fp;
	char linein[2000],lineout[2048];
	char fname[128];

	cgi_locale_filename(filename, fname);
	fp=fopen(fname,"r");		
	if(!fp)
	{
		cgidebug( "open html error\n");
		return;
	}

	//output file with changing
	while(fgets(linein,1999,fp))		
	{			
		if(cgi_html_parse_select(linein, lineout,pentry))
		{				
			fputs(lineout,stdout);			
		}			
		else if(cgi_html_parse_input(linein, lineout,pentry))
		{				
			fputs(lineout,stdout);
		}
		else if(strstr(linein,SPECOUT))
		{
			if(specout)
				(*specout)();
		}
		else if(strstr(linein,"%*$%"))
			continue;
		else			
		{				
			fputs(linein,stdout);			
		}
		
		fflush(stdout);
		fflush(stdin);
	}	
	
	fclose(fp);
	return;
}

void cgi_show_html_page(char *loc)
{
	printf("Location: %s\n\n",loc);
}

void cgi_mime_header(char *mime)
{
	/* char *mime = valid mime type */
	printf("Content-type: %s\n\n",mime);
}

void cgi_nph_header(char *status)
{
	printf("HTTP/1.0 %s\n",status);
	printf("Server: CGI using cgihtml\n");
}

void cgi_status(char *status)
{
	printf("Status: %s\n",status);
}

void cgi_pragma(char *msg)
{
	printf("Pragma: %s\n",msg);
}

void cgi_set_cookie(char *name, char *value, char *expires, char *path,
		char *domain, short secure)
{
  /* in later version, do checks for valid variables */
	printf("Set-Cookie: %s=%s;",name,value);

	if (expires != NULL)
		printf(" EXPIRES=%s;",expires);
	if (path != NULL)
		printf(" PATH=%s;",path);
	if (domain != NULL)
		printf(" DOMAIN=%s;",domain);
	if (secure)
		printf(" SECURE");

	printf("\n");
}

/* HTML shortcuts */
void cgi_html_begin(char *title)
{
	printf("<html> <head>\n");
	printf("<title>%s</title>\n",title);
	printf("</head>\n\n");
	printf("<body>\n");
}

void cgi_html_end()
{
	printf("</body> </html>\n");
}

void cgi_h1(char *header)
{
	printf("<h1>%s</h1>\n",header);
}

void cgi_h2(char *header)
{
	printf("<h2>%s</h2>\n",header);
}

void cgi_h3(char *header)
{
	printf("<h3>%s</h3>\n",header);
}

void cgi_h4(char *header)
{
	printf("<h4>%s</h4>\n",header);
}

void cgi_h5(char *header)
{
	printf("<h5>%s</h5>\n",header);
}

void cgi_h6(char *header)
{
	printf("<h6>%s</h6>\n",header);
}

/* state related functions */
void cgi_hidden(char *name, char *value)
{
	printf("<input type=hidden name=\"%s\" value=\"%s\">\n",name,value);
}


void cgi_error(char *errstring)
{
	char *htmlname = HTML_ERROR;
	FILE *fp;
	char linein[1024],fname[128];
	
	cgi_locale_filename(htmlname, fname);
	fp = fopen(fname,"r");
	
	if(!fp)
	{
		printf("%s\n", gettext("can not open error page") );
		return ;
	}		
	
	while(fgets(linein,1024-1,fp))		
	{
		if(strstr(linein,SPECOUT))
		{
			if(strstr(errstring,"User is calling"))
				printf("%s <br>", gettext("PBX"));
			else if(strstr(errstring,"CGIPBX"))
				printf("%s <br>", gettext("CGIPBX"));
		}
		else printf("%s",linein);
	}		
	fclose(fp);
	exit(-1);
}

#endif

