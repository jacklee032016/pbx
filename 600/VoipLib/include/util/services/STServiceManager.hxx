#if !defined(VOCAL_ST_SERVICE_MANAGER_HXX)
#define VOCAL_ST_SERVICE_MANAGER_HXX
/*
 * $Id: STServiceManager.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#include "ServiceManager.hxx"
#include "EventObserver.hxx"
#include "FifoBase.h"
#include "Event.hxx"
#include "Sptr.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure common to VOCAL to provide event based services.<br><br>
 */
namespace Services
{


using Vocal::Behavioral::EventObserver;


/** 
 */
class STServiceManager : public Vocal::Services::ServiceManager
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
            : public Vocal::Behavioral::EventObserver< Sptr<Event> >
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
} // namespace Vocal


#endif // !defined(VOCAL_ST_SERVICE_MANAGER_HXX)
