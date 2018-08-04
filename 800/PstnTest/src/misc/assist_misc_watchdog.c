#include "as_misc_dev.h"
#include "assist_lib.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
static long __dechex(long bcd)
{    
	long a2=0,a3=0,a4=0,a5=0;   
	long sum=0;     
      	a2=bcd>>24;   
   	a3=bcd>>16;  
   	a4=bcd>>8;  
   	a5=bcd;      
   	sum=sum+((a5>>4)*10+(a5&0x0f)*1);  
   	sum=sum+((a4>>4)*1000+(a4&0x0f)*100);   
   	sum=sum+((a3>>4)*100000+(a3&0x0f)*10000);  
   	sum=sum+((a2>>4)*10000000+(a2&0x0f)*1000000); 
     	return(sum);
} 

static int __as_watchdog_write_cmd(AS_WATCHDOG_COMMAND *cmd )
{
	char str[128];
	int res = 0;
	int fd;

	sprintf(str, "/proc/%s/%s",  AS_MISC_PROC_DIR_NAME , AS_MISC_PROC_WATCHDOG);
	fd = open(str, O_RDWR);
	if (fd < 0) 
	{
		fprintf(stderr, "Unable to open %s: %s\n", str, strerror(errno));
		exit(1);
	}

	res = write(fd, cmd, sizeof(AS_WATCHDOG_COMMAND) );
	if(res < 0)
	{
		fprintf(stderr, "Write %s failed : %s\n", str, strerror(errno));
		close(fd);
		return -1;
	}
	
	close(fd);
	return 0;
}

/* time for warm-reset, when both set to 0, it set to maxium: about 64 seconds */
int as_watchdog_enable(int seconds, int millseconds)
{
	long ns;
	AS_WATCHDOG_COMMAND cmd;

	cmd.type = AS_WATCHDOG_ENABLE;
	if(millseconds >= 1000)
	{
		seconds++;
		millseconds = 0;
	}
	
	if((seconds >= 64)||(seconds==0 && millseconds==0 ))
	{
		cmd.value = AS_IXP_WATCHDOG_DEFAULT_COUNT;
	}
	else
	{	
		ns = seconds*1000*1000 + millseconds *1000;		
		ns = ns*67;
		cmd.value = __dechex(ns);
	}

	return __as_watchdog_write_cmd(&cmd);
}

int as_watchdog_disable(void)
{
	AS_WATCHDOG_COMMAND cmd;

	cmd.type = AS_WATCHDOG_DISABLE;
	cmd.value = AS_IXP_WATCHDOG_DEFAULT_COUNT;
	
	return __as_watchdog_write_cmd(&cmd);
}

int as_watchdog_update(void)
{
	AS_WATCHDOG_COMMAND cmd;

	cmd.type = AS_WATCHDOG_UPDATE;
	cmd.value = AS_IXP_WATCHDOG_DEFAULT_COUNT;

	return __as_watchdog_write_cmd(&cmd);
}

