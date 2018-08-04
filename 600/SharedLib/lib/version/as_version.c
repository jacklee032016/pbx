/*
 * $Id: as_version.c,v 1.3 2007/03/16 19:29:34 lizhijie Exp $
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
		printf("can not create version log %s\r\n", file );
		exit(-1);
	}

	fprintf(fp, "%s", info);

	fclose(fp);
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

