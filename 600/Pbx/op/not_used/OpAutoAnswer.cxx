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

static const char* const OpAutoAnswer_cxx_Version =
    "$Id: OpAutoAnswer.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "global.h"
#include "SipEvent.hxx"
#include "InviteMsg.hxx"
#include "OpAutoAnswer.hxx"
#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "UaDigitTimerEvent.hxx"
#include "UaDevice.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"
#include "SdpHandler.hxx"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpConnection;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;

OpAutoAnswer::OpAutoAnswer()
{
}


OpAutoAnswer::~OpAutoAnswer()
{
}


const char* const
OpAutoAnswer::name() const
{
    return "OpAutoAnswer";
}


const Sptr < State >
OpAutoAnswer::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAutoAnswer operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }

    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < InviteMsg > inviteMsg;
    inviteMsg.dynamicCast( sipMsg );
    if ( inviteMsg == 0 )
    {
        cpLog( LOG_WARNING, "Not an INVITE" );
        return 0;
    }

    // printf("***** OpAutoAnswer operation *****\n" );
    cpDebug(LOG_DEBUG, "Answer Call" );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );


    // Save INVITE
    call->setRingInvite( new InviteMsg( *inviteMsg ) );

    Sptr < Contact > contact = new Contact( *inviteMsg );
    // Set as current contact
    call->setContact( contact );
    // Add to contact list
    call->addContact( contact );

    // make sure that previously allocated rtp port is released
    // UaDevice::instance()->releaseRtpPort( event->getDeviceId() );

    Sptr < InviteMsg > msg = call->getRingInvite();
    assert( msg != 0 );

    // Get remote SDP from message
    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast ( msg->getContentData(0) );
    if ( remoteSdp == NULL )
    {
        cpLog( LOG_WARNING, "No remote SDP in INVITE" );
    }
    else
    {
        call->setRemoteSdp( new SipSdp( *remoteSdp ) );
    }

    StatusMsg status( *msg, 200/*OK*/ );

    // Add myself to the contact
    Sptr< SipUrl > myUrl = new SipUrl;
    myUrl->setUserValue( UaConfiguration::instance()->getUserName(), "phone" );
    myUrl->setHost( Data( theSystem.gethostAddress() ) );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    SipContact me;
    me.setUrl( myUrl );
    status.setNumContact( 0 );    // Clear
    status.setContact( me );

    // Create local SDP base on remote SDP
    Sptr<SipSdp> localSdp;
    localSdp.dynamicCast ( status.getContentData(0) );
    if ( localSdp != 0 )
    {
        // temporary hardcode -jeff
		localSdp->setRtpPort( UaDevice::instance()->getRtpPort( event->getDeviceId() ) );
		//localSdp->setRtpPort( 12355 );

 	// Set rtpPacketSize to default in ua.cfg file
	int rtpPacketSize = UaConfiguration::instance()->getNetworkRtpRate();

        SdpSession sdpDesc = localSdp->getSdpDescriptor();
        list < SdpMedia* > mediaList;
        mediaList = sdpDesc.getMediaList();
        list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

        vector < Data > * formatList = (*mediaIterator)->getStringFormatList();
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
            vector < ValueAttribute* > * valueAttribList = mediaAttrib->getValueAttributes();
            vector < ValueAttribute* > ::iterator attribIterator = valueAttribList->begin();
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
            mediaAttrib = new MediaAttributes();
            assert(mediaAttrib);
            (*mediaIterator)->setMediaAttributes(mediaAttrib);

            // create the new value attribute object
            ValueAttribute* attrib = new ValueAttribute();
            // set the attribute and its value
            attrib->setAttribute("ptime");
            LocalScopeAllocator lo;
            attrib->setValue( Data( UaConfiguration::instance()->getNetworkRtpRate() ).getData(lo) );

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
        call->setLocalSdp( new SipSdp( *localSdp ) );
    }
    else
    {
        // May not have SDP in original INVITE for 3rd party call control
        SipSdp sdp;
        SdpSession sdpDesc;
	
        Data connAddr = theSystem.gethostAddress();
        setHost(sdp, connAddr);

        SdpMedia* media = new SdpMedia;
        media->setPort( 23456 );   // which port to use when making a call
        // RTP doesn't have a suggestd number

        sdpDesc.addMedia( media );

        //To further perform any other operation on that particular media
        //item, use loc.
        sdp.setSdpDescriptor( sdpDesc );
        sdp.setUserName( theSystem.getUserName() );

        sdp.setSessionName( theSystem.getSessionName() );
        sdp.setAddress( theSystem.gethostAddress() );
        status.setContentData( &sdp, 0 );

        call->setLocalSdp( new SipSdp( sdp ) );
    }

    sipEvent->getSipStack()->sendReply( status );
    // deviceEvent->getSipStack()->sendReply( status );

    // Start callee guard timer
    Sptr < UaDigitTimerEvent > callerTO = new UaDigitTimerEvent( event->getFifo() );

    assert( callerTO != 0 );

    callerTO->callId = new SipCallId( sipEvent->getSipCallLeg()->getCallId() );
    callerTO->setLoadGenSignalType( LoadGenRestartCall );
    callerTO->setSipStack( sipEvent->getSipStack() );

	callerTO->setDeviceId( event->getDeviceId() );

    (event->getFifo())->addDelayMs( callerTO,
                                    UaConfiguration::instance()->getAnswerRestartTimer() );

    Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    return stateMachine->findState( "StateAutoInCall" );
}


