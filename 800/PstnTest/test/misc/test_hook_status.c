#include <stdlib.h>
#include "assist_lib.h"
//#include "as_version.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
	phone_hook_status res;

	//printf( AS_VERSION_INFO("Test Hook Status"));

	res = as_hook_check();

	if(res == G_PHONE_OFFHOOK)
	{
		printf("HOOK STATUS: offhook\n");
	}
	else if(res == G_PHONE_ONHOOK)
	{
		printf("HOOK STATUS: onhook\n");
	}
	else
	{
		printf("ERROR: return Lawless parameter\n");
		return -1;
	}

	printf("HOOK STATUS: Test succeed\n");
	return 0;

}



