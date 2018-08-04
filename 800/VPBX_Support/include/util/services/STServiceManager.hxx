#if !defined(ASSIST_ST_SERVICE_MANAGER_HXX)
#define ASSIST_ST_SERVICE_MANAGER_HXX
/*
* $Id: STServiceManager.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "ServiceManager.hxx"
#include "EventObserver.hxx"
#include "FifoBase.h"
#include "Event.hxx"
#include "Sptr.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/** Infrastructure common to ASSIST to provide event based services.<br><br>
 */
namespace Services
{


using Assist::Behavioral::EventObserver;


/** 
 */
class STServiceManager : public Assist::Services::ServiceManager
{

    public:

    	/** Create the service manager, given the fifo that the
         *  will use.
    	 */    
    	STServiceManager(SignalAction * signalAction = 0);
	

    	/** Virtual destructor
    	 */    
	virtual ~STServiceManager();


    	/** Request the service manager to manage the given service.
	 */	
	virtual void	        manage(Service &);


    	/** Request the service manager to unmanage the service.
	 */	
	virtual void	        unmanage();


	/** Starts managed service. 
	 */
	virtual ReturnCode	start();


    	/** Blocks waiting for either a signal.
	 */
	virtual ReturnCode	run();


	/** Stops managed service.
	 */
	virtual	void	 	stop();


	/** Shuts down manage service.
	 */
	virtual	void	    	shutdown();


    protected:
    

        class STSMObserver 
            : public Assist::Behavioral::EventObserver< Sptr<Event> >
        {
            public:
            
                STSMObserver(Service & service);

                virtual ~STSMObserver();
                
    	        virtual bool        interestedIn(const Sptr<Event> & event);
    	        virtual void        onEvent(Sptr<Event>);
                ostream &           writeTo(ostream & out) const;
            private:
                
                Service                 &   myService;
        };
        
        Service                     *   myService;
    	STSMObserver                *   myObserver;
};


} // namespace Services
} // namespace Assist


#endif // !defined(ASSIST_ST_SERVICE_MANAGER_HXX)
