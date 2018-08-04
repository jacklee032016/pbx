#include "global.h"

#include "InviteMsg.hxx"
#include "OpInCallInvite.hxx"
#include "StateInCallTrying.hxx"
#include "UaDigitTimerEvent.hxx"
#include "DigitCollector.hxx"
#include "UaDevice.hxx"
#include "PbxConfiguration.hxx"
#include "SipContext.hxx"
#include "UaStateMachine.hxx"
#include "Sdp2Session.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"
#include "Sdp2Media.hxx"
#include "SipVia.hxx"

using namespace Assist;

using Assist::SDP::SdpSession;
using Assist::SDP::SdpMedia;
using Assist::SDP::MediaAttributes;
using Assist::SDP::ValueAttribute;
using Assist::SDP::SdpRtpMapAttribute;


const Sptr < State > VoipOpInCallInvite::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpInCallInvite operation" );

    Sptr < UaDigitTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );
    if ( timerEvent == 0 )
    {
        return 0;
    }

	Sptr < DigitCollector > digitCollector = UaDevice::instance()->getDigitCollector( event->getDeviceId() );
	assert( digitCollector != 0 );

    if ( !( digitCollector->isValidDial() ) )
    {
        return 0;
    }
    cpDebug(LOG_DEBUG, "Dialing complete" );

    //cout << endl;

    Sptr< SipUrl > toUrl = new SipUrl;
    try
    {
        Sptr< SipUrl > tmpUrl = new SipUrl( digitCollector->getUrl() );
        string dialMethod = digitCollector->getDialedType();

//        if ( dialMethod == "phone" )
//            toUrl.setPhoneNumber( tmpUrl.getUser() );
//        else
//            toUrl.setUser( tmpUrl.getUser() );

	toUrl->setUserValue( tmpUrl->getUserValue(), "phone" );
        toUrl->setHost( tmpUrl->getHost() );
        toUrl->setPort( tmpUrl->getPort() );
    }
    catch ( SipUrlParserException& )
    {
        cpLog( LOG_ERR, "Invalid url %s", digitCollector->getUrl().c_str() );
        return 0;
    }

    string proxyServer = PbxConfiguration::instance()->getProxyServer();

    Sptr < SipUrl > proxyUrl = new SipUrl;
    try
    {
        proxyUrl = new SipUrl( "sip:" + proxyServer );
    }
    catch ( SipUrlParserException& )
    {
        cpLog( LOG_ERR, "Invalid url %s", proxyServer.c_str() );
        return 0;
    }

    if ( toUrl->getHost().length() <= 0 )
    {
        toUrl->setHost( proxyUrl->getHost() );
        toUrl->setPort( proxyUrl->getPort() );
    }

    string sipPort = PbxConfiguration::instance()->getLocalSipPort();

    Sptr < SipContext > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    //invite message
    InviteMsg msg( toUrl, atoi( sipPort.c_str() ) );

	Data hip="192.168.3.172";//theSystem.gethostAddress(&toUrl->getHost());
	
    Sptr < Contact > contact = call->getContact();
    assert( contact != 0 );

    // get the original invite message
    const InviteMsg& oldMsg = contact->getInviteMsg();
    if ( &oldMsg == 0 )
    {
        cpDebug(LOG_DEBUG, "No previous invite message");
        return 0;
    }

    //use the same call ID
    msg.setCallId( oldMsg.getCallId() );

    if ( proxyServer.length() > 0 )
    {
        SipRequestLine reqLine = msg.getRequestLine();

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
        msg.setRequestLine( reqLine );
    }

    unsigned int cseq = contact->getCSeqNum();
    contact->setCSeqNum( ++cseq );
    SipCSeq sipCSeq = msg.getCSeq();
    sipCSeq.setCSeq( cseq );
    msg.setCSeq( sipCSeq );

    SipFrom from = msg.getFrom();
    from.setDisplayName( Data( PbxConfiguration::instance()->getDisplayName()));

    Sptr< BaseUrl > baseUrl = from.getUrl();
    assert( baseUrl != 0 );
    // Assume we have a SIP_URL
    Sptr< SipUrl > fromUrl;
    fromUrl.dynamicCast( baseUrl );
    assert( fromUrl != 0 );

    //fromUrl->setUserValue( Data( PbxConfiguration::instance()->getUserName() ),
	//		   "phone" );
    string callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());

    fromUrl->setUserValue(Data(callernum),   dial_phone);
    from.setUrl( fromUrl );
	from.setHost(hip);
    msg.setFrom( from );

    // Set transport in Via:
    SipVia via = msg.getVia();
    msg.removeVia();
    via.setTransport( PbxConfiguration::instance()->getSipTransport() );
	via.setHost(hip);
    msg.setVia( via );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    //myUrl->setUserValue( PbxConfiguration::instance()->getUserName(), "phone" );
    myUrl->setUserValue(Data(callernum),   dial_phone);
    myUrl->setHost(hip );
    myUrl->setPort( PbxConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    msg.setNumContact( 0 );    // Clear
    msg.setContact( me );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    // Compare both old & new invite msgs to make sure
    // we aren't calling the same person
    SipTo oldTo = oldMsg.getTo();
    SipFrom oldFrom = oldMsg.getFrom();
    SipTo to = msg.getTo();

    if ( oldTo.getUrl() == to.getUrl() || oldFrom.getUrl() == to.getUrl() )
    {
        return stateMachine->findState( "StateInCallFastBusy" );
    }

    Sptr<SipSdp> sipSdp;
    sipSdp.dynamicCast ( msg.getContentData( 0 ) );
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
        attrib->setValue( Data( PbxConfiguration::instance()
				->getNetworkRtpRate() ).getData(lo) );

        //add the rtpmap attribute for the default codec
        SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
        rtpMapAttrib->setPayloadType( 0 );
        rtpMapAttrib->setEncodingName( "PCMU" );
        rtpMapAttrib->setClockRate( 8000 );

        // add the value attribute just created to the media attribute object
        mediaAttrib->addValueAttribute( attrib );
        mediaAttrib->addmap( rtpMapAttrib );
        sipSdp->setSdpDescriptor( sdpDesc );

		sipSdp->setRtpPort( UaDevice::instance()->getRtpPort( event->getDeviceId() ) );

        call->setLocal2Sdp( new SipSdp( *sipSdp ) );
	int tmp;
        cpDebug(LOG_DEBUG, "Local SDP:\n%s", call->getLocalSdp()
	       ->encodeBody(tmp).logData() );
    }

    // Save the invite
    call->setRing2Invite( new InviteMsg( msg ) );

    cpDebug(LOG_DEBUG, "sending the following invite");
    cpDebug(LOG_DEBUG, "%s", msg.encode().logData() );
    // Send INVITE
    timerEvent->getSipStack()->sendAsync( msg );

    // Set the new contact
    Sptr < Contact > newContact = new Contact(msg);
    call->setContact( newContact );
    // Add to contact list
    call->addContact( newContact );

    return stateMachine->findState( "StateInCallTrying" );
}
