/*
 * $Id: vthread-linux.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#ifndef __vxworks

#include "global.h"
#include <cassert>
#include "VThread.hxx"
#include "cpLog.h"

/*********************************************************************
 
VThread for Linux
 
*********************************************************************/

VThread::VThread()
    : myId(VTHREAD_ID_INVALID)
{
    memset(&myAttributes, 0, sizeof(myAttributes));
}

VThread::~VThread()
{
    if ( myId != VTHREAD_ID_INVALID )
    {
        // warn user but don't kill thread
        cpLog( LOG_WARNING, "active VThread:%d going out of scope", myId );
	assert(0); // this is probably a design error
    }
    myId = VTHREAD_ID_INVALID;
    pthread_attr_destroy(&myAttributes);
}

int
VThread::spawn( void *( *startFunc )( void * ),
                void *startArgs /*Default Arguments*/,
                unsigned long flags /*Default Arguments*/,
                unsigned long priority /*Default Arguments*/,
                unsigned long stack_size /* Default Arguments*/
              ) throw (VThreadException&)
{
    if ( myId != VTHREAD_ID_INVALID )
    {
        // return without spawning another thread
        cpLog( LOG_ERR, "VThread:%d is already active", myId );
        return -1;  // type of vthread_t is not int necessarily
    }


    struct sched_param priorityParams;
    int retVal = 0;

    retVal = pthread_attr_init(&myAttributes);
    assert (retVal == 0);

    // extract scheduling preference from flags and set scheduling
    // policy accordingly
    switch ( flags & VTHREAD_SCHED_MASK )
    {
        case VTHREAD_SCHED_FIFO:
        {
            #if !defined(WIN32)
            retVal = pthread_attr_setinheritsched( &myAttributes,
						   PTHREAD_EXPLICIT_SCHED );
	    assert ( retVal == 0 );

            retVal = pthread_attr_setschedpolicy( &myAttributes,
                                                  SCHED_FIFO );
            if ( retVal )
            {
                throw VThreadExceptionInvalidAttributes(
                    "cannot set schedule as VTHREAD_SCHED_FIFO",
                    __FILE__,
                    __LINE__,
                    retVal );
            }
            #else
            cpLog(LOG_ERR, "priorities not implemented");
            assert(0);
            #endif
        }
        break;

        case VTHREAD_SCHED_RR:
        {
            #if !defined(WIN32)
            retVal = pthread_attr_setinheritsched( &myAttributes,
						   PTHREAD_EXPLICIT_SCHED );
	    assert (retVal == 0 );
            retVal = pthread_attr_setschedpolicy( &myAttributes,
                                                  SCHED_RR );
            if ( retVal )
            {
                throw VThreadExceptionInvalidAttributes(
                    "cannot set schedule as VTHREAD_SCHED_RR",
                    __FILE__,
                    __LINE__,
                    retVal );
            }
            #else
                cpLog(LOG_ERR, "priorities not implemented");
                assert(0);
            #endif
        }
        break;

        case VTHREAD_SCHED_DEFAULT:
        default:
        {
            // PTHREAD_INHERIT_SCHED is not supported by Solaris
            // and is not required for Linux so it has been removed

            // inherit scheduling policy of parent as default
            //assert( pthread_attr_setinheritsched( &myAttributes,
            //                                      PTHREAD_INHERIT_SCHED )
            //    == 0 );
        }

        break;
    }

    // if anything expect default, set scheduling priority explicitly;
    // note that by default the priority of the parent thread is inherited
    if ( priority != VTHREAD_PRIORITY_DEFAULT )
    {
        // probably should improve to use relative values
        priorityParams.sched_priority = priority;
        retVal = pthread_attr_setschedparam( &myAttributes,
                                             &priorityParams );
        if ( retVal )
        {
            throw VThreadExceptionInvalidPriority(
                "cannot set priority",
                __FILE__,
                __LINE__,
                retVal );
        }
    }
#if defined (__SUNPRO_CC)
    pthread_attr_setscope(&myAttributes, PTHREAD_SCOPE_SYSTEM);
#endif

#if defined (__FreeBSD__)
    // Default FreeBSD thread stack size (64KB) is not sufficient for the
    // b2bua bloatware
    pthread_attr_setstacksize (&myAttributes, 1024*1024);
#endif

    // spawn the thread
    return ( pthread_create( &myId, &myAttributes, startFunc, startArgs ) );
}


int
VThread::join( void **status /*Default Arguments*/)
{
    int retVal = pthread_join( myId, status );
    myId = VTHREAD_ID_INVALID;
    return retVal;
}


int
VThread::getPriority() const
{
    struct sched_param priorityParams;

    int retVal = pthread_attr_getschedparam( &myAttributes, &priorityParams );

    assert( retVal == 0 );

    return priorityParams.sched_priority;
}


int
VThread::setPriority( int priority ) throw (VThreadExceptionInvalidPriority&)
{
    struct sched_param priorityParams;
    int policy = 0;
    int retVal = 0;

    retVal = pthread_attr_setschedparam( &myAttributes, &priorityParams );
    if ( retVal )
    {
        throw VThreadExceptionInvalidPriority(
            "cannot set priority",
            __FILE__,
            __LINE__,
            retVal );
    }
#ifndef WIN32
    retVal = pthread_attr_getschedpolicy( &myAttributes, &policy );
    if ( retVal )
    {
        throw VThreadExceptionInvalidPriority(
            "cannot set policy",
            __FILE__,
            __LINE__,
            retVal );
    }
#endif
    return ( pthread_setschedparam( myId, policy, &priorityParams ) );
}


const vthread_t*
VThread::getId() const
{
    return &myId;
}


const vthread_attr_t*
VThread::getAttributes() const
{
    return &myAttributes;
}


void
VThread::exit()
{
    if ( myId != VTHREAD_ID_INVALID )
    {
        pthread_cancel( myId );
        myId = VTHREAD_ID_INVALID;
    }
}


vthread_t
VThread::selfId()
{
    return pthread_self();
}

#if defined(WIN32)
int
sleep( int seconds )
{
	Sleep (seconds * 1000);
	return 0;
}

int
usleep( int useconds )
{
    Sleep(useconds / 1000);
	return 0;
}
#endif

#endif
// not __vxworks
