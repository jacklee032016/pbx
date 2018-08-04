/*
 * $Id: TimerEvent.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include "TimerEvent.hxx"
#include "CallContainer.hxx"

using namespace Vocal;


TimerEvent::TimerEvent( 
    Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo 
)
    :   SipProxyEvent(outputFifo)
{
}


TimerEvent::~TimerEvent()
{
}


void
TimerEvent::cancelTimer()
{
    myFifo->cancel(myId);
}


void
TimerEvent::startTimer(Sptr < SipProxyEvent > newEvent, const int timerMs)
{
    assert( newEvent != 0 );

    myId = myFifo->addDelayMs(newEvent, timerMs);
}


void
TimerEvent::removeCall()
{
    assert( myCallInfo != 0 );
    assert( myCallContainer != 0 );

    Sptr < SipCallLeg > call = getCallInfo()->getSipCallLeg();
    assert( call != 0 );

    myCallContainer->removeCall(call);
}


FifoEventId
TimerEvent::getTimerId()
{
    return ( myId );
}


void
TimerEvent::setSipStack(const Sptr < SipTransceiver > sipStack)
{
    assert( sipStack != 0 );

    mySipStack = sipStack;
}



Sptr < SipTransceiver >
TimerEvent::getSipStack() const
{
    return ( mySipStack );
}
