/*
 * $Id: as_thread.h,v 1.1.1.1 2006/11/30 16:21:59 lizhijie Exp $
 * 
*/
#ifndef __AS_THREAD_H__
#define __AS_THREAD_H__

#include "pthread.h"

//void *(run)(void *data);

//typedef void*(*run)(void *data)  as_thread_handler;
#define AS_MAX_NAME_LENGTH 64

struct AS_THREAD
{
	pthread_t  id;
	char name[AS_MAX_NAME_LENGTH];
	void*(*handler)(void *data);
	int (*log)(const char *format, ...);
	
	void *private; 					/* private data for this thread : pointer to device which thread used */
};

typedef struct AS_THREAD as_thread_t;

int as_thread_create(as_thread_t *thread);

#endif

