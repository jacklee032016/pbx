/*
 * $Id: UaTimerEvent.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "UaTimerEvent.hxx"

using namespace Vocal;

///
UaTimerEvent::UaTimerEvent ( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
        : TimerEvent( outputFifo )
{}


///
void
UaTimerEvent::setCallId( const Sptr < SipCallId > callid )
{
    callId = callid;
}


///
const Sptr < SipCallId >
UaTimerEvent::getCallId() const
{
    return callId;
}

///
const char* const
UaTimerEvent::name() const
{
    return "User Agent Timer Event";
}


///
UaTimerEvent::~UaTimerEvent()
{}
