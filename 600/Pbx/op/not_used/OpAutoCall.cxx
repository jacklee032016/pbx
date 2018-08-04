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


static const char* const OpAutoCall_cxx_Version =
    "$Id: OpAutoCall.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";


#include "global.h"
#include "InviteMsg.hxx"

#include "OpAutoCall.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaConfiguration.hxx"
#include "UaCallInfo.hxx"
#include "UaStateMachine.hxx"
#if 0
#include "LoadGenMonitor.hxx"
#endif
#include "UaDevice.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
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


OpAutoCall::OpAutoCall()
{
}


OpAutoCall::~OpAutoCall()
{
}


const char* const
OpAutoCall::name() const
{
    return "OpAutoCall";
}


const Sptr < State >
OpAutoCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoCall operation" );


    // build invite message to be sent

    Sptr < UaDigitTimerEvent > timerEvent;
    timerEvent.dynamicCast( event );

    if (timerEvent == 0)
    {
        return 0;
    }

    if ( timerEvent->getLoadGenSigType() != LoadGenStartCall )
    {
        return 0;
    }

    // printf( "***** OpAutoCall operation *****\n" );

    // make sure that previously allocated rtp port is released
    // UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

    Sptr< SipUrl > toUrl =
	new SipUrl( UaConfiguration::instance()->getCallUrl());
//    toUrl->setPhoneNumber( toUrl.getUser() );
    // This looks awkward, but...
    toUrl->setUserValue( toUrl->getUserValue(), "phone" );

    string sipPort = UaConfiguration::instance()->getLocalSipPort();

    InviteMsg msg( toUrl, atoi( sipPort.c_str() ) );


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

    reqUrl->setUserValue( toUrl->getUserValue() );
    //    reqUrl.setPort( Data(sipPort) );    // ??
    reqLine.setUrl( reqUrl );
    msg.setRequestLine( reqLine );

    SipFrom from = msg.getFrom();
    from.setUser( Data( UaConfiguration::instance()->getUserName() ) );
    from.setDisplayName( Data( UaConfiguration::instance()->getDisplayName() ) );
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

    // Set transport in Via:
    SipVia via = msg.getVia();
    msg.removeVia();
    via.setTransport( UaConfiguration::instance()->getSipTransport() );
    msg.setVia( via );

    // Set Contact: header
    Sptr< SipUrl > myUrl = new SipUrl;
    myUrl->setUserValue( UaConfiguration::instance()->getUserName() );
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

        // Hard code port (temporary) -jeff
		sipSdp->setRtpPort( UaDevice::instance()->getRtpPort( event->getDeviceId() ) );
		//sipSdp->setRtpPort( 12345 );

        call->setLocalSdp( new SipSdp( *sipSdp ) );
	int tmp;
        cpDebug(LOG_DEBUG, "Local SDP:\n%s", call->getLocalSdp()->encodeBody(tmp).logData() );
    }

    // Save INVITE
    call->setRingInvite( new InviteMsg( msg ) );

    // Send INVITE
    timerEvent->getSipStack()->sendAsync( msg );


    // Monitor msg:  Invite was sent
    if ( UaConfiguration::instance()->getMonitorMsgOn() )
    {
        // send back the time the INVITE was sent
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

    // Start caller guard timer
    Sptr < UaDigitTimerEvent > callerTO = new UaDigitTimerEvent( event->getFifo() );

    assert( callerTO != 0 );

    callerTO->callId = new SipCallId( *(timerEvent->callId) );
    callerTO->setLoadGenSignalType( LoadGenRestartCall );
    callerTO->setSipStack( timerEvent->getSipStack() );

    (event->getFifo())->addDelayMs( callerTO,
                                    UaConfiguration::instance()->getCallRestartTimer() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateAutoTrying" );
}


