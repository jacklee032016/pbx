/*
* $Id: setup_flash.c,v 1.1 2006/12/19 14:23:21 lizhijie Exp $
* setup flash after test on product line, li Zhijie, 2006.12.03 
* This program should be burned into flash root filesystem
*/

#include <stdlib.h>
#include<ctype.h>

#include "aslib.h"
#include "as_version.h"

int main()
{
	AS_FLASH_INFO_T info;
	
	as_ver_debug(AS_VERSION_INFO(AS_SETUP_MODULE_NAME));	

	as_default_write(&factoryDefault);

	if(as_default_read( &info) )
	{
		printf("read Factory Default failed\n");
		return 0;
	}
	
	as_default_info_debug(&factoryDefault);
	
	printf("read Factory Default Address : (%p)\n", &info);
	as_default_info_debug(&info);
	
	return 0;
}

