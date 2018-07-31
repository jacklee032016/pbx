
#include "global.h"
#include "OpXferComplete.hxx"
#include "UaStateMachine.hxx"
#include "PbxConfiguration.hxx"
#include "SipContext.hxx"
#include "UaDevice.hxx"

#include "SipEvent.hxx"
#include "ByeMsg.hxx"
#include "CallInfo.hxx"
#include "CancelMsg.hxx"
#include "SipVia.hxx"

using namespace Assist;

const Sptr < State > VoipOpXferComplete::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpXferComplete operation" );

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

    // Check 200 OK (TRANSFER)
    int status = msg->getStatusLine().getStatusCode();
    if ( status == 200 )
    {
        SipCSeq cseq = msg->getCSeq();
        if ( cseq.getMethod() == "TRANSFER" )
        {
            SipVia sipVia;
            sipVia.setprotoVersion( "2.0" );
            sipVia.setPort( atoi( PbxConfiguration::instance()->getLocalSipPort().c_str() ) );
            sipVia.setTransport( PbxConfiguration::instance()->getSipTransport() );

            Sptr < SipContext > call;
            call.dynamicCast( event->getCallInfo() );
            assert( call != 0 );

            // Send BYE to transferee(previous contact)
            // Send BYE/CANCEL to transfer target(current contact)
            Sptr < Contact > contact;
            Sptr < ContactList > contacts = call->getContactList();
            ContactList::iterator iter = contacts->begin();
            while ( iter != contacts->end() )
            {
                contact = (*iter);
                assert( contact != 0 );
                int status = contact->getStatus();

                unsigned int cseq = contact->getCSeqNum();
                contact->setCSeqNum( ++cseq );

                if ( status == 200 )
                {
                    cpDebug(LOG_DEBUG, "BYE callee" );
                    ByeMsg msg( contact->getStatusMsg() );
		sipVia.setHost( contact->getStatusMsg().getFrom().getHost());
                    msg.flushViaList();
                    msg.setVia( sipVia, 0 );

                    SipCSeq sipCSeq = msg.getCSeq();
                    sipCSeq.setCSeq( cseq );
                    msg.setCSeq( sipCSeq );

                    sipEvent->getSipStack()->sendAsync( msg );
                }
                else if ( status < 200 && status >= 100 )
                {
                    cpDebug(LOG_DEBUG, "CANCEL contact" );
                    CancelMsg msg( contact->getInviteMsg() );

                    SipCSeq sipCSeq = msg.getCSeq();
                    sipCSeq.setCSeq( cseq );
                    msg.setCSeq( sipCSeq );

                    sipEvent->getSipStack()->sendAsync( msg );
                }
                else if ( status < 100 )
                {
                    cpDebug(LOG_DEBUG, "BYE caller" );
                    ByeMsg msg( contact->getInviteMsg() );
		sipVia.setHost(contact->getInviteMsg().getTo().getHost() );
                    msg.flushViaList();
                    msg.setVia( sipVia, 0 );

                    SipCSeq sipCSeq = msg.getCSeq();
                    sipCSeq.setCSeq( cseq );
                    msg.setCSeq( sipCSeq );

                    sipEvent->getSipStack()->sendAsync( msg );
                }
                iter++;
            }
            // Free device
            UaDevice::instance()->setCallId( 0 , event->getDeviceId() );

            Sptr < UaStateMachine > stateMachine;
            stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
            assert( stateMachine != 0 );

            //cout << "Ready" << endl;
            return stateMachine->findState( "StateIdle" );
        }
    }
    return 0;
}
