/*
 * $Id: Service.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "Service.hxx"
#include "VException.hxx"
#include "SignalAction.hxx"
#include "SignalHandler.hxx"
#include "Thread.hxx"
#include "VLog.hxx"
#include <cassert>


using namespace Vocal;
using Vocal::Services::Service;
using Vocal::Services::Event;
using Vocal::Services::ControlEvent;
using Vocal::Behavioral::EventObserver;
using Vocal::Behavioral::EventSubject;
using Vocal::Signals::SignalAction;
using Vocal::Threads::Thread;
using Vocal::Logging::VLog;


Service::Service(
    const char      	    	*   name
)
    :	myServiceManagerFifo(0),
    	myName(name ? name : "Service"),
        mySignalAction(0)
{
}


Service::~Service()
{
}


FifoBase< Sptr<Event> > &   
Service::getServiceManagerFifo()
{
    return ( myServiceManagerFifo ? *myServiceManagerFifo : getFifo() );
}
		

const FifoBase< Sptr<Event> > &   
Service::getServiceManagerFifo() const
{
    return ( myServiceManagerFifo ? *myServiceManagerFifo : getFifo() );
}
		

void
Service::setServiceManagerFifo(FifoBase< Sptr<Event> > * fifo)
{
    myServiceManagerFifo = fifo;
}


EventSubject< Sptr<Event> > &   
Service::getEventSubject()
{
    return ( mySubject );
}


const EventSubject< Sptr<Event> > &   
Service::getEventSubject() const
{
    return ( mySubject );
}


void                    
Service::setSignalAction(SignalAction * signalAction)
{
    mySignalAction = signalAction;
}
        
        
ReturnCode  	    
Service::run()
{
    const string    fn("Service::run");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": " << myName << " starting." << VDEBUG_END(log);

    ReturnCode	    rc = SUCCESS;

    bool    	    done = false;

    while ( !done )
    {
    	int numberMsgActive = 0;

    	// Wait for activity.
	//
    	try
	{
    	    VVERBOSE(log)<< fn << ": before wait"
	    	    	 << ", number msgs active = " << numberMsgActive
			 << VVERBOSE_END(log);

            numberMsgActive = block();

    	    VVERBOSE(log) << fn << ": after wait"
	    	    	  << ", number msgs active = " << numberMsgActive
	    	    	  << VVERBOSE_END(log);
	}
	catch ( VException & exception )
	{
	    onVException(exception);
	    continue;
	}
	catch ( ... )
	{
	    onException();
	    continue;
	}

    	// Process the messages.
	//
    	try
	{
    	    done = processMessages(numberMsgActive, rc);
	}
	catch ( VException & exception )
	{
	    onVException(exception);
	    continue;
	}
	catch ( ... )
	{
	    onException();
	    continue;
	}
    }

    VDEBUG(log) << fn << ": " << myName << " exiting"
	    	<< ", rc = " << rc << VDEBUG_END(log);

    return ( rc );
}


void
Service::subscribe(EventObserver< Sptr<Event> > & observer)
{
    const string    fn("Service::subscribe");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": observer = " << observer << VDEBUG_END(log);
    
    mySubject.attach(observer);

    VVERBOSE(log) << fn << ": subject = { " << mySubject << " }" << VVERBOSE_END(log);
}


void	    	    
Service::unsubscribe(EventObserver< Sptr<Event> > & observer)
{
    const string    fn("Service::unsubscribe");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": observer = " << observer << VDEBUG_END(log);
    
    mySubject.detach(observer);

    VVERBOSE(log) << fn << ": subject = { " << mySubject << " }" << VVERBOSE_END(log);
}



ostream &           
Service::writeTo(ostream & out) const
{
    return ( out << myName << ", subject = " << mySubject );
}


const string &
Service::name() const
{
    return ( myName );
}


ReturnCode
Service::onStart()
{
    return ( SUCCESS );
}


ReturnCode
Service::onStop()
{
    return ( SUCCESS );
}


ReturnCode	    	
Service::onShutdown()
{
    return ( SUCCESS );
}
		

void	    	
Service::onVException(VException & exception)
{
    const string    fn("Service::onVException");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": ignoring." << VDEBUG_END(log);
}		


void	    	
Service::onException()
{
    const string    fn("Service::onException");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": doing a hardShutdown." << VDEBUG_END(log);

    hardShutdown(!SUCCESS);
}		


void	    	
Service::onSignalCaught()
{
    const string    fn("Service::onSignalCaught");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": doing a shutdown via signal." << VDEBUG_END(log);

    hardShutdown(SUCCESS);
}		


bool	    
Service::processMessages(int, ReturnCode & rc)
{
    const string    fn("Service::processMessages");
    VLog    	    log(fn);
    
    bool	    done = false;
    rc = SUCCESS;

    while ( !done && getFifo().messageAvailable() )
    {
    	VVERBOSE(log) << fn << ": before getMessage." << VVERBOSE_END(log);

    	Sptr<Event> event;

    	try
	{
    	    event = getFifo().getNext();
	}
	catch ( ... )
	{
	    // We checked messageAvailable, so we shouldn't ever get here.
	    //
	    assert(0);
	}

    	Sptr<ControlEvent> ctrlEvent;
    	ctrlEvent.dynamicCast(event);

    	// Service control events detected. Call the appropriate
	// Service callbacks.
	//
    	if ( ctrlEvent != 0 )
    	{
	    VDEBUG(log) << fn << ": event = { " << *ctrlEvent 
		    	<< " }" << VDEBUG_END(log);

    	    switch ( ctrlEvent->getType() )
    	    {
		case ControlEvent::START:
		{
		    start(ctrlEvent);
		    break;
		}
		case ControlEvent::STOP:
		{
		    stop(ctrlEvent);
		    break;
		}
		case ControlEvent::SHUTDOWN:
		{
		    rc = shutdown(ctrlEvent);
		    done = true;
		    break;
		}
		default:
		{
		    // Perhaps someone has derived off of the 
		    // ControlEvent and wants to see it.
		    //
		    mySubject.setEvent(event);
		    break;
		}
	    }
    	}

    	// Unknown event, give it to the EventSubject to distribute it
	// to interested observers.
	//
	else
	{
	    mySubject.setEvent(event);
	}
    }

    return ( done );
}


void	    	
Service::hardShutdown(ReturnCode errorCode)
{
    const string    fn("Service::hardShutdown");
    VLog    	    log(fn);

    VDEBUG(log) << fn << ": shutting down service"
    	    	<< ", error = " << errorCode << VDEBUG_END(log);

    shutdown(0);
    
    ControlEvent * ctrlEvent 
        = new ControlEvent(ControlEvent::SHUTDOWN, getFifo());

    ctrlEvent->setReturnCode(errorCode);
    ctrlEvent->response();
    
    Sptr<Event> shutdownEvent = ctrlEvent;

    getServiceManagerFifo().add(shutdownEvent);
}


void	    	
Service::start(Sptr<ControlEvent> startEvent)
{
    const string    fn("Service::start");
    VLog    	    log(fn);

    ReturnCode      rc = SUCCESS;
    
    FifoBase< Sptr<Event> > *	serviceMgrFifo = startEvent->getReplyFifo();

    if ( !serviceMgrFifo || *serviceMgrFifo != getServiceManagerFifo() )
    {
    	VWARN(log)  << fn << ": received unathorized start event, event = "
	    	    << *startEvent << VWARN_END(log);
        rc = !SUCCESS;
    }
    else
    {
        rc = onStart();
    }

    startEvent->setReturnCode(rc);
    startEvent->response();

    Sptr<Event> event = startEvent;

    serviceMgrFifo->add(event);

    VDEBUG(log) << fn << ": returning start = " << *startEvent 
    	    	<< VDEBUG_END(log);
}


void	    	
Service::stop(Sptr<ControlEvent> stopEvent)
{
    const string    fn("Service::stop");
    VLog    	    log(fn);
    
    ReturnCode      rc = SUCCESS;
    
    FifoBase< Sptr<Event> > *	serviceMgrFifo = stopEvent->getReplyFifo();
    
    if ( !serviceMgrFifo || *serviceMgrFifo != getServiceManagerFifo() )
    {
    	VWARN(log)  << fn << ": received unathorized stop event, event = "
	    	    << *stopEvent << VWARN_END(log);
                    
        rc = !SUCCESS;
    }
    else
    {
        rc = onStop();
    }

    stopEvent->setReturnCode(rc);
    stopEvent->response();
    
    Sptr<Event> event = stopEvent;

    serviceMgrFifo->add(event);

    VDEBUG(log) << fn << ": returning stop = " << *stopEvent 
    	    	<< VDEBUG_END(log);
}


ReturnCode	    	
Service::shutdown(Sptr<ControlEvent> shutdownEvent)
{
    const string    fn("Service::shutdown");
    VLog    	    log(fn);

    if ( shutdownEvent == 0 )
    {
    	VDEBUG(log) << fn << ": hard shutdown." << VDEBUG_END(log);

	return ( SUCCESS );
    }
    
    FifoBase< Sptr<Event> > *	serviceMgrFifo = shutdownEvent->getReplyFifo();
    
    if ( !serviceMgrFifo || *serviceMgrFifo != getServiceManagerFifo() )
    {
    	VWARN(log)  << fn << ": received unathorized shutdown event, event = "
	    	    << *shutdownEvent << VWARN_END(log);
        return ( SUCCESS );
    }
    
    ReturnCode	rc = shutdownEvent->getReturnCode();

    ReturnCode	onShutdownRC = onShutdown();

    if ( rc == SUCCESS )
    {
	rc = onShutdownRC;
    }
    
    VDEBUG(log) << fn << ": recevied shutdown = " << *shutdownEvent 
		<< ", rc = " << rc
    	    	<< VDEBUG_END(log);

    return ( rc );
}


void	    	
Service::blockSignals()
{
    if ( mySignalAction )
    {
        Thread::self()->signalHandler().block(mySignalAction->signalSet());
    }
}
	
	
void	    	
Service::unblockSignals()
{
    if ( mySignalAction )
    {
        Thread::self()->signalHandler().unblock(mySignalAction->signalSet());
    }
}
	

int
Service::block()
{
    const string    fn("Service::block");
    VLog    	    log(fn);
    
    unblockSignals();

    int numberMsgActive = getFifo().block();

    blockSignals();

    if ( mySignalAction && mySignalAction->signalCaught() )
    {
	mySignalAction->reset();

        onSignalCaught();
    }

    return ( numberMsgActive );
}
