#ifndef VSEMAPHORE_HXX_
#define VSEMAPHORE_HXX_
/*
* $Id: VSemaphore.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#ifdef WIN32
#include <pthread.h>
#endif

#if defined( __linux__ ) || defined(__FreeBSD__) || defined (WIN32) || defined (__APPLE__)
#include <semaphore.h>
#else
#include <synch.h>
#endif

#if defined (__linux__) || (__FreeBSD__) || defined (WIN32) || defined (__APPLE__)
typedef sem_t vsem_t;
#else
typedef sema_t vsem_t;
#endif



/** Class implements a semaphore */
class VSemaphore
{
    public:
	/// default constructor -- sets the count to 1
        VSemaphore();
	/** constructor which can set the count to a value
	    @param available  value to set the count to
	*/
	VSemaphore(int available);
	/// destructor
	virtual ~VSemaphore();

	/** if count == 0, wait on the semaphore.  If count > 0,
	 * decrement count and continue */
        void Wait();
	/// increment count, waking a waiting thread if there are any
        void Post();
	/** increment count by how_many, waking a waiting thread if
	 * there are any */
        void Post(int how_many);
    private:
	vsem_t semaphore;
} ;
#endif
