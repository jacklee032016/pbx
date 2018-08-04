/*
* $Id: VoipOpCallSetupIdle.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "DialingValidator.hxx"
#include "VoipOpBuilder.hxx"
#include "PbxAgent.hxx"

using namespace Assist;

/* process _CallSetup when in IDLE state */
const Sptr <PbxState> VoipOpCallSetupIdle::process( const Sptr <PbxEvent> event )
{
	if( checkCallMsg(event) != CALL_SETUP )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <CommonMsg> callMsg = isCallMsg( event);
	assert(callMsg != 0);
	
	Sptr <CallEvent> callEvent = isCallEvent(event);

	event->getEndPoint()->setPeerSignalType( callMsg->getPeerSignalType() );
	
	Sptr <VoipEndPoint> vep = getVoipEndPoint(event);
	Sptr <SipEvent> sipEvent;
	sipEvent.dynamicCast( event);

	string peerNumber;
	peerNumber = callMsg->getTo();

#if 0	
	string::size_type pos = peerNumber.find("#");
	cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
	if (pos != string::npos && pos< string::npos )
	{
		cpDebug(LOG_DEBUG, "# is in poistion %d of %s" , pos, peerNumber.c_str() );
		peerNumber= peerNumber.substr(pos+1, peerNumber.length()-pos-1);
	}
#endif

	Sptr <DialingValidator> dg = PbxAgent::instance()->getGateway()->getDialingValidator();
	assert (dg != 0);

	string currentUrl = dg->getUrl( peerNumber );
	cpLog(LOG_DEBUG, "The INVITE url is %s", currentUrl.c_str());

	//string currentUrl = devEvent->text.logData(); // digitCollector->getUrl();
	//string currentUrl = 
	if(currentUrl == "") 
	{
		sipEvent->setErrorInfo(ERR_DIAL);

		sendCallSignal(event, CALL_RELEASE);
		return PBX_STATE_CONTINUE;
	}

	vep->setPhoneNumber(callMsg->getFrom() );

	//NetworkAddress *sendaddr;
	bool bLocal = false;
	if (currentUrl.find("127.0.0.1") != string::npos)
		bLocal = true;	
     
      /*added by wuaiwu, 2005/10/11 */
	Data sendIp("");
	int sendPort = 0;
	bool proxies = false;
	
	string proxyServer = PbxConfiguration::instance()->getProxyServer();
	cpDebug(LOG_DEBUG, "proxyServer is %s", proxyServer.c_str() );
	if (proxyServer.length() > 0)
	{
		proxies = true;
		cpDebug(LOG_DEBUG, "-------------------------------USE PROXY SERVER");
	}
	
	if (!bLocal && proxies)
	{
		NetworkAddress pna( proxyServer );
		sendIp = pna.getIpName();
		if (sendIp.length() <= 0 && !bLocal)
		{
			cpLog(LOG_EMERG, "Can't resolve the proxyServer: %s, this InviteMsg discarded", proxyServer.c_str());
			sipEvent->setErrorInfo(ERR_CONNECT);
			

			sendCallSignal(event, CALL_RELEASE);
			return PBX_STATE_CONTINUE;
		}
		sendPort = pna.getPort();
		sendPort = (sendPort <= 0)? 5060 : sendPort;

	}
 	else if (bLocal)
 	{
 		sendIp = "127.0.0.1";
		sendPort = 5060;
 	}
	else 
 	{
		string thost(""); // = currentUrl
		string tport("");
		string::size_type tpos = currentUrl.find("@");
		if (tpos != string::npos)
		{
			thost = currentUrl.substr(tpos+1, currentUrl.size()-tpos-1);
			
			string::size_type tpos2 = thost.find(":");
			if (tpos2 != string::npos)
			{
				string tport = thost.substr(tpos2+1, thost.size()-tpos2-1);
				thost = thost.substr(0, tpos2);
			
			}
		}

		NetworkAddress tna(thost);
		sendIp = tna.getIpName();
		if (sendIp.length() <= 0)
		{
			cpLog(LOG_EMERG, "Can't resolve the send address: %s, this InviteMsg discarded", thost.c_str());
			sipEvent->setErrorInfo(ERR_CONNECT);

			sendCallSignal(event, CALL_RELEASE);
			return PBX_STATE_CONTINUE;
		}
		sendPort = (tport.size() <= 0)? 5060 : atoi(tport.c_str());
 	}
	/* end of added */
	
	cpDebug(LOG_DEBUG, "Request url %s", currentUrl.c_str() );
	Sptr <SipUrl> toUrl;
	try
	{
		toUrl = new SipUrl( currentUrl );
	//	if ( dial_phone == digitCollector->getDialedType() )
		{
			cpDebug(LOG_DEBUG, "setting phone # %s", toUrl->getUserValue().logData());
			toUrl->setUserValue( toUrl->getUserValue(), dial_phone );
		}
	}
	catch ( SipUrlParserException& )
	{
		cpLog( LOG_ERR, "Invalid url %s", currentUrl.c_str());
TRACE;
		sendCallSignal(event, CALL_RELEASE);
		return PBX_STATE_CONTINUE;
	}

	assert( toUrl != 0 );

	if ( toUrl->getHost().length() <= 0 && proxies && !bLocal)
	{
		toUrl->setHost(sendIp);
		if( sendPort > 0 )
		{
			toUrl->setPort( Data(sendPort));
		}
	}

TRACE;
  	
    	string sipPort = PbxConfiguration::instance()->getLocalSipPort();
    	Sptr <InviteMsg> msg = new InviteMsg( toUrl, atoi( sipPort.c_str() ) );
	Data hip= theSystem.gethostAddress(&toUrl->getHost());

	Sptr <SipCallId>  sipCallId = new SipCallId;
	msg->setCallId( *sipCallId );
	
TRACE;

	SipRequestLine reqLine = msg->getRequestLine();
	Sptr <BaseUrl> baseUrl = reqLine.getUrl();
	assert( baseUrl != 0 );
	if( baseUrl->getType() == TEL_URL )
	{
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}

	// Assume we have a SIP_URL
	Sptr <SipUrl> reqUrl;
	reqUrl.dynamicCast( baseUrl );
	assert( reqUrl != 0 );

	if(PbxConfiguration::instance()->getSipTransport() == "TCP")
	{
		reqUrl->setTransportParam( Data("tcp"));
	}
   
	if ( proxies && !bLocal)
	{
    	//TODO clean up
    		Sptr <SipUrl> proxyUrl;
		try
		{
			proxyUrl = new SipUrl( "sip:" + proxyServer );
		}
		catch ( SipUrlParserException& )
		{
			cpLog( LOG_ERR, "Invalid url %s", proxyServer.c_str() );

			sendCallSignal(event, CALL_RELEASE);
			return PBX_STATE_CONTINUE;
		}

		reqUrl->setHost( proxyUrl->getHost() );
		reqUrl->setPort( proxyUrl->getPort() );
	}

	reqLine.setUrl( reqUrl );		 
	msg->setRequestLine( reqLine );

	SipFrom from = msg->getFrom();
	from.setDisplayName( Data( PbxConfiguration::instance()->getDisplayName()));

	baseUrl = from.getUrl();
	assert( baseUrl != 0 );
	if( baseUrl->getType() == TEL_URL )
	{
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}

	// Assume we have a SIP_URL
	Sptr <SipUrl> fromUrl;
	fromUrl.dynamicCast( baseUrl );
	assert( fromUrl != 0 );

#if 1
	/* modified by lizhijie, for 'FROM' header */
//	cpDebug(LOG_DEBUG, "from URL : '%s', caller is '%s'",  fromUrl->encode().logData() ,callernum.c_str() );
	fromUrl->setUserValue(Data(vep->getPhoneNumber() ),   Data( PbxConfiguration::instance()->getUserName() )  );
//	cpDebug(LOG_DEBUG, "from URL : '%s', caller is '%s'",  fromUrl->encode().logData() ,callernum.c_str() );
#else	
	fromUrl->setUserValue( Data( PbxConfiguration::instance()->getUserName() ), "phone" );
#endif
	fromUrl->setHost(hip);
	from.setUrl( fromUrl );
	msg->setFrom( from );

    // Set transport in Via:
	SipVia via = msg->getVia();
	msg->removeVia();
	via.setTransport( PbxConfiguration::instance()->getSipTransport() );
	via.setHost(hip);
	msg->setVia( via );

	// Set Contact: header
	/* modified by lizhijie, for CONTACT header */
	Sptr <SipUrl> myUrl = new SipUrl;
#if 0	
	/* second parameter must be 'phone' here for setUserValue , lizhijie 2005.02.19  */
	myUrl->setUserValue(Data(callernum),  string("phone") );
//	cpDebug(LOG_DEBUG, "Contact URL : '%s', caller is '%s'",  myUrl->encode().logData() ,callernum.c_str() );
//	cpDebug(LOG_DEBUG, "Contact URL : '%s', caller is '%s' ,isOK %d",  myUrl->encode().logData() ,callernum.c_str(), isOK );
#else
	myUrl->setUserValue( PbxConfiguration::instance()->getUserName(), "phone" );
#endif
	myUrl->setHost( hip );
	myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );
	
	if(PbxConfiguration::instance()->getSipTransport() == "TCP")
	{
		myUrl->setTransportParam( Data("tcp"));
	}

	cpDebug(LOG_DEBUG_STACK, "Contact URL : '%s', caller is '%s'",  myUrl->encode().logData() ,peerNumber.c_str() );

	SipContact me;
	me.setUrl( myUrl );
	msg->setNumContact( 0 );    // Clear
	msg->setContact( me );

	vep->setSipContext(new SipContext);
	vep->setSipCallId( *sipCallId);
	Sptr <SipContext> context = vep->getSipContext();
	assert(context != 0);

	cpLog(LOG_DEBUG, "RTP Rate %d", PbxConfiguration::instance()->getNetworkRtpRate() );
	cpLog(LOG_DEBUG, "RTP port %d", vep->getRtpPort() );
	
TRACE;
	addSdpToMsg(*msg, Data(PbxConfiguration::instance()->getNetworkRtpRate()), vep->getRtpPort() );
TRACE;

	Sptr <SipSdp> sipSdp;
	sipSdp.dynamicCast ( msg->getContentData( 0 ) );
	if ( sipSdp != 0 )
	{
		context->setLocalSdp( new SipSdp( *sipSdp ) );
		int tmp;
		cpDebug(LOG_DEBUG, "Local SDP:\n%s",  context->getLocalSdp()->encodeBody(tmp).logData() );
	}

	// Save INVITE.  Needs to encode() to make sure length is correct
	context->setRingInvite( new InviteMsg( *msg ) );
	cpDebug(LOG_DEBUG, "OpInvite\r\n"  );

	vep->getSipStack()->sendAsync( msg, sendIp, Data(sendPort) );


	cpDebug(LOG_DEBUG, "OpInvite\r\n"  );
	context->setContactMsg(*msg);
	cpDebug(LOG_DEBUG, "OpInvite\r\n"  );

	return lookupEPState(event, VOIP_EP_STATE_TRYING );
}

