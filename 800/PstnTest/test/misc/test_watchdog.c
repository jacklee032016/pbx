#include <stdlib.h>
//#include "as_version.h"
#include "assist_lib.h"
#include <stdio.h>
#include <unistd.h>



int main()
{
	
	int c,d;
	int g;
	char a;

	//printf( AS_VERSION_INFO("WatchDog Test"));
	
	while(1)
	{
		printf("Now you can test the WatchDog\n");
		printf("please choice test function:\n");
		printf("1\tTest to reboot WatchDog whether or not succeed\n");
		printf("2\tJust close WatchDog later , the system whether or not reboot\n");
		printf("3\tJust to update the WatchDog whether or not reboot\n");
		printf("4\tExit testing\n");
		printf("please choice[1B]:");
		scanf("%1d",&g);
		printf("-----%d-----\n",g);
		switch(g)
		{	
			case 1:
				printf("=====now we can test WatchDog reboot timing [MAX 64sec. to reboot system]=====\n");
				printf("please enter timer[sec][0-64s][2B]:");
				scanf("%2d",&c);printf("-----%d-----\n",c);
				printf("please enter timer[millisecond][0-1000ms][4B]:");
				scanf("%4d",&d);printf("-----%d-----\n",d);
				printf("\n");
				printf("please wait [%d]sec. [%d]ms to reboot system\n",c,d);
				as_watchdog_enable(c,d);
				while(1);
				break;
				
			case 2:
				printf("=====now we can disable WatchDog function the system whether or not reboot=====\n");
				as_watchdog_enable(0,0);
				printf("enable WatchDog funtion, wait 30 sec.\n");
				sleep(30);
				as_watchdog_disable();
				printf("30sec. passed , disable WatchDog\n");
				sleep(36);
				printf("36sec passed ,system do not reboot ,test succeed\n");
				break;
				
			case 3:
				printf("=====now we can test to update WatchDog, the systme whether or not reboot=====\n");
				as_watchdog_enable(0,0);
				printf("enable WatchDag!\nthe system update WatchDog at all times\n");
				printf("wait 64sec. to look-in the system whether or not reboot\n");
				for(a=0;a<14;a++)
				{
					sleep(5);
					as_watchdog_update();
					printf("assist watchdog update....\n");
					printf("%d\tsec\n",(a+1)*5);
				}
				printf("64sec. passed , Test succeed\n");
				as_watchdog_disable();
				break;

			case 4:
				return 0;
				
			default:
				printf("your choice have failure ,again please\n");
				break;
		}
		
		printf("\n\n");
	}
	return 0;

}


