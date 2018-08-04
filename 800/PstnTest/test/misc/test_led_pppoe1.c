#include <stdlib.h>
#include "assist_lib.h"
//#include "as_version.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
	//printf( AS_VERSION_INFO("Turn on or Turn off PPPOE1-LED Test"));
	
	if (argc < 2) 
	{
		printf( "Usage: led_line <'0' Turn On LED -- '1' Turn Off LED>\n");
		exit(1);
	}
	
	as_led_pppoe1(atoi(argv[1]));

	return 0;

}

