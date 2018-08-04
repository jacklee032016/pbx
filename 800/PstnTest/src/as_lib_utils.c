/*
 * $Author: lizhijie $
 * $Log: as_lib_utils.c,v $
 * Revision 1.1.1.1  2006/11/30 16:23:05  lizhijie
 * AS800 PstnTest
 *
 * Revision 1.1.1.1  2006/01/14 09:36:42  lizhijie
 * PSTN test library
 *
 * Revision 1.2  2005/04/26 06:06:10  lizhijie
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/03/14 06:56:04  lizhijie
 * new drivers for all devices
 *
 * Revision 1.1.1.5  2004/12/31 08:48:47  fengshikui
 * no message
 *
 * Revision 1.1.1.4  2004/12/31 08:38:05  fengshikui
 * no message
 *
 * Revision 1.1.1.3  2004/12/31 08:23:52  fengshikui
 * �޸İ�
 *
 * Revision 1.1.1.2  2004/12/31 08:00:11  fengshikui
 * �޸İ�
 *
 * Revision 1.1.1.1  2004/11/18 07:00:04  lizhijie
 * driver for assist telephone cards Tiger320-Si3210/3050
 *
 * $Revision: 1.1.1.1 $
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

