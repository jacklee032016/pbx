/*
 * $Id: MTServiceManager.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "MTServiceManager.hxx"
#include "Service.hxx"
#include "ControlEvent.hxx"
#include "SignalAction.hxx"
#include "SignalHandler.hxx"
#include "Thread.hxx"
#include "VLog.hxx"


using namespace Vocal;
using Vocal::Services::Event;
using Vocal::Services::Service;
using Vocal::Services::ServiceManager;
using Vocal::Services::MTServiceManager;
using Vocal::Services::ControlEvent;
using Vocal::Signals::SignalAction;
using Vocal::Threads::Thread;
using Vocal::Logging::VLog;


MTServiceManager::MTServiceManager(
    FifoBase< Sptr<Event> > 	&   serviceMgrFifo,
    SignalAction                *   signalAction
)
    :	ServiceManager(signalAction),
        myFifo(serviceMgrFifo)
{
    const string    fn("MTServiceManager::MTServiceManager");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": this = " << this << VDEBUG_END(log);
}


MTServiceManager::~MTServiceManager()
{
    const string    fn("MTServiceManager::~MTServiceManager");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": this = " << this << VDEBUG_END(log);
}


void	
MTServiceManager::manage(Service & service)
{
    const string    fn("MTServiceManager::manage");
    VLog    	    log(fn);
    
    VINFO(log) << fn << ": service = " << service.name() << VINFO_END(log);

    service.setServiceManagerFifo(&myFifo);

    ServiceInfo serviceInfo;
    
    serviceInfo.service = &service;
    serviceInfo.thread = new Thread(service, service.name().c_str());
    
    myServiceList.push_back(serviceInfo);
}


void	
MTServiceManager::unmanage()
{
    const string    fn("MTServiceManager::unmanage");
    VLog    	    log(fn);
    
    for (   ServiceList::reverse_iterator i = myServiceList.rbegin();
            i != myServiceList.rend();
            ++i
        )
    {
        VINFO(log) << fn << ": service = " << i->service->name() 
                   << VINFO_END(log);
                   
        i->thread->join();
        delete i->thread;
        i->thread = 0;

	i->service->setServiceManagerFifo(0);
    }
    
    myServiceList.clear();
}


ReturnCode
MTServiceManager::start()
{
    const string    fn("MTServiceManager::start");
    VLog    	    log(fn);

    ReturnCode	rc = SUCCESS;
        
    for (   ServiceList::iterator iter = myServiceList.begin();
    	    rc == SUCCESS && iter != myServiceList.end();
	    ++iter
	)
    {
    	Service &   service = *(iter->service);

        ControlEvent    * ctrlEvent 
    	    = new ControlEvent(ControlEvent::START, myFifo);

	Sptr<Event>	startEvent = ctrlEvent;
    
    	int     startId = ctrlEvent->getControlEventId();
    
    	service.getFifo().add(startEvent);
    
	bool    started = false;

    	while ( !started )
    	{
	    bool done = block();
	    
	    if ( done == true )
	    {
                VINFO(log) << fn << ": Exiting on signal." << VINFO_END(log);
	    	break;
	    }
	        
            Sptr<Event> event;
    
            while ( !started && myFifo.messageAvailable() )
            {
            	event = myFifo.getNext();

    	    	Sptr<ControlEvent> ctrlEvent;
    	    	ctrlEvent.dynamicCast(event);

    		if ( ctrlEvent == 0 )
		{
		    VWARN(log)  << fn << ": Unknown event: " 
	    	    		<< ( event == 0 ? "No event" : event->name() )
		    		<< VWARN_END(log);
	    	    continue;
		}

    		if  (   ctrlEvent->getType() == ControlEvent::START 
                    ||  ctrlEvent->getType() == ControlEvent::START_RESPONSE 
                    )
		{
    	    	    if ( ctrlEvent->getControlEventId() == startId )
		    {
			started = true;

    	    	    	rc = ctrlEvent->getReturnCode();

    	    		if ( rc != SUCCESS )
			{
			    VWARN(log)  << fn << ": Service: " << service.name()
                                        << ", start returned error. shutting down."
		    	    		<< VWARN_END(log);
			}

    	    		VINFO(log) << fn << ": Service: " << service.name()
                                   << ", started, rc = " << rc
		    	    	   << VINFO_END(log);
		    }
		    else
		    {
			VWARN(log)  << fn << ": Unknown control event: " 
		    		    << *ctrlEvent << VWARN_END(log);
		    }
		}
		else
		{
		    VWARN(log)  << fn << ": Unknown control event: " 
		    		<< *ctrlEvent << VWARN_END(log);
		}
	    }
	}
    }

    return ( rc );
}


ReturnCode
MTServiceManager::run()
{
    const string    fn("MTServiceManager::run");
    VLog    	    log(fn);

    ReturnCode	rc  	    = SUCCESS;
    bool    	done        = false;
    
    while ( !done )
    {
    	done = block(); 
	
	if ( done == true )
	{
            VINFO(log) << fn << ": exiting on signal." << VINFO_END(log);
	    break;
	}

    	Sptr<Event> event;

    	while ( !done && myFifo.messageAvailable() )
    	{
    	    event = myFifo.getNext();

    	    Sptr<ControlEvent> ctrlEvent;
    	    ctrlEvent.dynamicCast(event);

    	    if ( ctrlEvent == 0 )
	    {
		VWARN(log)  << fn << ": Unknown event: " 
	    	    	    << ( event == 0 ? "No event" : event->name() )
		    	    << VWARN_END(log);
    	    }
	    
    	    switch ( ctrlEvent->getType() )
    	    {
	    	// We received an unsolicited stop or shutdown message.
		// It's trouble, so let's shutdown.
		//
		case ControlEvent::STOP:
		case ControlEvent::SHUTDOWN:
                case ControlEvent::STOP_RESPONSE:
                case ControlEvent::SHUTDOWN_RESPONSE:
		{
		    rc = ctrlEvent->getReturnCode();
		    done = true;

    	    	    VWARN(log) << fn << ": terminate via Service." 
		    	       << VWARN_END(log);
		    break;
		}
		default:
	    	{
		    break;
		}
	    }
	}
    }
    
    return ( rc );
}


void
MTServiceManager::stop()
{
    const string    fn("MTServiceManager::stop");
    VLog    	    log(fn);

    for (   ServiceList::reverse_iterator iter = myServiceList.rbegin();
    	    iter != myServiceList.rend();
	    ++iter
	)
    {
    	Service &   service = *(iter->service);

        ControlEvent * ctrlEvent 
            = new ControlEvent(ControlEvent::STOP, myFifo);

	Sptr<Event> stopEvent = ctrlEvent;

	int stopId = ctrlEvent->getControlEventId();

	service.getFifo().add(stopEvent);

	bool stopped = false;

	while ( !stopped )
	{
	    block();

            Sptr<Event> event;

            while ( !stopped && myFifo.messageAvailable() )
            {
        	event = myFifo.getNext();

    		Sptr<ControlEvent> ctrlEvent;
    		ctrlEvent.dynamicCast(event);

    		if ( ctrlEvent == 0 )
		{
		    VWARN(log)  << fn << ": Unknown event: " 
	    	    		<< ( event == 0 ? "No event" : event->name() )
		    		<< VWARN_END(log);
		    continue;
		}

    		if  (   (   ctrlEvent->getType() == ControlEvent::STOP
                        ||  ctrlEvent->getType() == ControlEvent::STOP_RESPONSE
                        )
		    &&  ctrlEvent->getControlEventId() == stopId
		    )
		{
		    stopped = true;

    	    	    VINFO(log) << fn << ": Serivce: " << service.name()
                               << ", stopped." << VINFO_END(log);
		}
		else
		{
		    VWARN(log)  << fn << ": Unknown service control event: " 
		    		<< *ctrlEvent << VWARN_END(log);
		}
	    }
	}
    }
}


void	
MTServiceManager::shutdown()
{
    const string    fn("MTServiceManager::shutdown");
    VLog    	    log(fn);

    for (   ServiceList::reverse_iterator iter = myServiceList.rbegin();
    	    iter != myServiceList.rend();
	    ++iter
	)
    {
    	Service &   service = *(iter->service);

	ControlEvent * shutdownEvent
	    = new ControlEvent(ControlEvent::SHUTDOWN, myFifo);

	Sptr<Event> event = shutdownEvent;

	service.getFifo().add(event);

	VINFO(log) << fn << ": shutdown sent." << VINFO_END(log);
    }
}


void	    	
MTServiceManager::blockSignals()
{
    if ( mySignalAction )
    {
        Thread::self()->signalHandler().block(mySignalAction->signalSet());
    }
}
	
	
void	    	
MTServiceManager::unblockSignals()
{
    if ( mySignalAction )
    {
        Thread::self()->signalHandler().unblock(mySignalAction->signalSet());
    }
}
	

bool
MTServiceManager::signalCaught()
{
    if ( mySignalAction )
    {
        return ( mySignalAction->signalCaught() );
    }
    return ( false );
}


bool
MTServiceManager::block()
{
    const string    fn("MTServiceManager::block");
    VLog    	    log(fn);
    
    unblockSignals();

    myFifo.block();

    blockSignals();

    VVERBOSE(log) << fn << ": fifo block returned." << VVERBOSE_END(log);

    return ( signalCaught() );
}
