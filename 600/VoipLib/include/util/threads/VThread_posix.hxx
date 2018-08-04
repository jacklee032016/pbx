#ifndef VTHREAD_POSIX_HXX_
#define VTHREAD_POSIX_HXX_
/*
 * $Id: VThread_posix.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


// posix stuff for VThread

#ifndef __vxworks

#include <pthread.h>

#ifndef WIN32
typedef pthread_t vthread_t;
typedef pthread_mutex_t vmutex_t;
typedef pthread_attr_t vthread_attr_t;
typedef pthread_cond_t vcondition_t;

#else // WIN32
/**
 ** Suspend execution of the calling task for the specified number 
 ** of seconds
 */
int 
sleep( int seconds );

/**
 ** Suspend execution of the calling task for the specified number 
 ** of micro seconds
 */
int 
usleep( int useconds );

#include <pthread.h>
// VThread types for WIN32 
typedef pthread_t  vthread_t;
typedef pthread_mutex_t vmutex_t;
typedef pthread_attr_t vthread_attr_t;    
typedef pthread_cond_t  vcondition_t;
// VThread constants for Win32


#endif //!WIN32
#endif // !__vxworks


#endif
