#if !defined(ASSIST_WIN32_VTHREAD_H)
#define ASSIST_WIN32_VTHREAD_H
/*
* $Id: vthread.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <thread.h>
#include <synch.h>


typedef thread_t            vthread_t;
typedef mutex_t             vmutex_t;
typedef cond_t              vcondition_t;


#define     vmutex_init(mutex) \
                mutex_init((mutex),USYNC_THREAD,0)
                

#define     vmutex_destroy(mutex) \
                mutex_destroy( (mutex) )
                

#define     vmutex_lock(mutex) \
                mutex_lock( (mutex) )


#define     vmutex_unlock(mutex) \
                mutex_unlock( (mutex) )
                

#define     vcond_init(cond) \
                cond_init((cond),USYNC_THREAD,0)
                

#define     vcond_destroy(cond) \
                cond_destroy((cond))
                

#define     vcond_wait(cond, mutex) \
                cond_wait((cond),(mutex))


#define     vcond_timedwait(cond, mutex, timeout) \
                cond_timedwait((cond),(mutex),(timestruc_t *)(timeout))


#define     vcond_signal(cond) \
                cond_signal((cond))


#define     vcond_broadcast(cond) \
                cond_broadcast((cond))
                

#endif // !defined(ASSIST_WIN32_VTHREAD_H)
