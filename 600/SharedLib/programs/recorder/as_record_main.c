/*
* $Id: as_record_main.c,v 1.3 2007/03/26 20:24:45 lizhijie Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include "aslib.h"
#include "as_version.h"

const char* usage = "recorder <config filename>";
#define	TAPER_NAME	"RECORD"

struct str_parameter
{
	char device_type[10];
	int device_fd;
	char file_path[256];
	int time_lenght;
	char starttip_file[256];
	char endtip_file[256];
	char action_type[10];
	int waite_time;
};

typedef struct str_parameter recorder_parameter_t;

log_stru_t taperLog = 
{
	name		:	TAPER_NAME,
	lstyle		:	USE_CONSOLE,
	llevel		: 	AS_LOG_DEBUG,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_RECORD, 
	isDaemonized	:	0
};


char cfg[60][128];
static const char *filter = "\t\r\n";
recorder_parameter_t local_param;

static void __erase(char *buf, int pos, int nums)
{
	int length1 = strlen(buf);
	memcpy(buf+pos, buf+pos+nums, length1-nums-pos);
	int length2 = length1-nums;
	memset(buf+length2, 0, nums);
}

static void __filter_c(char *source, char *filter)
{
	char *sv = source;
	int n = strlen(filter);
	int length = strlen(sv);
	int i,j;
	
	for (i = 0; i < length; ++i) 
	{
		for (j = 0; j < n; ++j)
		{
			if (*(sv+i) == filter[j])
			{
				__erase(source,i,1);
			}
		}
	}	
}

static void __parser(recorder_parameter_t *out_param)
{
	strcpy(out_param->device_type, (char*)cfg[2]); 
	out_param->device_fd = atoi((char*)cfg[5]);
	strcpy(out_param->file_path, (char*)cfg[8]); // voice_path
	out_param->time_lenght = atoi((char*)cfg[11]); // voice_length
	strcpy(out_param->starttip_file, (char*)cfg[14]);
	strcpy(out_param->endtip_file, (char*)cfg[17]);
	strcpy(out_param->action_type, (char*)cfg[20]);
	out_param->waite_time = atoi((char*)cfg[23]);

	AS_MSG_DEBUG(AS_LOG_DEBUG, "device_type = %s", out_param->device_type);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "device_fd = %d", out_param->device_fd);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "file_path = %s", out_param->file_path);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "time_length = %d", out_param->time_lenght);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "starttip_file = %s", out_param->starttip_file);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "endtip_file = %s", out_param->endtip_file);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "action_type = %s", out_param->action_type);
	AS_MSG_DEBUG(AS_LOG_DEBUG, "waite_time = %d", out_param->waite_time);

	memcpy(&local_param, out_param, sizeof(recorder_parameter_t));

}

int _get_conf(const char* in_conf_file, recorder_parameter_t *out_param)
{	
	FILE *fp = fopen(in_conf_file, "rb");
	if (fp == NULL)
	{
		AS_MSG_LOG(AS_LOG_ERR,"RECORD configuration File '%s' open failed", in_conf_file);
		return ERR_OPEN_C;
	}

	int index1 = 0, index2 = 0;
	memset(cfg, 0, sizeof(cfg));

	while (!feof(fp))
	{
		char linebuf[256] = {0}; 
		fgets(linebuf, sizeof(linebuf), fp);
		if (strlen(linebuf) < 10 || linebuf[0] == '#')
			continue;
		__filter_c(linebuf, (char*)filter);

		int l = strlen(linebuf);
		int inc = 1;
		int i = 0;
		
		for (i = 0; i < l; ++i)
		{
			if (linebuf[i] == ' ' && inc == 1)
			{
				inc = 0;
				index1++;
				index2 = 0;
			}
			else if (linebuf[i] != ' ')
			{
				cfg[index1][index2++] += linebuf[i];
				inc = 1;
			}
		}

		index1++;
		index2 = 0;

	}
	fclose(fp);
	
	__parser(out_param);
	
	return ERR_SUCCESS;
}

int	_init_record(RECORD_T *record, recorder_parameter_t *params)
{
	if (strcmp(params->device_type, "PCM") == 0)
	{
		record->type = RECORD_TYPE_PCM;
		record->devIndex = -1;
	}
	else
	{
		record->type = RECORD_TYPE_SLIC;
		record->devIndex = params->device_fd;
	}
	if (strcmp(params->action_type, "recorder") == 0)
	{
		record->action = RECORD_ACTION_RECORD;
	}
	else
	{
		record->action = RECORD_ACTION_PLAY;
	}
	record->timeLength = params->time_lenght;
	record->waitTime = params->waite_time;
	record->state = RECORD_STATE_WAIT_OFFHOOK;

	sprintf(record->fileName, "%s", params->file_path);
	
	return 0;
}

RECORD_T	_record;
int main(int argc, char* argv[])
{
	RECORD_T	*record = &_record;
	//version
	as_ver_debug(AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));	
	as_ver_log(AS_RECORDER_MODULE_NAME,AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));
	
	if (argc < 2)
	{
		printf("usage: %s\n", usage);
//		return 0;
	}

	if (as_log_init(&taperLog) < 0)
	{
		printf("%s Log Init Failed.\n", taperLog.name);
		return 0;
	}
	
	recorder_parameter_t rdrparam;
	_get_conf((argc<2)?RECORD_CONFIG_FILE:argv[1], &rdrparam);
	_init_record(record, &rdrparam);

	record->app = RECORD_APP_STANDALONE;
	as_msg_queue_init( 0);
	as_timer_register_handler(as_record_time_handler, record);
	
	AS_MSG_LOG(AS_LOG_INFO, "waiting %s device offhook...", rdrparam.device_type);
	
	int wrs = record_wait_offhook(record);
	if (wrs == ERR_UNABLE)
	{
		AS_MSG_LOG(AS_LOG_ERR,"request unable.");
		goto err;
	}
	else if (wrs == ERR_OVERTIME)
	{
		AS_MSG_LOG(AS_LOG_ERR,"time over! please offhook your device inside %d seconds", rdrparam.waite_time);
		goto err;
	}

	record_start_work(record);

err:
	record_clean_all(record);
	
	return 0;
}

