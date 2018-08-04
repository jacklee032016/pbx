/*
 * $Id: OpStartCall.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStartCall.hxx"
#include "UaDeviceEvent.hxx"
#include "UaDevice.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpStartCall::OpStartCall()
{}


OpStartCall::~OpStartCall()
{}


///

const char* const
OpStartCall::name() const
{
    return "OpStartCall";
}


///
const Sptr < State >
OpStartCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStartCall operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }
    if ( deviceEvent->type != DeviceEventHookUp )
    {
        return 0;
    }

    //    UaDevice::instance()->getDigitCollector()->reset();

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateDialing" );
}
