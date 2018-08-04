#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/ioctl.h>
#include "as_thread.h"
#include "assist_lib.h"
#include "as_dev_ctl.h"
#include "as_version.h"
/*  
 * test logic of FXO as callee  
*/
int fds;
int fdo;

#define MAX_PHONE_NUM		3

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

int main(int argc, char *argv[])
{
	int res;
	unsigned char buf[LENGTH];
	char phonenum[32];
	unsigned char phone[32];

	printf( AS_VERSION_INFO("Test callee progress"));
	
	memset(phone, 0, 32);
	if (argc < 3) 
	{
		fprintf(stderr, "Usage: callee <astel FXS device> <astel FXO device>\n");
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

	printf("FXO device as callee\r\nSTEP 1: reset FXO device\r\n");
	as_lib_onhook(fdo);

	as_lib_wait_offhook(fdo);
	
	
	printf("STEP 2: set the FXO device into OFFHOOK status\r\n");
	as_lib_offhook( fdo);
	printf("\tFXO device OFF_HOOK \r\n" );

	int hook=AS_RING;
	ioctl(fds,AS_CTL_HOOK,&hook);

	
	printf("STEP 3:Waiting phone offhook.....\r\n");
	as_lib_wait_offhook(fds);
	printf("STEP 4:communicating now.....\r\n");
	
	as_thread_create(&fxo2fxs_thread);
	
	while(1)
	{
		memset(phonenum, 0, 32);
		res = read(fds,  buf,  LENGTH);
		if(res != 160/*LENGTH*/)
				printf("read Length is not correct : %d\r\n" , res);
			
		res = write( fdo, buf , LENGTH);
		if(res != 160 /*LENGTH*/)
			printf("write Length is not correct : %d\r\n" , res);
	};

	printf("FXO_CALLEE: Test ended successfully!\n");
	
	close(fdo);
	close(fds);

	return 0;
}

