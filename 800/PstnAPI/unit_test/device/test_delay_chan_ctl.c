
#include <stdlib.h>
#include "assist_lib.h"
#include "as_version.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
	printf( AS_VERSION_INFO("to switch LINE or PSTN Test"));
	
	if (argc < 2) 
	{
		printf("Usage: delay_chan_ctl <switch '0'-Line or '1'-FXS>\n");
		exit(1);
	}
	
	as_delay_chan_ctl(atoi(argv[1]));

	printf("DELAY_CHAN_CTL:  Test ended successfully!\n");

	return 0;

}

