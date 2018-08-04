/*
 * $Author: lizhijie $
 * $Log: as_test.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.7  2005/09/16 09:38:00  wangwei
 * no message
 *
 * Revision 1.6  2005/09/16 09:31:29  wangwei
 * no message
 *
 * Revision 1.5  2005/09/05 07:14:30  wangwei
 * add write serial no to flash
 *
 * Revision 1.4  2005/06/24 05:25:03  wangwei
 * 增加CF卡出错的种类
 *
 * Revision 1.3  2005/06/03 04:00:09  lizhijie
 * no message
 *
 * Revision 1.2  2005/05/27 10:01:52  lizhijie
 * make ZarLink test as a independent self-test case
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_test.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
 * as_test.h , Li Zhijie, 2005.05.26
 * All data structure and macro for test is defined here
*/
#ifndef  __AS_TEST_H__
#define __AS_TEST_H__

#include "as_global.h"

typedef enum
{
	AS_TEST_NO_ERROR	= 0,
	AS_TEST_PCM_DRIVER_NOT_INITTED,
#if ZARLINK_SUPPORT
	AS_TEST_ZARLINK_DRIVER_NOT_INITED,
#endif
	AS_TEST_UART_NOT_FOUND,
	AS_TEST_PBX_NO_RESPONSE,
	AS_TEST_PBX_BUSY_HERE,
	AS_TEST_ETH0_FAIL,
	AS_TEST_ETH1_FAIL,
#if ETH2_SUPPORT
	AS_TEST_ETH2_FAIL,
#endif	
	AS_TEST_PBX_SOFTWARE_ERROR,		/* for example, the channel_no return by PBX is not validate */
	AS_CFC_NOT_EXT_FAIL,
	AS_CFC_BLOCK_SIZE_FAIL
}as_test_result_type;


typedef struct
{
	char  name[AS_MAX_NAME_LENGTH];
	void *data;
	/* return <0 : error */
	int (*handler)(void *data);
}as_test_entry;

#if PCM_SUPPORT
int as_test_pcm_channels( void *data);
#endif 
int as_test_pcm_drivers(void *data);

#if ZARLINK_SUPPORT
int as_test_zarlink_driver(void *data);
#endif

int as_test_dev_eth0(void *data);
int as_test_dev_eth1(void *data);
#if ETH2_SUPPORT
int as_test_dev_eth2(void *data);
#endif
#if CF_SUPPORT
int as_test_cf_card(void *data);
#endif
int as_test_serial_no(void);
char *as_test_result_output( as_test_result_type type);
#if LED_SUPPORT
void as_test_led(void);
#endif

#endif

