/*
* $Id: test_serialno.c,v 1.2 2006/12/19 14:11:27 lizhijie Exp $
*/

#include <stdlib.h>
#include "aslib.h"
#include<ctype.h>

int main()
{
	unsigned char serial_no[AS_FLASH_ITEM_LENGTH];
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
	unsigned char serial_no2[AS_FLASH_ITEM_LENGTH];
	serNO = serial_no2;
	as_read_serial_no(serNO, sizeof(serial_no2));
	printf("422 serial NO is :\n");
	int i;
	for(i=0;i<20;i++)
	{
		printf("%c",*(serNO+i));
	}
	printf("\n");
	
	return 0;

}

