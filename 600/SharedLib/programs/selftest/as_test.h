/*
 * $Author: lizhijie $
 * $Id: as_test.h,v 1.4 2007/03/16 19:29:34 lizhijie Exp $
 * as_test.h , Li Zhijie, 2005.05.26
 * All data structure and macro for test is defined here
*/
#ifndef  __AS_TEST_H__
#define __AS_TEST_H__

#include "as_global.h"
#include "aslib.h"

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
	AS_TEST_PBX_SOFTWARE_ERROR,		/* for example, the channel_no return by PBX is not validate */
	AS_CFC_NOT_EXT_FAIL,
	AS_CFC_BLOCK_SIZE_FAIL, 
	AS_FLASH_SERIAL_NO_FAIL,
	AS_UNKNOWN_ERROR
}as_test_result_type;


typedef struct
{
	char  name[AS_MAX_NAME_LENGTH];
	void *data;
	/* return <0 : error */
	int (*handler)(void *data);
}as_test_entry;

int as_test_pcm_channels( void *data);

int as_test_pcm_drivers(void *data);

#if ZARLINK_SUPPORT
int as_test_zarlink_driver(void *data);
#endif

int as_test_dev_eth0(void *data);
int as_test_dev_eth1(void *data);
int as_test_cf_card(void *data);
int as_test_serial_no(void *data);

char *as_test_result_output( as_test_result_type type);

#endif

