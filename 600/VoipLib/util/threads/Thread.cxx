/*
 * $Id: Thread.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "Thread.hxx"
#include "Runnable.hxx"
#include "SignalHandler.hxx"
#include "Application.hxx"
#include "Lock.hxx"
#include <cstdio>
#include <cassert>

using Vocal::Threads::Thread;
using Vocal::Threads::Mutex;
using Vocal::Threads::Lock;
using Vocal::Signals::SignalHandler;
using Vocal::Runnable;
using Vocal::ReturnCode;


Thread::ThreadMap   		*   Thread::myThreadMap = 0;
Mutex		    	    	    Thread::myThreadMapMutex;
Thread          	    	*   Thread::myMainThread = 0;


extern "C"
{

static void *  
localThreadMain(void * data)
{
    return ( Thread::threadMain(data) );
}

} // extern "C"


void	    	    
Thread::init()
{
    Lock    lock(myThreadMapMutex);
    
    if ( !myThreadMap )
    {
        myThreadMap = new map<vthread_t, Thread *>;
        myMainThread = new Thread;

    	vthread_t   mainThreadId = VThread::selfId();
	
	(*myThreadMap)[mainThreadId] = myMainThread;
    }
}


void	    	    
Thread::uninit()
{
    Lock    lock(myThreadMapMutex);

    delete myThreadMap;
    myThreadMap = 0;

    delete myMainThread;
    myMainThread = 0;
}


Thread *
Thread::self()
{
    init();
    
    Lock    lock(myThreadMapMutex);

    vthread_t	id = VThread::selfId();

    assert( myThreadMap != 0 );
    
    ThreadMap::iterator it = myThreadMap->find(id);
    
    return ( it == myThreadMap->end() ? 0 : (*it).second );
}


vthread_t
Thread::selfId()
{
    return ( VThread::selfId() );
}


Thread::Thread(Runnable & runnable, const char * name)
    :	myThread(new VThread),
    	myRunnable(runnable),
	mySignalHandler(0),
	myName(name ? name : "thread")
{
    init();

    myThread->spawn(localThreadMain, this);
}


Thread::~Thread()
{
    const vthread_t *   threadId = myThread->getId();

    if ( threadId )
    {
        Lock    lock(myThreadMapMutex);

    	if ( myThreadMap )
	{
            myThreadMap->erase(*threadId);
	}
	else
	{
    	    assert( myThreadMap != 0 );
	}
    }

    delete myThread;
    
    delete mySignalHandler;
}


ReturnCode  	    	    
Thread::join()
{
    // Can't join from your own thread.
    //
    if ( this == self() )
    {
    	return ( 0 );
    }
    
    ReturnCode rc = 0;
    
    if ( myThread )
    {
    	rc = myThread->join();
    }
    return ( rc );
}


void
Thread::exit()
{
    // Can't exit unless this is the thread of execution.
    //
    if ( myThread == 0 || this != self() )
    {
    	return;
    }

    myThread->exit();
}


Runnable &
Thread::runnable()
{
    return ( myRunnable );
}	


SignalHandler &     	    
Thread::signalHandler()
{
    assert( mySignalHandler != 0 );
    return ( *mySignalHandler );
}


ostream &           
Thread::writeTo(ostream & out) const
{
    return ( out << myName.c_str() );
}	


Thread::Thread()
    :	myThread(0),
    	myRunnable(Application::instance()),
	mySignalHandler(new SignalHandler),
	myName("main")
{
    char    buffer[64];
    sprintf(buffer, "%lu", (unsigned long)VThread::selfId());
    myName += '.';
    myName += buffer;
}


void *	    
Thread::threadMain(void * data)
{
    Thread *	thread = (Thread *)data;
    
    if ( thread == 0 )
    {
    	return ( (void *)1 );
    }
    
    const vthread_t	*   threadId = thread->myThread->getId();

    // Scope the lock for update of thread map only.
    //
    {
        Lock    lock(myThreadMapMutex);
	if ( myThreadMap )
	{
    	    (*myThreadMap)[*threadId] = thread;
	}
	else
	{
	    assert( myThreadMap != 0 );
	    return ( (void *)1 );
	}
    }

    // Update name
    //
    char	    buffer[64];
    sprintf(buffer, "%lu", (unsigned long)*threadId);
    thread->myName += '.';
    thread->myName += buffer;

    // The signalHandler will persist for the lifetime of the run.
    //
    thread->mySignalHandler = new SignalHandler;

    void    * rc = (void *)(thread->myRunnable.run());

    delete thread->mySignalHandler;
    thread->mySignalHandler = 0;
    
    return ( rc );
}
