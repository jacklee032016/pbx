#include "aslib.h"
#include "as_prog_taper.h"
#include "as_dev_ctl.h"
#include "as_pbx_cmd.h"
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <pthread.h>


char cfg[60][128];
static const char *filter = "\t\r\n";
static const char *dev = "/dev/astel/";
recorder_parameter_t local_param;

int get_conf(const char* in_conf_file, recorder_parameter_t *out_param)
{	
	FILE *fp = fopen(in_conf_file, "rb");
	if (fp == NULL)
	{
		//cpLog(LOG_ERR,"fp == NULL");
		return ERR_OPEN_C;
	}

	int index1 = 0, index2 = 0;
	memset(cfg, 0, sizeof(cfg));

	while (!feof(fp))
	{
		char linebuf[256] = {0}; 
		fgets(linebuf, sizeof(linebuf), fp);
		if (strlen(linebuf) < 10 || linebuf[0] == '#') continue;
		filter_c(linebuf, (char*)filter);

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
	parser(out_param);
	
	return ERR_SUCCESS;
}


void filter_c(char *source, char *filter)
{
	char *sv = source;
	int n = strlen(filter);
	int length = strlen(sv);
	int i,j;
	for (i = 0; i < length; ++i) 
		for (j = 0; j < n; ++j)
		{
			if (*(sv+i) == filter[j]) {
				erase(source,i,1);
			}
		}
}

void erase(char *buf, int pos, int nums)
{
	int length1 = strlen(buf);
	memcpy(buf+pos, buf+pos+nums, length1-nums-pos);
	int length2 = length1-nums;
	memset(buf+length2, 0, nums);
}

void parser(recorder_parameter_t *out_param)
{
	strcpy(out_param->device_type, (char*)cfg[2]); 
	out_param->device_fd = atoi((char*)cfg[5]);
	strcpy(out_param->file_path, (char*)cfg[8]); // voice_path
	out_param->time_lenght = atoi((char*)cfg[11]); // voice_length
	strcpy(out_param->starttip_file, (char*)cfg[14]);
	strcpy(out_param->endtip_file, (char*)cfg[17]);
	strcpy(out_param->action_type, (char*)cfg[20]);
	out_param->waite_time = atoi((char*)cfg[23]);

	printf("out_param->device_type = %s\n", out_param->device_type);
	printf("out_param->device_fd = %d\n", out_param->device_fd);
	printf("out_param->file_path = %s\n", out_param->file_path);
	printf("out_param->time_length = %d\n", out_param->time_lenght);
	printf("out_param->starttip_file = %s\n", out_param->starttip_file);
	printf("out_param->endtip_file = %s\n", out_param->endtip_file);
	printf("out_param->action_type = %s\n", out_param->action_type);
	printf("out_param->waite_time = %d\n", out_param->waite_time);

	memcpy(&local_param, out_param, sizeof(recorder_parameter_t));

}


int wait_offhook()
{
	//int result = ERR_UNABLE;
	if (strcmp(local_param.device_type, "PCM") == 0)
	{
		return wait_pcm_offhook();
	}
	else if (strcmp(local_param.device_type, "SLIC") == 0)
	{
		return wait_slic_offhook(local_param.file_path);
	}
	else return ERR_UNABLE;
	
}
int wait_pcm_offhook()
{
	int channel = recorder_send_cmd(PCM_RECORDER_QEQUEST, local_param.waite_time);
	if (channel == ERR_UNABLE)
		return channel;

	channel = ERR_UNABLE;
	channel = wait_pbx_response();
	as_cmd_uart_destroy();
		
	printf(" ===========>> channel = %d <<==========\n", channel);
	
	return channel;

}

int wait_slic_offhook()
{
	char devName[32] = {0};
	sprintf(devName, "%s%d", dev, local_param.device_fd);
	int fd = open(devName, O_RDWR);
	if (fd == -1)
	{
		printf("can't open %s for test waitoffhook\n", devName);
		return ERR_UNABLE;
	}

	struct timeval start, end;
	gettimeofday(&start, 0);
	int res;	
	
	do
      {
        	res = event_get(fd);
		gettimeofday(&end, 0);
		
      }while(res != AS_EVENT_RINGOFFHOOK && (end.tv_sec - start.tv_sec) < local_param.waite_time);

	close(fd);
	if (res != AS_EVENT_RINGOFFHOOK) return ERR_OVERTIME; //timeover
	return local_param.device_fd;
	
}

int event_get(int fd)
{
         int j = ERR_UNABLE;     
         if (ioctl(fd, AS_CTL_GETEVENT, &j) == ERR_UNABLE) 
                   return ERR_UNABLE;
         return j;
}

int wait_pbx_response()
{
	PBX_COMMAND *response;
	int chan_no = -1;

	printf("Waiting for pbx response ...\n");

	while (1)
	{

		response = as_cmd_from_uart();
		if(NULL == response)
		{
			fprintf(stderr, "No response are received\r\n");
			continue;
		}
		
		if(response->op != PCM_RECORDER_RESPONSE)
		{
			fprintf(stderr, "Not validate command for recorder : op is %x\r\n", response->op );
			free(response);
			continue;
		}
		else break;

	}
	
	chan_no = response->value[0];
	free(response);
	return chan_no;	
	
}


void clean_all()
{
	if (strcmp(local_param.device_type, "PCM") == 0)
		recorder_send_cmd(PCM_RECORDER_END,0);
}

int recorder_send_cmd(unsigned char op, int period)
{
	int res;
	PBX_COMMAND cmd;
	res = as_cmd_uart_init(period, 1);
	if(res < 0 )
	{
		fprintf(stderr, "UART port open failed\r\n");
		return ERR_UNABLE;
	}
	
	cmd.op = op;
	cmd.id = 1;
	cmd.value[0] = 0xFF;
	
	as_cmd_to_uart( &cmd);
	return ERR_SUCCESS;
}


