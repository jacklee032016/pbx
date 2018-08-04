/*
 * $Id: assist_utils.c,v 1.2 2007/03/06 18:56:20 lizhijie Exp $
*/
#include <stdio.h> 
#include <getopt.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "assist_version.h"

int as_error_msg(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return 0;
}

#if ! __KERNEL__
int as_ver_logcv(int argc, char **argv) 
{
	FILE *fp;
	char file[256];

	sprintf(file,"%s%s.%s",AS_VERSION_LOGPATH, AS_MODULENAME,"ver");
	fp=fopen(file,"w");
	if(!fp)
	{
		printf("can not create version log");
		exit(-1);
	}
	AS_VERSION_LOG(fp,AS_MODULENAME);
	fclose(fp);
	return 0;
}
#endif

int as_ver_opt (int argc, char **argv) 
{
	 int c;

	int option_index = 0;
	static struct option long_options[] = 
	{
		{"version", 0, 0, 0},
		{0, 0, 0, 0}
	};
       
	while (1)
	{
		c = getopt_long (argc, argv, "v",long_options, &option_index);
		if (c == -1)
		break;

		switch (c)
		{
			case 'v':
			case 0:
				AS_VERSION_INFO(AS_MODULENAME);
				exit(0);
				break;

		}
		option_index++;
	}
	return 0;
}


