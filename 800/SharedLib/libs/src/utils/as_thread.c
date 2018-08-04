/*
 * $Author: lizhijie $
 * $Log: as_thread.c,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_thread.c,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
*/
#include "as_thread.h"
#include <stdio.h>

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

	
	res = pthread_create(&thread->id, &attr, thread->handler, thread);
	if( res)
	{
		printf("Thread %s create failed\r\n" , thread->name );
		return AS_FAIL;
	}
	(thread->log)("Thread %s(%d) created\r\n", thread->name,thread->id);
	
	return AS_OK;	
}

int as_thread_join(as_thread_t *thread)
{
	int res;
	
//	printf("Thread %s joined\r\n", thread->name);
	
	res = pthread_join(thread->id,NULL);
	if(res)
	{
		printf("Thread %s join failed\r\n" , thread->name );
		return AS_FAIL;
	}
	return AS_OK;	
}

