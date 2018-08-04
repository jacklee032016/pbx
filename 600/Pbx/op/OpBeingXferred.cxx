/*
 * $Id: OpBeingXferred.cxx,v 1.5 2007/03/16 19:33:56 lizhijie Exp $
 */

#include "global.h"
#include "SipEvent.hxx"
#include "TransferMsg.hxx"

#include "OpBeingXferred.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaDevice.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Media.hxx"
#include "SipTransferTo.hxx"
#include "SipVia.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


OpBeingXferred::OpBeingXferred()
{
}


OpBeingXferred::~OpBeingXferred()
{
}


const char* const
OpBeingXferred::name() const
{
    return "OpBeingXferred";
}


const Sptr < State >
OpBeingXferred::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpBeingXferred operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    assert( event != 0 );

    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < TransferMsg > msg;
    msg.dynamicCast( sipMsg );
    assert( msg != 0 );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    //save the transfer msg;
    call->setXferMsg(msg);

    //get to person to be tranferred to
    SipTransferTo xferTo = msg->getTransferTo();

    //form an invite to that person
    string sipPort = UaConfiguration::instance()->getLocalSipPort();

    Sptr< BaseUrl > baseUrl = xferTo.getUrl();
    assert( baseUrl != 0 );
    if( baseUrl->getType() == TEL_URL )
    {
	cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	assert( 0 );
    }
    // Assume we have a SIP_URL
    Sptr< SipUrl > xferUrl;
    xferUrl.dynamicCast( baseUrl );
    assert( xferUrl != 0 );

    InviteMsg inviteMsg( xferUrl, atoi( sipPort.c_str() ) );
	
    SipCallId callId;
    callId = call->getContact()->getInviteMsg().getCallId();
    inviteMsg.setCallId( callId );

    SipTo to = inviteMsg.getTo();

    baseUrl = to.getUrl();
    assert( baseUrl != 0 );
    if( baseUrl->getType() == TEL_URL )
    {
	cpLog( LOG_ERR, "TEL_URL currently not supported\n" );
	assert( 0 );
    }
    // Assume we have a SIP_URL
    Sptr< SipUrl > toUrl;
    toUrl.dynamicCast( baseUrl );
    assert( toUrl != 0 );

    to.setDisplayName( xferTo.getDisplayName() );
    inviteMsg.setTo( to );


    //force invite to go through proxy server if a proxy exists
    string proxyUrlStr = UaConfiguration::instance()->getProxyServer();

    if ( proxyUrlStr.length() > 0 )
    {
        Data urlStr = Data( string("sip:") + proxyUrlStr + ":" + UaConfiguration::instance()->getProxyPort() );
        Sptr< SipUrl > proxyUrl = new SipUrl( urlStr );
        SipRequestLine msRequestLine = inviteMsg.getRequestLine();

	baseUrl = msRequestLine.getUrl();
	assert( baseUrl != 0 );
	// Assume we have a SIP_URL
	Sptr< SipUrl > msUrl;
	msUrl.dynamicCast( baseUrl );
	assert( msUrl != 0 );

        msUrl->setHost( proxyUrl->getHost() );
        msUrl->setPort( proxyUrl->getPort() );

        msRequestLine.setUrl( msUrl );
        inviteMsg.setRequestLine( msRequestLine );

        toUrl->setHost( proxyUrl->getHost() );
        toUrl->setPort( proxyUrl->getPort() );
    }

	Data hip=theSystem.gethostAddress(&toUrl->getHost());

    SipFrom from = inviteMsg.getFrom();

    string	callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());


    from.setUser( Data(callernum)); //Data( UaConfiguration::instance()->getUserName() ) );
    from.setDisplayName( Data( UaConfiguration::instance()->getDisplayName() ) );
	from.setHost(hip);
    inviteMsg.setFrom( from );

    // Set transport in Via:
    SipVia via = inviteMsg.getVia();
    inviteMsg.removeVia();
    via.setTransport( UaConfiguration::instance()->getSipTransport() );
	via.setHost(hip);
    inviteMsg.setVia( via );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
//    myUrl.setPhoneNumber( UaConfiguration::instance()->getUserName() );
//    myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost( hip );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    inviteMsg.setNumContact( 0 );    // Clear
    inviteMsg.setContact( me );

    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( inviteMsg.getContentData( 0 ) );
	
	if ( sipSdp != 0 )
	{
		SdpSession sdpDesc = sipSdp->getSdpDescriptor();
		list < SdpMedia* > mediaList;
		mediaList = sdpDesc.getMediaList();
		list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();
		MediaAttributes* mediaAttrib;
		mediaAttrib = (*mediaIterator)->getMediaAttributes();
		if ( mediaAttrib == 0 )
		{
			mediaAttrib = new MediaAttributes();
			assert( mediaAttrib );
			(*mediaIterator)->setMediaAttributes( mediaAttrib );
		}
		ValueAttribute* attrib = new ValueAttribute();
		attrib->setAttribute( "ptime" );
		LocalScopeAllocator lo;
		attrib->setValue( Data( UaConfiguration::instance()->getNetworkRtpRate() ).getData(lo) );

		//add the rtpmap attribute for the default codec
		SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
		rtpMapAttrib->setPayloadType( 0 );
		rtpMapAttrib->setEncodingName( "PCMU" );
		rtpMapAttrib->setClockRate( 8000 );

		// add the value attribute just created to the media attribute object
		mediaAttrib->addValueAttribute( attrib );
		mediaAttrib->addmap( rtpMapAttrib );
		sipSdp->setSdpDescriptor( sdpDesc );

		//sipSdp->setRtpPort( atoi (UaConfiguration::instance()->getLocalRtpPort().c_str() )  );
		sipSdp->setRtpPort( UaDevice::instance()->getRtpPort( event->getDeviceId() ) );

		call->setLocalSdp( new SipSdp( *sipSdp ) );

#ifndef __AS_RELEASE__
		int tmp;
#endif
		cpDebug(LOG_DEBUG, "Local SDP:\n%s", call->getLocalSdp()->encodeBody(tmp).logData() );

	}

	// Save INVITE
	call->setRingInvite( new InviteMsg( inviteMsg ) );

	// Send INVITE
	sipEvent->getSipStack()->sendAsync( inviteMsg );

	Sptr < Contact > contact = new Contact( inviteMsg );
	// Set as current contact
	call->setContact( contact );
	// Add to contact list
	call->addContact( contact );

	return 0;
}

