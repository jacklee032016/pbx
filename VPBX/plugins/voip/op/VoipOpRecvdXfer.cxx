#include "global.h"
#include <SipEvent.hxx>
#include <TransferMsg.hxx>

#include "OpRecvdXfer.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"
#include "SipTransceiver.hxx"

using namespace Assist;

const Sptr < State > VoipOpRecvdXfer::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpRecvdXfer operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < TransferMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Being Transferred");

    StatusMsg status( *msg, 100/*Trying*/ );
    sipEvent->getSipStack()->sendReply( status );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateBeingXferred" );

}
