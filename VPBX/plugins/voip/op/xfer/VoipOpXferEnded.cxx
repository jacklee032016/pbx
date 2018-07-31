#include "global.h"
#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <StatusMsg.hxx>

#include "OpXferEnded.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"
#include "SipTransceiver.hxx"

using namespace Assist;


const Sptr < State > VoipOpXferEnded::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpXferEnded operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    // Check for BYE from Transferee or Transfer Target
    Sptr < ByeMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    // Send 200 for BYE
    StatusMsg status( *msg, 200/*OK*/ );
    sipEvent->getSipStack()->sendReply( status );

    //TODO Will the call go back to StateTrying for the transfer target?
    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateReRing" );
}
