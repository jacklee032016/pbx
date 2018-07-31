#include "global.h"
#include "OpStartConferencing.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "SipContext.hxx"

using namespace Assist;

const Sptr < State > VoipOpStartConferencing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStartConferencing operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }

    if ( deviceEvent->type != DeviceEventFlash )
    {
        return 0;
    }

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateConferencing" );

}
