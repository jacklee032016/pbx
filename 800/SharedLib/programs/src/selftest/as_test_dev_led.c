#include <stdlib.h>
#include "aslib.h"
#include<ctype.h>

#define delay_time 1700000

void as_test_led(void)
{
	as_led1_enable();usleep(delay_time);
	as_led2_enable();usleep(delay_time);
	as_led3_enable();usleep(delay_time);
	as_led4_enable();usleep(delay_time);
	as_led5_enable();usleep(delay_time);
	as_led6_enable();usleep(delay_time);
	as_led7_enable();usleep(delay_time);
	return;

}

