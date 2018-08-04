/*
 * $Id: ProtocolService.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "global.h"
#include "ProtocolService.hxx"
#include "ProtocolFactory.hxx"


using namespace Vocal;
using Vocal::Services::ProtocolService;
using Vocal::Services::PollFifoService;
using Vocal::Transport::ProtocolFactory;
using Vocal::Services::Event;
using Vocal::Configuration::Config;


ProtocolService::ProtocolService(
    const Config	    &	config,
    const char              *   name
)
    :   PollFifoService(name),
        myConfig(config)
{
}


ProtocolService::~ProtocolService()
{
    clear();
}


ReturnCode  
ProtocolService::onStart()
{
    ReturnCode rc = SUCCESS;
    try
    {
        createProtocols();
    }
    catch ( ... )
    {
        rc = !SUCCESS;
    }
    return ( rc );
}


ReturnCode  
ProtocolService::onStop()
{
    clear();
    return ( SUCCESS );
}


ReturnCode	    
ProtocolService::onShutdown()
{
    clear();
    return ( SUCCESS );
}


void    
ProtocolService::createProtocols()
{
    // TODO
}
        

void    
ProtocolService::clear()
{
    myProtocols.clear();
}
