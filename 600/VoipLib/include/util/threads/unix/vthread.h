#if !defined(VOCAL_UNIX_VTHREAD_H)
#define VOCAL_UNIX_VTHREAD_H
/*
 * $Id: vthread.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
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
                

#endif // !defined(VOCAL_UNIXX_VTHREAD_H)
