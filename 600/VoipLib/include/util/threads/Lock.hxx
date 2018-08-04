#if !defined(VOCAL_LOCK_HXX)
#define VOCAL_LOCK_HXX
/*
 * $Id: Lock.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Lockable.hxx"
#include "NonCopyable.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to create and manage threads.
 */
namespace Threads
{


enum LockType
{
    VOCAL_LOCK = 0,
    VOCAL_READLOCK,
    VOCAL_WRITELOCK
};


/** A lock resource that will lock the lockable on construction and unlock
 *  on destruction.<br><br>
 *
 *  @see    Vocal::Threads::Lockable
 */
class Lock : public NonCopyable
{
    public:

    	/** Lock the given VMutex.
	 */
        Lock(Lockable &, LockType = VOCAL_LOCK);


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
} // namespace Vocal


#include "deprecated/DeprecatedLock.hxx"


#endif // !defined(VOCAL_LOCK_HXX)
