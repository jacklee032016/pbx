/*
 * $Id: Condition.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "Condition.hxx"
#include "Mutex.hxx"
#include "TimeVal.hxx"
#include "VocalCommon.hxx"
#include <cassert>


using Vocal::Threads::Condition;
using Vocal::Threads::Mutex;
using Vocal::TimeAndDate::TimeVal;
using Vocal::ReturnCode;
using Vocal::SUCCESS;


Condition::Condition()
{
    ReturnCode  rc = vcond_init(&myId);
    assert( rc == SUCCESS );
}


Condition::~Condition ()
{
    ReturnCode  rc = vcond_destroy(&myId);
    assert( rc == SUCCESS );
}


int
Condition::wait (Mutex* mutex, int relativeTimeInMs)
{
    int utime = relativeTimeInMs < 1
                    ? relativeTimeInMs
                    : relativeTimeInMs * 1000;
    
    return ( uwait(mutex, utime) );
}


int
Condition::uwait (Mutex* mutex, int relativeTimeInUs)
{
    // SunOS 5.X (according to the man page for pthread_cond_timedwait)
    // can have a maximum value for the seconds portion of the abstime
    // of the current time + 100,000,000. The nsec can have a maximum
    // value of 1,000,000,000. You get EINVAL otherwise.
    //
    // Note that the relative time is in milliseconds, meaning
    // that even if we use MAXINT (assuming 32 bit ints), then
    // we get a number well below MAXINT (by a couple orders of
    // magnitude) for the seconds portion. The slightly challenging
    // part is making sure the nsec is less than 1,000,000,000.
    //
    if ( relativeTimeInUs < 0 )
    {
        return ( vcond_wait(&myId, mutex->getId()) );
    }

    // Relative time is specified, so create the absolute time of expiration.
    //
    TimeVal expiresTV;
    expiresTV.now();
    
    timeval     relativeTime;
    relativeTime.tv_sec = 0;
    relativeTime.tv_usec = relativeTimeInUs;

    expiresTV += relativeTime;

    timespec	expiresTS;

    expiresTS.tv_sec = expiresTV.tv_sec;
    expiresTS.tv_nsec = expiresTV.tv_usec * 1000;

    assert( expiresTV.tv_usec < 1000000000L );

    return ( vcond_timedwait(&myId, mutex->getId(), &expiresTS) );
}


int
Condition::signal ()
{
    return ( vcond_signal(&myId) );
}


int
Condition::broadcast()
{
    return ( vcond_broadcast(&myId) );
}


const vcondition_t*
Condition::getId () const
{
    return ( &myId );
}


void vusleep(unsigned long usec)
{
    Mutex      mutex;
    Condition  cond;
    
    mutex.lock();
    cond.uwait(&mutex, usec);
    mutex.unlock();
}
