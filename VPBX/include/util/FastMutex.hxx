#ifndef _FAST_MUTEX_HXX
#define _FAST_MUTEX_HXX
/*
* $Id: FastMutex.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#if defined(__i386__) || defined (__ia64__)|| defined(__INTEL_COMPILER)
#define ASSIST_FASTMUTEX_AVAILABLE 1
#endif


#if defined(ASSIST_FASTMUTEX_AVAILABLE)

#if defined(__ia64__)

#include "vtypes.h"
#define aSwap(x,X) \
{ \
__asm__ __volatile__ ("xchg1 %0=%1,%0 " :"=r"(x),"=m"(X) : "r"(x), "m"(X)); \
}

#else

#define aSwap(x,X) \
{ \
__asm__ __volatile__ ("lock; xchgl %1, %0 " :"=a"(x) : "m"(X), "a"(x)); \
}

#endif

class FastMutex
{
public:
    FastMutex();
    ~FastMutex();
    inline void lock()
	{
#if defined(__ia64__)
            u_int8_t localLock = 1;
#else
            int localLock = 1;
#endif
	    do{
		aSwap(localLock,X);
	    } while(localLock);
	}

    inline void unlock()
	{
	    X = 0;
	}

protected:
#if defined(__ia64__)
    u_int8_t X;
#else
    int X;
#endif

private:
    FastMutex(const FastMutex&);
    FastMutex& operator=(const FastMutex&);
};


inline FastMutex::FastMutex()
    :
    X(0)
{}


inline FastMutex::~FastMutex()
{}


#endif

#endif
