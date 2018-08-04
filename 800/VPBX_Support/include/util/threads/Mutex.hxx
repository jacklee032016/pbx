#if !defined(ASSIST_MUTEX_HXX)
#define ASSIST_MUTEX_HXX
/*
* $Id: Mutex.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "vthread.h"
#include "Lockable.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to create and manage threads.
 */
namespace Threads
{


/** Defines a simple mutex class which provides 'fast' mutex functionality, 
 *  meaning that if the calling thread already has the mutex locked, a call 
 *  to lock will block the thread forever.<br><br>
 */
class Mutex : public Lockable
{
    public:

        /** Create a VMutex object initialized with operating system
         *  dependent defaults (if any).
         */
        Mutex();


        /**
         ** Delete a VMutex object
         */
        virtual ~Mutex();


        /** Lock the mutex. If the mutex is currently unlocked, it
         *  becomes locked and owned by the calling thread, and
         *  lock returns immediately. If the mutex is already locked by 
         *  another thread, lock suspends the calling thread until the 
         *  mutex is unlocked. If the mutex is already locked by the calling 
         *  thread, the calling thread blocks forever.
         */
        virtual void    lock();


        /** Unlock the mutex. The mutex is assumed to be locked and owned
         *  by the calling thread, meaning that a call to unlock always
         *  returns it to the unlocked state. 
         */
        virtual void    unlock();


        /** Returns the operating system dependent unique id of the mutex.
         */
        vmutex_t    *   getId() const;


    private:


        mutable vmutex_t    myId;
};


} // namespace Threads
} // namespace Assist


#endif // !defined(ASSIST_MUTEX_HXX)
