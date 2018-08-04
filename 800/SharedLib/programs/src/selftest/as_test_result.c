/* 
 * $Log: as_test_result.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.2  2005/06/03 04:01:19  lizhijie
 * add defination for NPE test
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_test_result.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
 * as_test_result.c : test result output program for Self-Test 
 * Li Zhijie, 2005.05.26
*/
#include "as_test.h"

static char *as_test_results[] = 
{
	"No Error in IXP hardware test",
	"Tiger(PCM) driver not loaded now",
#if ZARLINK_SUPPORT
	"Zarlink(Echo Canceller) driver not loaded now",
#endif
	"UART for communication is not found",
	"No response from PBX",
	"PBX is busy now",
	"NPE device 'WAN0' failed",
	"NPE device 'WAN1' failed",
#if ETH2_SUPPORT
	"NPE device 'LAN' failed",
#endif
	"PBX software error"
};

char *as_test_result_output( as_test_result_type type)
{
	return as_test_results[type];
}


