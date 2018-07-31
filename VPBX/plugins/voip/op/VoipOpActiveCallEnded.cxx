/*$Id*/

#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <StatusMsg.hxx>

#include "OpActiveCallEnded.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"
#include "UaCallContainer.hxx"
#include "UaDevice.hxx"

using namespace Assist;

const Sptr <State> VoipOpActiveCallEnded::process( const Sptr < SipProxyEvent > event )
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
