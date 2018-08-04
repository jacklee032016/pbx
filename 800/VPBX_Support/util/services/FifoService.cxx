/*
* $Id: FifoService.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "FifoService.hxx"


using namespace Assist;
using Assist::Services::FifoService;
using Assist::Services::Service;
using Assist::Services::Event;


FifoService::FifoService(
    const char      	    	*   name
) 
    : 	Service(name ? name : "FifoService")
{
}


FifoService::~FifoService()
{
}


FifoBase< Sptr<Event> > &   
FifoService::getFifo()
{
    return ( myFifo );
}


const FifoBase< Sptr<Event> > &   
FifoService::getFifo() const
{
    return ( myFifo );
}
