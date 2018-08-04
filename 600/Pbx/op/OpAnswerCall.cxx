/*
 * $Id: OpAnswerCall.cxx,v 1.4 2007/06/18 11:11:17 lizhijie Exp $
 */

#include "global.h"
#include "OpAnswerCall.hxx"
#include "ResGwDevice.hxx"

#include "UaDeviceEvent.hxx"
#include "UaStateMachine.hxx"
#include "UaCallInfo.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "Sdp2Media.hxx"
#include "SystemInfo.hxx"
#include "SipSdp.hxx"

#include "pbx_globals.h"

using namespace Vocal;


using Vocal::SDP::SdpSession;
using Vocal::SDP::SdpMedia;
using Vocal::SDP::MediaAttributes;
using Vocal::SDP::ValueAttribute;
using Vocal::SDP::SdpRtpMapAttribute;


OpAnswerCall::OpAnswerCall()
{
}


OpAnswerCall::~OpAnswerCall()
{
}


const char* const
OpAnswerCall::name() const
{
    return "OpAnswerCall";
}


const Sptr < State >
OpAnswerCall::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpAnswerCall operation" );

    Sptr < UaDeviceEvent > deviceEvent;
    deviceEvent.dynamicCast( event );
    if ( deviceEvent == 0 )
    {
        return 0;
    }
    if ( deviceEvent->type != DeviceEventHookUp &&
            deviceEvent->type != DeviceEventFlash )
    {
        return 0;
    }

	 Sptr < UaStateMachine > stateMachine;
    stateMachine.dynamicCast( event->getCallInfo()->getFeature() );
    assert( stateMachine != 0 );

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

    Sptr <InviteMsg> msg = call->getRingInvite();
    assert( msg != 0 );

    SipCallId callId = msg->getCallId();

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
    //added by lijie
    else
    {
		//the device can not be ringing for some unkown reason
		if(!call->getCalleeIdle())
			return 0;
    }

    // Get remote SDP from message
    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast ( msg->getContentData(0) );
    if ( remoteSdp == 0 )
    {
        cpDebug(LOG_DEBUG, "No remote SDP in INVITE" );
    }
    else
    {
        call->setRemoteSdp( new SipSdp( *remoteSdp ) );
    }

	cpDebug(LOG_DEBUG, "--------------make a 200OK.  ---- with: %d", deviceEvent->type);
    StatusMsg status( *msg, 200/*OK*/ );

	Data hip=msg->getRequestLine().getHost();
    // Add myself to the contact
    Sptr< SipUrl > myUrl = new SipUrl;

    string callernum = UaDevice::instance()->getPhoneNumber(event->getDeviceId());

    myUrl->setUserValue(Data(callernum));

   myUrl->setHost( hip );
    myUrl->setPort( UaConfiguration::instance()->getLocalSipPort() );
    if(UaConfiguration::instance()->getSipTransport() == "TCP")
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
	
		localSdp->setRtpPort( UaDevice::instance()->getRtpPort( event->getDeviceId() ) );

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
	    cpDebug(LOG_DEBUG, "at least one mediaAttrib");
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
	    cpDebug(LOG_DEBUG, "no mediaAttrib");
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
        deviceEvent->getSipStack()->sendReply( status );
    }
    else  // localSdp == 0
    {
	cpDebug(LOG_DEBUG, "localSdp == 0");
        // May not have SDP in original INVITE for 3rd party call control
        SipSdp sdp;

	Data hostAddr = hip;

        if(UaConfiguration::instance()->getNATAddress() != "")
        {
            hostAddr = UaConfiguration::instance()->getNATAddress();
        }

		int rtpPort = UaDevice::instance()->getRtpPort( event->getDeviceId() );
		
        doAnswerStuff(sdp, remoteSdp, hostAddr, rtpPort);

        status.setContentData( &sdp, 0 );
        call->setLocalSdp( new SipSdp( sdp ) );
        deviceEvent->getSipStack()->sendReply( status );
    }

	/* modify by wuaiwu */

	if(event->getDeviceId() >= VIRTUAL_DEVICES_EPOCH )
		return stateMachine->findState( "StatePbxDialing" );
	else
		return stateMachine->findState( "StateInCall" );

	return 0;
}


