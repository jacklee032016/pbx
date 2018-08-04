/*
 * $Id: OpStartAudioDuplex.cxx,v 1.3 2007/03/03 18:39:28 lizhijie Exp $
 */

#include "global.h"
#include <cassert>

#include "cpLog.h"
#include "SipSdp.hxx"
#include "OpStartAudioDuplex.hxx"
#include "UaDevice.hxx"
#include "UaHardwareEvent.hxx"
#include "UaCallInfo.hxx"
#include "UaConfiguration.hxx"
#include "SdpHandler.hxx"
#include "pbx_globals.h"
#if __CDR_OPEN__

#include "SimulateDB.hxx"
using namespace SDB;

#endif

using namespace Vocal;


OpStartAudioDuplex::OpStartAudioDuplex()
{
}


OpStartAudioDuplex::~OpStartAudioDuplex()
{
}


const char* const
OpStartAudioDuplex::name() const
{
    return "OpStartAudioDuplex";
}


const Sptr < State >
OpStartAudioDuplex::process( const Sptr < SipProxyEvent > event )
{
    cpDebug(LOG_DEBUG, "StartAudioDuplex operation" );

   
    Sptr < UaCallInfo > call;
	
    call.dynamicCast( event->getCallInfo() );
    assert( call != 0 );
     //added by lijie  2005-06-03
    //when event from virtual pbx device , no need to this operation 
    /*if(call->getAudioSet())
		return 0;
    else
	  call->setAudioSet(true);	*/

    Sptr < SipSdp > remoteSdp;
    Sptr < SipSdp > localSdp;

    //get the current invite msg
    Sptr < InviteMsg > inviteMsg = call->getRingInvite();
    InviteMsg invMsg = call->getContact()->getInviteMsg();
    if ( *inviteMsg == invMsg )
    {
        remoteSdp = call->getRemoteSdp();
        localSdp = call->getLocalSdp();
    }
    else
    {
        remoteSdp = call->getRemote2Sdp();
        localSdp = call->getLocal2Sdp();
    }


    // if remote sdp is not avialable, don't start audio
    if ( remoteSdp == 0 )
    {
        cpDebug(LOG_DEBUG, "No Remote Sdp.  Audio not started");
        return 0;
    }

    if (event->getDeviceId() >= VIRTUAL_DEVICES_EPOCH)
    {
    		cpDebug(LOG_DEBUG, "-------------START startAudio ");
    		startAudio( localSdp, remoteSdp, event->getDeviceId() );
		return 0;
    }

    assert( localSdp != 0 );

    Sptr < UaHardwareEvent > signal 
	= new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareAudioType;

	signal->setDeviceId(event->getDeviceId() );

    struct HardwareAudioRequest* request = &(signal->signalOrRequest.request);

    request->type = AudioStart;

    // Set remote host and port
    LocalScopeAllocator lo;
    strcpy( request->remoteHost,
            remoteSdp->getConnAddress().getData(lo) );
    request->remotePort = remoteSdp->getRtpPort();

    // Set local host and port
    request->localPort = localSdp->getRtpPort();
    strcpy( request->localHost,
            localSdp->getConnAddress().getData(lo) );

    //TODO Why echo cancellation always true?
    request->echoCancellation = true;


    // Check local, not remote ???
    // How is this related to Network_RTP_Rate in configuration file???
    //    cpDebug(LOG_DEBUG, "Local SDP:\n%s", localSdp->encode().logData() );


    // determine local packet size

    request->rtpPacketSize = getRtpPacketSize(*localSdp);

    if(request->rtpPacketSize <= 0)
    {
        cpDebug(LOG_DEBUG, 
	       "could not find local SDP RTP packet size, setting to default" 
	    );
	// Set rtpPacketSize to default in ua.cfg file
	request->rtpPacketSize 
	    = UaConfiguration::instance()->getNetworkRtpRate();
    }

    cpDebug(LOG_DEBUG, "Start audio( duplex ):" );
    cpDebug(LOG_DEBUG, "localHost     = %s", request->localHost);
    cpDebug(LOG_DEBUG, "localPort     = %d", request->localPort);
    cpDebug(LOG_DEBUG, "remoteHost    = %s", request->remoteHost);
    cpDebug(LOG_DEBUG, "remotePort    = %d", request->remotePort);
    cpDebug(LOG_DEBUG, "rtpPacketSize = %d", request->rtpPacketSize);

    //TODO: this will be removed once 183 is integrated.
    //request->sendRingback = false;

 	request->actionType = ActionNull;

    UaDevice::getDeviceQueue()->add( signal );

#if __CDR_OPEN__  
    /* added by wuaiwu 2005/05/11 */
   
    if (SimulateDB::instance() != NULL)
    {
	    int deviceID = event->getDeviceId();
	    SimulateDB::instance()->addCdrClient(inviteMsg, deviceID, CALL_START);
    }

    /* end of added 2005/05/11 */
#endif
    return 0;
}

