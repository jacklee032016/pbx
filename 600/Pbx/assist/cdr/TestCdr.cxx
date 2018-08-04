
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

const char* usage = "test_cdr fifo [-s]\neg: test_cdr /etc/sip/fifo.cdr -2";

int main(int argc, char *argv[])
{
	int seconds = 5;
	
	if (argc < 2)
	{	
		printf(usage);
		return 0;
	}
	else if (argc == 3)
	{
		seconds = argv[2][1] - 0x30;
		printf("frequence = %d\n", seconds);
	}
	
	printf("test waiting until someone open %s to read...\n", argv[1]);
	int op = open(argv[1], O_WRONLY);
	if (op == -1)
	{
		perror("open on fifo for read");
		return 0;
	}
	
	printf("open fifo %s successful\n", argv[1]);
	char buf[526] = {0};
	strcpy(buf, "Call-ID: c162d454406b01525fdd2c8c3bda4485@192.168.3.171,CALL_START,DIRECTION_OUT,From: Phone<sip:8000@192.168.3.171:5060;user=18999>,To: wuaiwu<sip:wuaiwu@192.168.3.124:5060>,0,0,0,101,340,0,0,,0,,0,0,0,0,0");
	//printf("Server start writing...\n");
	
	while (1)
	{
		if (write(op, buf, strlen(buf)) < 0)
		{
			perror("write to fifo");
		}
		
		sleep(seconds);
	}
	
}


