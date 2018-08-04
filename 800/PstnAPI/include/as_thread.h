/*
 * $Log: as_thread.h,v $
 * Revision 1.1.1.1  2006/11/30 16:22:45  lizhijie
 * AS800 PstnAPI
 *
 * Revision 1.1.1.1  2006/03/10 09:31:43  lizhijie
 * PSTN APIs library for Gatewat project
 *
 * $Id: as_thread.h,v 1.1.1.1 2006/11/30 16:22:45 lizhijie Exp $
*/

#ifndef __AS_THREAD_H__
#define __AS_THREAD_H__

#include "pthread.h"

#include "assist_lib.h"
//void *(run)(void *data);

//typedef void*(*run)(void *data)  as_thread_handler;

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

