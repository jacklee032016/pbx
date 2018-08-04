/*
* $Id: ProtocolService.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ProtocolService.hxx"
#include "ProtocolFactory.hxx"


using namespace Assist;
using Assist::Services::ProtocolService;
using Assist::Services::PollFifoService;
using Assist::Transport::ProtocolFactory;
using Assist::Services::Event;
using Assist::Configuration::Config;


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
