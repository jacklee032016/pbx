
#include "global.h"
#include "AckMsg.hxx"

#include "OpConferencing.hxx"
#include "UaDevice.hxx"
#include "PbxConfiguration.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"
#include "SipVia.hxx"

using namespace Assist;

const Sptr < State > VoipOpConferencing::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpConferencing operation" );

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

    // get previous INVITE message
    Sptr < SipMsgQueue > sipReceive = sipEvent->getSipMsgQueue();

    assert( sipReceive != 0 );

    Sptr < SipMsg > prevMsg;
    Sptr < InviteMsg > inviteMsg;
    SipMsgQueue::reverse_iterator i = sipReceive->rbegin();
    i++;     // skip the status message we just received
    while ( i != sipReceive->rend() )
    {
        prevMsg = *i;
        inviteMsg.dynamicCast( prevMsg );
        if ( inviteMsg != 0 )
        {
            cpDebug(LOG_DEBUG, "Found invite" );
            break;
        }
        ++i;
    }
    if ( inviteMsg == 0 )
    {
        return 0;
    }

    if ( inviteMsg->computeCallLeg() == call->getContact()->getInviteMsg().computeCallLeg() )
    {
        cpDebug(LOG_DEBUG, "Start Conferencing");
    }
    else
    {
        return 0;
    }

    bool addingUser = false;

    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( inviteMsg->getContentData(0));
    if ( sipSdp->getSdpDescriptor().isHold() == false )
        return 0;

    // Hold transaction
    int status = msg->getStatusLine().getStatusCode();
    if ( status >= 200 )
    {
        cpDebug(LOG_DEBUG, "ACK for INVITE to transfer target" );
        AckMsg ack( *msg );

        vector < SipRoute* > routeList;
        // add the route info
        if ( *(call->getRingInvite()) == contact->getInviteMsg() )
        {
            cpDebug(LOG_DEBUG, "adding route info for the first call" );
            routeList = call->getRoute1List();
        }
        else
        {
            cpDebug(LOG_DEBUG, "adding route info for the second call" );
            routeList = call->getRoute2List();
        }

        if ( routeList.size() > 0 )
        {
            ack.setRouteList( routeList );
            //
            SipRoute siproute = ack.getRoute(0);
            ack.removeRoute(0);

            SipRequestLine reqLine = ack.getRequestLine();

	    Sptr< BaseUrl > baseUrl = siproute.getUrl();
	    reqLine.setUrl( baseUrl );
            ack.setRequestLine( reqLine );
        }

        //send ack for the "hold" re-invite
        sipEvent->getSipStack()->sendAsync( ack );

        if ( status == 200 )
        {
            // Send TRANSFER to transferees
            Sptr< SipUrl > transferToUrl = new SipUrl;
            try
            {
                Data urlStr = Data( string("sip:") + PbxConfiguration::instance()->getConfServer() );
                Sptr< SipUrl > tmpUrl = new SipUrl( urlStr );
                transferToUrl = tmpUrl;
            }
            catch ( SipUrlParserException exception )
            {
                cpLog( LOG_ERR, "Invalid url for conference server" );
                assert( 0 );
            }

            Sptr < InviteMsg > msg = call->getRingInvite();
            assert( msg != 0 );

	    Sptr< BaseUrl > baseUrl = msg->getTo().getUrl();
	    Sptr< SipUrl > tmpUrl;
	    tmpUrl.dynamicCast( baseUrl );

            if ( *tmpUrl == *transferToUrl )
            {
                addingUser = true;
            }
            else
            {
                sendTransfer( msg, transferToUrl, call, sipEvent );
            }

            Sptr < InviteMsg > msg2 = call->getRing2Invite();
            assert( msg2 != 0 );

	    baseUrl = msg2->getTo().getUrl();
	    tmpUrl.dynamicCast( baseUrl );

            if ( *tmpUrl == *transferToUrl )
            {
                addingUser = true;
            }
            else
            {
                sendTransfer( msg2, transferToUrl, call, sipEvent );
            }

            if ( addingUser == false )
            {
                //send invite msg to conference server
                InviteMsg inviteMsg( *msg2 );
                inviteMsg.setCallId( msg->getCallId() );

                SipTo to = inviteMsg.getTo();
                to.setUrl( transferToUrl );
                to.setDisplayName( transferToUrl->getUserValue() );
                inviteMsg.setTo( to );

		//force invite to go through proxy server if a proxy exists
		string proxyUrlStr = PbxConfiguration::instance()->getProxyServer();

		SipRequestLine reqLine = inviteMsg.getRequestLine();
		reqLine.setUrl( transferToUrl );

        	if ( proxyUrlStr.length() > 0 )
		{
		    Data urlStr = Data( string("sip:") + proxyUrlStr );
		    Sptr< SipUrl > proxyUrl = new SipUrl( urlStr );

		    baseUrl = reqLine.getUrl();
		    assert( baseUrl != 0 );
		    if( baseUrl->getType() == TEL_URL )
		    {
			cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
			assert( 0 );
		    }
		    // Assume we have a SIP_URL
		    Sptr< SipUrl > reqUrl;
		    reqUrl.dynamicCast( baseUrl );
		    assert( reqUrl != 0 );

		    reqUrl->setHost( proxyUrl->getHost() );
		    reqUrl->setPort( proxyUrl->getPort() );
		    reqLine.setUrl( reqUrl );
		}
		inviteMsg.setRequestLine( reqLine );

                sipEvent->getSipStack()->sendAsync( inviteMsg );

                // Save the invite
                call->setRingInvite( new InviteMsg( inviteMsg ) );
                call->setRing2Invite( 0 );

                Sptr < Contact > contact = new Contact( inviteMsg );
                // Set as current contact
                call->setContact( contact );
                // Add to contact list
                call->addContact( contact );
            }
            else
            {
                //send re-invite to conference server
                cpDebug(LOG_DEBUG, "re-invite callee" );
                Sptr < InviteMsg > reInvite;
                Sptr < InviteMsg > invite = call->getRingInvite();
                assert( invite != 0 );
                //reInvite = *invite;

                call->setRing2Invite( 0 );

                //set current contact to the conference server
                Sptr < Contact > contact = call->findContact(*invite);
                assert( contact != 0 );
                call->setContact( contact );

                const StatusMsg& sMsg = contact->getStatusMsg();
                if ( &msg != 0 )
                {
                    reInvite = new InviteMsg( sMsg );
                    //add SDP
                    Sptr < SipSdp > localSdp = call->getLocalSdp();
                    assert( localSdp != 0 );
                    SipSdp sipSdp = *localSdp;
                    reInvite->setContentData( &sipSdp );
                }
                //TODO Is it going to be a problem if the other side also
                //use the next TODO CSeq at the same time?
                unsigned int cseq = contact->getCSeqNum();
                contact->setCSeqNum( ++cseq );
                SipCSeq sipCSeq = reInvite->getCSeq();
                sipCSeq.setCSeq( cseq );
                reInvite->setCSeq( sipCSeq );

                //add route info
                vector < SipRoute* > routeList;
                // add the route info
                cpDebug(LOG_DEBUG, "adding route info for the first call" );
                routeList = call->getRoute1List();

                if ( routeList.size() > 0 )
                {
                    reInvite->setRouteList( routeList );
                    //
                    SipRoute siproute = reInvite->getRoute(0);
                    reInvite->removeRoute(0);

                    SipRequestLine reqLine = reInvite->getRequestLine();
		    baseUrl = siproute.getUrl();
                    reqLine.setUrl( baseUrl );
                    reInvite->setRequestLine( reqLine );
                }

                //send re-invite
                sipEvent->getSipStack()->sendAsync( *reInvite );
            }
        }
        else
        {
            //cout << "Transfer Target Hold status: " << status << endl;
            cpDebug(LOG_DEBUG, "Transfer target Hold failed status %d",
                   status );
            Sptr < UaStateMachine > stateMachine;
            stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
            assert( stateMachine != 0 );

            return stateMachine->findState( "StateReRinging" );
        }
    }

    return 0;
}

///
void
OpConferencing::sendTransfer( Sptr < InviteMsg > msg,
			      Sptr < SipUrl > transferToUrl,
			      Sptr < SipContext > call,
			      const Sptr < SipEvent > sipEvent )
{

    cpDebug(LOG_DEBUG, "\nInviteMsg used to construct Transfer\n%s", msg->encode().logData() );

    Sptr < Contact > contact = call->findContact( *msg );
    assert( contact != 0 );

    SipVia sipVia;
    sipVia.setprotoVersion( "2.0" );
    sipVia.setHost( Data(theSystem.gethostAddress(&transferToUrl->getHost()) ) );
    sipVia.setPort( atoi( PbxConfiguration::instance()->getLocalSipPort().c_str() ) );
    sipVia.setTransport( PbxConfiguration::instance()->getSipTransport() );

    if ( contact->getStatus() == 200 )
    {
        cpDebug(LOG_DEBUG, "Send TRANSFER to original callee" );
        StatusMsg status = contact->getStatusMsg();
        cpDebug(LOG_DEBUG, "\n\n%s", status.encode().logData() );

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

    return ;
}
