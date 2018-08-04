/*
* $Id: alert_main.c,v 1.4 2007/03/27 20:22:57 lizhijie Exp $
*/

#include <stdio.h>
#include "aslib.h"
#include "as_common_version.h"
#include "as_version.h"
#include "cgi_lib.h"
#include "cgi_alert.h"


#define	ALERT_NAME	"ALERT"

log_stru_t alertLog = 
{
	name		:	ALERT_NAME,
	lstyle		:	USE_FILE,
	llevel		: 	AS_LOG_DEBUG,
	lfacility		:	AS_LOG_7,
	logFileName	:	AS_LOG_FILE_RECORD, 
	isDaemonized	:	0
};

RECORD_T	_record;

int	_init_record(RECORD_T *record, llist *cfgs)
{
	if (!strcasecmp(GET_VALUE(cfgs, RECORD_DEVICE_TYPE), "PCM") )
	{
		record->type = RECORD_TYPE_PCM;
		record->devIndex = -1;
	}
	else
	{
		record->type = RECORD_TYPE_SLIC;
		record->devIndex = atoi(GET_VALUE(cfgs, RECORD_DEVICE_NUMBER) );
	}
	if (!strcasecmp(GET_VALUE(cfgs, RECORD_ACTION_TYPE), RECORD_ACTION_TYPE_PLAY) )
	{
		record->action = RECORD_ACTION_RECORD;
	}
	else
	{
		record->action = RECORD_ACTION_PLAY;
	}
	record->timeLength = atoi(GET_VALUE(cfgs, RECORD_TIME_LENGTH) );
	record->waitTime = atoi(GET_VALUE(cfgs, RECORD_WAIT_TIME) );
	record->state = RECORD_STATE_WAIT_OFFHOOK;

	sprintf(record->fileName, "%s%s", GET_VALUE(cfgs, RECORD_FILE_HOME), GET_VALUE(cfgs, RECORD_FILE_NAME) );
	
	return 0;
}

int	_update_config(RECORD_T *record, llist *cfgs)
{
	char	value[32];
	if(record->devIndex== ERR_UNABLE)
	{
		return ERR_UNABLE;
	}
	
	sprintf(value, "%d", record->devIndex);
	list_replace_value_by_key(cfgs, RECORD_DEVICE_NUMBER, value);
	
	if(writeconfig(RECORD_CONFIG_FILE, ITEM_TRI, NULL, cfgs) != 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "write %s failed : %s", RECORD_CONFIG_FILE, strerror(errno) );
		return -1;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	int 	c;
	int	level = AS_LOG_NOTICE;
	int	isPlay = 0;
	llist  cfgsRecord;
	RECORD_T	*record = &_record;
	char			*file = NULL;
	
	as_ver_debug(AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));	
	as_ver_log(AS_RECORDER_MODULE_NAME,AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));

	while (1)
	{
		c = getopt (argc, argv, "d:vpf:");
		if (c == -1)
			break;

		switch (c)
		{
			case 'v':
				as_ver_log(AS_RECORDER_MODULE_NAME,AS_VERSION_INFO(AS_RECORDER_MODULE_NAME));
				break;
			case 'd':
				level = atoi(optarg);
				break;
			case 'p':
				isPlay = 1;
				break;
			case 'f':
				file = optarg;
				break;
			default:
				break;
		}
	}	

	list_create(&cfgsRecord);
	if (readconfig(RECORD_CONFIG_FILE, ITEM_TRI, NULL, &cfgsRecord) != 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "%s read failed : %s", RECORD_CONFIG_FILE, strerror(errno) );
		return -1;
	}
	
	if (as_log_init(&alertLog) < 0)
	{
		printf("%s Log Init Failed.\n", alertLog.name);
		return 0;
	}
	_init_record(record, &cfgsRecord);

	if(isPlay)
	{
		record->action = RECORD_ACTION_PLAY;
		if(!file)
		{
			AS_MSG_LOG(AS_LOG_ERR, "File must be provided when play");
			exit(1);
		}
		sprintf(record->fileName, "%s%s", GET_VALUE(&cfgsRecord, RECORD_FILE_HOME), file);
	}	
	record->app = RECORD_APP_MSQ;

	as_record_init(record);
	
	AS_MSG_LOG(AS_LOG_INFO, "waiting device offhook..." );
	int wrs = record_wait_offhook(record);
	if (wrs == ERR_UNABLE)
	{
		AS_MSG_LOG(AS_LOG_ERR,"request unable.");
		goto err;
	}
	else if (wrs == ERR_OVERTIME)
	{
		AS_MSG_LOG(AS_LOG_ERR,"time over! please offhook your device inside %d seconds", record->waitTime );
		goto err;
	}
	else if (wrs == ERR_REGIST)
	{
		AS_MSG_LOG(AS_LOG_ERR,"Offhook is ended by user" );
		goto err;
	}

	_update_config(record, &cfgsRecord);
	record_start_work(record);

err:
	record_clean_all(record);
	
	return 0;
}

