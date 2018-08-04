#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include "as_version.h"
#include "as_thread.h"
#include "assist_lib.h"
#include "as_dev_ctl.h"

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
		if(length != 160/*LENGTH*/)
			printf("Read length from FXO device is %d\r\n", length );

		length = write(fds, buffer, length);
		if(length != 160/*LENGTH*/)
			printf("write length to FXS device is %d\r\n", length );
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
int as_lib_hard_dmtf_caller_id( int fd, unsigned char *phone, int max , int timeout)
{
	int count, res,  pollres=0;  //res2,
	struct pollfd *pfds=NULL;
	int signal;
	int position = 0;
	int timeout_times = 0;
	int errno;

	memset(phone, 0, max);
	
	for(;;) 
	{
		if (pfds)
			free(pfds);

		pfds = (struct pollfd *)malloc( sizeof(struct pollfd));
		if (!pfds) 
		{
			printf( "Critical memory error.  Program dies.\n");
			exit(1);
		}
		memset(pfds, 0 ,  sizeof(struct pollfd));
		
		count = 0;

		pfds[count].fd = fd;
		pfds[count].events = POLLPRI;//|POLLIN;

		count = 1;

		res = poll(pfds, count,  timeout  );
		if (res < 0) 
		{
			if ((errno != EAGAIN) && (errno != EINTR))
				printf( "poll return %d: %s\n", res, strerror(errno));
			continue;
		}
		if ( res == 0)
		{
			printf("polling is timeout (%d seconds) in calling thread\r\n" , timeout );
			timeout_times++;
			if(timeout_times>=max)
				return -1;
			else
				continue;
		}
		
		pollres = pfds[0].revents;
		if (pollres & POLLPRI) 
		{
			res = as_lib_event_get( fd);
			if (res <0 ) 
			{
				printf( "Get event failed : %s!\n", strerror(errno));
			}
			if( res==AS_EVENT_DTMFDIGIT)
			{
		
				printf("Detect DTMF signal\r\n");

				if (ioctl(fd, AS_CTL_GET_DTMF_DETECT, &signal) == -1) 
				{
					printf("IOCTL error\r\n");
				}
				else
				{
		
					printf("DTMF signal is '%c' \r\n", (unsigned char)signal);

					phone[position] = (unsigned char)signal;
					position ++;
					if((position+timeout_times) >= max)
					{
			
						printf("receive calling number is '%s'\r\n", phone);

						return position;
					}	
				}
				
			}
			else if(res==AS_EVENT_ONHOOK)
			{
		
				printf("Receive ONHOOK event in calling thread\r\n");

				return -2; /* ONHOOK when receive caller ID */
			}
			else
				printf("Detect signal %d\r\n", res);
		
				printf("Detect signal %d\r\n", res);
			
		}		
	}
	return -1;
}


int main(int argc, char *argv[])
{
	int res;
	unsigned char buf[LENGTH];
	char phonenum[64];
	char dialstring[128];

	printf( AS_VERSION_INFO("Test caller progress"));

	memset(phonenum, 0, sizeof(phonenum));
	memset(dialstring,0,sizeof(dialstring));
	if (argc < 3) 
	{
		fprintf(stderr, "Usage: fxo_call <astel FXS device> <astel FXO device> [call id]\n");
		return 0;
	}

	fds = open(argv[1], O_RDWR);
	if (fds < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		return 0;
	}

	fdo = open(argv[2], O_RDWR);
	if (fdo < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[2], strerror(errno));
		return 0;
	}
	
	printf("FXO device as caller\r\nSTEP 1: reset FXO device\r\n");
	as_lib_onhook(fdo);

	printf("STEP 2: Waiting OFFHOOK event in phone.....\r\n");
	as_lib_wait_offhook(fds);
	printf("\tPhone is  OFF_HOOK \r\n" );

	if(argc==4)
	{
		strcpy(dialstring,argv[3]);
		as_dsp_t *dsp = as_dsp_init_no_cfg(U_LAW_CODE,PSTN_STANDARD_JAPAN, 20);
		as_lib_offhook(fdo);
		usleep(400*1000);	/* 400 ms */
		as_dsp_play_dtmf( dsp, fdo, dialstring);
	}
	else
	{
		printf("STEP 3: Play dial tone in phone\r\n");
		as_tone_play_dial(fds);

		printf("STEP 4: receiving the out office number '0'.....\r\n\tDial can be timeout in 4s, Exit when phone ON_HOOK\r\n");
		while(1)
		{
			res = as_lib_hard_dmtf_caller_id(fds, phonenum, 1, 14000);
			if(res == -1 )
			{
				as_tone_play_congestion(fds);
				printf("Waiting ON_HOOK to exit\r\n");
				as_lib_wait_onhook(fds);
				exit(1);
			}
			else if(res == -2)
			{
				printf("\tPhone is ON_HOOK\r\n");
				exit(1);
			}
			else if( phonenum[0]=='0' )
			{
//				sleep(1);
				break;
			}	
		}
	
		printf("STEP 5: stop tone in FXS device\r\n");
	
		as_tone_play_stop(fds);
		
		printf("STEP 6: offhook the FXO device\r\n");
		as_lib_offhook(fdo);
	}
	printf("STEP 7: connect the FXS device to the remote switch\r\n");
	as_thread_create(&fxo2fxs_thread);
	while(1)
	{
		res = read(fds, buf, LENGTH);
		if(res != 160/*LENGTH*/)
				printf("read Length is not correct : %d\r\n" , res);
		res = write( fdo, buf , res );
		if(res !=160/* LENGTH*/)
				printf("write Length is not correct : %d\r\n" , res);
	};

 	close(fdo);
	close(fds);
	return 0;
}

