#include <stdlib.h>
#include "aslib.h"


int main()
{
	printf("test.....................\n");
	int temp;
	while(1)
	{
		temp = as_get_button_states();
		as_beep_long(1);
		printf("test button states -----------%d\n",temp);
	}
	return 0;
}



