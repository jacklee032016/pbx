#include <stdlib.h>
#include "assist_lib.h"
//#include "as_version.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	led_status_type res;
		
	//printf( AS_VERSION_INFO("Test LED status"));

	if (argc < 2) 
	{
		printf("Usage: delay_chan_ctl < '0'-ON or '1'-OFF>\n");
		exit(1);
	}

	res = atoi(argv[1]);
	
	as_led_status(res);

	return 0;
	


}

