/* 
 * $Id: as_test_main.c,v 1.7 2007/03/25 19:56:17 lizhijie Exp $
 * as_test_main.c : Main program for Self-Test 
 * Li Zhijie, 2005.05.26
*/
#include <stdlib.h>

#include "aslib.h"
#include "as_version.h"
#include "as_test.h"


as_test_entry  test_entries[] =
{
	{
		name	:	"NPE(Network Process Engine) LAN device",
		handler	:	as_test_dev_eth0
	},
	
	{
		name	:	"NPE(Network Process Engine) WAN device",
		handler	:	as_test_dev_eth1
	},

#if ZARLINK_SUPPORT
	{
		name	:	"ZarLink hardware Drivers Test",
		handler	:	as_test_zarlink_driver
	},
#endif

	{
		name	:	"PCM Channels Test",
		handler	:	as_test_pcm_channels	
	},
	{
		name	:	"Serial Number Test",
		handler	:	as_test_serial_no	
	},

	{
		name	:	"CompactFlash Card Test",
		handler	:	as_test_cf_card
	}
};

#define	SELFTEST_NAME		"SELFTEST"
log_stru_t selfTestLog = 
{
	name		:	SELFTEST_NAME,
	lstyle		:	USE_CONSOLE,
	llevel		: 	AS_LOG_INFO,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_SELFTEST,
	isDaemonized	:	0
};

int main(int argc, char *argv[])
{
	int i;
	int res;

	as_ver_debug(AS_VERSION_INFO(AS_SELFTEST_MODULE_NAME));	
	as_ver_log(AS_SELFTEST_MODULE_NAME,AS_VERSION_INFO(AS_SELFTEST_MODULE_NAME));

	if (as_log_init(&selfTestLog) < 0)
	{
		printf("%s Log Init Failed.\n", selfTestLog.name );
		return 0;
	}

	for( i=0; i< sizeof(test_entries)/sizeof(as_test_entry); i++)
	{
		AS_MSG_LOG(AS_LOG_INFO, "Test Case %d : %s.....", i+1, test_entries[i].name );
		
		res = (test_entries[i].handler)( test_entries[i].data );
		if(res <0 )
		{
			as_beep_long(1);
			AS_MSG_LOG(AS_LOG_INFO, "Test Case %d(%s) failed : %s\n\n", i+1, test_entries[i].name , as_test_result_output(-res));
		}
		else
		{
			AS_MSG_LOG(AS_LOG_INFO, "Test Case %d(%s) Sucessed\n\n", i+1, test_entries[i].name );
		}
	}

	AS_MSG_LOG(AS_LOG_INFO, "End of %s\r\n", selfTestLog.name );
	return 0;
}

