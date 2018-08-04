#include <stdio.h>
#include "as_thread.h"

int as_thread_create(as_thread_t *thread)
{
	int res;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	(thread->log)("Thread %s created\r\n", thread->name);
	
	res = pthread_create(&thread->id, &attr, thread->handler, thread);
	if( res)
	{
		printf("Thread %s create failed\r\n" , thread->name );
		return -1;
	}

	
	return thread->id;	
}

