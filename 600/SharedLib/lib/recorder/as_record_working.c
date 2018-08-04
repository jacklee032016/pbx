/*
* $Id: as_record_working.c,v 1.8 2007/06/19 09:31:51 lizhijie Exp $
*/

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include "aslib.h"
#include "as_record.h"

#if __WITH_NEW_STC__
#include "uart_cmd.h"
#endif

record_state_t		recordState = RECORD_STATE_WAIT_OFFHOOK;

static int _record_start_player(RECORD_T *record, char *voiceFile)
{	
	char buffer[VOICE_RATE];
	char devName[32] = {0};
	int	fr, fd;
	sprintf(devName, PSTN_DEVICE_HOME_DIR"%d", record->devIndex );
	
	fd = open(devName, O_WRONLY);
	if (fd < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Open %s failure, %s",devName, strerror(errno) );
		return ERR_OPEN_D;
	}

	fr = open(voiceFile, O_RDONLY);
	if (fr < 0)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Open %s failure, %s", voiceFile, strerror(errno) );
		close(fd);
		return ERR_OPEN_F;
	}
	
	while (record->state != RECORD_STATE_ENDING)
	{	
		int len = read(fr, buffer, VOICE_RATE);
		if (len == 0)
			break;
		write(fd, buffer, VOICE_RATE);
	}

	close(fr);
	close(fd);
	
	AS_MSG_LOG(AS_LOG_INFO, "End of Play %s", voiceFile);
	return ERR_SUCCESS;
}

static void *__record_playing_thread(void *p)
{
	RECORD_T *record = (RECORD_T *)p;
	
	_record_start_player(record, record->fileName );

	RECORD_EVENT_WORKS_END();
	return p; 
}

static void *__record_recording_thread(void *p)
{
	FILE *voice_file;
	char devName[32];
	char buffer[VOICE_RATE];
	RECORD_T *record = (RECORD_T *)p;
	
	if(!record)
	{
		exit(1);
	}	

	AS_MSG_LOG(AS_LOG_INFO, "Record Thread : record is 0x%x", record);
	if(record->app == RECORD_APP_STANDALONE )
	{
		AS_MSG_LOG(AS_LOG_INFO, "---Please Waiting until the TIP Tone over...");
		_record_start_player(record, ALERT_START_VOICE);
	}
	else
	{
		int i = record->state;
		AS_MSG_LOG(AS_LOG_INFO, "Recording thread waiting until the State(%d) is changed...", i);
		while(1)
		{
			if(i != record->state)
			{
				AS_MSG_LOG(AS_LOG_INFO, "Recording thread State from %d to %d" ,i, record->state );
			}
			if(record->state == RECORD_STATE_RECORDING || record->state == RECORD_STATE_ENDING )//||
//				recordState == RECORD_STATE_RECORDING || recordState == RECORD_STATE_ENDING)
//			if(recordState == RECORD_STATE_RECORDING || recordState == RECORD_STATE_ENDING)
				
			{
				AS_MSG_LOG(AS_LOG_INFO, "Recording thread enter into %s(from %d to %d)", (record->state == RECORD_STATE_ENDING)?"END":"RECORD",i, record->state );
				break;
			}
			
			usleep(1000);
		};
	}

	if(record->state == RECORD_STATE_ENDING)
//	if(recordState == RECORD_STATE_ENDING)
	{
		AS_MSG_LOG(AS_LOG_INFO, "Recording thread has enter into END state, so abort record");
		return p;
	}
	if(record->app == RECORD_APP_MSQ)
	{
		AS_MSG_LOG(AS_LOG_INFO, "Beep after MSG is RXed");
//		as_lib_dial(fd, "9", 1500);
		usleep(1200);
		_record_start_player(record, ALERT_START_VOICE);
	}

	voice_file = fopen(record->fileName, "wb");
	if (voice_file == NULL)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Open file %s failed : %s", record->fileName, strerror(errno) );
	}
	
	sprintf(devName, PSTN_DEVICE_HOME_DIR"%d", record->devIndex);
	int fd = open(devName, O_RDONLY);
	if (fd <= 0) 
	{
		AS_MSG_LOG(AS_LOG_ERR, "Open %s failure, %s", devName, strerror(errno) );
		fclose(voice_file);
	}

	
	as_timer_start(record->timeLength ); // set a timer.
	
	while (record->state != RECORD_STATE_ENDING )
//	while (recordState != RECORD_STATE_ENDING )
	{
		int rlen = read(fd, buffer, VOICE_RATE);
		if (rlen <= 0)
			continue;
		int i;
		for (i = 0; i < rlen; ++i)
		{
			putc(buffer[i], voice_file);
		}
	}

	as_timer_cancel();
	
	
	fclose(voice_file);
	close(fd);
	AS_MSG_LOG(AS_LOG_INFO, "Recording thread Recording Alert Voice Ended! Thanks!");
//	if(record->app == RECORD_APP_STANDALONE )
	{
		record->state = RECORD_STATE_WORKING;
		_record_start_player(record, ALERT_END_VOICE);
		record->state = RECORD_STATE_ENDING;
	}
	
	if(record->state != RECORD_STATE_ENDING)
//	if(recordState != RECORD_STATE_ENDING)
	{
		RECORD_EVENT_WORKS_END();
	}

	return p; 
}

// 
static int _record_start_recorder(RECORD_T *record)
{
	int result = ERR_UNABLE;
	pthread_t runid;

	recordState = RECORD_STATE_WORKING;
	pthread_create(&runid, NULL, &__record_recording_thread, record);
	
	AS_MSG_LOG(AS_LOG_INFO, "Main Thread : record is 0x%x", record);

	if(record->app == RECORD_APP_MSQ)
	{/* can be timeout or begin by CGI */
		while(1)
		{
			PBX_COMMAND *reply = record_wait_msg();
			if(NULL == reply)
			{
				AS_MSG_DEBUG(AS_LOG_ERR,"No response are received");
			}
			else if(reply->op == PCM_RECORDER_CGI_END || reply->op == PCM_RECORDER_CGI_TIMEOUT)
			{/* can be timeout(no click by user) or begin by CGI */
				AS_MSG_LOG(AS_LOG_NOTICE, "End record voice on Channel %d, enter into END state", record->devIndex );
				record->state = RECORD_STATE_ENDING;
				recordState = RECORD_STATE_ENDING;

				free(reply);
				break;
			}
			else if(reply->op == PCM_RECORDER_CGI_BEGIN)
			{/* Begin rxed */
				AS_MSG_LOG(AS_LOG_NOTICE, "Record Voice on Channel %d MSG is RXed, enter into RECORD state", record->devIndex );
				record->state = RECORD_STATE_RECORDING;
				recordState = RECORD_STATE_RECORDING;
			}
			else
			{
				AS_MSG_LOG(AS_LOG_NOTICE, "Unknown MSG(op=0x%x) is RXed when recording", reply->op );
			}

			AS_MSG_LOG(AS_LOG_INFO,"record(0x%x) state is %d", record, record->state);
			if(reply)
			{
				free(reply);
				reply = NULL;
			}	
		}
	}
	
	pthread_join(runid, NULL);
	//while (1);
	return result;
}


int record_start_work(RECORD_T *record)
{
	int result = ERR_SUCCESS;

	if (record->action == RECORD_ACTION_RECORD )
	{
		_record_start_recorder(record );
		AS_MSG_DEBUG(AS_LOG_ERR,"Recording Ended");
		
	}
	else if (record->action == RECORD_ACTION_PLAY)
	{
		pthread_t runid;
		pthread_create(&runid, NULL, &__record_playing_thread, record);
		
		if(record->app == RECORD_APP_MSQ)
		{
			/* can be timeout(no click by user) or begin by CGI */
			PBX_COMMAND *reply = record_wait_msg();
			if(NULL == reply)
			{
				AS_MSG_DEBUG(AS_LOG_ERR,"No response are received");
			}
			else if(reply->op == PCM_RECORDER_CGI_END||reply->op == PCM_RECORDER_CGI_TIMEOUT)
			{
				free(reply);
				AS_MSG_LOG(AS_LOG_NOTICE, "RX End Alert Voice Playing MSG on Channel %d", record->devIndex );
				record->state = RECORD_STATE_ENDING;
			}
			else
				result = ERR_UNABLE;
		}
		
		pthread_join(runid, NULL);
	}

	return result;
}

