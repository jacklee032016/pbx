/*
 * $Id: vthread-vxworks.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#ifdef __vxworks

#include "global.h"
#include <cassert>
#include "VThread.hxx"
#include "cpLog.h"

/*********************************************************************
 
VThread for VxWorks
 
*********************************************************************/

VThread::VThread( void )
    : myId(VTHREAD_ID_INVALID)
{
}

VThread::~VThread( void )
{
    if ( taskIdVerify( myId ) == OK )
    {
        // warn user but don't kill thread
        cpDebug(LOG_DEBUG, "active VThread:%d going out of scope", myId );
    }
}

int
VThread::spawn( void *( *startFunc )( void* ),
                void *startArgs = 0,
                unsigned long flags = 0,
                unsigned long priority = VTHREAD_PRIORITY_DEFAULT,
                int stackSize = VTHREAD_STACK_SIZE_DEFAULT
              )
{
    if ( myId != VTHREAD_ID_INVALID )
    {
        // make sure VThread doesn't already have an active thread
        // associated with it
        if ( taskIdVerify( myId ) == OK )
        {
            // return without spawning another thread
            cpLog( LOG_ERR, "VThread:%d is already active", myId );
            return VTHREAD_ID_INVALID;
        }
    }

    int vxworksPriority = VTHREAD_PRIORITY_VXWORKS_DEFAULT;
    int vxworksOptions = 0;

    assert( flags == 0 );  // handling tbd

    if ( priority != VTHREAD_PRIORITY_DEFAULT )
    {
        vxworksPriority = priority;
    }

    myId = taskSpawn( 0, vxworksPriority, vxworksOptions,
                      stackSize, (FUNCPTR) startFunc,
                      (int) startArgs, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
    if ( myId <= 0 )
    {
        throw VExceptionMemory(
            "cannot allocate VThread",
            __FILE__,
            __LINE__,
            myId );
    }
    return 0;
}


int
VThread::join( void **status = 0 )
{
    while ( taskIdVerify( myId ) == OK )
    {
        sleep( 1 );
    }
    myId = VTHREAD_ID_INVALID;
    return 0;
}


int
VThread::getPriority( void )
{
    int thePriority = VTHREAD_PRIORITY_DEFAULT;

    if ( taskIdVerify( myId ) == 0 )
    {
        assert( taskPriorityGet( myId, &thePriority ) == OK );
    }
    else
    {
        cpLog( LOG_ERR, "VThread:%d is not active", myId );
    }
    return thePriority;
}


int
VThread::setPriority( int priority )
{
    return ( taskPrioritySet( myId, priority ) );
}


const vthread_t*
VThread::getId( void )
{
    return &myId;
}


const vthread_attr_t*
VThread::getAttributes( void )
{
    return &myAttributes;
}


void
VThread::exit( void )
{
    taskDelete( myId );
    myId = VTHREAD_ID_INVALID;
}


int
sleep( int seconds )
{
    return (int) (taskDelay( seconds * CLOCKS_PER_SEC ) );
}

int
usleep( int useconds )
{
    return (int) (taskDelay( ( useconds * CLOCKS_PER_SEC ) / 1000000 ) );
}

void vusleep(unsigned long usec)
{
    return ( usleep(usec) );
}

const vthread_t
VThread::selfId( void )
{
    return taskIdSelf();
}


#endif
//__vxworks
