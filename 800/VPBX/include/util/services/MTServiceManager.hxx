#if !defined(ASSIST_MT_SERVICE_MANAGER_HXX)
#define ASSIST_MT_SERVICE_MANAGER_HXX
/*
* $Id: MTServiceManager.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "ServiceManager.hxx"
#include "FifoBase.h"
#include "Event.hxx"
#include "Sptr.hxx"
#include <list>


namespace Assist {
    namespace Threads {
        class Thread;
    }
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


using Assist::Threads::Thread;
using Assist::Signals::SignalAction;


class Service;


/** 
 */
class MTServiceManager : public Assist::Services::ServiceManager
{

    public:

    	/** Create the service manager, given the fifo that the
         *  will use.
    	 */    
    	MTServiceManager(
	    FifoBase< Sptr<Event> > 	&   serviceMgrFifo,
            SignalAction                *   signalAction = 0
	);
	

    	/** Virtual destructor
    	 */    
	virtual ~MTServiceManager();


    	/** Request the service manager to manage the given service.
	 */	
	void	manage(Service &);


    	/** Request the service manager to unmanage all the services.
	 */	
	void	unmanage();

	/** Starts all managed services. They are started serially, in the 
         *  order they were added to the manager.
	 */
	virtual ReturnCode	start();


    	/** Blocks waiting for either a signal, or an error message from
         *  a service indicating the process needs to terminate.
	 */
	virtual ReturnCode	run();


	/** Stops all managed services. They are stopped serially, in the
         *  reverse order as from the order they were started.
	 */
	virtual	void	 	stop();


	/** Shuts down all managed services. They are shutdown serially, in
         *  the reverse order from the order they were started.
	 */
	virtual	void	    	shutdown();


    private:

	void	    	        blockSignals();
	void	    	        unblockSignals();
	bool  	                signalCaught();
	
    	bool  	    	        block();
	
        struct ServiceInfo
        {
            Service     *   service;
            Thread      *   thread;
        };

    	typedef	    list<ServiceInfo> 	ServiceList;

	ServiceList 	    	    	myServiceList;
	FifoBase< Sptr<Event> >     &	myFifo;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_MT_SERVICE_MANAGER_HXX)
