
#include "global.h"
#include "OpStartBlindXferring.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "SipContext.hxx"

using namespace Assist;

const Sptr < State > VoipOpStartBlindXferring::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStartBlindXferring operation" );

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

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateBlindXferring" );
}
