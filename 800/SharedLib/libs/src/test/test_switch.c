
#include <stdlib.h>
#include "aslib.h"


int main(int argc, char *argv[])
{
	int status;
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: <status '0'-PSTN or '1'-IP> \n");
		exit(1);
	}
	status = atoi(argv[1]);

	if(status == 0)
	{
		as_switch_low();
		printf("PHONE switch to PSTN\n");
	}
	else if(status == 1)
	{
		as_switch_high();
		printf("PHONE switch to IP\n");
	}
	else
		printf("ER:PHONE switch failure\n");

	
	return 0;
	


}


