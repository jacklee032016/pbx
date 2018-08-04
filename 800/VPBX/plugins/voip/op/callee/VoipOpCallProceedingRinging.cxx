/*
* $Id: VoipOpCallProceedingRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

//#include "pbx_globals.h"

using namespace Assist;

using Assist::SDP::SdpSession;
using Assist::SDP::SdpMedia;
using Assist::SDP::MediaAttributes;
using Assist::SDP::ValueAttribute;
using Assist::SDP::SdpRtpMapAttribute;

const Sptr <PbxState> VoipOpCallProceedingRinging::process( const Sptr <PbxEvent > event )
{
	if(checkCallMsg(event) != CALL_PROCEEDING )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

	Sptr <InviteMsg> msg = context->getRingInvite();
	assert( msg != 0 );

	Sptr  <SipSdp> remoteSdp;
	remoteSdp.dynamicCast (msg->getContentData(0));
	bool ringbackTone = false;

	//constructing local SDP
	SipSdp localSdp;
	if ( remoteSdp != 0 )
	{
		localSdp = *remoteSdp;
		Data rh=localSdp.getAddress();
		Data host =  theSystem.gethostAddress(&rh);

		if(PbxConfiguration::instance()->getNATAddress() != "")
		{
			host = PbxConfiguration::instance()->getNATAddress();
		}
        
		setStandardSdp(localSdp, host, vep->getRtpPort( ));
	}

	// get the status of the hardware available or not available or
	// available for call waiting only

	// for now support remote ringback only for the first call, not on
	// call waiting

	StatusMsg statusMsg;
	if (PbxConfiguration::instance()->getProvideRingback() && (remoteSdp != 0) )
	{
		ringbackTone = true;
		StatusMsg status(*msg, SIP_STATUS_CODE_INFO_PROGRESS);
		status.setContentData( &localSdp );
		// call->setLocalSdp( new SipSdp( localSdp ) );
		statusMsg = status;

		context->setRemoteSdp( new SipSdp( *remoteSdp ) );
		context->setLocalSdp( new SipSdp( localSdp ) );
		Sptr <SipSdp> local2Sdp = context->getLocalSdp();
		Sptr <SipSdp> remoteSdp = context->getRemoteSdp();
		
#ifndef NO_POLICY
		if (PbxConfiguration::instance()->getUsePolicyServer() )
		{
			cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
		}
#endif

		setupRsvp(*local2Sdp, *remoteSdp);
	}
	else
	{
		// If ProvideRingback is off, then we send a 180 WITHOUT SDP
		StatusMsg status( *msg, SIP_STATUS_CODE_INFO_RINGING );
		statusMsg = status;
	}

//modified by lijie
#if 0
    if ( remoteSdp != 0 && PbxConfiguration::instance()->getProvideRingback() )
    {
        // do policy / rsvp stuff

        call->setRemoteSdp( new SipSdp( *remoteSdp ) );
        call->setLocalSdp( new SipSdp( localSdp ) );

        Sptr < SipSdp > localSdp = call->getLocalSdp();
        Sptr < SipSdp > remoteSdp = call->getRemoteSdp();

#ifndef NO_POLICY
        if (PbxConfiguration::instance()->getUsePolicyServer() )
        {
            cpDebug(LOG_DEBUG,"Enable QoS handled in Marshal");
        }
#endif

	setupRsvp(*localSdp, *remoteSdp);
    }
#endif

    // TODO Call log Show caller information
	cpDebug(LOG_DEBUG, "Call:%s", msg->getFrom().encode().logData());

	//turn on remote ringback if required
	if ( ringbackTone )
	{
		LocalScopeAllocator lo;
		struct HardwareAudioRequest audioRequest;

		audioRequest.type = AudioStart;
		audioRequest.localPort = 0;

		audioRequest.remotePort = remoteSdp->getRtpPort();
		strcpy(audioRequest.remoteHost, remoteSdp->getConnAddress().getData(lo));

		audioRequest.echoCancellation = true;
		//modified by ljiie 2005-06-03
		//ringbackSignal->signalOrRequest.request.sendRingback = true;
		audioRequest.actionType = StartRingBack;
		audioRequest.rtpPacketSize = getRtpPacketSize(*(remoteSdp) );
		if(audioRequest.rtpPacketSize <= 0)
		{
			audioRequest.rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();
		}

		vep->audioStart(audioRequest);
	}

	context->setCalleeIdle(true);
	getVoipEndPoint(event)->getSipStack()->sendReply( statusMsg );

	return PBX_STATE_CONTINUE;
}

