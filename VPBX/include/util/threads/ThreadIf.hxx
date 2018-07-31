#ifndef THREADIF_HXX
#define THREADIF_HXX
/*
* $Id: ThreadIf.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <cassert>
#include "VThread.hxx"
#include "VMutex.h"

static const char* const ThreadIf_hxx_version = 
"$Id: ThreadIf.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $";

/*

TODO:

move constructor and operator= to private.  This cannot be done
immediately since SipThread inherits from this and it has both a copy
constructor and an operator=.

*/

/** A wrapper class to create and spawn a thread.  It is a base class.
 * ThreadIf::thread() is a pure virtual method .
 *
 * <P>Usage:
 * To use this class, derive from it and override the thread() method.
 * To start the thread, call the run() method.  The code in thread() will
 * run in a separate thread.
 *
 * <P>Call shutdown() from the constructing thread to shut down the
 * code.  This will set the bool shutdown_ to true.  The code in
 * thread() should react properly to shutdown_ being set, by
 * returning.  Call join() to join the code.
 * <P>Sample:
 * <PRE>
 * ...
 *     DerivedThreadIf thread;
 *     thread.run();
 * ... do stuff ...
 *     thread.shutdown();
 *     thread.join();
 * </PRE>
 */
class ThreadIf
{
    public:
        /// default constructor
        ThreadIf();
        /// destructor
        virtual ~ThreadIf();
        /// runs the code in thread() .  Returns immediately
        virtual void run();
        /// joins to the thread running thread()
        virtual void join()
        {
            itsThread.join();
        }

        /// forces the thread running to exit()
        virtual void exit()
        {
            itsThread.exit();
        }

	/** request the thread running thread() to return, by setting
	 * the bool shutdown_ 
	 */
        void shutdown();

	/** returns true if the thread has been asked to shutdown_ or
	 * if the thread is not running
	 */
        bool isShutdown() const;

        /** thread is a virtual method.  Users should derive and define
	 * thread() such that it returns when isShutdown() is true.
	 */
        virtual void thread() = 0;

    protected:
	/// itsThread is protected so that thread() can retrieve its threadId 
        VThread itsThread;

    private:

        bool shutdown_;
        mutable VMutex shutdownMutex_;

        /** Suppress copying
         */
        ThreadIf(const ThreadIf &);

        /** Suppress copying
         */
        const ThreadIf & operator=(const ThreadIf &);

};


// THREADIF_HXX

#endif
