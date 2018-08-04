
/*$Id: VoipOpBlindXferHoldOk.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $*/

#include "global.h"
#include "AckMsg.hxx"
#include "OpBlindXferHoldOk.hxx"
#include "SipEvent.hxx"
#include "SipTransceiver.hxx"
#include "StatusMsg.hxx"
#include "SystemInfo.hxx"
#include "TransferMsg.hxx"
#include "SipContext.hxx"
#include "PbxConfiguration.hxx"


#include "UaStateMachine.hxx"
#include "SipVia.hxx"

using namespace Assist;

const Sptr < State > VoipOpBlindXferHoldOk::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBlindXferHoldOk operation" );

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
    if ( (int)(contact->getCSeqNum()) == cseq.getCSeqData().convertInt() )
    {
        if ( cseq.getMethod() == "INVITE" )
        {
            // Hold transaction
            int status = msg->getStatusLine().getStatusCode();

            if ( status == 100 )
                return 0;

            if ( status >= 200 )
            {
                cpDebug(LOG_DEBUG, "ACK for INVITE to transfer target" );
                AckMsg ack( *msg );
                sipEvent->getSipStack()->sendAsync( ack );
            }

            if ( status >= 180 && status <= 200 )
            {
                // Send TRANSFER to transferee
                Sptr < InviteMsg > msg = call->getRingInvite();
                assert( msg != 0 );
                Sptr < InviteMsg > msg2 = call->getRing2Invite();
                assert( msg2 != 0 );

                cpDebug(LOG_DEBUG, "\n\n%s", msg->encode().logData() );

                Sptr < Contact > contact = call->findContact( *msg );
                assert( contact != 0 );

                SipVia sipVia;
                sipVia.setprotoVersion( "2.0" );
		Data msg2host=msg2->getTo().getHost();
		Data hip= theSystem.gethostAddress(&msg2host);
                sipVia.setHost(hip);
                sipVia.setPort( atoi( PbxConfiguration::instance()->getLocalSipPort().c_str() ) );
                sipVia.setTransport( PbxConfiguration::instance()->getSipTransport() );

		// pull out the sipUrl
		Sptr< BaseUrl > baseUrl = msg2->getTo().getUrl();
		assert( baseUrl != 0 );
		if( baseUrl->getType() == TEL_URL )
		{
		    cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		    assert( 0 );
		}
		// Assume we have a SIP_URL
		Sptr< BaseUrl > transferToUrl;
		transferToUrl = baseUrl;

                if ( contact->getStatus() == 200 )
                {
                    cpDebug(LOG_DEBUG, "Send TRANSFER to original callee" );
                    const StatusMsg& status = contact->getStatusMsg();
                    //cpDebug(LOG_DEBUG, "\n\n%s", status.encode().logData() );

                    assert( &status != 0 );

                    TransferMsg transferMsg( status, transferToUrl );
                    transferMsg.flushViaList();
                    transferMsg.setVia( sipVia, 0 );
                    sipEvent->getSipStack()->sendAsync( transferMsg );
                }
                else
                {
                    cpDebug(LOG_DEBUG, "Send TRANSFER to original caller" );

                    TransferMsg transferMsg( *msg, transferToUrl );
                    transferMsg.flushViaList();
                    transferMsg.setVia( sipVia, 0 );
                    sipEvent->getSipStack()->sendAsync( transferMsg );
                }
            }
            else
            {
#if 0            
                if ( !UaCommandLine::instance()->getBoolOpt( "voicemail" ) )
                {
                    cout << "Transfer Target Hold status: " << status << endl;
                }
#endif				
                cpDebug(LOG_DEBUG, "Transfer target Hold failed status %d",
                       status );
                Sptr < UaStateMachine > stateMachine;
                stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
                assert( stateMachine != 0 );

                return stateMachine->findState( "StateReRinging" );
            }
        }
    }
    return 0;
}
