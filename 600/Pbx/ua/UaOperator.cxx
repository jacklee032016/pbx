/*
 * $Id: UaOperator.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>

#include "UaOperator.hxx"
#include "UaTimerEvent.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "CallContainer.hxx"

using namespace Vocal;

///
UaOperator::UaOperator()
{}


UaOperator::~UaOperator()
{}


///

const char* const
UaOperator::name() const
{
    return "UaOperator";
}

///
bool
UaOperator::setTimer( const Sptr < SipProxyEvent > event, unsigned int timeout )
{
    if ( timeout <= 0 ) return false;

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    Sptr < UaDigitTimerEvent > digitTimerEvent;
    digitTimerEvent.dynamicCast( event );

    if ( sipEvent == 0 && deviceEvent == 0 && digitTimerEvent == 0 )
    {
        cpDebug(LOG_DEBUG, "Expecting sipEvent, deviceEvent or digitTimerEvent" );
        assert( 0 );
    } /*if sipEvent  or deviceEvent */

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Fifo < Sptr < SipProxyEvent > > > msgQ;
    Sptr < SipTransceiver > sipStack;
    Sptr < SipCallId > callId;

    if ( sipEvent != 0 )
    {
        msgQ = sipEvent->getFifo();
        sipStack = sipEvent->getSipStack();
        Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
        assert( sipMsg != 0 );
        callId = new SipCallId( sipMsg->getCallId() );
    }
    else if ( deviceEvent != 0 )
    {
        msgQ = deviceEvent->getFifo();
        sipStack = deviceEvent->getSipStack();
        callId = deviceEvent->callId;
    }
    else if (digitTimerEvent !=0 )
    {
        msgQ = digitTimerEvent->getFifo();
        sipStack = digitTimerEvent->getSipStack();
        callId = digitTimerEvent->callId;
    }

    Sptr < UaTimerEvent > timerEvent = new UaTimerEvent( msgQ );
    timerEvent->setCallId(callId);

	/* added by lizhijie */
	timerEvent->setDeviceId( event->getDeviceId() );

    cpDebug(LOG_DEBUG, "Start TIMER for %d s.", timeout );

    // SAve the timer event in  Call Info
    call->setTimer (timerEvent); 
    timerEvent->setCallInfo( call,
                             event->getCallContainer() );
    // Set the sipStack
    timerEvent->setSipStack( sipStack );

    timerEvent->startTimer(timerEvent, timeout*1000);

    return true;
}  //setTimer

///
bool
UaOperator::cancelTimer( const Sptr < SipProxyEvent > event )
{
    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    Sptr < UaDigitTimerEvent > digitTimerEvent;
    digitTimerEvent.dynamicCast( event );

    if ( sipEvent == 0 && deviceEvent == 0 && digitTimerEvent == 0 )
    {
        cpDebug(LOG_DEBUG, "Expecting sipEvent, deviceEvent or digitTimerEvent." );
        return false;
    } /*if sipEvent  or deviceEvent */

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    if ( call->getTimer() != 0 )
    {
        cpDebug(LOG_DEBUG, "Cancelling timer." );
        call->getTimer()->cancelTimer();
	  //call->removeTimer();
        return true;
    }  //getTimer

    cpLog( LOG_ERR, "No timer was found." );
    return false;
}  // cancelTiemr

///
bool
UaOperator::isTimeout( const Sptr < SipProxyEvent > event )
{
    Sptr < UaTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );

    if ( timerEvent != 0 )
    {
        return true;
    }
    else
    {
        return false;
    }
}
