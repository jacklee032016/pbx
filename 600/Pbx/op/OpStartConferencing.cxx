/*
 * $Id: OpStartConferencing.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "OpStartConferencing.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"

///
using namespace Vocal;


OpStartConferencing::OpStartConferencing()
{}


OpStartConferencing::~OpStartConferencing()
{}


///

const char* const
OpStartConferencing::name() const
{
    return "OpStartConferencing";
}


///
const Sptr < State >
OpStartConferencing::process( const Sptr < SipProxyEvent > event )
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
