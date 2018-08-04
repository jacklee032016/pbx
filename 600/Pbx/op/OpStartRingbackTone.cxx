/*
 * $Id: OpStartRingbackTone.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "cpLog.h"
#include "SipEvent.hxx"
#include "StatusMsg.hxx"
#include "OpStartRingbackTone.hxx"
#include "UaHardwareEvent.hxx"
#include "UaDevice.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "SipSdp.hxx"

#ifdef HAS_RSVPLIB
#include "SuaRsvp.hxx"
#endif


using namespace Vocal;


OpStartRingbackTone::OpStartRingbackTone()
{
}


OpStartRingbackTone::~OpStartRingbackTone()
{
}


const char* const
OpStartRingbackTone::name() const
{
    return "OpStartRingbackTone";
}


const Sptr < State >
OpStartRingbackTone::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "OpStartRingbackTone operation" );

    Sptr < SipEvent > sipEvent;
    sipEvent.dynamicCast( event );
    if ( sipEvent == 0 )
    {
        return 0;
    }
    Sptr < SipMsg > sipMsg = sipEvent->getSipMsg();
    assert( sipMsg != 0 );

    Sptr < StatusMsg > msg;
    msg.dynamicCast( sipMsg );
    if ( msg == 0 )
    {
        return 0;
    }

    Sptr < UaCallInfo > call;
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );

	int statusCode = msg->getStatusLine().getStatusCode();
	
    if ( statusCode != 180 && statusCode != 183 && statusCode != 182 )
    {
        return 0;
    };

//remove for long time ring no response problem ,modified by lijie
#if 0
    if ( cancelTimer(event) )
    {
        cpDebug(LOG_DEBUG, "Ua Timer cancelled" );
    }
#endif 

    // This is a temporary fix to handle the case in which more than one
    // 180/183 messages are received before 200 arrives
    // TODO: check sdp in the 180/183.  If different from previous 180/183,
    // use the lastest 180/183 sdp
    Sptr < Contact > contact = call->findContact( *msg );
    assert( contact != 0 );

    int status = contact->getStatus();

    if ( status == 180 || status == 183 || status == 182 )
    {
        cpLog( LOG_ERR, "Have existing 180/182/183 message" );
        cpLog( LOG_ERR, "Igore the current 180/182/183 message" );
        return 0;
    }
	
   //added by lijie 2005-06-03
    if(statusCode == 182)
    {
	 cancelTimer(event);
    }

    cpDebug(LOG_DEBUG, "Start Ringback tone" );

    bool remoteRingback = true;

    //update contact
    contact->update( *msg );

    //figure out if this is the first call or the second call
    Sptr < SipSdp > localSdp;
    Sptr < InviteMsg > inviteMsg = call->getRingInvite();

    InviteMsg invMsg = call->getContact()->getInviteMsg();
    if ( *inviteMsg == invMsg )
    {
        localSdp = call->getLocalSdp();
    }
    else
    {
	cpDebug(LOG_DEBUG, "INVITE did not match, getting Local2 SDP");
	cpDebug(LOG_DEBUG_STACK, 
	      "Original: %s", 
	      (inviteMsg)->encode().logData());
	cpDebug(LOG_DEBUG_STACK, "New: %s", invMsg.encode().logData());
        localSdp = call->getLocal2Sdp();
    }

    /// should this really be an assert?  this seems dangerous
    assert( localSdp != 0 );
    
    // Set rtpPacketSize to default in ua.cfg file
    int rtpPacketSize = UaConfiguration::instance()->getNetworkRtpRate();

    Sptr<SipSdp> remoteSdp;
    remoteSdp.dynamicCast( sipMsg->getContentData(0) );
    
    if( remoteSdp == 0 )
    {
        cpDebug(LOG_DEBUG, "No remote SDP in 180/183" );
        remoteRingback = false;
    }
    else
    {
	// get the remote SDP's rtp rate
	int rtpPacketSize = getRtpPacketSize(*remoteSdp);

	if(rtpPacketSize > 0)
	{
	    setRtpPacketSize(*localSdp, rtpPacketSize);
	    call->setRemoteSdp( new SipSdp( *remoteSdp ) );
	}
	else
	{
	    remoteRingback = false;
	}
    }

    Sptr < UaHardwareEvent > signal =
	new UaHardwareEvent( UaDevice::getDeviceQueue() );

	signal->setDeviceId( event->getDeviceId() );
    
    if ( remoteRingback )
    {
        cpDebug(LOG_DEBUG, "Early RTP media" );

        call->getContact()->setRemoteRingback( true );

        signal->type = HardwareAudioType;

        struct HardwareAudioRequest* request 
	    = &(signal->signalOrRequest.request);

        request->type = AudioStart;	

        strcpy( request->remoteHost, "\0" );
        request->remotePort = 0;
        LocalScopeAllocator lo;
        strcpy( request->localHost, localSdp->getConnAddress().getData(lo) );
        request->localPort = localSdp->getRtpPort();

//        request->echoCancellation = true;

        request->rtpPacketSize = rtpPacketSize;
    }
    else
    {
        call->getContact()->setRemoteRingback( false );
        signal->type = HardwareSignalType;
	//below is added by lijie	
	if(statusCode != 182)
       	 signal->signalOrRequest.signal = DeviceSignalLocalRingbackStart;
	else
	{
		signal->signalOrRequest.signal = DeviceSignalStartCallWaitingBeep;
	}	
    }

    UaDevice::getDeviceQueue()->add( signal );

    return 0;
}

