#if !defined(ASSIST_SERVICE_APPLICATION_HXX)
#define ASSIST_SERVICE_APPLICATION_HXX
/*
* $Id: ServiceApplication.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "Application.hxx"
#include "Sptr.hxx"
#include "FifoBase.h"
#include <vector>
#include <string>


namespace Assist {
    namespace Signals {
        class SignalSet;
        class SignalAction;
    }
    namespace Configuration {
        class Config;
    }
}
    


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to provide event based services.<br><br>
 */
namespace Services
{


using Assist::Signals::SignalSet;
using Assist::Signals::SignalAction;
using Assist::Configuration::Config;


class ServiceManager;
class Event;


/** Service based application.<br><br>
 */
class ServiceApplication : public Assist::Application
{
    public:

    	/** Create the service application. Initialization is done in init, 
         *  not here.
	 */
    	ServiceApplication(bool singleThreaded = false);


    	/** Destroy the service application. Uninitialization is done in 
         *  uninit, not here.
    	 */
    	virtual ~ServiceApplication();


    	/** Initialize the service application.
    	 */
    	ReturnCode  	    init(int, char **, char **);

    	
	/** Cleanup.
    	 */
	void	    	    uninit();
	
    	
	/** Run the service application.
    	 */
    	ReturnCode  	    run();


    protected:


        /** The user should provide a factory method for the user defined
         *  configuration. If the user doesn't provide this, the default
         *  (and somewhat incomplete) Config class will be used.
         */
        virtual Config *    createConfig();
        
        
        /** The user has the ability to execute code after the configuration
         *  has been loaded and before initialization begins.
         */
        virtual ReturnCode  postConfig();
        
        
        /** The user can optionally provide the set of signals that will 
         *  cause the application to terminate gracefully. The default 
         *  signal set will include SIGHUP, SIGINT and SIGTERM.
         */
        virtual const vector<int> &     terminateSignals() const;
        
        
        /** The user can provide the filename of the log. 
         *  The config has been created when this is called.
         */
        virtual string  logFile() const;
         
        
        /** The last thing called by init to give the user a chance to
         *  initialize. All server objects have been initialized when
         *  this is called.
         */
        virtual ReturnCode  userInit();


        /** The first thing called by uninit to give the user a chance to
         *  cleanup.
         */
        virtual void    userUninit();
        
        
        /** Config accessor
         */        
        Config &    config();
        
        
        /** Config accessor
         */
        const Config &  config() const;


        /** Signal action accessor
         */
        SignalAction &  signalAction();
          
  
        /** Fifo accessor
         */ 
        FifoBase< Sptr<Event> > &   fifo();
        
        
        /** ServiceManager accessor
         */
        ServiceManager &    serviceManager();      


    private:
    
    	/** Server configuration information.
	 */
	Config              *   myConfig;
	
	
    	/** The set of signals we are observing.
	 */
	SignalSet   	    *	myTerminateSignals;


    	/** The signal handler for the pervious signal set.
	 */
	SignalAction     *	myTerminateAction;


    	/** Return message fifo. Used by the service manager to 
	 *  communicate with the subordinate threads / services.
	 */
	FifoBase< Sptr<Event> > *   myFifo;


    	/** Mananges all services. It starts, stops and shutsdown
	 *  the services.
	 */
    	ServiceManager  	*   myServiceMgr;
        

        /** If set, the application is running single threaded.
         */
        bool                        mySingleThreaded;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_SERVICE_APPLICATION_HXX)
