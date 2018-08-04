/*
 * $Id: OpXferHold.cxx,v 1.3 2007/03/16 19:33:56 lizhijie Exp $
 */

#include "global.h"
#include <InviteMsg.hxx>

#include "OpXferHold.hxx"
#include "UaDeviceEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SipRecordRoute.hxx"
#include "SipVia.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;

///
OpXferHold::OpXferHold()
{}


OpXferHold::~OpXferHold()
{}


///

const char* const
OpXferHold::name() const
{
    return "OpXferHold";
}


const Sptr <State> OpXferHold::process( const Sptr < SipProxyEvent > event )
{
	cpDebug(LOG_DEBUG, "OpXferHold operation" );

	Sptr <UaDeviceEvent> deviceEvent;
	deviceEvent.dynamicCast( event );
	if ( deviceEvent == 0 )
	{
		return 0;
	}

	if ( deviceEvent->type != DeviceEventHookDown && deviceEvent->type != DeviceEventFlash )
	{
		return 0;
	}

	// Put current contact on hold
	Sptr <InviteMsg> reInvite;

	Sptr < UaCallInfo > call;
	call.dynamicCast( event->getCallInfo() );
	assert( call != 0 );

	Sptr < Contact > contact = call->getContact();
	assert( contact != 0 );

	int status = contact->getStatus();
	
	if ( status == 200 )
	{
		cpDebug(LOG_DEBUG, "Put callee on hold" );

		const StatusMsg& msg = contact->getStatusMsg();
		if ( &msg != 0 )
		{
			reInvite = new InviteMsg( msg );
			//add SDP
			Sptr < SipSdp > localSdp = call->getLocalSdp();
			assert( localSdp != 0 );
			SipSdp sipSdp = *localSdp;
			reInvite->setContentData( &sipSdp );
		}
		else
		{
			cpLog( LOG_ERR , "No status message for re-INVITE" );
		}
	}
	else if ( status < 200 && status >= 180 )
	{
		cpDebug(LOG_DEBUG, "Put ringing callee on hold" );
		const InviteMsg& msg = contact->getInviteMsg();
		if ( &msg != 0 )
		{
			reInvite = new InviteMsg( msg );
		}
		else
		{
			cpLog( LOG_ERR , "No status message for re-INVITE" );
		}
	}
	else
	{
		cpDebug(LOG_DEBUG, "Put caller on hold" );

		const InviteMsg& msg = contact->getInviteMsg();
		if ( &msg != 0 )
		{
			string sipPort = UaConfiguration::instance()->getLocalSipPort();
			reInvite = new InviteMsg( msg.getFrom().getUrl(), atoi( sipPort.c_str() ) );
			SipFrom from( msg.getTo().getUrl() );
			reInvite->setFrom( from );
			//TODO Check if it is necessary to set To:
			reInvite->setCallId( msg.getCallId() );

			// Convert RecordRoute to reverse Route
			int numRecordRoute = msg.getNumRecordRoute();
			SipRecordRoute recordroute;
			SipRoute route;

			for ( int i = 0; i < numRecordRoute; i++ )
			{
				recordroute = msg.getRecordRoute( i );
				route.setUrl( recordroute.getUrl() );
				reInvite->setRoute( route );     // to beginning
			}

			int numContact = msg.getNumContact();
			if ( numContact )
			{
				SipContact contact = msg.getContact( numContact - 1 );
				route.setUrl( contact.getUrl() );
				reInvite->setRoute( route );     // to beginning
			}

		}
		else
		{
			cpLog( LOG_ERR , "No INVITE message for re-INVITE" );
		}
	}
	assert( reInvite != 0 );

	//force invite message to go through the proxy server if one exists
	string proxyServer = UaConfiguration::instance()->getProxyServer();

	Sptr< SipUrl > proxyUrl = new SipUrl;
	try
	{
		proxyUrl = new SipUrl( "sip:" + proxyServer +":" + UaConfiguration::instance()->getProxyPort() );
	}
	catch ( SipUrlParserException& )
	{
		cpLog( LOG_ERR, "Invalid url %s", proxyServer.c_str() );
		return 0;
	}

	if ( proxyServer.length() > 0 )
	{
		SipRequestLine reqLine = reInvite->getRequestLine();

		Sptr< BaseUrl > baseUrl = reqLine.getUrl();
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
		reInvite->setRequestLine( reqLine );
	}

	//set via
	Data tmph=reInvite->getRequestLine().getHost();
	SipVia sipVia;
	sipVia.setprotoVersion( "2.0" );
	sipVia.setHost( theSystem.gethostAddress(&tmph) );
	sipVia.setPort( atoi( UaConfiguration::instance()->getLocalSipPort().c_str() ) );
	reInvite->flushViaList();
	reInvite->setVia( sipVia, 0 );

	//TODO Is it going to be a problem if the other side also use the next
	//TODO CSeq at the same time?
	unsigned int cseq = contact->getCSeqNum();
	contact->setCSeqNum( ++cseq );
	SipCSeq sipCSeq = reInvite->getCSeq();
	sipCSeq.setCSeq( cseq );
	reInvite->setCSeq( sipCSeq );

	Sptr<SipSdp> sipSdp;
	sipSdp.dynamicCast ( reInvite->getContentData( 0 ) );
	assert ( sipSdp != 0 );
	SdpSession sdpDesc = sipSdp->getSdpDescriptor();
	sdpDesc.setHold();
	sipSdp->setSdpDescriptor( sdpDesc );

	deviceEvent->getSipStack()->sendAsync( *reInvite );

	return 0;
}

