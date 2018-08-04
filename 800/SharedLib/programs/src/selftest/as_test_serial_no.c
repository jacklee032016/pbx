#include <stdlib.h>
#include "aslib.h"
#include<ctype.h>


int as_test_serial_no(void)
{
	unsigned char serial_no[20];
	unsigned char *serNO;

	/******************
	*    write serial no to flash
	********************/
	serNO = serial_no;
	printf("please enter 422' serial NO [20B] :\t");
	scanf("%s", serNO);
	as_write_serial_no(serNO);

	/******************
	*   read serial no from flash
	*********************/
	unsigned char serial_no2[20];
	serNO = serial_no2;
	as_read_serial_no(serNO);
	printf("422 serial NO is :\n\t---  ");
	int i;
	for(i=0;i<20;i++)
	{
		printf("%c",*(serNO+i));
	}
	printf("   ---\n");
	
	return 0;

}
