#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#include "ask_isdn.h"
#include "ask_test.h"

#include "as_thread.h"
#include <sys/ioctl.h>


static void *B_rawdev_thread(void *data)
{
	int 	fd, rawFd;
	unsigned char buf[PLAY_SIZE];
	int x = 0;

	int len, ret;
#ifdef AUDIO_RECORD	
	int readLengtht;
	unsigned char audio[1024];
#endif

	static int length = 0;
	as_thread_t *thread = (as_thread_t *)data;
	devinfo_t *dev = (devinfo_t *)thread->private;

	printf("%s : running on B Channel %d\n", thread->name, dev->used_bchannel );
	assert(  (fd = open("greeting2.u"/*FileNameOut*/, O_RDONLY))>0 );

	rawFd = dev->bfd[dev->used_bchannel];
	ret = ioctl(dev->bfd[0],  AS_ISDN_CTL_ACTIVATE, &x);
	ret = ioctl(dev->bfd[1],  AS_ISDN_CTL_ACTIVATE, &x);

	while(1)
	{
	
		len = read(fd, buf , PLAY_SIZE);
		if (len<PLAY_SIZE )
		{
			printf("read file end,exit now %d: \"%s\"\n", errno, strerror(errno));
			close(fd);
			exit(1);
		}
		length += len;
		printf("read file  %d bytes\n", length );

		ret = write(rawFd, buf, len);
		if (ret < 0)
		{
			fprintf(stdout,"play write error %d %s, return %d\n", errno, strerror(errno), ret);
			pthread_exit(NULL);
		}
		else
			fprintf(stdout,"play write ret=%d\n", ret);

#ifdef AUDIO_RECORD	
		readLength = read(rawFd, audio, 1024);
		if(readLength < 0 )
		{
			fprintf(stdout,"read from raw device error %s\n", strerror(errno));
			pthread_exit(NULL);
		}
#endif				
	};

}

static as_thread_t  rawDevThread =
{
	name	:	"B-RAW",
	handler	:	B_rawdev_thread,
	log		: 	printf,
	
	private	:	NULL
};


int create_raw_thread( devinfo_t *dev)
{
	rawDevThread.private = (void *)dev;

	return as_thread_create( &rawDevThread);
}

