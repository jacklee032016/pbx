/*
 * $Id: FifoService.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "FifoService.hxx"


using namespace Vocal;
using Vocal::Services::FifoService;
using Vocal::Services::Service;
using Vocal::Services::Event;


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
