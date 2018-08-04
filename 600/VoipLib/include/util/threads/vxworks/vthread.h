#if !defined(VOCAL_VXWORKS_VTHREAD_H)
#define VOCAL_VXWORKS_VTHREAD_H
/*
 * $Id: vthread.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "VocalCommon.hxx"
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
Vocal::ReturnCode
vmutex_init(vmutex_t * mutex)
{
    *mutex = semMCreate(SEM_Q_FIFO);
    assert( *mutex != 0 );

    return ( Vocal::SUCCESS );
}
                

inline
Vocal::ReturnCode
vmutex_destroy(vmutex_t * mutex) 
{
    STATUS errorcode = semDelete(*mutex);
    assert(errorcode == OK);
    
    return ( Vocal::SUCCESS );
}


inline
Vocal::ReturnCode
vmutex_lock(vmutex_t * mutex)
{
    semTake(*mutex, WAIT_FOREVER);
    
    return ( Vocal::SUCCESS );
}


inline
Vocal::ReturnCode
vmutex_unlock(vmutex_t * mutex)
{
    semGive(*mutex);
    
    return ( Vocal::SUCCESS );
}
                

inline
Vocal::ReturnCode
vcond_init(vcond_t * cond)
{
    // Conditional variable is implemented as a binary semaphore
    // which is initialized to empty
    //
    *cond = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    assert( *cond != 0 );

    return ( Vocal::SUCCESS );
}
                

inline
Vocal::ReturnCode
vcond_destroy(vcond_t * cond)
{
    STATUS errorcode = semDelete(*cond);
    assert(errorcode == OK);

    return ( Vocal::SUCCESS );
}
                

inline
Vocal::ReturnCode
vcond_wait(vcond_t * cond, vmutex_t * mutex)
{
    Vocal::ReturnCode rc = SUCCESS;

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
Vocal::ReturnCode
vcond_timedwait(vcond_t * cond, vmutex_t * mutex, timespec * ts)
{
    Vocal::ReturnCode rc = SUCCESS;
    
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
Vocal::ReturnCode
vcond_signal(vcond_t * cond)
{
    return ( (Vocal::ReturnCode) semGive(*cond) );
}


inline
Vocal::ReturnCode
vcond_broadcast(vcond_t * cond)
{
    // TODO
    assert( 0 );
    return ( (Vocal::ReturnCode) semGive(*cond) );
}
                

#endif // !defined(VOCAL_VXWORKS_VTHREAD_H)
