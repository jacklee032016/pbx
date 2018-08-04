/*
 * $Id: OpActiveCallEnded.cxx,v 1.2 2007/03/02 04:19:27 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <StatusMsg.hxx>

#include "OpActiveCallEnded.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaCallContainer.hxx"
#include "UaDevice.hxx"

#if 0
#include "UaCommandLine.hxx"  
#endif
///
using namespace Vocal;


OpActiveCallEnded::OpActiveCallEnded()
{}


OpActiveCallEnded::~OpActiveCallEnded()
{}


///

const char* const
OpActiveCallEnded::name() const
{
    return "OpActiveCallEnded";
}


///
const Sptr < State >
OpActiveCallEnded::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpActiveCallEnded operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < ByeMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Active Call Ended" );

    //send 200 for the bye
    StatusMsg status( *msg, 200/*OK*/ );
    sipEvent->getSipStack()->sendReply( status );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    // Remove call info from call container
    Sptr < UaCallContainer > calls;
    calls.dynamicCast( event->getCallContainer() );
    assert( calls != 0 );

		calls->deleteCall( *(UaDevice::instance()->getCallId(event->getDeviceId() )) , event->getDeviceId());
		// Clear call id in device
		UaDevice::instance()->setCallId( 0 , event->getDeviceId() );

#if 0		
    if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
    {
        cout << "Ready" << endl;
    }
#endif

    return stateMachine->findState( "StateIdle" );

}
