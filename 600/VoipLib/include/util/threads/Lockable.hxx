#if !defined(VOCAL_LOCKABLE_HXX)
#define VOCAL_LOCKABLE_HXX
/*
 * $Id: Lockable.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "NonCopyable.hxx"


/** Infrastructure common to VOCAL.<br><br>
 */
namespace Vocal
{


/** Infrastructure common to VOCAL to create and manage threads.
 */
namespace Threads
{


/** Interface for things that can be locked.<br><br>
 *
 *  If a lockable doesn't naturally have a lock() method, like
 *  a read write mutex, it should use the most restrictive lock,
 *  i.e. a write lock.
 */
class Lockable : public NonCopyable
{
    protected:

        /** Default constructor
         */
    	Lockable();
	
    public:
    
        /** Virtual destructor
         */
	virtual ~Lockable();


        /** You must be able to lock a lockable.
         */
    	virtual void	lock() = 0;


        /** You must be able to unlock a lockable.
         */
	virtual void	unlock() = 0;


        /** You must be able to read lock a lockable. The default
         *  behavior is to call lock().
         */
	virtual void	readlock();
	

        /** You must be able to read lock a lockable. The default
         *  behavior is to call lock().
         */
	virtual void	writelock();
};


} // namespace Process
} // namespace Vocal


#endif // !defined(VOCAL__LOCKABLE_HXX)
