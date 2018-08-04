#if !defined(ASSIST_LOCK_HXX)
#define ASSIST_LOCK_HXX
/*
* $Id: Lock.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Lockable.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to create and manage threads.
 */
namespace Threads
{


enum LockType
{
    ASSIST_LOCK = 0,
    ASSIST_READLOCK,
    ASSIST_WRITELOCK
};


/** A lock resource that will lock the lockable on construction and unlock
 *  on destruction.<br><br>
 *
 *  @see    Assist::Threads::Lockable
 */
class Lock : public NonCopyable
{
    public:

    	/** Lock the given VMutex.
	 */
        Lock(Lockable &, LockType = ASSIST_LOCK);


    	/** Unlock the given VMutex.
	 */
        virtual ~Lock();

    private:

        Lockable	&   myLockable;
};


/**
 */
class ReadLock : public Lock
{
    public:
    
	/**
	 */
    	ReadLock(Lockable &);
	
	virtual ~ReadLock();
};


/**
 */
class WriteLock : public Lock
{
    public:
    
	/**
	 */
    	WriteLock(Lockable &);
	
	virtual ~WriteLock();
};

} // namespace Threads
} // namespace Assist


#include "deprecated/DeprecatedLock.hxx"


#endif // !defined(ASSIST_LOCK_HXX)
