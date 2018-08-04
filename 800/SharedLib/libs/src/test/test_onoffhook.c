#include <stdlib.h>
#include "aslib.h"


int main()
{
	int temp;
	temp = as_get_onoffhook_states();
	if(temp == 1)
		printf("PHONE OnHook\n");
	else if(temp == 0)
		printf("PHONE OffHook\n");
	return 0;
}



