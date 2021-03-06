/*
 * $Id: OpStartBlindXferring.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStartBlindXferring.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"

///
using namespace Vocal;


OpStartBlindXferring::OpStartBlindXferring()
{}


OpStartBlindXferring::~OpStartBlindXferring()
{}


///

const char* const
OpStartBlindXferring::name() const
{
    return "OpStartBlindXferring";
}


///
const Sptr < State >
OpStartBlindXferring::process( const Sptr < SipProxyEvent > event )
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
