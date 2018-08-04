#if !defined(VOCAL_RWMUTEX_HXX)
#define VOCAL_RWMUTEX_HXX
/*
 * $Id: RWMutex.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "Lockable.hxx"
#include "Mutex.hxx"
#include "Condition.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to create and manage threads.
 */
namespace Threads
{


/** Read/write mutex.<br><br>
 *
 *  Allows serialized write access and concurrent read access.
 */
class RWMutex : public Lockable
{
    public:

        /** Create a lock.
         */
        RWMutex();


        /** Destroy the lock.
         */        
        virtual ~RWMutex();
        
        
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


    	/** Accessor to readerCount.
	 */
    	u_int32_t   	readerCount() const;
	

    	/** Accessor to pendingWriterCount.
	 */
	u_int32_t   	pendingWriterCount() const;


    private:


        /** Access to the lock is synchronous.
         */
        Mutex      myMutex;


        /** Readers will block until all writers have completed.
         */
        Condition  myReadCondition;

        
        /** Pending writers will block while while waiting for the writer
         *  to finish.
         */
        Condition  myPendingWriteCondition;
        

        /** We can have multiple readers blocking.
         */
        u_int32_t   myReaderCount;
        

        /** We can have only a single writer.
         */
        bool        myWriterHasLock;


        /** We can have many pending writers.
         */
        u_int32_t   myPendingWriterCount;
};


} // namespace Threads
} // namespace Vocal


#endif // !defined(VOCAL_RWMUTEX_HXX)
