/*
 * $Id: OpTimeFinish.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "UaTimerEvent.hxx"
#include "UaCallContainer.hxx"
#include "OpTimeFinish.hxx"
#include "UaDevice.hxx"

///
using namespace Vocal;


OpTimeFinish::OpTimeFinish()
{}


OpTimeFinish::~OpTimeFinish()
{}


///

const char* const
OpTimeFinish::name() const
{
    return "OpTimeFinish";
}


///
const Sptr < State >
OpTimeFinish::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpTimeFinish operation" );

    if ( isTimeout(event) == false )
        return 0;

    Sptr < UaTimerEvent > timerEvent;

     timerEvent.dynamicCast(event);
     			 
 // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

 /*   Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
	///
    assert( call != 0 );
*/
 //   calls->deleteCall( *(timerEvent->getCallId()), event->getDeviceId() );

    calls->deleteCall( *(UaDevice::instance()->getCallId( event->getDeviceId() )) ,event->getDeviceId());

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateIdle" );


}
