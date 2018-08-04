#include <stdlib.h>
#include "assist_lib.h"
//#include "as_version.h"
#include <stdio.h>


int main(int argc, char *argv[])
{

	//printf( AS_VERSION_INFO("Test BEEP"));

	printf("long beep ------- 3\n");
	as_beep(3,BEEP_LONG);
	printf("short beep ------ 5\n");
	as_beep(5,BEEP_SHORT);
	return 0;
	


}
