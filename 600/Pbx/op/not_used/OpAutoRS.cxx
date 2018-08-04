/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const OpAutoRS_cxx_Version =
    "$Id: OpAutoRS.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "global.h"

#include "InviteMsg.hxx"
#include "OpAutoRS.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#include "LoadGenMonitor.hxx"
#include "Sdp2Media.hxx"
#include "SipSdp.hxx"
#include "SystemInfo.hxx"
#include "SipVia.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


extern void cjGetTimeOfDay( struct timeval* now );
extern int numOfCalls;
extern double lenOfCalls;
extern double lenOfInvite;
extern double lenOfBye;
extern int numOfBusys;
extern int numOfInvite;
extern int numOfBye;
extern timeval inviteTime;
extern timeval inviteokTime;
extern timeval byeTime;
extern timeval byeokTime;


OpAutoRS::OpAutoRS()
{
}


OpAutoRS::~OpAutoRS()
{
}


const char* const
OpAutoRS::name() const
{
    return "OpAutoRS";
}


///
const Sptr < State >
OpAutoRS::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoRS operation" );

    // build invite message to be sent

    Sptr < UaDigitTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );

    if (timerEvent == 0)
    {
        return 0;
    }


    Sptr< SipUrl > toUrl =
	new SipUrl( UaConfiguration::instance()->getCallUrl());
    Sptr< SipUrl > url = new SipUrl;

    url->setUserValue( toUrl->getUserValue(), "phone" );
    url->setHost( UaConfiguration::instance()->getProxyServer() );
    string sipPort = UaConfiguration::instance()->getLocalSipPort();

    InviteMsg msg( url, atoi( sipPort.c_str() ) );

    // reuse the callid from the event

    msg.setCallId( *(timerEvent->callId) );

    // Why we need to do this ???
    //change the port number in the requestline
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

    reqUrl->setUserValue( toUrl->getUserValue(), "phone" );
    //    reqUrl.setPort( Data(sipPort) );    // ??
    reqLine.setUrl( reqUrl );
    msg.setRequestLine( reqLine );

    SipFrom from = msg.getFrom();

    baseUrl = from.getUrl();
    assert( baseUrl != 0 );
    Sptr< SipUrl > fromUrl;
    fromUrl.dynamicCast( baseUrl );
    assert( fromUrl != 0 );

    fromUrl->setUserValue( Data( UaConfiguration::instance()->getUserName() ),
			   "phone" );
    from.setUrl( fromUrl );
    from.setDisplayName( Data( UaConfiguration::instance()
			       ->getDisplayName() ) );
    msg.setFrom( from );

    // Make sure the To: header has host portion in it
    // We may not need to do this later on
    SipTo to = msg.getTo();
    if ( toUrl->getHost().length() <= 0 )
    {
        toUrl->setHost( UaConfiguration::instance()->getProxyServer() );
    }
    to.setUrl( toUrl );
    msg.setTo( to );


    /*
        // Set a fake via for marshal  -ctam
        SipVia rsVia = msg.getVia();
        rsVia.setTransport( UaConfiguration::instance()->getSipTransport() );
        rsVia.setHost( "192.168.5.20");
        rsVia.setPort(5060);
    //    msg.setVia( rsVia );
    */

    // Set transport in Via:
    SipVia via = msg.getVia();
    msg.removeVia();
    via.setTransport( UaConfiguration::instance()->getSipTransport() );
    msg.setVia( via );

    // Set a fake via for marshal  -ctam
    via.setHost( "192.168.5.20");
    via.setPort(10000);
    msg.setVia( via , 0);


    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    myUrl->setHost( Data( theSystem.gethostAddress() ) );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    msg.setNumContact( 0 );    // Clear
    msg.setContact( me );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

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

        call->setLocalSdp( new SipSdp( *sipSdp ) );
	int tmp;
        cpDebug(LOG_DEBUG, "Local SDP:\n%s", call->getLocalSdp()
	       ->encodeBody(tmp).logData() );
    }

    // Save INVITE
    call->setRingInvite( new InviteMsg( msg ) );

    // Send INVITE
    timerEvent->getSipStack()->sendAsync( msg );


    // Monitor msg:  Invite was sent
    if ( UaConfiguration::instance()->getMonitorMsgOn() )
    {
        if (UaConfiguration::instance()->getCJTimeOfDay() )
        {
            cjGetTimeOfDay(&inviteTime);
        }
        else
        {
            gettimeofday(&inviteTime, NULL);
        }
    }


    Sptr < Contact > contact = new Contact( msg );
    // Set as current contact
    call->setContact( contact );
    // Add to contact list
    call->addContact( contact );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateAutoRS" );
}

