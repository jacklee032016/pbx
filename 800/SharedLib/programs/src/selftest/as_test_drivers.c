/* 
 * $Log: as_test_drivers.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:38  wangwei
 * no message
 *
 * Revision 1.3  2005/06/24 07:30:33  lijie
 * *** empty log message ***
 *
 * Revision 1.2  2005/05/27 10:01:52  lizhijie
 * make ZarLink test as a independent self-test case
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Id: as_test_drivers.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
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
		fprintf(stderr, "Unable to open %s: %s\n", file, strerror(errno));
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

