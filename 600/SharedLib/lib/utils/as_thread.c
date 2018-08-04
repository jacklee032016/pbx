/*
 * $Id: as_thread.c,v 1.6 2007/05/27 08:37:20 lizhijie Exp $
*/
#include "as_thread.h"
#include <stdio.h>

#include "aslib.h"

int as_thread_create(as_thread_t *thread )
{
	int res;
//	pthread_t threadid;
	
	pthread_attr_t attr;

	struct sched_param scheduling_value; 
	scheduling_value.sched_priority = thread->priority; 

	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setschedparam(&attr,&scheduling_value);

	
//	res = pthread_create(&thread->id, &attr, thread->handler, thread);
	res = pthread_create(&thread->id, &attr, thread->handler, thread->priv);
	if( res)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Thread %s create failed" , thread->name );
		return AS_FAIL;
	}
	AS_MSG_DEBUG(AS_LOG_INFO, "Thread %s(%d) created", thread->name,thread->id);
	
	return AS_OK;	
}

int as_thread_join(as_thread_t *thread)
{
	int res;
	
//	printf("Thread %s joined\r\n", thread->name);
	
	res = pthread_join(thread->id,NULL);
	if(res)
	{
		AS_MSG_LOG(AS_LOG_ERR, "Thread %s join failed" , thread->name );
		return AS_FAIL;
	}
	return AS_OK;	
}

