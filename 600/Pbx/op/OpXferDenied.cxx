/*
 * $Id: OpXferDenied.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */

#include "global.h"
#include <SipEvent.hxx>
#include <StatusMsg.hxx>

#include "OpXferDenied.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"


///
using namespace Vocal;


OpXferDenied::OpXferDenied()
{}


OpXferDenied::~OpXferDenied()
{}


///

const char* const
OpXferDenied::name() const
{
    return "OpXferDenied";
}


///
const Sptr < State >
OpXferDenied::process( const Sptr < SipProxyEvent > event )
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
