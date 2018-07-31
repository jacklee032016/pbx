/*
* $Author: lizhijie $
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cgiLib.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/* HTTP headers */
void cgi_html_header()
{
	printf("Content-type: text/html\r\n\r\n");
}



