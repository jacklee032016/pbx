#include "as_taper.h"
#include "aslib.h"
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


static const char *dev = "/dev/astel/";
recorder_parameter_t parameter;
int overtime = 0;

//////////////////////////////////////////////////////////////////////////
void set_parameter(recorder_parameter_t *param)
{
	memset(&parameter, 0, sizeof(recorder_parameter_t));
	memcpy(&parameter, param, sizeof(recorder_parameter_t));
}

// 
int start_recorder()
{
	int regrst = regrst = register_signal();
	if (regrst < 0)return regrst;
	pthread_t runid;
	pthread_create(&runid, NULL, &recorder, NULL);
	pthread_join(runid, NULL);
	//while (1);
	return 0;
}

// 
int active_test()
{
	char devName[32] = {0};
	sprintf(devName, "%s%d",dev, parameter.device_fd);
	return open(devName, O_RDONLY);
}

//
int start_player(const char* fn)
{	
	char devName[32] = {0};
	sprintf(devName, "%s%d",dev, parameter.device_fd);
	
	int fd = open(devName, O_WRONLY);
	if (fd <= 0) {
		printf("open %s failure with %d\n",devName,fd);
		return ERR_OPEN_D;
	}

	int fr = open(fn, O_RDONLY);
	if (fr <= 0)
	{
		printf("%s:%d open %s failure with %d\n",__FILE__,__LINE__,fn, fr);
		close(fd);
		return ERR_OPEN_F;
	}
	
	char buffer[VOICE_RATE];
	while (1)
	{	
		
		int len = read(fr, buffer, VOICE_RATE);
		if (len == 0) break;
		write(fd, buffer, VOICE_RATE);
	}

	close(fr);
	close(fd);
	
	return ERR_SUCCESS;
}

void signal_handler(int signo)
{
	if (signo == SIGALRM)
	{
		overtime = 1;
		printf("Alarm : timer is overtime.\n");
	}
}


int register_signal()
{
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGALRM);
	act.sa_handler = signal_handler;
	return sigaction(SIGALRM, &act, NULL);
}

void* recorder(void *p)
{
	int result;
	FILE *voice_file = fopen(parameter.file_path, "wb");
	if (voice_file == NULL)
	{
		perror("open file_path");
		result = ERR_OPEN_F;
	}
	
	printf("---Please Waiting until the tip tone over...\n");
	start_player(parameter.starttip_file);

	char devName[32] = {0};
	sprintf(devName, "%s%d",dev, parameter.device_fd);
	
	int fd = open(devName, O_RDONLY);
	if (fd <= 0) 
	{
		printf("open %s failure with %d\n",devName,fd);
		result = ERR_OPEN_D;
	}
	
	alarm(parameter.time_lenght); // set a timer.
	char buffer[VOICE_RATE] = {0};
	
	while (!overtime)
	{
		int rlen = read(fd,buffer,VOICE_RATE);
		if (rlen <= 0)continue;
		int i;
		for (i = 0; i < rlen; ++i)
		{
			putc(buffer[i], voice_file);
		}
	}

	
	alarm(0);
	fclose(voice_file);
	close(fd);
	printf("--- recoder over! thanks for uses!\n");
	start_player(parameter.endtip_file);
	result = ERR_SUCCESS;

	return p; 
}

int start_work(int chno)
{
	int result = ERR_SUCCESS;
	parameter.device_fd = chno; // 
	if (strcmp(parameter.action_type, "recorder") == 0)
	{
		start_recorder();
	}
	else if (strcmp(parameter.action_type, "play") == 0)
	{
		start_player(parameter.file_path);
	}
	else result = ERR_UNABLE;

	return result;
}



