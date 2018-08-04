/*
 * $Author: lizhijie $
 * $Log: as_version.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.3  2005/09/01 08:59:58  tangr
 * deal version
 *
 * Revision 1.2  2005/07/08 01:34:30  lijie
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_version.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/

#include "as_common_version.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <getopt.h>
#include "aslib.h"

int as_ver_log( char *modulename, char *info)
{
	FILE *fp;
	char file[256];

	sprintf(file,"%s/%s.ver",AS_VERSION_LOGPATH,modulename);

	fp=fopen(file,"w");
	
	if(!fp)
	{
		printf("can not open version log %s\r\n", file );
		exit(-1);
	}


	fprintf(fp, "%s", info);

	fclose(fp);
       return 0;
}

int as_error_msg(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	return 0;
}

int as_ver_opt (int argc, char **argv, char *info) 
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

				#if  __KERNEL__
					printk("%s", info);
				#else
					printf("%s", info);						
				#endif
				
				exit(0);
				
				break;

		}
		option_index++;
	}
	return 0;
}

int as_ver_debug( char *verInfo)
{

	#if  __KERNEL__
		printk(verInfo);
	#else
		printf(verInfo);
	#endif
	
	return 0;
}

