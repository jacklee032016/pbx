#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "assist_lib.h"
#include "as_version.h"

unsigned short as_fsk_crc(unsigned char *msg, int nBytes);
void as_fsk_crc_init(void);

int main(int argc, char *argv[])
{
	int j = 0;
	unsigned char	msg[128];
	unsigned short result;

	as_fsk_crc_init();

	j=0;
	j += sprintf(msg+j, "%c%c%c%c",  0x07, 0x10, 0x02, 0x40);
	j += sprintf(msg+j, "%c%c%c%c%c%c", 0x03, 0x04, 0x01, 0x4f, 0x10, 0x03);

	result = as_fsk_crc(msg, j);
	printf("0x%x(0x05) 0x%x(0xb4)\n", result>>8, result&0xff);
	
	return 0;
}

