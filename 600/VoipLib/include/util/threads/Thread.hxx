#if !defined(VOCAL_THREAD_HXX)
#define VOCAL_THREAD_HXX
/*
 * $Id: Thread.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "Writer.hxx"
#include "VocalCommon.hxx"
#include "VThread.hxx"
#include "Mutex.hxx"
#include <map>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


// Forward declarations
//
class Runnable;

namespace Signals
{
    class SignalHandler;
}

/** Infrastructure common to VOCAL to create and manage processes.
 */
namespace Threads
{


using Vocal::Signals::SignalHandler;


/** Executes a Runnable in it's own thread of execution.<br><br>
 *
 *  Each thread creates a SignalHandler.
 *
 *  @see    Vocal::Runnable
 *  @see    Vocal::Signals::SignalHandler
 *  @see    Vocal::ReturnCode
 */
class Thread : public Vocal::IO::Writer
{
    public:
    
    	/** Globally initialize the Thread facility. This should be done
	 *  in Application::init on the context of the initial thread. 
	 *  When called, the current thread is added to the ThreadMap so 
	 *  that self() will work in the initial thread.
	 */
    	static	void	    	    init();


    	/** Globally uninitialize the Thread facility. This should be done
	 *  in Application::uninit.
	 */
	static	void	    	    uninit();


    	/** Returns the Thread class associated with the calling thread.
	 */
    	static	Thread *    	    self();


    	/** Globally initialize the Thread facility. This should be done
	 *  in Application::init
	 */
    	static	vthread_t   	    selfId();
	

    	/** Start a new thread. The thread starts running during construction.
	 */
    	Thread(Runnable &, const char * name = 0);
				    

	/** Delete Thread object. This does not stop the thread from executing.
	 */
    	~Thread();


    	/** Join the thread. Must be called from outside the thread of
	 *  execution. Returns the result of the Runnable's run() method.
	 */
	ReturnCode  	    	    join();
	

	/** Exit the thread. Must be called from within the thread
	 *  of execution.
	 */
	void	    	    	    exit();
	

	/** Return the runnable associated with this thread. If called in the
	 *  initializing thread, the Application is returned.
	 */
	Runnable &  	    	    runnable();
	

	/** Return the signalHandler associated with this thread.
	 */
	SignalHandler &     	    signalHandler();
	

    	/** Write thread information to stream. See Writer.
	 */
	ostream &           	    writeTo(ostream &) const;
	

        static void *   threadMain(void *);		
	
    private:

    	/** Default constructor for main (Application's) thread. Called
	 *  from init().
	 */
    	Thread();


	typedef map<vthread_t, Thread *>	    ThreadMap;
	
    	static	ThreadMap   			*   myThreadMap;
    	static	Mutex	    	    	    	    myThreadMapMutex;
	static	Thread      	    	     	*   myMainThread;
	
	    	VThread     	    	    	*   myThread;
		Runnable    	    	    	&   myRunnable;
		SignalHandler	    	    	*   mySignalHandler;
		string	    	    	    	    myName;
};


} // namespace Process
} // namespace Vocal


#include "DeprecatedThread.hxx"


#endif // !defined(VOCAL_THREAD_HXX)
