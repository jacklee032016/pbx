/* 
 * $Log: as_test_main.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.8  2005/09/16 09:31:17  wangwei
 * 读写产品序列号改到共享库，调用共享库
 *
 * Revision 1.7  2005/09/05 07:14:41  wangwei
 * add write serial no to flash
 *
 * Revision 1.6  2005/09/01 08:59:58  tangr
 * deal version
 *
 * Revision 1.5  2005/07/08 01:46:36  lijie
 * no message
 *
 * Revision 1.4  2005/06/24 05:25:18  wangwei
 * 增加对CF卡的检查
 *
 * Revision 1.3  2005/06/03 04:01:19  lizhijie
 * add defination for NPE test
 *
 * Revision 1.2  2005/05/27 10:01:52  lizhijie
 * make ZarLink test as a independent self-test case
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_test_main.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
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
		name	:	"NPE(Network Process Engine) WAN1 device",
		handler	:	as_test_dev_eth0
	}
	,
	{
		name	:	"NPE(Network Process Engine) WAN2 device",
		handler	:	as_test_dev_eth1
	}
#if ETH2_SUPPORT
	,
	{
		name	:	"NPE(Network Process Engine) WAN2 device",
		handler	:	as_test_dev_eth2
	}
#endif
#if ZARLINK_SUPPORT
	,
	{
		name	:	"ZarLink hardware Drivers Test",
		handler	:	as_test_zarlink_driver
	}
#endif
#if PCM_SUPPORT
	,
	{
		name	:	"PCM Channels Test",
		handler	:	as_test_pcm_channels	
	}
#endif
#if CF_SUPPORT
	,
	{
		name	:	"CompactFlash Card Test",
		handler	:	as_test_cf_card
	}
#endif
#if LED_SUPPORT
	,
	{
		name	:	"LED Test",
		handler	:	as_test_led
	}
#endif
};


int main(int argc, char *argv[])
{
	int i;
	int res;

	//version
	as_ver_debug(AS_VERSION_INFO(AS_SELFTEST_MODULE_NAME));	
	as_ver_log(AS_SELFTEST_MODULE_NAME,AS_VERSION_INFO(AS_SELFTEST_MODULE_NAME));

	for( i=0; i< sizeof(test_entries)/sizeof(as_test_entry); i++)
	{
		printf("\r\nTest Case %d : %s.....\r\n", i, test_entries[i].name );
		res = (test_entries[i].handler)( test_entries[i].data );
		if(res <0 )
		{
			as_beep_long(1);
			printf("Test Case %d(%s) failed : %s\r\n", i, test_entries[i].name , as_test_result_output(-res));
//			exit(1);
		}
	}

	as_test_serial_no();
	printf("\r\nEnd of program\r\n");
	return 0;
}

