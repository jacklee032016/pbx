#include <stdlib.h>
#include "assist_lib.h"
//#include "as_version.h"
#include <stdio.h>


int main(int argc, char *argv[])
{
	factory_button_status res;

	//printf( AS_VERSION_INFO("Test Factory Button Status"));

	while(1)
	{
		res = as_factory_button_states();

		if(res == FACTORY_TOUCH)
		{
			printf("FACTORY BUTTON: touch off\n");
			break;
		}
		else if(res == FACTORY_NORMAL)
		{
			printf("FACTORY BUTTON: not touch off\n");
		}
		else
		{
			printf("ERROR: Lawless parameter\n");
			return -1;
		}
	}

	printf("FACTORY BUTTON: Test succeed\n");
	return 0;

}

