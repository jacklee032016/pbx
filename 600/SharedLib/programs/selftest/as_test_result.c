/* 
 * $Id: as_test_result.c,v 1.3 2007/03/16 19:29:34 lizhijie Exp $
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
	"NPE device 'LAN' failed",
	"NPE device 'WAN' failed",
	"PBX software error", 
	"CF Card is not found", 
	"Block Size of CF Card is Error", 
	"Serial Number on Flash Error",
	"Unknown Error"
};

char *as_test_result_output( as_test_result_type type)
{
	int _type = type;
	if(type<AS_TEST_NO_ERROR || type > AS_UNKNOWN_ERROR )
		_type = AS_UNKNOWN_ERROR;
	
	return as_test_results[_type];
}

