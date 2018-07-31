/*
* $Id: VoipOpMsgStatusInfoTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

/* process SIP Status msg when in TRYING state */
const Sptr <PbxState> VoipOpMsgStatusInfoTrying::process( const Sptr <PbxEvent > event )
{
	Sptr <SipMsg> sipMsg = isSipMsg(event);
	if( sipMsg == 0 )
		return PBX_STATE_CONTINUE;

	Sptr  <StatusMsg> msg;
	msg.dynamicCast( sipMsg );
	if ( msg == 0 )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> call = getSipContext(event);
	assert( call != 0 );

	int statusCode = msg->getStatusLine().getStatusCode();
	if ( statusCode != SIP_STATUS_CODE_INFO_TRYING 
		&& statusCode != SIP_STATUS_CODE_INFO_QUEUED 
		&& statusCode != SIP_STATUS_CODE_INFO_PROGRESS )
	{
		return PBX_STATE_CONTINUE;
	}

//remove for long time ring no response problem ,modified by lijie
#if 0
	if ( cancelTimer(event) )
	{
		cpDebug(LOG_DEBUG, "Ua Timer cancelled" );
	}
#endif 

	// This is a temporary fix to handle the case in which more than one
	// 180/183 messages are received before 200 arrives
	// TODO: check sdp in the 180/183.  If different from previous 180/183,
	// TODO :  use the lastest 180/183 sdp
	Sptr <Contact> contact = call->findContact( *msg );
	assert( contact != 0 );
	
	int status = contact->getStatus();
	if ( status == SIP_STATUS_CODE_INFO_TRYING || status == SIP_STATUS_CODE_INFO_PROGRESS || status == SIP_STATUS_CODE_INFO_QUEUED )
	{
		cpLog( LOG_ERR, "Have existing 180/182/183 message" );
		cpLog( LOG_ERR, "Igore the current 180/182/183 message" );
		
		return PBX_STATE_DONE_WITH_EVENT;
	}

	//added by lijie 2005-06-03
	if(statusCode == SIP_STATUS_CODE_INFO_QUEUED)
	{
		cancelTimer(event);
	}

	bool remoteRingback = true;

	//update contact
	contact->update( *msg );

	//figure out if this is the first call or the second call
	Sptr <SipSdp> localSdp;
	Sptr <InviteMsg> inviteMsg = call->getRingInvite();

	InviteMsg invMsg = call->getContact()->getInviteMsg();
	if ( *inviteMsg == invMsg )
	{
		localSdp = call->getLocalSdp();
	}
	else
	{
		cpDebug(LOG_DEBUG, "INVITE did not match, getting Local2 SDP");
		cpDebug(LOG_DEBUG_STACK, "Original: %s", (inviteMsg)->encode().logData());
		cpDebug(LOG_DEBUG_STACK, "New: %s", invMsg.encode().logData());
		localSdp = call->getLocal2Sdp();
	}

	/// should this really be an assert?  this seems dangerous
	assert( localSdp != 0 );

//	int rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();
	Sptr <SipSdp> remoteSdp;
	remoteSdp.dynamicCast( sipMsg->getContentData(0) );

	if( remoteSdp == 0 )
	{
		cpDebug(LOG_DEBUG, "No remote SDP in 180/183" );
		remoteRingback = false;
	}
	else
	{
		// get the remote SDP's rtp rate
		int rtpPacketSize = getRtpPacketSize(*remoteSdp);
		if(rtpPacketSize > 0)
		{
			setRtpPacketSize(*localSdp, rtpPacketSize);
			call->setRemoteSdp( new SipSdp( *remoteSdp ) );
		}
		else
		{
			remoteRingback = false;
		}
	}

#if 0
	if ( remoteRingback )
	{
		cpDebug(LOG_DEBUG, "Early RTP media" );
		call->getContact()->setRemoteRingback( true );

		signal->type = HardwareAudioType;
		struct HardwareAudioRequest* request  = &(signal->signalOrRequest.request);
		request->type = AudioStart;	

		strcpy( request->remoteHost, "\0" );
		request->remotePort = 0;
		LocalScopeAllocator lo;
		strcpy( request->localHost, localSdp->getConnAddress().getData(lo) );
		request->localPort = localSdp->getRtpPort();

		//        request->echoCancellation = true;
		request->rtpPacketSize = rtpPacketSize;
	}
	else
	{
		call->getContact()->setRemoteRingback( false );
		signal->type = HardwareSignalType;
		//below is added by lijie	
		if(statusCode != 182)
			signal->signalOrRequest.signal = DeviceSignalLocalRingbackStart;
		else
		{
			signal->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
		}
	}
#endif

	sendCallSignal(event, CALL_ALERTING);

	return PBX_STATE_DONE_WITH_EVENT;
}

