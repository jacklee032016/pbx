

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "assist_lib.h"
#include "as_thread.h"


int fda;
int fdb;
#define LENGTH   1024
char buffer[LENGTH];
int length;
	
int __event_get(int fd)
{
	// Avoid the silly zt_getevent which ignores a bunch of events 
	int j =-1;
	
	if (ioctl(fd, AS_CTL_GETEVENT, &j) == -1) 
		return -1;
	return j;
}

void wait_off_hook(int fd)
{
	int res;
	
	do
	{
		res = __event_get( fd);
	}while(res != AS_EVENT_RINGOFFHOOK );
	printf("RING OFFHOOK : %d\r\n" , res);
}



/* set hook switch state */
int as_lib_sethook(int fd , int hookstate )
{
	int	i;

	if (fd<0) /* if NULL arg */
	{
		errno= EINVAL;  /* set to invalid arg */
		return(-1);  /* return with error */
	}
	i = AS_ONHOOK;
	if (hookstate) 
		i = AS_OFFHOOK;
	return(ioctl(fd,AS_CTL_HOOK,&i) );
}


int fxo_try_dial(int fd,char *digitstring)
{
	int res =0;
	
	printf("\nafter ring...\n");

	res = as_lib_onhook(fd);
	printf("ONHOOK = %d\n",res); 

	int hook = AS_ONHOOK;
	hook = AS_RING;
	printf("fd is %d\n",fd);
	res = ioctl(fd, AS_CTL_HOOK, &hook);
	if(res<0)
	{
		printf("error in HOOK IOCTL : %s\r\n", strerror(errno) );
		exit(1);
	}
	
	usleep(350000);     
	
	res = as_lib_offhook(fd);
	printf("OFFHOOK = %d\n",res);
	
	res = as_lib_dial(fd,  digitstring, 50);
	printf("DIAL = %d\n",res);	  
	if(res==-1)	  
	{
	      		printf("\nzap_dial fail\n");
	}
	      
	else
		printf("dial ok\n");

	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
	int res;

#if 1
	if (argc < 3) 
	{
		fprintf(stderr, "Usage: file_test <astel device> <phone NO> \n");
		exit(1);
	}

	fd = open(argv[1], O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", argv[1], strerror(errno));
		exit(1);
	}
#endif

	char * digitstring;
	digitstring = argv[2];
	printf("caller NO is '%d' \n",atoi(argv[2]));

	res = fxo_try_dial(fd,digitstring);

	sleep(10);
		
	res = as_lib_onhook(fd);

	return 0;
}
		  

