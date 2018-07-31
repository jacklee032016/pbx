#include "global.h"
#include "OpXferTargetInviteOk.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"

#include "SipEvent.hxx"
#include "StatusMsg.hxx"
#include "AckMsg.hxx"
#include "CancelMsg.hxx"
#include "SipTransceiver.hxx"

using namespace Assist;

const Sptr < State > VoipOpXferTargetInviteOk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpXferTargetInviteOk operation" );

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

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    SipCSeq cseq = msg->getCSeq();
    if ( (int)(contact->getCSeqNum()) > cseq.getCSeqData().convertInt() )
    {
        if ( cseq.getMethod() == "INVITE" )
        {
            // INVITE transfer target transaction
            int status = msg->getStatusLine().getStatusCode();

            if ( status >= 200 )
            {
                cpDebug(LOG_DEBUG, "ACK for INVITE to transfer target" );
                AckMsg ack( *msg );
                sipEvent->getSipStack()->sendAsync( ack );
            }
            if ( status > 200 )
            {
                //cout << "Transfer Target INVITE status: " << status << endl;
                cpDebug(LOG_DEBUG, "INVITE transfer target failed status %d",
                       status );

                if ( contact->getStatus() < 200 )
                {
                    cpDebug(LOG_DEBUG, "CANCEL transfer target hold" );
                    InviteMsg inviteMsg = contact->getInviteMsg();
                    CancelMsg cancel( inviteMsg );
                    sipEvent->getSipStack()->sendAsync( cancel );
                }

                Sptr < UaStateMachine > stateMachine;
                stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
                assert( stateMachine != 0 );

                return stateMachine->findState( "StateReRinging" );
            }
        }
    }
    return 0;
}
