/*
* $Id: STServiceManager.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "STServiceManager.hxx"
#include "Service.hxx"
#include "ControlEvent.hxx"
#include "VLog.hxx"


using namespace Assist;
using Assist::Services::Event;
using Assist::Services::Service;
using Assist::Services::ServiceManager;
using Assist::Services::STServiceManager;
using Assist::Services::ControlEvent;
using Assist::Signals::SignalAction;
using Assist::Behavioral::EventObserver;
using Assist::Logging::VLog;


STServiceManager::STServiceManager(
    SignalAction                *   signalAction
)
    :   ServiceManager(signalAction),
        myService(0),
        myObserver(0)
{
    const string    fn("STServiceManager::STServiceManager");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": this = " << this << VDEBUG_END(log);
}


STServiceManager::~STServiceManager()
{
    const string    fn("STServiceManager::~STServiceManager");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": this = " << this << VDEBUG_END(log);
}


void	        
STServiceManager::manage(Service & service)
{
    const string    fn("STServiceManager::manage");
    VLog            log(fn);
    
    assert( myService == 0 );
    
    myService = &service;

    myService->setSignalAction(mySignalAction);
    
    myObserver = new STSMObserver(*myService);

    VINFO(log)  << fn << ": Managing service: " << myService->name()
                << VINFO_END(log);
}


void	        
STServiceManager::unmanage()
{
    const string    fn("STServiceManager::unmanage");
    VLog            log(fn);

    assert( myService != 0 );

    VINFO(log)  << fn << ": Unmanaging service: " << myService->name()
                << VINFO_END(log);
    
    delete myObserver;
    myObserver = 0;

    myService->setSignalAction(0);
    
    myService = 0;
}


ReturnCode	
STServiceManager::start()
{
    const string    fn("STServiceManager::start");
    VLog            log(fn);

    assert( myService != 0 );
        
    VINFO(log)  << fn << ": Service starting: " << myService->name() 
                << VINFO_END(log);

    ControlEvent * ctrlEvent 
        = new ControlEvent(ControlEvent::START, myService->getFifo());

    Sptr<Event> startEvent = ctrlEvent;

    myService->getFifo().add(startEvent);

    return ( SUCCESS );
}


ReturnCode	
STServiceManager::run()
{
    const string    fn("STServiceManager::run");
    VLog            log(fn);
    
    assert( myService != 0 );

    VINFO(log)  << fn << ": Service running: " << myService->name()
                << VINFO_END(log);
                
    return ( myService->run() );
}


void	 	
STServiceManager::stop()
{
    // Tear down will be taken care of in run.
}


void	    	
STServiceManager::shutdown()
{
    // Tear down will be taken care of in run.
}



STServiceManager::STSMObserver::STSMObserver(
    Service                 &   service
)
    :   EventObserver< Sptr<Event> >(service.getEventSubject()),
        myService(service)
{
    myService.subscribe(*this);
}        
                
STServiceManager::STSMObserver::~STSMObserver()
{
    myService.unsubscribe(*this);
}        


bool        
STServiceManager::STSMObserver::interestedIn(const Sptr<Event> & event)
{
    Sptr<ControlEvent> ctrlEvent;
    ctrlEvent.dynamicCast(event);

    return ( ctrlEvent != 0 );
}


void        
STServiceManager::STSMObserver::onEvent(Sptr<Event> event)
{
    const string    fn("STServiceManager::STSMObserver::onEvent");
    VLog    	    log(fn);

    Sptr<ControlEvent> ctrlEvent;
    ctrlEvent.dynamicCast(event);

    if ( ctrlEvent == 0 )
    {
    	return;
    }
    
    switch ( ctrlEvent->getType() )
    {
        case ControlEvent::START_RESPONSE:
        {
            ReturnCode  rc = ctrlEvent->getReturnCode();

            VINFO(log)  << fn << ": Service started: " << myService.name()
                        << ", rc = " << ctrlEvent->getReturnCode()
                        << VINFO_END(log);
                        
            if ( rc != SUCCESS )
            {
                ControlEvent * ctrlEvent 
                    = new ControlEvent(ControlEvent::SHUTDOWN, myService.getFifo());

		ctrlEvent->setReturnCode(rc);

                Sptr<Event> shutdownEvent = ctrlEvent;

                myService.getFifo().add(shutdownEvent);
            }

            break;
        }
        case ControlEvent::STOP_RESPONSE:
        case ControlEvent::SHUTDOWN_RESPONSE:
        {
	    ReturnCode	rc = ctrlEvent->getReturnCode();

            VINFO(log)  << fn << ": Service shutting down: " 
                        << myService.name() << ", rc = " << rc
                        << VINFO_END(log);
                        
            ControlEvent * ctrlEvent 
                = new ControlEvent(ControlEvent::SHUTDOWN, myService.getFifo());

	    ctrlEvent->setReturnCode(rc);

            Sptr<Event> shutdownEvent = ctrlEvent;

            myService.getFifo().add(shutdownEvent);

            break;
        }
        default:
        {
            VWARN(log)  << fn << ": Unknown control event: "
                        << *ctrlEvent << VWARN_END(log);
            break;
        }
    }
}


ostream &           
STServiceManager::STSMObserver::writeTo(ostream & out) const
{
    return ( out << "STSMObserver" );
}
