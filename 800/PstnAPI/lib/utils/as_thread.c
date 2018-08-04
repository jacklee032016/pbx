/*
 * $Log: as_thread.c,v $
 * Revision 1.1.1.1  2006/11/30 16:22:46  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_thread.c,v 1.1.1.1 2006/11/30 16:22:46 lizhijie Exp $
*/

#include <stdio.h>
#include "as_thread.h"

int as_thread_create(as_thread_t *thread)
{
	int res;
	
//	pthread_t threadid;
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	(thread->log)("Thread %s created\r\n", thread->name);
	
	res = pthread_create(&thread->id, &attr, thread->handler, thread);
	if( res)
	{
		printf("Thread %s create failed\r\n" , thread->name );
		return AS_FAIL;
	}

	
	return AS_OK;	
}

