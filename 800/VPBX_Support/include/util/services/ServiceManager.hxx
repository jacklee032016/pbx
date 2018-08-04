#if !defined(ASSIST_SERVICE_MANAGER_HXX)
#define ASSIST_SERVICE_MANAGER_HXX
/*
* $Id: ServiceManager.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "NonCopyable.hxx"
#include <list>


namespace Assist {
    namespace Signals {
        class SignalAction;
    }
}
    

/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to provide event based services.<br><br>
 */
namespace Services
{


using Assist::Signals::SignalAction;


class Service;


/** 
 */
class ServiceManager : public Assist::NonCopyable
{

    public:

    	/** Create the service manager, given the fifo that the
         *  will use.
    	 */    
    	ServiceManager(SignalAction * signalAction = 0);
	

    	/** Virtual destructor
    	 */    
	virtual ~ServiceManager();


    	/** Request the service manager to manage the given service.
	 */	
	virtual void	        manage(Service &) = 0;


    	/** Request the service manager to unmanage all the services.
	 */	
	virtual void	        unmanage() = 0;


	/** Starts all managed services. They are started serially, in the 
         *  order they were added to the manager.
	 */
	virtual ReturnCode	start() = 0;


    	/** Blocks waiting for either a signal, or an error message from
         *  a service indicating the process needs to terminate.
	 */
	virtual ReturnCode	run() = 0;


	/** Stops all managed services. They are stopped serially, in the
         *  reverse order as from the order they were started.
	 */
	virtual	void	 	stop() = 0;


	/** Shuts down all managed services. They are shutdown serially, in
         *  the reverse order from the order they were started.
	 */
	virtual	void	    	shutdown() = 0;


    protected:

        SignalAction                *   mySignalAction;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_SERVICE_MANAGER_HXX)
