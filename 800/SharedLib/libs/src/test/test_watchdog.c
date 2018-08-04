#include <stdlib.h>
#include "aslib.h"

int main()
{
	printf("test.....................\n");
	int c,d;
	int g;
	char a;
	while(1)
	{
		printf("现在你可以开始对看门狗进行测试\n");
		printf("请选择测试方法:\n");
		printf("1\t测试看门狗重新启动时间是否正常\n");
		printf("2\t测试关闭看门狗后系统是否重启\n");
		printf("3\t测试对看门狗进行更新后是否会重启\n");
		printf("4\t退出测试\n");
		printf("请选择[1位]:");
		scanf("%1d",&g);
		printf("-----%d-----\n",g);
		switch(g)
		{	
			case 1:
				printf("=====我们现在开始测试看门狗重新启动时间watchdog  wait MAX 64s to reboot system=====\n");
				printf("请输入时间[单位秒][0-64s][2位\t位数不够在前面补0]:");
				scanf("%2d",&c);printf("-----%d-----\n",c);
				printf("请输入时间[单位毫秒][0-1000ms][4位\t位数不够在前面补0]:");
				scanf("%4d",&d);printf("-----%d-----\n",d);
				printf("\n");
				printf("等待[%d]秒[%d]毫秒后系统重启\n",c,d);
				printf("\nplease waiting...........\n\n");
				as_watchdog_enable(c,d);
				while(1);
				break;
				
			case 2:
				printf("=====我们现在开始测试关闭看门狗后系统是否重启=====\n");
				as_watchdog_enable(0,0);
				printf("看门狗已经打开\t在30秒的时候我们关闭看门狗\n");
				sleep(30);
				as_watchdog_disable();
				printf("30秒.......看门狗已经关闭\n");
				printf("再过34秒系统没有重启则关闭看门狗成功\n");
				sleep(36);
				printf("关闭看门狗测试成功! \n");
				break;
				
			case 3:
				printf("=====我们现在开始测试对看门狗进行更新后是否会重启=====\n");
				as_watchdog_enable(0,0);
				printf("看门狗已经打开\n系统不停刷新看门狗\n");
				printf("请开始记时\t64秒后没有重启则测试成功\n");
				for(a=0;a<14;a++)
				{
					sleep(5);
					as_watchdog_update();
					printf("assist watchdog update....\n");
					printf("%d\tsec\n",(a+1)*5);
				}
				printf("64秒已经过去测试成功\n");
				as_watchdog_disable();
				break;

			case 4:
				return 0;
				
			default:
				printf("您的选择有误\t请重新选择!\n");
				break;
		}
		
		printf("\n\n");
	}
	return 0;

}




