/*
* $Id: as_test_serial_no.c,v 1.6 2007/03/16 20:39:41 lizhijie Exp $
*/

#include <stdlib.h>
#include <ctype.h>

#include "aslib.h"
#include "as_test.h"

int as_test_serial_no(void *data)
{
	int i;
	unsigned char serial_write[AS_FLASH_ITEM_LENGTH];
	unsigned char serial_read[AS_FLASH_ITEM_LENGTH];

	memset(serial_read, 0 , sizeof(serial_read) );
	memset(serial_write, 0, sizeof(serial_write) );
	
	AS_MSG_LOG(AS_LOG_INFO,"please enter 422 serial NO [20B] :\t");
	scanf("%s", serial_write );
	as_write_serial_no(serial_write);

	as_read_serial_no(serial_read, sizeof(serial_read) );
	AS_MSG_LOG(AS_LOG_INFO,"422 serial NO is :\n\t---  ");
	for(i=0;i<20;i++)
	{
		AS_MSG_LOG(AS_LOG_INFO,"%c",*(serial_read+i));
	}
	AS_MSG_LOG(AS_LOG_INFO,"   ---");

	if(memcmp(serial_read, serial_write , 20) )
		return AS_FLASH_SERIAL_NO_FAIL;
	
	return 0;
}

