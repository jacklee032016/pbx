/*
 * $Author: lizhijie $
 * $Id: as_thread.h,v 1.5 2007/03/27 18:26:47 lizhijie Exp $
*/
#ifndef __AS_THREAD_H__
#define __AS_THREAD_H__

#include "pthread.h"

#include "as_global.h"

#define AS_THREAD_NAME_LENGTH  		64
//void *(run)(void *data);

//typedef void*(*run)(void *data)  as_thread_handler;

struct AS_THREAD
{
	char *name;//[AS_THREAD_NAME_LENGTH];
	pthread_t  id;
	int  priority;
	void *(*handler)(void *data);
	int (*log)(const char *format, ...);
	
	void *priv; 					/* private data for this thread : pointer to device which thread used */

};


typedef struct AS_THREAD as_thread_t;

#endif

