/*
 * $Id: ThreadIf.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "ThreadIf.hxx"
#include "Lock.hxx"

using Vocal::Process::Lock;


static 
void*
threadWrapper( void* threadParm )
{
    assert( threadParm );
    ThreadIf* t = static_cast < ThreadIf* > ( threadParm );

    assert( t );
    t->thread();
    return 0;
}


ThreadIf::ThreadIf() : shutdown_(false)
{
}


ThreadIf::~ThreadIf()
{
}


void
ThreadIf::run()
{
    itsThread.spawn( threadWrapper, this );
}


void
ThreadIf::shutdown()
{
    Lock lock(shutdownMutex_);
    (void)lock;
    shutdown_ = true;
}


bool
ThreadIf::isShutdown() const
{
    Lock lock(shutdownMutex_);
    (void)lock;
    return ( shutdown_ );
}


// End of File
