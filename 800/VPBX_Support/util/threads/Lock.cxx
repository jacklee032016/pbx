/*
* $Id: Lock.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "Lock.hxx"


using Assist::Threads::Lock;
using Assist::Threads::LockType;
using Assist::Threads::Lockable;
using Assist::Threads::ReadLock;
using Assist::Threads::WriteLock;


Lock::Lock(Lockable & lockable, LockType lockType)
        : myLockable(lockable)
{
    switch ( lockType )
    {
	case ASSIST_READLOCK:
	{
	    myLockable.readlock();
	    break;
	}
	    
	case ASSIST_WRITELOCK:
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
    :	Lock(lockable, ASSIST_READLOCK)
{
}


ReadLock::~ReadLock()
{
}


WriteLock::WriteLock(Lockable & lockable)
    :	Lock(lockable, ASSIST_WRITELOCK)
{
}


WriteLock::~WriteLock()
{
}
