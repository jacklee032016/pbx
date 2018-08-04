/* 
 * $Id: as_test_drivers.c,v 1.3 2007/03/16 20:39:41 lizhijie Exp $
 * as_test_drivers.c : Test Tiger and Zarlink Drivers 
 * Li Zhijie, 2005.05.26
*/
#include <stdio.h>

#include "aslib.h"
#include "as_test.h"

#define AS_PCM_FILE				"/dev/astel/1"
#if ZARLINK_SUPPORT
#define AS_ZARLINK_FILE			"/proc/zlec"
#endif

static int __test_file_open(char *file)
{
	int  fd;

	fd = open(file, O_RDONLY);
	if (fd < 0) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Unable to open %s: %s", file, strerror(errno));
		return AS_FAIL;
	}
	close(fd);
	return AS_OK;
}

int as_test_pcm_drivers(void *data)
{
	if(__test_file_open(AS_PCM_FILE)<0)
	{
		return -AS_TEST_PCM_DRIVER_NOT_INITTED;
	}

	return AS_OK;
}

#if ZARLINK_SUPPORT
int as_test_zarlink_driver(void *data)
{
	if(__test_file_open(AS_ZARLINK_FILE) < 0 )
	{
		return -AS_TEST_ZARLINK_DRIVER_NOT_INITED;
	}
	return AS_OK;
}
#endif

