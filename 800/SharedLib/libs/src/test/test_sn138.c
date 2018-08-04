#include <stdlib.h>
#include "aslib.h"

#define SPRING_TIME 50
#define DELAY_TIME 1000


static void led(int lenno)
{
	switch(lenno)
		{
		 case 1:
		 	as_led1_enable();break;
		case 2:
			as_led2_enable();break;
		case 3:
			as_led3_enable();break;
		case 4:
			as_led4_enable();break;
		case 5:
			as_led5_enable();break;
		case 6:
			as_led6_enable();break;
		case 7:
			as_led7_enable();break;
		default:
			printf("no this NO of LED\n");
			break;
		}
	return;
}

int main(int argc, char *argv[])
{
	if(argc<2)
	{
		fprintf(stderr, "Usage:  <LED NO> \n");
		exit(1);
	}
	led (atoi(argv[1]));
/*
	led1();usleep(DELAY_TIME);
	led2();usleep(DELAY_TIME);
	led3();usleep(DELAY_TIME);
	led4();usleep(DELAY_TIME);
	led5();usleep(DELAY_TIME);
	led6();usleep(DELAY_TIME);
	led7();usleep(DELAY_TIME);
*/
	return 0;
	


}