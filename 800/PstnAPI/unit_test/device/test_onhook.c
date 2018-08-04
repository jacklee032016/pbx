#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include"as_thread.h"
#include"assist_lib.h"
#include "as_version.h"

int main(int argc, char **argv)
{
	int fds;

	printf( AS_VERSION_INFO("Test onhook\n"));
	
	if (argc < 2)
	{
		fprintf (stderr, "Usage: onhook FXO/FXS!\n");
		exit (1);
	}

	fds = open (argv[1], O_RDWR);
	if (fds < 0)
	{
		fprintf (stderr, "error open file %s\n", argv[1]);
		exit (1);
	}

	if(argc>=3)
	{
		fprintf (stderr, "Prepare to off hook %s\n", argv[1]);
		as_lib_offhook (fds);
	}
	else
	{
		fprintf (stderr, "Prepare to on hook %s\n", argv[1]);
		as_lib_onhook (fds);
	}

	printf("ONHOOK:  Test ended successfully!\n");

	close (fds);
	return 0;
}

