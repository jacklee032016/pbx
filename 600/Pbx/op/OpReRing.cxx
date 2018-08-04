/*
 * $Id: OpReRing.cxx,v 1.3 2007/05/11 10:54:04 lizhijie Exp $
 */

#include "global.h"
#include "OpReRing.hxx"

#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"

///
using namespace Vocal;


OpReRing::OpReRing()
{}


OpReRing::~OpReRing()
{}

const char* const OpReRing::name() const
{
	return "OpReRing";
}


const Sptr <State> OpReRing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpReRing operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventHookDown )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Going back to ReRinging State for the First Call" );

    Sptr < SipCallId > callId = deviceEvent->callId;
    assert ( callId != 0 );
    if ( callId != 0 && UaDevice::instance()->isOnCallWaiting( *callId, event->getDeviceId() ) )
    {
		// grab the hardware and remove itself from call waiting list
		UaDevice::instance()->setCallId( callId , event->getDeviceId() );
		UaDevice::instance()->removeCallWaitingId( *callId, event->getDeviceId());
    }

    //TODO: if there are sdp info for the second call, we need to clean
    //it up.

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateReRinging" );
}
