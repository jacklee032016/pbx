/*
 * $Id: RWMutex.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "RWMutex.hxx"
#include "Lock.hxx"
#include <cassert>


using Vocal::Threads::RWMutex;
using Vocal::Threads::Lock;


RWMutex::RWMutex()
    :   myReaderCount(0),
    	myWriterHasLock(false),
	myPendingWriterCount(0)
{
}


RWMutex::~RWMutex()
{
}


void
RWMutex::readlock()
{
    Lock    lock(myMutex);
    
    while ( myWriterHasLock || myPendingWriterCount > 0 )
    {
    	myReadCondition.wait(&myMutex);
    }

    myReaderCount++;
}


void
RWMutex::writelock()
{
    Lock    lock(myMutex);

    myPendingWriterCount++;
    
    while ( myWriterHasLock || myReaderCount > 0 )
    {
    	myPendingWriteCondition.wait(&myMutex);
    }

    myPendingWriterCount--;
    
    myWriterHasLock = true;
}


void
RWMutex::lock()
{
    writelock();
}


void
RWMutex::unlock()
{
    Lock    lock(myMutex);
    
    // Unlocking a write lock.
    //
    if ( myWriterHasLock )
    {
    	assert( myReaderCount == 0 );
	
    	myWriterHasLock = false;

        // Pending writers have priority. Could potentially starve readers.
        //
    	if ( myPendingWriterCount > 0 )
	{	
	    myPendingWriteCondition.signal();
	}

        // No writer, no pending writers, so all the readers can go.
        //	
	else
	{
	    myReadCondition.broadcast();
	}

    }
    
    // Unlocking a read lock.
    //
    else
    {
    	assert( myReaderCount > 0 );
	
    	myReaderCount--;
	
	if ( myReaderCount == 0 && myPendingWriterCount > 0 )
	{
	    myPendingWriteCondition.signal();
	}
    }
}


u_int32_t   	
RWMutex::readerCount() const
{
    return ( myReaderCount );
}


u_int32_t   	
RWMutex::pendingWriterCount() const
{
    return ( myPendingWriterCount );
}
