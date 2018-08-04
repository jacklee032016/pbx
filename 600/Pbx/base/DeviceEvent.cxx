/*
 * $Id: DeviceEvent.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "DeviceEvent.hxx"


using namespace Vocal;

DeviceEvent::DeviceEvent(Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo)
    :   SipProxyEvent(outputFifo)
{
    assert( myFifo != 0 );
}


DeviceEvent::~DeviceEvent()
{
}
