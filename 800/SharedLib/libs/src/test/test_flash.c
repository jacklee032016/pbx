#include <stdlib.h>
#include "aslib.h"
#include<ctype.h>




static char *as_flash_name[] = 
{
"MAC address of WAN",
"Serial No",
"User name",
"PassWord",
"IP of WAN",
"IP of LAN",
"MAC address of LAN"
};


int main()
{
	struct options_flash *opflash = NULL;
	int i ;
	int j;

	opflash = (struct options_flash *)malloc( sizeof(struct options_flash));
	memset(opflash, 0, sizeof(struct options_flash));
	
	as_read_flash(opflash);
#if 0
	for(j=0;j<opflash->options_num;j++)
	{
		printf("%s\t\tlength[%d] : ",as_flash_name[j],opflash->options_len[j]);
		for(i=0;i<opflash->options_len[j];i++)
		{
			printf("%c",*(opflash->options_value[j]+i));
		}
		printf("\n");
	}
#endif

	/******************
	*    write serial no to flash
	********************/

	unsigned char serial_no1[40];
	unsigned char serial_no2[40];
	unsigned char serial_no3[40];
	unsigned char serial_no4[40];
	unsigned char serial_no5[40];
	unsigned char serial_no6[40];
	unsigned char serial_no7[40];
	unsigned char *serNO;


	serNO = serial_no1;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[0],opflash->options_len[0]);
	scanf("%s", serNO);
	opflash->options_value[0] = serNO;
	
	serNO = serial_no2;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[1],opflash->options_len[1]);
	scanf("%s", serNO);
	opflash->options_value[1] = serNO;

	serNO = serial_no3;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[2],opflash->options_len[2]);
	scanf("%s", serNO);
	opflash->options_value[2] = serNO;

	serNO = serial_no4;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[3],opflash->options_len[3]);
	scanf("%s", serNO);
	opflash->options_value[3] = serNO;

	serNO = serial_no5;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[4],opflash->options_len[4]);
	scanf("%s", serNO);
	opflash->options_value[4] = serNO;

	serNO = serial_no6;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[5],opflash->options_len[5]);
	scanf("%s", serNO);
	opflash->options_value[5] = serNO;

	serNO = serial_no7;
	printf("please enter 422' %s\t[%dB] :\t",as_flash_name[6],opflash->options_len[6]);
	scanf("%s", serNO);
	opflash->options_value[6] = serNO;

#if 0
	for(i=0;i<opflash->options_num;i++)
	{
		for(j=0;j<opflash->options_len[i];j++)
		{
			printf("%c",toascii(*(opflash->options_value[i]+j)));
		}
		printf("\n");
	}
#endif

	as_write_flash(opflash);

	as_read_flash(opflash);
	for(j=0;j<opflash->options_num;j++)
	{
		printf("%s\t:\t",as_flash_name[j]);
		for(i=0;i<opflash->options_len[j];i++)
		{
			printf("%c",*(opflash->options_value[j]+i));
		}
		printf("\n");
	}


	return 0;

}

