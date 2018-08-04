/*
 * $Id: UaHardwareEvent.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "UaHardwareEvent.hxx"

using namespace Vocal;

///
UaHardwareEvent::UaHardwareEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
{
    assert( outputFifo != 0 );

    myFifo = outputFifo;
    //TODO initialize data members: type and signalOrRequest
}

///
const char* const
UaHardwareEvent::name() const
{
    return "Hardware Event";
}

void UaHardwareEvent::startTimer(Sptr < SipProxyEvent > newEvent, const int timerMs)
{
    assert( newEvent != 0 );

    myId = myFifo->addDelayMs(newEvent, timerMs);
}

///
UaHardwareEvent::~UaHardwareEvent()
{}
