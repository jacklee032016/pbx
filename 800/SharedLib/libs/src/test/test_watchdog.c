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
		printf("��������Կ�ʼ�Կ��Ź����в���\n");
		printf("��ѡ����Է���:\n");
		printf("1\t���Կ��Ź���������ʱ���Ƿ�����\n");
		printf("2\t���Թرտ��Ź���ϵͳ�Ƿ�����\n");
		printf("3\t���ԶԿ��Ź����и��º��Ƿ������\n");
		printf("4\t�˳�����\n");
		printf("��ѡ��[1λ]:");
		scanf("%1d",&g);
		printf("-----%d-----\n",g);
		switch(g)
		{	
			case 1:
				printf("=====�������ڿ�ʼ���Կ��Ź���������ʱ��watchdog  wait MAX 64s to reboot system=====\n");
				printf("������ʱ��[��λ��][0-64s][2λ\tλ��������ǰ�油0]:");
				scanf("%2d",&c);printf("-----%d-----\n",c);
				printf("������ʱ��[��λ����][0-1000ms][4λ\tλ��������ǰ�油0]:");
				scanf("%4d",&d);printf("-----%d-----\n",d);
				printf("\n");
				printf("�ȴ�[%d]��[%d]�����ϵͳ����\n",c,d);
				printf("\nplease waiting...........\n\n");
				as_watchdog_enable(c,d);
				while(1);
				break;
				
			case 2:
				printf("=====�������ڿ�ʼ���Թرտ��Ź���ϵͳ�Ƿ�����=====\n");
				as_watchdog_enable(0,0);
				printf("���Ź��Ѿ���\t��30���ʱ�����ǹرտ��Ź�\n");
				sleep(30);
				as_watchdog_disable();
				printf("30��.......���Ź��Ѿ��ر�\n");
				printf("�ٹ�34��ϵͳû��������رտ��Ź��ɹ�\n");
				sleep(36);
				printf("�رտ��Ź����Գɹ�! \n");
				break;
				
			case 3:
				printf("=====�������ڿ�ʼ���ԶԿ��Ź����и��º��Ƿ������=====\n");
				as_watchdog_enable(0,0);
				printf("���Ź��Ѿ���\nϵͳ��ͣˢ�¿��Ź�\n");
				printf("�뿪ʼ��ʱ\t64���û����������Գɹ�\n");
				for(a=0;a<14;a++)
				{
					sleep(5);
					as_watchdog_update();
					printf("assist watchdog update....\n");
					printf("%d\tsec\n",(a+1)*5);
				}
				printf("64���Ѿ���ȥ���Գɹ�\n");
				as_watchdog_disable();
				break;

			case 4:
				return 0;
				
			default:
				printf("����ѡ������\t������ѡ��!\n");
				break;
		}
		
		printf("\n\n");
	}
	return 0;

}




