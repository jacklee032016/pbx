/*
* $Id: VoipOpCallConnectRinging.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

using Assist::SDP::SdpSession;
using Assist::SDP::SdpMedia;
using Assist::SDP::MediaAttributes;
using Assist::SDP::ValueAttribute;
using Assist::SDP::SdpRtpMapAttribute;

const Sptr <PbxState> VoipOpCallConnectRinging::process( const Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_CONNECT )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	Sptr <InviteMsg> msg = context->getRingInvite();
	assert( msg != 0 );
	SipCallId callId = msg->getCallId();
	//assert(callId != 0);

#if 0
	if ( UaDevice::instance()->isMyHardware( callId, event->getDeviceId() ) == false )
	{
		Sptr < SipCallId > callWaitingId = UaDevice::instance()->getCallWaitingId(deviceEvent->getDeviceId());
		if ( callWaitingId == 0 )
		{
			cpLog( LOG_ERR, "No call on call waiting ");
			cpLog( LOG_ERR, "Ignore flash hook");
			return 0;
		}

		if ( *callWaitingId != callId )
		{
			cpLog( LOG_ERR, "The call on call waiting is not this call ");
			cpLog( LOG_ERR, "Ignore flash hook");
			return 0;
		}

		if ( UaDevice::instance()->getHardwareStatus( event->getDeviceId() ) ==
                HARDWARE_UNAVAILABLE )
		{
			cpLog( LOG_ERR, "Hardware not available" );
			cpLog( LOG_ERR, "Ignore flash hook");
			return 0;
		}

		// grab the hardware
		UaDevice::instance()->setCallId( callWaitingId , event->getDeviceId() );
		UaDevice::instance()->removeCallWaitingId( *callWaitingId, event->getDeviceId());
	}	
	else
	{
		//the device can not be ringing for some unkown reason
		if(!context->getCalleeIdle())
			return 0;
	}
#endif

	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);

    // Get remote SDP from message
	Sptr<SipSdp> remoteSdp;
	remoteSdp.dynamicCast ( msg->getContentData(0) );
	if ( remoteSdp == 0 )
	{
		cpDebug(LOG_DEBUG, "No remote SDP in INVITE" );
	}
	else
	{
		context->setRemoteSdp( new SipSdp( *remoteSdp ) );
	}

	StatusMsg status( *msg, SIP_STATUS_CODE_SUCCESS_OK );

	Data hip=msg->getRequestLine().getHost();
	// Add myself to the contact
	Sptr< SipUrl > myUrl = new SipUrl;

	const char *callernum = vep->getPhoneNumber();;
	cpLog(LOG_DEBUG, "Caller : %s", callernum);

	myUrl->setUserValue(Data(callernum));

	myUrl->setHost( hip );
	myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );
	if(PbxConfiguration::instance()->getSipTransport() == "TCP")
	{
		myUrl->setTransportParam( Data("tcp"));
	}

	SipContact me;
	me.setUrl( myUrl );
	status.setNumContact( 0 );    // Clear
	status.setContact( me );

    // Create local SDP base on remote SDP
	Sptr<SipSdp> localSdp;
	localSdp.dynamicCast ( status.getContentData(0) );
	if ( localSdp != 0 )
	{
		cpDebug(LOG_DEBUG, "localSdp != 0");
		localSdp->setRtpPort( vep->getRtpPort() );

		// Set rtpPacketSize to default in ua.cfg file
		int rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();
		SdpSession sdpDesc = localSdp->getSdpDescriptor();
		list < SdpMedia* > mediaList;
		mediaList = sdpDesc.getMediaList();
		list <SdpMedia*> ::iterator mediaIterator = mediaList.begin();
		vector <Data> * formatList = (*mediaIterator)->getStringFormatList();
		if ( formatList != 0 )
		{
			formatList->clear();
		}
		//using the default codec
		(*mediaIterator)->addFormat( 0 );

		MediaAttributes* mediaAttrib;
		mediaAttrib = (*mediaIterator)->getMediaAttributes();
		if ( mediaAttrib != 0 )
		{
			cpDebug(LOG_DEBUG, "at least one mediaAttrib");
			vector <ValueAttribute*> * valueAttribList = mediaAttrib->getValueAttributes();
			vector <ValueAttribute*> ::iterator attribIterator = valueAttribList->begin();
			while ( attribIterator != valueAttribList->end() )
			{
				char* attribName = (*attribIterator)->getAttribute();
				if ( strcmp( attribName, "ptime" ) == 0 )
				{
					rtpPacketSize = Data((*attribIterator)->getValue()).convertInt();
					break;
				}
				attribIterator++;
			}
			mediaAttrib->flushValueAttributes();
			mediaAttrib->flushrtpmap();

			ValueAttribute* attrib = new ValueAttribute();
			attrib->setAttribute( "ptime" );
			LocalScopeAllocator lo;
			attrib->setValue( Data( rtpPacketSize ).getData(lo) );
			//add the rtpmap attribute for the default codec
			SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
			rtpMapAttrib->setPayloadType( 0 );
			rtpMapAttrib->setEncodingName( "PCMU" );
			rtpMapAttrib->setClockRate( 8000 );
			mediaAttrib->addValueAttribute( attrib );
			mediaAttrib->addmap( rtpMapAttrib );
		}
		else
		{
			cpDebug(LOG_DEBUG, "no mediaAttrib");
			mediaAttrib = new MediaAttributes();
			assert(mediaAttrib);
			(*mediaIterator)->setMediaAttributes(mediaAttrib);

			// create the new value attribute object
			ValueAttribute* attrib = new ValueAttribute();
			// set the attribute and its value
			attrib->setAttribute("ptime");
			LocalScopeAllocator lo;
			attrib->setValue( Data( PbxConfiguration::instance()->getNetworkRtpRate() ).getData(lo) );

			//add the rtpmap attribute for the default codec
			SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
			rtpMapAttrib->setPayloadType(0);
			rtpMapAttrib->setEncodingName("PCMU");
			rtpMapAttrib->setClockRate(8000);

			// add the value attribute just created to the media attribute object
			mediaAttrib->addValueAttribute(attrib);
			mediaAttrib->addmap(rtpMapAttrib);
		}
		
		localSdp->setSdpDescriptor(sdpDesc);
		context->setLocalSdp( new SipSdp( *localSdp ) );

		vep->getSipStack()->sendReply( status );
	}
	else  // localSdp == 0
	{
		cpDebug(LOG_DEBUG, "localSdp == 0");
		// May not have SDP in original INVITE for 3rd party call control
		SipSdp sdp;

		Data hostAddr = hip;

		if(PbxConfiguration::instance()->getNATAddress() != "")
		{
			hostAddr = PbxConfiguration::instance()->getNATAddress();
		}

		int rtpPort = vep->getRtpPort();
		cpLog(LOG_DEBUG, "RtpPort = %d", rtpPort);
		
		doAnswerStuff(sdp, remoteSdp, hostAddr, rtpPort);
		cpLog(LOG_DEBUG, "[------After doAnswerStuff");
		//sleep(5);

		status.setContentData( &sdp, 0 );
		cpLog(LOG_DEBUG, "After setContentData");
		context->setLocalSdp( new SipSdp( sdp ) );
		cpLog(LOG_DEBUG, "After new SipSdp( sdp ) ");
		vep->getSipStack()->sendReply( status );
		cpLog(LOG_DEBUG, "After sendReply");
	}

	cancelTimer(event);

	/* activate callee's audio channel before audioThread can process it */
	Sptr <AudioChannel> audio = vep->getAudio();
	assert(audio != 0);
	audio->activate();

	/* reply ConnectAck which make CallScheduler enter CONNECT state and activate audioThread for my audioChannel */
	sendCallSignal(event, CALL_CONNECT_ACK);
	
	return lookupEPState(event, VOIP_EP_STATE_INCALL );
	
}

