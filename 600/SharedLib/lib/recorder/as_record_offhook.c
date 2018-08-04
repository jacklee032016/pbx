/*
* $id$
*/

#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include "aslib.h"
#include "as_dev_ctl.h"

#if __WITH_NEW_STC__
#include "uart_cmd.h"
#endif

static int __recorder_send_pcm_cmd(unsigned char op )
{
	PBX_COMMAND cmd;
	
	cmd.op = op;
	cmd.id = 1;
	cmd.value[0] = 0xFF;
	
	as_cmd_apps_to_uart(&cmd, AS_MSG_TYPE_RECORD);
	return ERR_SUCCESS;
}

static int __record_wait_pcm_offhook(RECORD_T *record)
{
	PBX_COMMAND *response;
	int	result= ERR_UNABLE;
#if 0	
	/* make uartd service for RECORD */
	__recorder_send_pcm_cmd(SIP_CMD_STATE_CHANGED);
#endif	
#if __WITH_NEW_STC__
	__recorder_send_pcm_cmd(CMD_RECORD_REQUEST);
#else
	__recorder_send_pcm_cmd(PCM_RECORDER_QEQUEST);
#endif
	AS_MSG_LOG(AS_LOG_NOTICE, "Waiting MSG when waiting OffHook" );
	response = record_wait_msg();
	if(NULL == response)
	{
		AS_MSG_DEBUG(AS_LOG_ERR,"No response are received when waiting OffHook");
		result = ERR_UNABLE;
	}
#if __WITH_NEW_STC__
	else if(response->op == CMD_RECORD_REQUEST)
#else
	else if(response->op == PCM_RECORDER_RESPONSE)
#endif
	{
		record->devIndex = response->value[0];
		AS_MSG_LOG(AS_LOG_NOTICE, "Alert Voice is happened on Channel %d when waiting OffHook", record->devIndex );
		result = ERR_SUCCESS;
	}
	else if(response->op == PCM_RECORDER_CGI_TIMEOUT)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Waiting Off-Hook is Timeout" );/* this signal is send by CGI or alarm handler */
		result = ERR_OVERTIME;
	}
	else if(response->op == PCM_RECORDER_CGI_END)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Off-Hook is terminated by user" );/* this signal is send by CGI or alarm handler */
		result = ERR_REGIST;
	}
	
	if(response)
	{
		free(response);
	}
	
	return result;
}

static int __record_event_get(int fd)
{
         int j = ERR_UNABLE;     
         if (ioctl(fd, AS_CTL_GETEVENT, &j) == ERR_UNABLE) 
                   return ERR_UNABLE;
         return j;
}

static int __record_wait_slic_offhook(RECORD_T *record)
{
	char devName[32] = {0};
	
	sprintf(devName, PSTN_DEVICE_HOME_DIR"%d", record->devIndex );
	int fd = open(devName, O_RDWR);
	if (fd == -1)
	{
		AS_MSG_LOG(AS_LOG_ERR, "can't open %s for test waitoffhook", devName);
		return ERR_UNABLE;
	}

	struct timeval start, end;
	gettimeofday(&start, 0);
	int res;	
	
	do
	{
		res = __record_event_get(fd);
		gettimeofday(&end, 0);
	}while(res != AS_EVENT_RINGOFFHOOK && (end.tv_sec - start.tv_sec) < record->waitTime );

	close(fd);
	if (res != AS_EVENT_RINGOFFHOOK)
		return ERR_OVERTIME; //timeover
		
	return record->devIndex;
}


int record_wait_offhook(RECORD_T *record)
{
	int res = ERR_UNABLE;
	
	as_timer_start(record->waitTime);
	
	if (record->type == RECORD_TYPE_PCM)
	{
		res = __record_wait_pcm_offhook(record);
	}
	else if (record->type == RECORD_TYPE_SLIC)
	{
		res = __record_wait_slic_offhook(record);
	}

	as_timer_cancel();
	if(res==ERR_SUCCESS)
	{
		record->state = RECORD_STATE_WORKING;
	}
	
	return res;
}

void record_clean_all(RECORD_T *record)
{
	if (record->type == RECORD_TYPE_PCM )
	{
		AS_MSG_LOG(AS_LOG_INFO, "Clean all setup of Record/Play");
#if 0	
		__recorder_send_pbx_cmd(SIP_CMD_STATE_CHANGED);/*make UARTD service for SIP UA again */
#endif		
#if __WITH_NEW_STC__
		__recorder_send_pcm_cmd(CMD_RECORD_END );
#else
		__recorder_send_pcm_cmd(PCM_RECORDER_END );
#endif
	}
}

int	as_record_time_handler(void *data)
{
	RECORD_T  *record = (RECORD_T *)data;
	
	AS_MSG_LOG(AS_LOG_INFO, "Timeout event");
	if(!record)
	{
		AS_MSG_LOG(AS_LOG_ERR, "No RECORD found");
		return -1;
	}

	if(record->state == RECORD_STATE_WAIT_OFFHOOK)
	{
		AS_MSG_LOG(AS_LOG_INFO, "Timeout when Off-Hook");
		RECORD_EVENT_TIMEOUT();
	}
	else //if(record->state == RECORD_STATE_WORKING )
	{
		AS_MSG_LOG(AS_LOG_INFO, "Timeout when Working");
		record->state = RECORD_STATE_ENDING;
	}

	return ERR_SUCCESS;
}

int	record_msg_cmd(record_msg_t msgType)
{
	PBX_COMMAND cmd;
	
	if(msgType == RECORD_MSG_OFFHOOK_TIMEOUT)
		cmd.op = PCM_RECORDER_CGI_TIMEOUT;
	else if(msgType == RECORD_MSG_BEGIN)
		cmd.op = PCM_RECORDER_CGI_BEGIN;
	else if(msgType == RECORD_MSG_END)
		cmd.op = PCM_RECORDER_CGI_END;
	else
		return -1;
	
	cmd.id = 1;
	cmd.value[0] = 0xFF;
	AS_MSG_LOG(AS_LOG_INFO, "MSQ send to RECORD MQ");
	return as_cmd_uart_to_apps(&cmd, AS_MSG_TYPE_RECORD);
}

/* PBX_COMMAND must be freed by client */
PBX_COMMAND *record_wait_msg()
{
	PBX_COMMAND *reply;

	AS_MSG_LOG(AS_LOG_INFO, "Waiting for PBX Message Queue.....");

	while (1)
	{
		reply = as_cmd_record_from_uart();
		if(NULL == reply)
		{
			AS_MSG_DEBUG(AS_LOG_DEBUG,"Message Queue Reply is NULL");
			continue;
		}

		return reply;
	}
	
	return NULL;	
}


int	as_record_init(RECORD_T *record)
{
	as_msg_queue_init( 0);
	as_timer_register_handler( as_record_time_handler, record);

	return 0;
}	

