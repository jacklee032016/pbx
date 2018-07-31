#if !defined(ASSIST_VXWORKS_VTHREAD_H)
#define ASSIST_VXWORKS_VTHREAD_H
/*
* $Id: vthread.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "AssistCommon.hxx"
#include <taskLib.h>
#include <sysLib.h>
#include <semLib.h>
#include <cassert>


struct timespec
{
    long    ts_sec;
    long    ts_nsec;
};


typedef int                 vthread_t;
typedef SEM_ID              vmutex_t;
typedef SEM_ID              vcondition_t;


const unsigned long VTHREAD_PRIORITY_VXWORKS_DEFAULT = 150;


inline  
Assist::ReturnCode
vmutex_init(vmutex_t * mutex)
{
    *mutex = semMCreate(SEM_Q_FIFO);
    assert( *mutex != 0 );

    return ( Assist::SUCCESS );
}
                

inline
Assist::ReturnCode
vmutex_destroy(vmutex_t * mutex) 
{
    STATUS errorcode = semDelete(*mutex);
    assert(errorcode == OK);
    
    return ( Assist::SUCCESS );
}


inline
Assist::ReturnCode
vmutex_lock(vmutex_t * mutex)
{
    semTake(*mutex, WAIT_FOREVER);
    
    return ( Assist::SUCCESS );
}


inline
Assist::ReturnCode
vmutex_unlock(vmutex_t * mutex)
{
    semGive(*mutex);
    
    return ( Assist::SUCCESS );
}
                

inline
Assist::ReturnCode
vcond_init(vcond_t * cond)
{
    // Conditional variable is implemented as a binary semaphore
    // which is initialized to empty
    //
    *cond = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    assert( *cond != 0 );

    return ( Assist::SUCCESS );
}
                

inline
Assist::ReturnCode
vcond_destroy(vcond_t * cond)
{
    STATUS errorcode = semDelete(*cond);
    assert(errorcode == OK);

    return ( Assist::SUCCESS );
}
                

inline
Assist::ReturnCode
vcond_wait(vcond_t * cond, vmutex_t * mutex)
{
    Assist::ReturnCode rc = SUCCESS;

    int delayInTicks = WAIT_FOREVER;

    // Note, the call to taskLock() guarantees that the task will not
    // be scheduled out; however, it will still give up the cpu if it
    // blocks on a semaphore, thereby avoiding a deadlock.
    //
    bool taskLockCode = taskLock();
    assert( taskLockCode == OK );

    bool unlockCode = mutex->unlock();
    assert( unlockCode == 0 );

    rc = (int) semTake(*cond, delayInTicks);

    if ( rc != OK )
    {
        assert(errno == S_objLib_OBJ_TIMEOUT);
    }

    bool mutexLockCode = mutex->lock();
    assert( mutexLockCode == 0 );

    bool taskUnlockCode = taskUnlock();
    assert( taskUnlockCode == OK );

    return ( rc );
}


inline
Assist::ReturnCode
vcond_timedwait(vcond_t * cond, vmutex_t * mutex, timespec * ts)
{
    Assist::ReturnCode rc = SUCCESS;
    
    int relativeTimeInUs = ts.ts_sec * 1000000 + ts.ts_nsec / 1000;
    assert( relativeTimeInUs > 0 );

    int ticksPerSecond = sysClkRateGet();
    assert( ticksPerSecond > 0 );

    int delayInTicks = relativeTimeInUs * ( ticksPerSecond / 1000000 );
    
    // Note, the call to taskLock() guarantees that the task will not
    // be scheduled out; however, it will still give up the cpu if it
    // blocks on a semaphore, thereby avoiding a deadlock.
    //
    bool taskLockCode = taskLock();
    assert( taskLockCode == OK );

    bool unlockCode = mutex->unlock();
    assert( unlockCode == 0 );

    rc = (int) semTake(*cond, delayInTicks);

    if ( retval != OK )
    {
        assert(errno == S_objLib_OBJ_TIMEOUT);
    }

    bool mutexLockCode = mutex->lock();
    assert( mutexLockCode == 0 );

    bool taskUnlockCode = taskUnlock();
    assert( taskUnlockCode == OK );

    return ( rc );
}


inline
Assist::ReturnCode
vcond_signal(vcond_t * cond)
{
    return ( (Assist::ReturnCode) semGive(*cond) );
}


inline
Assist::ReturnCode
vcond_broadcast(vcond_t * cond)
{
    // TODO
    assert( 0 );
    return ( (Assist::ReturnCode) semGive(*cond) );
}
                

#endif // !defined(ASSIST_VXWORKS_VTHREAD_H)
