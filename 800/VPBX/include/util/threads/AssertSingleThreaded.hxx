#if !defined(ASSERT_SINGLE_THREADED_HXX)
#define ASSERT_SINGLE_THREADED_HXX
/*
* $Id: AssertSingleThreaded.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Lockable.hxx"
#include "CountSemaphore.hxx"
#include "RWMutex.hxx"

#define ASSERT_WRITE(varName___, x___) Assist::Threads::WriteLock varName___(x___)
#define ASSERT_READ(varName___, x___) Assist::Threads::ReadLock varName___(x___)

namespace Assist
{

namespace Threads
{

class SingleThreadAssertable : public Lockable
{
    private:
	CountSemaphore myReadCount;
	bool myWriteBool;

    public:
	SingleThreadAssertable();
	virtual ~SingleThreadAssertable();


        /** Obtain the read lock. This will block until all writers have
         *  completed.
         */ 
        virtual void    readlock();
        

        /** Obtain the write lock. Provides serialized writing.
         */
        virtual void    writelock();
        

    	/** Defaults to the strictest lock, the writelock.
	 */
        virtual void 	lock();
    

        /** Unlocks either the read or write lock.
         */
        virtual void    unlock();
};


}
}


#endif
