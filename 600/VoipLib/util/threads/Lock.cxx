/*
 * $Id: Lock.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "Lock.hxx"


using Vocal::Threads::Lock;
using Vocal::Threads::LockType;
using Vocal::Threads::Lockable;
using Vocal::Threads::ReadLock;
using Vocal::Threads::WriteLock;


Lock::Lock(Lockable & lockable, LockType lockType)
        : myLockable(lockable)
{
    switch ( lockType )
    {
	case VOCAL_READLOCK:
	{
	    myLockable.readlock();
	    break;
	}
	    
	case VOCAL_WRITELOCK:
	{
	    myLockable.writelock();
	    break;
	}

    	default:
	{
    	    myLockable.lock();
	    break;
	}
    }
}


Lock::~Lock()
{
    myLockable.unlock();
}


ReadLock::ReadLock(Lockable & lockable)
    :	Lock(lockable, VOCAL_READLOCK)
{
}


ReadLock::~ReadLock()
{
}


WriteLock::WriteLock(Lockable & lockable)
    :	Lock(lockable, VOCAL_WRITELOCK)
{
}


WriteLock::~WriteLock()
{
}
