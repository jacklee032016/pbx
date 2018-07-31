#include "global.h"

#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "StatusMsg.hxx"
#include "OpHeldCallEndedDialing.hxx"
#include "UaStateMachine.hxx"
#include "SipContext.hxx"
#include "SipTransceiver.hxx"

using namespace Assist;

const Sptr < State > VoipOpHeldCallEndedDialing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpHeldCallEndedDialing operation" );

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

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->findContact( *msg );

    if ( contact == 0 )
    {
        //bye from an unknow person
        cpDebug(LOG_DEBUG, "Bye from an unkown person.  No action taken" );
        return 0;
    }

    cpDebug(LOG_DEBUG, "Held Call Ended" );

    //send 200 for the bye
    StatusMsg status( *msg, 200/*OK*/ );
    sipEvent->getSipStack()->sendReply( status );

    //remove this person from the contact list
    call->removeContact( contact );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateDialing" );
}
