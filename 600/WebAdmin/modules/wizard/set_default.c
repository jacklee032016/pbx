/*
* $Id: set_default.c,v 1.3 2007/09/06 09:19:59 lizhijie Exp $
* setup Factory Default flash configuration after test on product line, li Zhijie, 2006.12.03 
*/

#include <stdlib.h>
#include <ctype.h>
#include "aslib.h"
#include "as_common_version.h"

#include "cgi_lib.h"
#include "cgi_data.h"
#include "cgi_syscfg.h"
#include "as_version.h"

#define	FACTORY_DEFAULT_DATA			"/etc/sys/factory.default"

#define	FACTORY_KEY_DEVICE_ID		"DEVICEID"
#define	FACTORY_KEY_USER				"USER"
#define	FACTORY_KEY_PASSWD			"PASSWD"

#define	FACTORY_KEY_WAN_MAC			"WAN_MAC"
#define	FACTORY_KEY_WAN_IP			"WAN_IP"
#define	FACTORY_KEY_WAN_MASK		"WAN_MASK"

#define	FACTORY_KEY_LAN_MAC			"LAN_MAC"
#define	FACTORY_KEY_LAN_IP			"LAN_IP"
#define	FACTORY_KEY_LAN_MASK			"LAN_MASK"

#define	DEFAULT_INIT_ITEM(dest, list, key) \
	do{char *value = GET_VALUE((list), (key)); \
		strncpy(dest, value,(AS_FLASH_ITEM_LENGTH>strlen(value))? strlen(value):AS_FLASH_ITEM_LENGTH ); \
		}while(0);

static int	__init_default(AS_FLASH_INFO_T *factory, llist *cfgs)
{
	DEFAULT_INIT_ITEM(factory->serialNo, cfgs, FACTORY_KEY_DEVICE_ID);
	DEFAULT_INIT_ITEM(factory->userName, cfgs, FACTORY_KEY_USER);
	DEFAULT_INIT_ITEM(factory->passwd, cfgs, FACTORY_KEY_PASSWD);
	DEFAULT_INIT_ITEM(factory->wanMac, cfgs, FACTORY_KEY_WAN_MAC);
	DEFAULT_INIT_ITEM(factory->wanIp, cfgs, FACTORY_KEY_WAN_IP);
	DEFAULT_INIT_ITEM(factory->wanMask, cfgs, FACTORY_KEY_WAN_MASK);

	DEFAULT_INIT_ITEM(factory->lanMac, cfgs, FACTORY_KEY_LAN_MAC);
	DEFAULT_INIT_ITEM(factory->lanIp, cfgs, FACTORY_KEY_LAN_IP);
	DEFAULT_INIT_ITEM(factory->lanMask, cfgs, FACTORY_KEY_LAN_MASK);
	printf("Factory Default Data will write to NVRAM is :\n");
	as_default_info_debug(factory);
	return 0;
}

static void __usage()
{
	printf("Usage :\t%s [-w]\n", AS_SETUP_MODULE_NAME);
	printf("       no param : read NVRAM and print this message\n");
	printf("       -w       : read factory default data from '%s' and write to NVRAM\n", FACTORY_DEFAULT_DATA);
}

int main(int argc, char *argv[])
{
	llist 				cfgs;
	AS_FLASH_INFO_T	factory;

	as_ver_debug(AS_VERSION_INFO(AS_SETUP_MODULE_NAME));	
	memset(&factory, 0, sizeof(AS_FLASH_INFO_T) );
	if(argc > 1)	
	{
		list_create(&cfgs);
		if (readconfig(FACTORY_DEFAULT_DATA, ITEM_EQU, NULL, &cfgs ) != 0)
		{
			printf("No Factory Default Data!\nPlease create and edit the file '%s' firstly!\n\n", FACTORY_DEFAULT_DATA);
			return -1;
		}

		__init_default(&factory, &cfgs);
		as_default_write(&factory);
	}
	
	if(as_default_read( &factory) )
	{
		printf("read Factory Default failed\n");
		return 0;
	}
	
	printf("Factory Default read from NVRAM is :\n" );
	as_default_info_debug(&factory);

	if(argc ==1 )
		__usage();
	
	return 0;
}

