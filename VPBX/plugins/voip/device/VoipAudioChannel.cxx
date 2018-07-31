/*
* $Id: VoipAudioChannel.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "cpLog.h"
#include "VoipAudioChannel.hxx"
#include "EndPoint.hxx"
#include "VoipCtrlDevice.hxx"
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
 
#include "SipContext.hxx"
#include "InviteMsg.hxx"
#include "SipSdp.hxx"
#include "SdpHandler.hxx"

#include "PbxAgent.hxx"
#include "CallMsgBuilder.hxx"
#include "CallEvent.hxx"

#include "rtpTools.hxx"

// #define ULAW_PAYLOAD 1012
#define ULAW_SAMPLE_RATE 				160
#define RESID_RTP_RATE 					160
#define NETWORK_RTP_RATE 				320

using namespace Assist;


VoipAudioChannel::VoipAudioChannel(VoipCtrlDevice  *_parent, int fd )
	: AudioChannel(_parent, "VOIPAudio"),
	audioStack(0),
	inRtpPkt(0),
	_DTMFInterface(0),
	outRtpPkt(0)
{
	myFd = fd;
	myState = AUDIO_STATE_IDLE;
	
	//actionType = StartGreeting;
	greetingFd = open("/usr/etc/vpbx/tones/greeting.u", O_RDONLY);
	assert(greetingFd > 0);

	mySeedRtpTime = generate32();
	prevRtpTime = mySeedRtpTime;	
}


VoipAudioChannel::~VoipAudioChannel()
{
	cpLog(LOG_DEBUG, "VoipAudioChannel Destorying.....");
}


/* activate/deactivate bchannel */
void VoipAudioChannel::activate( )
{
	Sptr <VoipEndPoint> vep;
	vep.dynamicCast(getEndPoint());
	assert(vep != 0);
	Sptr <SipContext> context = vep->getSipContext();
	assert( context != 0 );
     //added by lijie  2005-06-03
    //when event from virtual pbx device , no need to this operation 
    /*if(call->getAudioSet())
		return 0;
    else
	  call->setAudioSet(true);	*/

	Sptr <SipSdp> remoteSdp;
	Sptr <SipSdp> localSdp;

	//get the current invite msg
	Sptr <InviteMsg> inviteMsg = context->getRingInvite();
	InviteMsg invMsg = context->getContact()->getInviteMsg();
	if ( *inviteMsg == invMsg )
	{
		remoteSdp = context->getRemoteSdp();
		localSdp = context->getLocalSdp();
	}
	else
	{
		remoteSdp = context->getRemote2Sdp();
		localSdp = context->getLocal2Sdp();
	}

	// if remote sdp is not avialable, don't start audio
	if ( remoteSdp == 0 )
	{
		cpDebug(LOG_DEBUG, "No Remote Sdp.  Audio not started");
		return ;
	}

	assert( localSdp != 0 );

	struct HardwareAudioRequest request;
	request.type = AudioStart;

	// Set remote host and port
	LocalScopeAllocator lo;
	strcpy( request.remoteHost, remoteSdp->getConnAddress().getData(lo) );
	request.remotePort = remoteSdp->getRtpPort();

	// Set local host and port
	request.localPort = localSdp->getRtpPort();
	strcpy( request.localHost, localSdp->getConnAddress().getData(lo) );

	//TODO Why echo cancellation always true?
	request.echoCancellation = true;

	// Check local, not remote ???
	// How is this related to Network_RTP_Rate in configuration file???
	//    cpDebug(LOG_DEBUG, "Local SDP:\n%s", localSdp->encode().logData() );

	// determine local packet size
	request.rtpPacketSize = getRtpPacketSize(*localSdp);
	if(request.rtpPacketSize <= 0)
	{
		cpDebug(LOG_DEBUG,  "could not find local SDP RTP packet size, setting to default" );
		// Set rtpPacketSize to default in ua.cfg file
		request.rtpPacketSize = PbxConfiguration::instance()->getNetworkRtpRate();
	}

	cpDebug(LOG_DEBUG, "Start audio( duplex ):" );
	cpDebug(LOG_DEBUG, "localHost     = %s", request.localHost);
	cpDebug(LOG_DEBUG, "localPort     = %d", request.localPort);
	cpDebug(LOG_DEBUG, "remoteHost    = %s", request.remoteHost);
	cpDebug(LOG_DEBUG, "remotePort    = %d", request.remotePort);
	cpDebug(LOG_DEBUG, "rtpPacketSize = %d", request.rtpPacketSize);

    //TODO: this will be removed once 183 is integrated.
    //request->sendRingback = false;

 	request.actionType = ActionNull;

	currentDigit = -1;

	audioStart(request);
}

void VoipAudioChannel::deactivate()
{
	audioStop();
}

int VoipAudioChannel::ReadVData(char * buffer)
{
	int n = 0;

	assert(audioStack != 0);
	RtpSessionState sessionState = audioStack->getSessionState();
	
	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
		inRtpPkt = audioStack->receive();
		if (inRtpPkt)
		{
			int length = inRtpPkt->getPayloadUsage();
			if ( length != RESID_RTP_RATE)
				cpLog(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			char *vBuf;
			vBuf = inRtpPkt->getPayloadLoc();
			memcpy(buffer, vBuf, VOICE_RATE);
			//write (myDev->fd, vBuf, RESID_RTP_RATE);
//			myPeer->WriteVData(vBuf, RESID_RTP_RATE);
			cpLog(LOG_DEBUG, "%s read %d bytes", getName(), length);
			delete inRtpPkt; 
			inRtpPkt = NULL;

			return VOICE_RATE;
		}
	}

	return n;
}


int VoipAudioChannel::WriteVData(char * buffer, int length)
{
	if(length <= 0)
		return 0;

	int cc = 0;

	assert(audioStack != 0);
	RtpSessionState sessionState = audioStack->getSessionState();
	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
#if 0
		if (length > 0)
			cc = audioStack->transmitRaw(buffer, length);
#else		
		int result;
		outRtpPkt = audioStack->createPacket();

		if(length != outRtpPkt->getPayloadSize() )
			cpDebug(LOG_DEBUG, "RTP Packet load size is %d, but buffer is % bytes", outRtpPkt->getPayloadSize(), length );

		cc = (length > outRtpPkt->getPayloadSize() )? outRtpPkt->getPayloadSize():length;
		memcpy(outRtpPkt->getPayloadLoc(), buffer, cc );
		outRtpPkt->setPayloadUsage(cc );

		outRtpPkt->setRtpTime( prevRtpTime );
		prevRtpTime += cc ;

		result = audioStack->transmit(outRtpPkt);
		if (result < 0)
		{
			cpLog(LOG_ERR, "RTP transmit failed");
		}

		delete outRtpPkt;
		outRtpPkt = NULL;
		
#endif		
/*		
		if(currentDigit != 0)
		{
		}
*/


	}

	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}

	return cc;	
}

int VoipAudioChannel::audioStart (const Assist::HardwareAudioRequest& request)
{

	int fd = -1;
	cpLog( LOG_DEBUG, ">> Establishing audio on device %s" , getName() );
	cpLog( LOG_DEBUG, ">> Listening on port: %d", request.localPort );
	cpLog( LOG_DEBUG, ">> Sending to host  : %s", request.remoteHost );
	cpLog( LOG_DEBUG, ">> Sending to port  : %d", request.remotePort );
	cpLog( LOG_DEBUG, ">> RTP packet size  : %d ms", request.rtpPacketSize );

	int jitterNew = PbxConfiguration::instance()->getJitterBuffer();

	actionMutex.lock();
	
	actionType = request.actionType;

	if(actionType ==  0 /*StartGreeting*/)
		fd = greetingFd;
	else  if(actionType == 2 /*StartRingBack*/)
		fd = ringbackFd;
	else if(actionType == 4 /* StartBusyTone*/)
		fd = busyFd;
	else if(actionType == 7 /*StartCallWaiting*/)
		fd = callwaitingFd;

	if(fd != -1)
	{
		//gettimeofday (&prevTime, 0);
		lseek(fd, 0, SEEK_SET);
	}
	actionMutex.unlock();


	audioMutex.lock();

	openDeviceFile();
	
	
	pbxDtmfMode = as_pbx_dtmf_rfc2883;
	if ( audioStack == 0 )
	{
		int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
		int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
		cpLog( LOG_DEBUG, "Remote RTCP port : %d", remoteRtcpPort );
		cpLog( LOG_DEBUG, "Local RTCP port  : %d", localRtcpPort );

		const char* remoteHost = 0;
		if ( request.remotePort != 0 )
			remoteHost = request.remoteHost;
		audioStack = new RtpSession(remoteHost, request.remotePort,  request.localPort, remoteRtcpPort, localRtcpPort, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

	}
	else
	{
		cpLog( LOG_DEBUG, "Modifying existing audio connection" );

		//need to determine what mode RtpSession is in
		RtpSessionState sessionState = audioStack->getSessionState();
		switch ( sessionState )
		{
			case rtp_session_sendonly:
				cpLog( LOG_DEBUG, "Session sendonly" );
				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpLog( LOG_DEBUG, "turning on receive" );
					cpLog( LOG_DEBUG, "local host: %s", request.localHost);
					cpLog( LOG_DEBUG, "local port: %d", request.localPort );
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

				}
				break;
			case rtp_session_recvonly:
				cpLog( LOG_DEBUG, "Session recvonly" );
				//turn on the send
				cpLog( LOG_DEBUG, "turning on the send" );
				cpLog( LOG_DEBUG, "remote host: %s", request.remoteHost );
				cpLog( LOG_DEBUG, "remote port: %d", request.remotePort );
				if ( request.remotePort != 0 )
				{
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setTransmiter(request.remoteHost, request.remotePort,  request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

				}
				break;
			case rtp_session_sendrecv:
				cpLog( LOG_DEBUG, "Session sendrecv both, RTP parameters can not be changed" );
				audioMutex.unlock();
				return 0;
				break;
			default:
				if ( request.remotePort != 0 && request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendrecv );
					cpLog( LOG_DEBUG, "Session state is sendrecv" );
				}
				else if ( request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_recvonly );
					cpLog( LOG_DEBUG, "Session state is recvonly" );
				}
				else if ( request.remotePort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendonly );
					cpLog( LOG_DEBUG, "Session state is sendonly" );
				}
				else
				{
					cpLog( LOG_ERR, "Session state is undefined" );
				}

				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpLog( LOG_DEBUG, "Turn on recv" );
					audioStack->setReceiver( request.localPort, request.localPort + 1, 0,  rtpPayloadPCMU, rtpPayloadPCMU, jitterNew );
				}

				//turn on the send
				if ( request.remotePort != 0 )
				{
					cpLog( LOG_DEBUG, "Turn on send" );
					audioStack->setTransmiter( request.remoteHost, request.remotePort, request.remotePort + 1,  rtpPayloadPCMU, rtpPayloadPCMU );

				}
				break;
		}
	}

	RtpSessionState sessionState = audioStack->getSessionState();
	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_recvonly)
	{
		audioActive = false;
		audioHalfActive = true;
	}
	else
	{
		audioActive = true;
		audioHalfActive = false;
	}

//	if(type == as_virtual_device)
	{
		_DTMFInterface = new RtpDevice <VoipAudioChannel> ( this );
		audioStack->setDTMFInterface( _DTMFInterface );
	}
	
	// inRtpPkt = 0;
	if( inRtpPkt )
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}

	audioStack->setApiFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);
	audioStack->setNetworkFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);

#if 0
	if(!as_tran)
	{
		PbxConfiguration *uaConf=PbxConfiguration::instance();
		if(uaConf->getVad())
		{
			as_tran=new AssistRtpTransVad(audioStack,rtpPayloadPCMU,rtpPayloadPCMU,1);
			cpLog(LOG_DEBUG, ":::::: [VAD] enabled ::::::");
		}
		else
		{
			as_tran=new AssistRtpTransmitter(audioStack);
			cpLog(LOG_DEBUG, ":::::: [VAD] disabled ::::::");
		}
	}
	assert(as_tran);
#endif 
	audioMutex.unlock();
	
	return 0;
} // end audioStart()


/*
*
*/
int VoipAudioChannel::audioStop()
{
	cpLog( LOG_DEBUG, "***   Stopping audio on device %s  ***" , getName());
	vusleep(200);

	// make hardware calls to stop audio

	audioMutex.lock();

	// mark audio as deactivated.
	audioActive = false;
	audioHalfActive = false;
	
	// close RTP session
	if ( audioStack )
	{
		RtpSessionState sessionState = audioStack->getSessionState();
		if ( sessionState == rtp_session_sendonly ||
			sessionState == rtp_session_sendrecv )
		{
			audioStack->transmitRTCPBYE();
		}
	}

	if (inRtpPkt)
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}

#if 0
	if (as_tran)
	{
		delete as_tran;
		as_tran = 0;
	}
#endif 

	if (audioStack)
	{
//		if(type==as_virtual_device) 
			audioStack->unsetDTMFInterface( _DTMFInterface );
		
		delete audioStack;
		audioStack = 0;
	}

	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}

	audioMutex.unlock();
	return 0;
}


int VoipAudioChannel::audioSuspend ()
{

	cpLog( LOG_INFO, "Suspending audio on device %s" , getName());
	cpLog( LOG_INFO, "Setting all RTP/RTCP ports to 0");

	audioMutex.lock();

	if ( audioStack != 0 )
	{ //set rtp session to inactive
		audioStack->setSessionState(rtp_session_inactive);
		int jitterNew=PbxConfiguration::instance()->getJitterBuffer();
		audioStack->setReceiver(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU);
	}

	audioActive = false;
	audioHalfActive = false;

	audioMutex.unlock();
	
	return 0;
}


int VoipAudioChannel::audioResume (const Assist::HardwareAudioRequest& request)
{
	int fd = -1;
	
	cpLog( LOG_INFO, "Resuming audio on device %s" , getName());
	cpLog( LOG_INFO, "Listening on port: %d", request.localPort);
	cpLog( LOG_INFO, "Sending to host: %s", request.remoteHost);
	cpLog( LOG_INFO, "Sending to port: %d", request.remotePort);
	cpLog( LOG_INFO, "RTP packet size: %d", request.rtpPacketSize);

	actionMutex.lock();
	
	actionType = request.actionType;

	if(actionType == StartGreeting)
		fd = greetingFd;
	else  if(actionType == StartRingBack)
		fd = ringbackFd;
	else if(actionType == StartBusyTone)
		fd = busyFd;
	else if(actionType == StartCallWaiting)
		fd = callwaitingFd;

	if(fd != -1)
	{
		gettimeofday (&prevTime, 0);
		lseek(fd, 0, SEEK_SET);
	}
	actionMutex.unlock();


	audioMutex.lock();
	if ( audioStack == 0 )
	{
		cpLog( LOG_ERR, "Try to resume an existing audio channel, but no existing audio channel");
		audioMutex.unlock();
		return 0;
	}

    //resuming audio channel
    //int remoteRtcpPort=(request.remotePort>0)?request.remotePort+1:0;
    //int localRtcpPort=(request.localPort>0)?request.localPort+1:0;
	const char* remoteHost = 0;
	if ( request.remotePort != 0 )
		remoteHost = request.remoteHost;

	int jitterNew=PbxConfiguration::instance()->getJitterBuffer();
	if ( request.localPort != 0 && request.remotePort != 0 )
	{        //full duplex audio
		audioStack->setSessionState(rtp_session_sendrecv);
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);	
		audioActive = true;
		audioHalfActive = false;
	}
	else if ( request.localPort != 0 )
	{//receive only
		audioStack->setSessionState(rtp_session_sendrecv);
		audioActive = false;
		audioHalfActive = true;
	}
	else if ( request.remotePort != 0 )
	{ //transmit only
		audioStack->setSessionState(rtp_session_sendrecv);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
		audioActive = false;
		audioHalfActive = true;
	}

	audioMutex.unlock();
//
	return 0;
	
}


int VoipAudioChannel::openDeviceFile()
{
	//Sptr <AudioChannel> audio = getAudio();
	//assert(audio != 0);
	
	return 0;
}

/* callback in RTP stacks */
void VoipAudioChannel::recvRTPDTMF( int digit, int duration )
{
	const char *phone_pad = {"0123456789*#"};
	cpLog(LOG_DEBUG, "DTMF digit %d(duration is %d) has been received from RTP stack on %s", digit, duration, getName());

#if 0
	if(pbxDtmfMode != as_pbx_dtmf_audio)
		pbxDtmfMode = as_pbx_dtmf_rfc2883;
	else
		return;
#endif

	if( digit >=0 && digit< 12 )
	{
		/* send SETUP msg to Scheduler directly */ 
//		myPeer->emitDtmfSignal();
		char num[10];
		num[0] = phone_pad[digit];
		num[1] = 0;	/* terminate it */
		Sptr <CallEvent> setupEvent = new CallEvent();
		Sptr <CommonMsg> setupMsg = new _CallSetup;
		setupMsg->setTo(num);

		setupEvent->setCallMsg(setupMsg);
		setupEvent->setEndPoint( getEndPoint() );
		setupEvent->setCallId( getEndPoint()->getCallId() );
		PbxAgent::instance()->getScheduler()->reportEvent(setupEvent);
	}
	else
	{
		cpLog(LOG_ERR, "Unrecognized DTMF key received from RTP stack");
		return;
	}
	return ;
}

/* send DTMF signal in the method of RFC2833 */

void VoipAudioChannel::transmitRTPDTMF(char digit)
{
	switch( digit )
	{
		case '*':
			currentDigit = 10;
			break;
		case '#':
			currentDigit = 11;
			break;
		default:	
			currentDigit = digit - '0' ;
			break;
	}
	if(currentDigit < 0 && currentDigit > 11 )
		currentDigit = -1;

	/* digit should be transmit in audiothread */
	if( audioStack && currentDigit != -1 )
	{
		cpDebug(LOG_DEBUG, "DTMF %c sent out in RTP ", digit );
		audioStack->transmitEvent( currentDigit);
		currentDigit = -1;
	}
	else
	{
		cpDebug(LOG_DEBUG, "No audio stack existing to send DTMF event" );
	}

}


//return -1 means that no invalid fd exists
int VoipAudioChannel::getSendBackTone(char *ringData, int size)
{
	int cc = -1;

#if 0
	//actionType = StartGreeting;
	//fd = greetingFd;
	int fd = -1;
	int interval;
	struct timeval nowTime;      	
	static bool round_1 = false;
	actionMutex.lock();
	if(actionType == StartGreeting)
		fd = greetingFd;
	else  if(actionType == StartRingBack)
		fd = ringbackFd;
	else if(actionType == StartBusyTone)
		fd = busyFd;
	else if(actionType == StartCallWaiting)
		fd = callwaitingFd;
	else
	{
		actionMutex.unlock();
		return -1;
	}

	gettimeofday (&nowTime, 0);


	interval = 1000000*(nowTime.tv_sec-prevTime.tv_sec) + nowTime.tv_usec - prevTime.tv_usec;
	
	if(interval<20000)
	{
		actionMutex.unlock();
		return 0;
	}
#endif 

#if 0
	for (int i = 0; i < 1500; ++i)
	{
		float a = i*105/203;
	}
	cc = read(fd, ringData, RESID_RTP_RATE);
		
	while ( cc < size )
	{
		lseek(fd, 0, SEEK_SET);
		cc += read( fd, ringData + cc, size - cc);
		round_1 = !round_1;
	}

	if (round_1)
	{
		sleep(1);
		round_1 = !round_1;
	}
#endif 

#if 0
	prevTime = nowTime;

	actionMutex.unlock();
#endif 
	return cc;
	
}

int VoipAudioChannel::getRtpPort()
{
	//this is an arbitrarily defined number
	const int HIGH_RTP_PORT = PbxConfiguration::instance()->getMaxRTPPort();//PbxConfiguration::instance()->getMaxRtpPort();
	const int LOW_RTP_PORT = PbxConfiguration::instance()->getMinRTPPort();

	int port = 0;
	int lowPort = LOW_RTP_PORT;

	// create a rtp session if there is no existing session alread this Rtp session will be idle

	audioMutex.lock();
	if ( audioStack == 0 )
	{
		audioStack = new RtpSession(0);
	}

	int jitterNew=PbxConfiguration::instance()->getJitterBuffer();

	while ( 1 )
	{// resever a rtp port
		port=reserveRtpPort(lowPort, HIGH_RTP_PORT,jitterNew);

		lowPort = (port > lowPort) ? port : lowPort;

		// attempt to reserve a rtcp port on port number higher than the rtp port
		if ( port != 0 && audioStack->reserveRtcpPort(port + 1, 0) != 0 )
		{
			cpLog( LOG_DEBUG, ":::::: Reserved rtp/rtcp ports %d/%d ::::::", port, port+1 );
			break;
		}

		// if not successful allocating rtcp port, increment the minimum rtp
		// port and try again.  If lowPort reaches HIGH_RTP_PORT, return 0
		// to indicate port allocation failed.
		lowPort += 2;
		if ( lowPort > HIGH_RTP_PORT )
		{
			port = 0;
			break;
		}
	}

	audioMutex.unlock();
	return port;
}

int VoipAudioChannel::reserveRtpPort(int localMin, int localMax, int jitterNew)
{
	int port = 0;

	if ( audioStack->getRtpRecv() == 0 )
	{
		try
		{
			//let RtpReceiver() automatically generate a port number
			audioStack->setSessionState(rtp_session_recvonly);
			audioStack->setReceiver(localMin,0, localMax-localMin, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
			audioStack->setSessionState(rtp_session_undefined);
			port = audioStack->getRtpRecv() ->getPort();
		}
		catch ( UdpStackExceptionPortsInUse& )
		{
			if ( localMin == localMax )
				cpLog( LOG_ERR, "port %d is not available", localMin );
			else
				cpLog( LOG_ERR, "No ports between %d and %d are available", localMin, localMax);
			audioStack->releaseRtpPort();
		}
	}
	else
	{
		port = audioStack->getRtpRecv()->getPort();
	}

	return port;
}

void VoipAudioChannel::releaseRtpPort()
{
	cpLog(LOG_DEBUG, "VoipAudioChannel::releaseRtpPort()");
	audioMutex.lock();
	if ( audioStack )
	{
		RtpSessionState sessionState = audioStack->getSessionState();
		if ( sessionState == rtp_session_sendonly ||
			sessionState == rtp_session_sendrecv )
		{
			if(audioStack->getRtcpTran())audioStack->transmitRTCPBYE();
		}
		
//		if(type==as_virtual_device )
			audioStack->unsetDTMFInterface( _DTMFInterface );

		int port = audioStack->releaseRtpPort();
		cpLog( LOG_DEBUG, "rtp port %d released", port );
		port = audioStack->releaseRtcpPort();
		cpLog( LOG_DEBUG, "rtcp port %d released", port );
		delete audioStack;
		audioStack = 0;
	}

	if (inRtpPkt)
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}
	
	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}
	audioMutex.unlock();

}

