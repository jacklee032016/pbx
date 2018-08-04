/*
* $Id: VoipOpCallDisconnectInCall.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "ByeMsg.hxx"

#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpCallDisconnectInCall::process( const Sptr <PbxEvent> event )
{
	if( checkCallMsg(event) != CALL_DISCONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	cpDebug(LOG_DEBUG, "Terminate Call" );
	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	Sptr <Contact> contact = context->getContact();

	if ( contact != 0 )
	{
		SipVia sipVia;
		sipVia.setprotoVersion( "2.0" );
		sipVia.setPort( atoi( PbxConfiguration::instance()->getLocalSipPort().c_str() ) );
		sipVia.setTransport( PbxConfiguration::instance()->getSipTransport() );

		unsigned int cseq = contact->getCSeqNum();
		contact->setCSeqNum( ++cseq );

		Sptr <ByeMsg> bye;
		Data hip;

		if ( contact->getStatus() != SIP_STATUS_CODE_SUCCESS_OK || (context->getAckMsg() != 0) )
		{
			// we use the ACK to create the BYE because the INVITE may not contain the tag

			Sptr <AckMsg> ack = context->getAckMsg();
			if ( ack != 0 )
			{
				cpDebug(LOG_DEBUG, "Sending BYE to caller" );
				bye = new ByeMsg( *ack );
				//set the Transport also 
				SipRequestLine& reqLine = bye->getMutableRequestLine();
				hip=reqLine.getHost();
				reqLine.setTransportParam(PbxConfiguration::instance()->getSipTransport());

				/** Added proxy authorization for BYE from vmserver */   
				//bye->setProxyAuthorization( msg.getProxyAuthorization() );
			}
			else
			{
				cpLog( LOG_ERR , "No ACK msg to produce BYE" );

				/* added by lizhijie, for local device call, 2005.02.17 */
				Sptr< InviteMsg > invite = context->getRingInvite();
				bye = new ByeMsg( *invite );
				SipRequestLine& reqLine = bye->getMutableRequestLine();
				hip=reqLine.getHost();
				reqLine.setTransportParam(PbxConfiguration::instance()->getSipTransport());
				/* end of added */
			}
		}
		else  // status == 200
		{
			const StatusMsg& msg = contact->getStatusMsg();
			if ( &msg != 0 )
			{
				cpDebug(LOG_DEBUG, "Sending BYE to callee" );
				bye = new ByeMsg( msg );
				//set the Transport also 
				SipRequestLine& reqLine = bye->getMutableRequestLine();
				Data tmph=reqLine.getHost();
				hip= theSystem.gethostAddress(&tmph);
				reqLine.setTransportParam(PbxConfiguration::instance()->getSipTransport());
			}
			else
			{
				cpLog( LOG_ERR , "No status message for BYE" );
			}
		}

		if( bye == 0 )
		{
			cpLog( LOG_ERR, "bye == 0, unable to send BYE msg" );
			return PBX_STATE_CONTINUE;
		}

		bye->flushViaList();
		sipVia.setHost( hip );
		bye->setVia( sipVia, 0 );

		SipCSeq sipCSeq = bye->getCSeq();
		sipCSeq.setCSeq( cseq );
		bye->setCSeq( sipCSeq );

		vector <SipRoute*> routeList;

		// add the route info
		// We haven't saved route info from the ACK.  We just use
		// what was saved from the INVITE.  Is this ok?
		InviteMsg invMsg = contact->getInviteMsg();
		if ( *(context->getRingInvite()) == invMsg )
		{
			cpDebug(LOG_DEBUG, "adding route info for the first call" );
			routeList = context->getRoute1List();
		}
		else
		{
			cpDebug(LOG_DEBUG, "adding route info for the second call" );
			routeList = context->getRoute2List();
		}

 		cpDebug(LOG_DEBUG, "Test");

		if ( routeList.size() > 0 )
		{
			bye->setRouteList( routeList );
			//
			SipRoute siproute = bye->getRoute(0);
			bye->removeRoute(0);

			SipRequestLine reqLine = bye->getRequestLine();
			reqLine.setUrl( siproute.getUrl() );
			reqLine.setTransportParam(PbxConfiguration::instance()->getSipTransport());
			bye->setRequestLine( reqLine );
		}

		vep->getSipStack()->sendAsync( *bye );

		cpDebug(LOG_DEBUG, "Bye :%s", bye->getTo().encode().logData());

#ifdef HAS_RSVPLIB
        // RSVP stuff ported from old OpTerminateCall.cxx
		if ( PbxConfiguration::instance()->getRsvpOn() )
		{
			UaDevice::instance()->getSuaRsvpReserv().close();
			UaDevice::instance()->getSuaRsvpSender().close();
		}
#endif

#ifndef NO_POLICY
		if ( PbxConfiguration::instance()->getUsePolicyServer() )
		{
			cpDebug(LOG_DEBUG,"Disble QoS handled in Marshal");
		}
#endif

#if __CDR_OPEN__
		if (SimulateDB::instance() != NULL)
		{
			int deviceID = event->getDeviceId();
			SimulateDB::instance()->addCdrClient(bye, deviceID, CALL_END);
		}
#endif
	}
	else
	{
		cpLog( LOG_ERR , "No current contact" );
	}

	
#if 0
    // if there is a call on call waiting, send that call an onhook evnet
    Sptr < SipCallId > call2Id = UaDevice::instance()->getCallWaitingId(event->getDeviceId());
    if ( call2Id != 0 )
    {
        //if ( PbxConfiguration::instance()->getCallWaitingOn() )
        if ( UaDevice::instance()->getCallWaitingOn(event->getDeviceId()) )
        {
            // send a message to the call on call waiting, notifying
            // it that the active call is completed
            Sptr < UaCallContainer > calls;
            calls.dynamicCast( event->getCallContainer() );
            assert( calls != 0 );

			Sptr < SipContext > call2 = calls->findCall( *call2Id, event->getDeviceId() );
			
            if ( call2 == 0 )
            {
                cpLog( LOG_ERR, "No call info found for the call"
		       " on call waiting");
                return 0;
            }

		UaDevice::instance()->setCallId( call2Id ,event->getDeviceId()  );
		
            Sptr < Fifo < Sptr < SipProxyEvent > > > eventQ = deviceEvent->getFifo();
			
			Sptr < UaDeviceEvent > signal = new UaDeviceEvent( eventQ );
			signal->type = DeviceEventHookDown;
			//signal->callId = call2Id;
			signal->setDeviceId( event->getDeviceId() );
			
			eventQ->add( signal );
        }
    }
#endif

	Sptr <AudioChannel> audio = vep->getAudio();
	assert(audio != 0);
	audio->deactivate();
	
	vep->reset();
	
	return lookupEPState(event, VOIP_EP_STATE_IDLE);
}

