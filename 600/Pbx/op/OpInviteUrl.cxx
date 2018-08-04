/*
 * $Id: OpInviteUrl.cxx,v 1.6 2007/04/12 17:43:45 lizhijie Exp $
 */

#include "global.h"
#include "InviteMsg.hxx"
#include "OpInviteUrl.hxx"
#include "UaDigitTimerEvent.hxx"
#include "DigitCollector.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "SdpHandler.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SipVia.hxx"

using namespace Vocal;


OpInviteUrl::OpInviteUrl()
{
}

OpInviteUrl::~OpInviteUrl()
{
}

const char* const OpInviteUrl::name() const
{
	return "OpInviteUrl";
}


const Sptr <State> OpInviteUrl::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpInviteUrl operation" );

	Sptr <UaDigitTimerEvent> timerEvent;
	timerEvent.dynamicCast( event );
	if ( timerEvent == 0 )
	{
		return 0;
	}

	Sptr <DigitCollector> digitCollector = UaDevice::instance()->getDigitCollector(event->getDeviceId() );

	assert( digitCollector != 0 );

	if ( !( digitCollector->isValidDial() ) )
	{
		return 0;
	}
	cpDebug(LOG_DEBUG, "Dialing complete" );

	Sptr <UaStateMachine> stateMachine;
	stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
	assert( stateMachine != 0 );

	string currentUrl = digitCollector->getUrl();
	if(currentUrl == "") 
	{
		event->setErrorInfo(ERR_DIAL);
		return stateMachine->findState( "StateError" );
	}

	//NetworkAddress *sendaddr;

	bool bLocal = false;
	if (currentUrl.find("127.0.0.1") != string::npos)
		bLocal = true;	

	/*added by wuaiwu, 2005/10/11 */
	Data sendIp("");
	int sendPort = 0;
	bool proxies = false;
	
	string proxyServer = UaConfiguration::instance()->getProxyServer();
	string proxyPort = UaConfiguration::instance()->getProxyPort();
	
	cpDebug(LOG_DEBUG, "proxyServer is %s", proxyServer.c_str() );
#if 0
	if (proxyServer.length() > 0)
#else
	if (currentUrl.find(proxyServer.c_str()) != string::npos && digitCollector->getDialMethod() != SPEED_DIAL )
#endif
	{
		proxies = true;
		cpDebug(LOG_DEBUG, "-------------------------------USE PROXY SERVER");
	}

	/* get sendIP and sendPort */
	if (!bLocal && proxies)
	{/* proxy and not local */
		NetworkAddress pna( proxyServer );
		sendIp = pna.getIpName();
		if (sendIp.length() <= 0 && !bLocal)
		{
			cpLog(LOG_EMERG, "Can't resolve the proxyServer: %s, this InviteMsg discarded", proxyServer.c_str());
			event->setErrorInfo(ERR_CONNECT);
			return stateMachine->findState( "StateError" );
		}
#if 0/* lzj 2007.03.09 */		
		sendPort = pna.getPort();
#else
		sendPort = atoi( UaConfiguration::instance()->getProxyPort().c_str() );
#endif
		sendPort = (sendPort <= 0)? 5060 : sendPort;
	}
	else if (bLocal)
	{/* local */
		sendIp = "127.0.0.1";
#if 0		
		sendPort = 5060;
#else
		sendPort = atoi( UaConfiguration::instance()->getLocalSipPort().c_str() );
#endif
	}
	else 
	{/* not proxy and not local, such as speed dial */
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
			event->setErrorInfo(ERR_CONNECT);
			return stateMachine->findState( "StateError" );
		}
		
		sendPort = (tport.size() <= 0)? 5060 : atoi(tport.c_str());
	}
	/* end of added */

	/* URL get from dial plan is only used as TO header */
	cpDebug(LOG_DEBUG, "Request url %s", currentUrl.c_str() );
	Sptr <SipUrl> toUrl;
	try
	{
		toUrl = new SipUrl( currentUrl );
		if ( dial_phone == digitCollector->getDialedType() )
		{
			cpDebug(LOG_DEBUG_STACK, "setting phone # %s", toUrl->getUserValue().logData());
			toUrl->setUserValue( toUrl->getUserValue(), dial_phone );
		}
	}
	catch ( SipUrlParserException& )
	{
		cpLog( LOG_ERR, "Invalid url %s", currentUrl.c_str());
		return 0;
	}

	assert( toUrl != 0 );
	if ( toUrl->getHost().length() <= 0 && proxies && !bLocal)
	{/* when PROXY, update port field in TO header */
		toUrl->setHost(sendIp);
		if( sendPort > 0 )
		{
			toUrl->setPort( Data(sendPort));
		}
	}

	/* create INVITE msg */
	string sipPort = UaConfiguration::instance()->getLocalSipPort();
	Sptr <InviteMsg> msg = new InviteMsg( toUrl, atoi( sipPort.c_str() ) );
	Data hip=theSystem.gethostAddress(&toUrl->getHost());
	msg->setCallId( *(UaDevice::instance()->getCallId(event->getDeviceId() )) );

	/* added 2007.03.09 for BYE msg for call begin from here, refer to OpEndCall */
	SipCSeq cseq = msg->getCSeq();
	cseq.setCSeq(1);
	msg->setCSeq(cseq);
	/* end of added 2007.03.09 */

	/* update REQUEST_LINE in INVITE msg */
	SipRequestLine reqLine = msg->getRequestLine();
	Sptr <BaseUrl> baseUrl = reqLine.getUrl();
	assert( baseUrl != 0 );
	if( baseUrl->getType() == TEL_URL )
	{
		cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
		assert( 0 );
	}
	
	/* Assume we have a SIP_URL */
	Sptr <SipUrl> reqUrl;
	reqUrl.dynamicCast( baseUrl );
	assert( reqUrl != 0 );

	if(UaConfiguration::instance()->getSipTransport() == "TCP")
	{
		reqUrl->setTransportParam( Data("tcp"));
	}

	if ( proxies && !bLocal)
	{
		//TODO clean up
		Sptr <SipUrl> proxyUrl;
		try
		{
			proxyUrl = new SipUrl( "sip:" + proxyServer + ":" + proxyPort  );
		}
		catch ( SipUrlParserException& )
		{
			cpLog( LOG_ERR, "Invalid url %s", proxyServer.c_str() );
			return stateMachine->findState( "StateError" );
		}

		reqUrl->setHost( proxyUrl->getHost() );
		reqUrl->setPort( proxyUrl->getPort() );
	}

	reqLine.setUrl( reqUrl );		 
	msg->setRequestLine( reqLine );
	/* end of update request line */

	/* update FROM header in INVITE */
	SipFrom from = msg->getFrom();
	from.setDisplayName( Data( UaConfiguration::instance()->getDisplayName()));

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

	/* modified by lizhijie, for 'FROM' header */
	string callernum;
	callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId() );
	//	fromUrl->setUserValue( Data( UaConfiguration::instance()->getUserName() ), "phone" );
	//	cpDebug(LOG_DEBUG, "from URL : '%s', caller is '%s'",  fromUrl->encode().logData() ,callernum.c_str() );
	fromUrl->setUserValue(Data(callernum),   Data( UaConfiguration::instance()->getUserName() )  );
	//	cpDebug(LOG_DEBUG, "from URL : '%s', caller is '%s'",  fromUrl->encode().logData() ,callernum.c_str() );
	fromUrl->setHost(hip);
	from.setUrl( fromUrl );
	msg->setFrom( from );
	/*****  end of FROM header *******/

	// Set transport in Via:
	SipVia via = msg->getVia();
	msg->removeVia();
	via.setTransport( UaConfiguration::instance()->getSipTransport() );
	via.setHost(hip);
	msg->setVia( via );

	// Set Contact header
	/* modified by lizhijie, for CONTACT header */
	Sptr <SipUrl> myUrl = new SipUrl;
	/* second parameter must be 'phone' here for setUserValue , lizhijie 2005.02.19  */
	//	myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
	myUrl->setUserValue(Data(callernum),  string("phone") );
	myUrl->setHost( hip );
	//	cpDebug(LOG_DEBUG, "Contact URL : '%s', caller is '%s'",  myUrl->encode().logData() ,callernum.c_str() );
	//	cpDebug(LOG_DEBUG, "Contact URL : '%s', caller is '%s' ,isOK %d",  myUrl->encode().logData() ,callernum.c_str(), isOK );
	myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );

	if(UaConfiguration::instance()->getSipTransport() == "TCP")
	{
		myUrl->setTransportParam( Data("tcp"));
	}

	cpDebug(LOG_DEBUG_STACK, "Contact URL : '%s', caller is '%s'",  myUrl->encode().logData() ,callernum.c_str() );

	SipContact me;
	me.setUrl( myUrl );
	msg->setNumContact( 0 );    // Clear
	msg->setContact( me );
	/* end of create INVITE msg */

	Sptr <UaCallInfo> call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	addSdpToMsg(*msg, Data(UaConfiguration::instance()->getNetworkRtpRate()),
	UaDevice::instance()->getRtpPort(event->getDeviceId() ));

	Sptr <SipSdp> sipSdp;
	sipSdp.dynamicCast ( msg->getContentData( 0 ) );

	if ( sipSdp != 0 )
	{
		call->setLocalSdp( new SipSdp( *sipSdp ) );
#ifndef __AS_RELEASE__
		int tmp;
#endif
		cpDebug(LOG_DEBUG, "Local SDP:\n%s",  call->getLocalSdp()->encodeBody(tmp).logData() );
	}

	// Save INVITE.  Needs to encode() to make sure length is correct
	call->setRingInvite( new InviteMsg( *msg ) );
	cpDebug(LOG_DEBUG, "OpInvite\r\n"  );

	// Send INVITE msg 
	timerEvent->getSipStack()->sendAsync( msg, sendIp, Data(sendPort) );

	call->setContactMsg(*msg);
	cpDebug(LOG_DEBUG, "OpInvite\r\n"  );

	return stateMachine->findState( "StateTrying" );
	//sleep(10);
	//return stateMachine->findState( "StateIdle" );
}

