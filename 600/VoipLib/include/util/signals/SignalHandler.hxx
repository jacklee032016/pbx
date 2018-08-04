  #if !defined(VOCAL_SIGNAL_HANDLER_HXX)
#define VOCAL_SIGNAL_HANDLER_HXX

/*
 * $Id: SignalHandler.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "NonCopyable.hxx"
#include "global.h"
#include "SignalSet.hxx"
#include "SignalAction.hxx"
#include "VThread.hxx"
#include <csignal>
#include <map>

#if defined(__MACH__)
#include <signal.h>
typedef int siginfo_t;
#endif

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to handle signals.
 */
namespace Signals
{


using std::map;

#ifdef __APPLE__
    typedef struct {
        int	si_signo;
        int	si_errno;
        int 	si_code;
    } siginfo_t;
#endif

/** Support for unix type signals in a mutlithreaded or single threaded
 *  environment.<br><br>
 *
 *  @see    Vocal::Signals::SignalAction
 *  @see    Vocal::Signals::SignalSet
 *  @see    Vocal::Application
 *  @see    Vocal::Threads::Thread
 */
class SignalHandler : public Vocal::NonCopyable
{

    public:


	/** Global initialization. Should be called in Application::init().
	 */
    	static void     init();


	/** Global uninitialization. Should be called in Application::unit().
	 */
	static void 	uninit();
	

	/** Only one signal handler per thread can be created.
	 *  It needs to be created in the context of the thread.
	 */
    	SignalHandler();
	

	/** Virtual destructor.
	 */
	virtual     ~SignalHandler();


    	/** Adds an action to be handled for this thread.
	 */
    	void	    add(SignalAction &);
	

	/** Removes an action to be handled for this thread.
	 */
    	void	    remove(SignalAction &);


	/** Sets the blocked signals for this thread.
	 */
    	void	    setBlocked(const SignalSet &);


	/** Adds the signals to those blocked for this thread.
	 */
    	void	    block(const SignalSet &);


	/** Removes the signals from those blocked for this thread.
	 */
	void	    unblock(const SignalSet &);


	/** Sets the blocked signals for this process.
	 */
    	static void setGlobalBlocked(const SignalSet &);


	/** Adds the signals to those blocked for this process.
	 */
    	static void globalBlock(const SignalSet &);


	/** Removes the signals from those blocked for this process.
	 */
	static void globalUnblock(const SignalSet &);


	/** The actual signal handling routine.
	 */
    	static void signalHandler(int, siginfo_t *, void *);


    private:


    	typedef map<int, size_t>                    SignalActionRefCountMap;
    	typedef map<int, SignalAction *>    	    SignalActionMap;
    	typedef map<vthread_t, SignalActionMap *>   ThreadSignalMap;


    	static SignalActionRefCountMap	    	    myActionRefCountMap;
    	static ThreadSignalMap     	    	    myThreadSignalMap;
};


} // namespace Signals
} // namespace Vocal


#include "DeprecatedSignalHandler.hxx"


#endif // !defined(VOCAL_SIGNAL_HANDLER_HXX)
