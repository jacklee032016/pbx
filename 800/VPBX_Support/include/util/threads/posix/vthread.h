#if !defined(ASSIST_POSIX_VTHREAD_H)
#define ASSIST_POSIX_VTHREAD_H
/*
* $Id: vthread.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <pthread.h>


typedef pthread_t           vthread_t;
typedef pthread_mutex_t     vmutex_t;
typedef pthread_cond_t      vcondition_t;


#define     vmutex_init(mutex) \
                pthread_mutex_init((mutex),0)
                

#define     vmutex_destroy(mutex) \
                pthread_mutex_destroy((mutex))
                

#define     vmutex_lock(mutex) \
                pthread_mutex_lock((mutex))


#define     vmutex_unlock(mutex) \
                pthread_mutex_unlock((mutex))
                

#define     vcond_init(cond) \
                pthread_cond_init((cond),0)
                

#define     vcond_destroy(cond) \
                pthread_cond_destroy((cond))
                

#define     vcond_wait(cond, mutex) \
                pthread_cond_wait((cond),(mutex))


#define     vcond_timedwait(cond, mutex, timeout) \
                pthread_cond_timedwait((cond),(mutex),(timeout))


#define     vcond_signal(cond) \
                pthread_cond_signal((cond))


#define     vcond_broadcast(cond) \
                pthread_cond_broadcast((cond))
                



#endif // !defined(ASSIST_POSIX_VTHREAD_H)
