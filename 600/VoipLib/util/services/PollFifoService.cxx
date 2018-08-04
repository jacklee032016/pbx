/*
 * $Id: PollFifoService.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "PollFifoService.hxx"
#include "Protocol.hxx"
#include "SystemException.hxx"
#include "ProtocolException.hxx"
#include "VLog.hxx"
#include <cassert>


using namespace Vocal;
using Vocal::Services::PollFifoService;
using Vocal::Services::Event;
using Vocal::Transport::Protocol;
using Vocal::Transport::ProtocolException;
using Vocal::Logging::VLog;
using Vocal::SystemException;


PollFifoService::PollFifoService(
    const char      	    	*   name
)
    :	Service(name ? name : "PollFifoService")
{
}


PollFifoService::~PollFifoService()
{
}


FifoBase< Sptr<Event> > &   
PollFifoService::getFifo()
{
    return ( myFifo );
}
		

const FifoBase< Sptr<Event> > &   
PollFifoService::getFifo() const
{
    return ( myFifo );
}
		

void	    	    
PollFifoService::registerProtocol(Protocol & protocol)
{
    const string    fn("PollFifoService::registerProtocol");
    VLog    	    log(fn);
    
    myFifo.registerProtocol(protocol);
}


void	    	    
PollFifoService::updateProtocol(Protocol & protocol)
{
    const string    fn("PollFifoService::updateProtocol");
    VLog    	    log(fn);
    
    myFifo.updateProtocol(protocol);
}


void	    	    
PollFifoService::unregisterProtocol(Protocol & protocol)
{
    const string    fn("Service::unregisterProtocol");
    VLog    	    log(fn);
    
    myFifo.unregisterProtocol(protocol);
}


void	
PollFifoService::onVException(VException & exception)
{
    // See if we have a transport exception.
    //
    SystemException  *   systemException 
    	= dynamic_cast<SystemException *>(&exception);
	
    if ( systemException != 0 )
    {
    	onSystemException(*systemException);
	return;
    }

    // See if we have a protocol exception.
    //
    ProtocolException  *   protocolException 
    	= dynamic_cast<ProtocolException *>(&exception);

    if ( protocolException != 0 )
    {
    	onProtocolException(*protocolException);
	return;
    }

    // Do the default behavior.
    //    
    Service::onVException(exception);
}


void	    	
PollFifoService::onSystemException(SystemException & systemException)
{
    const string    fn("PollFifoService::onSystemException");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": shutting down." << VDEBUG_END(log);
    
    hardShutdown(systemException.getError());
}
		

void	    	
PollFifoService::onProtocolException(ProtocolException & protocolException)
{
    const string    fn("PollFifoService::onProtocolException");
    VLog    	    log(fn);
    
    VDEBUG(log) << fn << ": ignoring." << VDEBUG_END(log);
}		


bool	    
PollFifoService::processMessages(int numberFdsActive, ReturnCode & rc)
{
    const string    fn("PollFifoService::processMessages");
    VLog    	    log(fn);
    
    bool    	    done = Service::processMessages(numberFdsActive, rc);
    
    if ( !done )
    {
    	processProtocols(numberFdsActive);
    }

    return ( done );
}


void	    	
PollFifoService::processProtocols(int numberFdsActive)
{
    const string    fn("PollFifoService::processProtocols");
    VLog    	    log(fn);

    myFifo.processProtocols(numberFdsActive);
}
