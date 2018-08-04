#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "as_thread.h"
#include "as_fxs.h"


/* get an event of the device */
int as_switch_to_line(int fd)
{
	int res;
	int switchCmd = AS_SWITCH_2_LINE;

	res=ioctl(fd, AS_SWITCH_PHONE, &switchCmd);
	if(res<0)
	{
		printf("ioctl failed\n");
		return -1;
	}
#if 0
	printf("PHONE switch from FXS port to line\n");
#endif
	return 0;
}

int as_switch_to_fxs(int fd)
{
	int res;
	int switchCmd = AS_SWITCH_2_FXS;

	res=ioctl(fd, AS_SWITCH_PHONE, &switchCmd);
	if(res<0)
	{
		printf("ioctl failed\n");
		return -1;
	}
#if 0	
	printf("PHONE switch from Line to FXS\n");
#endif
	return 0;
}

#if 1
int main(int argc, char *argv[])
{
	int status;
	int fd;
	int res;
	if (argc < 2) 
	{
		fprintf(stderr, "Usage: <status '0'-PSTN or '1'-IP> \n");
		exit(1);
	}

	fd = open("/dev/astel/0", O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	status = atoi(argv[1]);

	if(status == 0)
	{
		res = as_switch_to_line(fd);
		printf("PHONE switch to PSTN\n");
	}
	else if(status == 1)
	{
		res = as_switch_to_fxs(fd);
		printf("PHONE switch to FXS\n");
	}
	else
		printf("ER:PHONE switch failure\n");

	
	return 0;

}

#endif


#if 0
int fds;
int fdo;

#define LENGTH   1024

void *fxo2fxs_thread_handler(void *data)
{
	char buffer[LENGTH];
	int length;

	printf("Thread running...\r\n");
	while(1)
	{
		length = read(fdo, buffer, LENGTH);

		length = write(fds, buffer, length);
	}		

	return NULL;
}

as_thread_t  fxo2fxs_thread =
{
	name	:	"FXO 2 FXS",
	handler	:	fxo2fxs_thread_handler,
	log		: 	printf,

	private	:	NULL
};

int main(int argc, char *argv[])
{
	int res;
	unsigned char buf[LENGTH];
	char phonenum[64];
//	int linear ;

	memset(phonenum, 0, 64);
	if (argc < 3) 
	{
		fprintf(stderr, "Usage: fxo_call <astel FXS device> <astel FXO device>\n");
		exit(1);
	}

	fds = open(argv[1], O_RDWR);
	if (fds < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}

	fdo = open(argv[2], O_RDWR);
	if (fdo < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[2], strerror(errno));
		exit(1);
	}

	printf("FXO device as caller\r\nSTEP 1: reset FXO device\r\n");
	as_lib_onhook(fdo);

	printf("STEP 2: Waiting OFFHOOK event in phone.....\r\n");
	as_lib_wait_offhook(fds);
	printf("\tPhone is  OFF_HOOK \r\n" );
	
	printf("STEP 3: Play dial tone in phone\r\n");
	as_tone_play_dial(fds);

	printf("STEP 4: receiving the out office number '0'.....\r\n\tDial can be timeout in 4s, Exit when phone ON_HOOK\r\n");
	while(1)
	{
		res = as_lib_hard_dmtf_caller_id(fds, phonenum, 1, 14000);
		if(res == AS_ERROR_TIMEOUT )
		{
			as_tone_play_congestion(fds);
			printf("Waiting ON_HOOK to exit\r\n");
			as_lib_wait_onhook(fds);
			exit(1);
		}
		else if(res == AS_ERROR_ONHOOK)
		{
			printf("\tPhone is ON_HOOK\r\n");
			exit(1);
		}
		else if( phonenum[0]=='1' )
		{
			as_switch_pstn(fds);
			while(1)
			{
				res = as_lib_event_get( fds);
				if(res == AS_EVENT_ONHOOK) 
				{
					as_switch_ip(fds);
					close(fdo);
					close(fds);
					return 0;
				}
			}	
		}
		else if( phonenum[0]=='0' )
		{
			break;
		}
		else
		{
			close(fdo);
			close(fds);
			return 0;
		}
	}
	
	printf("STEP 5: stop tone in FXS device\r\n");
	
	as_tone_play_stop(fds);
//	as_lib_flush_data(fdo);
//	as_lib_flush_data(fds);
	
	printf("STEP 6: offhook the FXO device\r\n");
	as_lib_offhook(fdo);

	/* wait FXO off-hook finish and overwrite the dtmf data in DMA buffer ,lizhijie,2006.01.16 */
	printf("STEP 7: Wait FXO off-hook finish\r\n");
	as_lib_wait_offhook(fdo);

	printf("STEP 8: connect the FXS device to the remote switch\r\n");
	as_thread_create(&fxo2fxs_thread);
	while(1)
	{
		res = read(fds, buf, LENGTH);
		res = write( fdo, buf , res );
		res = as_lib_event_get( fds);
		if(res == AS_EVENT_ONHOOK) 
		{
			break;
		}
	};
	
	close(fdo);
	close(fds);
	return 0;
}


#endif

