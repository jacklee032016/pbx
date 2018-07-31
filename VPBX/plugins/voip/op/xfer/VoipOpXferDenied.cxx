
#include "global.h"
#include <SipEvent.hxx>
#include <StatusMsg.hxx>

#include "OpXferDenied.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"

using namespace Assist;

const Sptr < State > VoipOpXferDenied::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpXferDenied operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }

    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );
    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }
    // Check for status > 200
    int status = msg->getStatusLine().getStatusCode();
    if ( status > 200 )
    {
        // Back to ReRing
        Sptr < UaStateMachine > stateMachine;
        stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
        assert( stateMachine != 0 );

        return stateMachine->findState( "StateReRing" );
    }
    return 0;
}
