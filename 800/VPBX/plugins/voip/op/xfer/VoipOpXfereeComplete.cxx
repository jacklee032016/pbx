#include "global.h"

#include <SipEvent.hxx>
#include <ByeMsg.hxx>
#include <StatusMsg.hxx>

#include "OpXfereeComplete.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"
#include "SipTransceiver.hxx"

using namespace Assist;

const Sptr < State > VoipOpXfereeComplete::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpXfereeComplete operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    //look for bye from the originator of the transfer
    Sptr < ByeMsg > byeMsg;
    byeMsg.dynamicCast( sipMsg );

    if ( byeMsg == 0 )
    {
        return 0;
    }

    cpDebug(LOG_DEBUG, "Xferee Complete");

    //reply with a 200 OK
    StatusMsg status(*byeMsg, 200/*OK*/);
    sipEvent->getSipStack()->sendReply( status );

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    int statusCode = call->getContact()->getStatus();

    if ( statusCode == 180 || statusCode == 183 )
    {
        // clear the contact list and add current contact to the contact
        call->clearContactList();
        call->addContact( contact );
        return stateMachine->findState( "StateTrying" );
    }
    else if ( statusCode == 200 )
    {
        // clear the contact list and add current contact to the contact
        call->clearContactList();
        call->addContact( contact );
        return stateMachine->findState( "StateInCall" );
    }
    else
    {
        return 0;
    }
}
