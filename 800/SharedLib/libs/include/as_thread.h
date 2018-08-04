/*
 * $Author: lizhijie $
 * $Log: as_thread.h,v $
 * Revision 1.1.1.1  2006/11/30 16:24:17  lizhijie
 * AS800 SharedLib
 *
 * Revision 1.1.1.1  2006/02/21 09:38:37  wangwei
 * no message
 *
 * Revision 1.2  2005/07/08 01:36:16  lijie
 * no message
 *
 * Revision 1.1.1.1  2005/05/27 08:43:37  lizhijie
 * add shared library and programs depend on it
 *
 * $Revision: 1.1.1.1 $
 * $Id: as_thread.h,v 1.1.1.1 2006/11/30 16:24:17 lizhijie Exp $
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
	char name[AS_THREAD_NAME_LENGTH];
	pthread_t  id;
	int  priority;
	void *(*handler)(void *data);
	int (*log)(const char *format, ...);
	
	//void *private; 					/* private data for this thread : pointer to device which thread used */

};


typedef struct AS_THREAD as_thread_t;

#endif

