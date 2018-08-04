#ifndef COUNT_SEMAPHORE_HXX_
#define COUNT_SEMAPHORE_HXX_

/*
* $Id: CountSemaphore.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "VMutex.h"
#include "global.h"
#include <assert.h>


#if (defined(i386) && defined(__GNUC__)) || defined(__INTEL_COMPILER)
#define USE_VMUTEX_LOCKING 0
#else
#define USE_VMUTEX_LOCKING 1
#endif

#if USE_VMUTEX_LOCKING

// these macros define the appropriate operators if using VMutex as the basis for
// the reference counting.
#define VLOCK() mutex.lock()
#define VUNLOCK() mutex.unlock()
#define INC(x) ++x
#define DEC(x) --x
#define DEC_AND_CMP(x,r) --x,r = (x == 0)
#define EXCHANGE(ptr,val) void* tmp; tmp = *ptr,*ptr = val, val = tmp
#else

// these macros define the appropriate operators if using i386 assembly for
// the reference counting.

#define VLOCK()
#define VUNLOCK()

#define INC(x) \
{   \
asm volatile ("lock; incl %0" : : "m" (x) ); \
}

#define DEC(x) \
{   \
asm volatile ("lock; decl %0" : : "m" (x) ); \
}

#define DEC_AND_CMP(x,r) \
{   \
asm volatile ("lock; decl %1; sete %0 " : "=m" (r) : "m" (x) ); \
}

#define EXCHANGE(ptr,val) \
{   \
asm volatile ("lock; xchg %0,%1" : "=r" (val), "=m" (*ptr) : "m" (*ptr) , "0" (val) ); \
}

#endif

#define INLINE_ inline

/* TODO: 

   fix the name.  It should be called ReferenceCount or something like
   that.  

   figure out why the count is volatile.

   change decrement() to decrementAndCompare() .

*/

/** A thread safe reference count.  The implementation is designed to
 * use either VMutex or inline assembly when using GCC (for
 * efficiency).
 */
class CountSemaphore
{
    public:
	/// constructor
        INLINE_ explicit CountSemaphore(int value=0)
        :
#if USE_VMUTEX_LOCKING
        mutex(),
#endif
        count(value)
        {}

	/** compares the current value in the reference count to
	 * value.  returns true if equal.
	 * @param value   integer to compare against.
	 */
        INLINE_ bool compare(int value)
        {
            bool retVal = false;
            VLOCK();
            if (value == count) retVal = true;
            VUNLOCK();
            return retVal;
        }

	/** increment the reference count by one.  This operation is atomic.
	 */
        INLINE_ void increment()
        {
            VLOCK();
            INC(count);
            VUNLOCK();
        }

	/** decrement the reference count by one.  This operation is
	 * atomic.  If the reference count now equals 0, decrement
	 * returns true.
	 */
        INLINE_ bool decrement()
        {
            bool retVal;
            VLOCK();
            assert(count > 0);
            DEC_AND_CMP(count, retVal);
            VUNLOCK();
            return retVal;
        }

        INLINE_ void exchange(void** ptr, void** val)
        {
            VLOCK();
	    EXCHANGE(ptr, *val);
            VUNLOCK();
        }

	INLINE_ int getCount()
	{
	    return count;
	}

	bool operator==(int value) const
	{
            return (value == count);
	}

        void lock() 
        {
            VLOCK();
        }

        void unlock() 
        {
            VUNLOCK();
        }

    private:
	/// suppress copying
        CountSemaphore(const CountSemaphore&);
	/// suppress copying
        const CountSemaphore& operator=(const CountSemaphore&);
	/// suppress comparison
        bool operator==(const CountSemaphore&);

#if USE_VMUTEX_LOCKING
        VMutex mutex;
#endif

        volatile int count; // why is this volatile ?
};

#endif
