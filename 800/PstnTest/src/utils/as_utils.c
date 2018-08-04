/*
 * $Author: lizhijie $
 * $Revision: 1.1.1.1 $
 * $Log: as_utils.c,v $
 * Revision 1.1.1.1  2006/11/30 16:23:06  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
 * PSTN test library
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.1  2004/11/25 07:24:15  lizhijie
 * move these files from user to userlib
 *
*/

#include <stdio.h> 
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


int as_error_msg(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return 0;
}



