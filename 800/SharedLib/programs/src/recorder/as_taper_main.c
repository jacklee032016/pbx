#include "aslib.h"
#include "as_version.h"
#include "as_prog_taper.h"
#include "as_taper.h"
#include <stdio.h>

const char* usage = "recorder <config filename>";

int main(int argc, char* argv[])
{
	//version
	as_ver_debug(AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));	
	as_ver_log(AS_RECORDER_MODULE_NAME,AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));
	
	if (argc < 2)
	{
		printf("usage: %s\n", usage);
		return 0;
	}

	recorder_parameter_t rdrparam;
	get_conf(argv[1], &rdrparam);
	set_parameter(&rdrparam);
	
	printf("waiting %s device offhook...\n", rdrparam.device_type);
	
	int wrs = wait_offhook();
	
	if (wrs == ERR_UNABLE)
	{
		printf("request unable.\n");
		return 0;
		
	}
	else if (wrs == ERR_OVERTIME)
	{
		printf("time over! please offhook your device inside %d seconds\n", rdrparam.waite_time);
		return 0;
	}
	else
	{
		start_work(wrs);
	}

	clean_all();
	
	return 0;
}
