/*
* $Id: VThreadTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include <signal.h>
#include <sys/types.h>
#include <typeinfo>
#include <unistd.h>
#include <stdio.h>
#include <cassert>
#include "VTime.hxx"
#include "VThread.hxx"
#include "VMutex.h"
#include "VCondition.h"
#include "cpLog.h"
#include "VThreadException.hxx"
#include <iostream>

VMutex *theMutex;
VCondition *theCondition;

/// displays 'name' strlen( name ) times
void *
display( void* name )
{
    static int first = 1;
    int length = strlen( ( char* ) name );

    cerr << "see if this works" << endl;
    theMutex->lock();
    if ( first )
    {
        first = 0;
        assert( theCondition->wait( theMutex ) == 0 );
        while ( length-- > 0 )
        {
            cpLog( LOG_ERR, "%s\n", ( char* ) name );
            sleep( 1 );
        }
    }
    else
    {
        while ( length-- > 0 )
        {
            cpDebug(LOG_DEBUG, "%s\n", ( char* ) name );
            sleep( 1 );
        }
        assert( theCondition->signal() == 0 );
    }
    theMutex->unlock();
    return name;
}


void*
startThreads( void* name )
{
    // setup logfile
    cpLogSetLabel( "VThreadTest" );
    cpLogSetPriority( LOG_DEBUG );

    VThread* thread1;
    thread1 = new VThread();
    VThread thread2;

    char arg1[] = "first";
    char arg2[] = "second";

    theMutex = new VMutex;
    theCondition = new VCondition;

    // make sure cpLog() functionality is present
    cpDebug(LOG_DEBUG, "Is cpLog() working???" );

    try
    {
        thread1->spawn( display, ( void* ) arg1 );
        // this should result in a cpLog warning
        //thread1->spawn( display, ( void* ) arg1 );
    }

    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpDebug(LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }

    try
    {
        // for linux this should throw an exception which gets caught
        //thread2.spawn(display, ( void * ) arg2, 0, 175 );
        thread2.spawn(display, ( void * ) arg2 );
    }
    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpDebug(LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }
    catch ( VThreadExceptionInvalidPriority& ex )
    {
        cpDebug(LOG_DEBUG, "handle VThreadExceptionInvalidPriority" );
        thread2.spawn( display, ( void * ) arg2 );
    }

    printf( "thread1 priority: %d\n", thread1->getPriority() );
    printf( "thread2 priority: %d\n", thread2.getPriority() );
#ifdef __vxworks
    // include this if you want to change the order of execution
    //thread2.setPriority( 75 );
    //printf("thread2 new priority: %d\n", thread2.getPriority() );
#endif // __vxworks

    //sleep(10);
    //thread1->exit();
    //thread2.exit();
    thread1->join();
    printf("joined with thread1\n" );
    thread2.join();
    printf("joined with thread2\n" );

    delete theMutex; theMutex = 0;
    delete theCondition; theCondition = 0;
    delete thread1; thread1 = 0;
    return 0;
} // end main()


int
main( int argc, char* argv[] )
{
    VThread* mainThread;
    mainThread = new VThread();

    try
    {
        mainThread->spawn( startThreads );
    }
    catch ( VThreadExceptionInvalidAttributes& ex )
    {
        cpDebug(LOG_DEBUG, "handle VThreadExceptionInvalidAttributes" );
    }

    mainThread->join();
    delete mainThread; mainThread = 0;
    return 0;
} // end main()
