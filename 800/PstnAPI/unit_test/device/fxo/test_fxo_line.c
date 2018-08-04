/*
 * $Log: test_fxo_line.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.1  2006/04/19 12:25:58  lizhijie
 * no message
 *
 * Revision 1.2  2006/03/25 11:27:28  lizhijie
 * add FXO line status detect
 *
 * Revision 1.1  2006/03/25 11:26:47  lizhijie
 * no message
 *
 * $Id: test_fxo_line.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

/*
* Test Line Status of FXO/DAA port
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "assist_lib.h"
#include "as_version.h"

int main(int argc, char *argv[])
{
	int fd;
	
	printf( AS_VERSION_INFO("Read Line Status of FXO port"));
	printf("INTENT: read line status of FXO(DAA)\n");
	
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: fxo_line <FXO device>\n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	if( as_device_check_fxo_online( fd, "FXO(DAA)")  )
	{
		printf("FXO/DAA(%s) has line plugin\n", argv[1]);
	}
	else
	{
		printf("FXO/DAA(%s) has no line plugin\n", argv[1]);
	}

	printf("STATUS: Test succeed\n");
	return 0;
}

