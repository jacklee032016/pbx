/*
 * $Id: AssistDevice.cxx,v 1.5 2007/09/03 21:29:17 lizhijie Exp $
*/

#include "AssistDevice.hxx"
#include "UaConfiguration.hxx"
#include "AssistNullDevice.hxx"
#include "AssistRtpTransVad.hxx"
#include <string.h>

using namespace Vocal;


#if 0 //modified by lijie 2005-06-03
AssistDevice::	AssistDevice(AssistNullDevice *_agent, char *devicename, device_type_t _type, int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:myCallId(0),
	audioStack(0),
	inRtpPkt(0),
	type(_type),
	myDeviceId(deviceId),
	callProcessingQueue(sessionQ),
	myReadBuffer(160*14),
	myWriteBuffer(160*14)

{
	//added by lijie
	callWaitingIdList = new CallWaitingIdList;
	myCallId = 0;
	//added ends here
	myDeviceName = (char *)malloc(AS_DEVICE_NAME_LENGTH);
	memset(myDeviceName, 0 , AS_DEVICE_NAME_LENGTH);
	sprintf(myDeviceName, "%s", 	devicename);

	dsp = assist_dsp_init(UaConfiguration::instance()->getG711Law(), UaConfiguration::instance()->getRxGain(),
		UaConfiguration::instance()->getTxGain(),DTMF_PARSE_LENGTH);

	myDigitCollector = new DigitCollector(deviceId , sessionQ );
	agent = _agent;
	as_tran=0;
}


AssistDevice::~AssistDevice()
{
	if(myDeviceName)
	{
		free(myDeviceName);
		myDeviceName =  0;
	}

	assist_dsp_destory( dsp);
#if 0	
	delete myDigitCollector;
	myDigitCollector = 0;
#endif	
}


/* send DTMF signal in the method of RFC2833 */
void AssistDevice::provideDtmf(DeviceSignalType signal)
{
	char digit = ' ';
	int dtmfEvent = 0;

	switch( signal )
	{
		case DeviceSignalDigitStar: 
			digit = '*';
			dtmfEvent = 10;
			break;
		case DeviceSignalDigitHash:
			digit = '#';
			dtmfEvent = 11;
			break;
		case DeviceSignalDigit0:
		case DeviceSignalDigit1:
		case DeviceSignalDigit2:
		case DeviceSignalDigit3:
		case DeviceSignalDigit4:
		case DeviceSignalDigit5:
		case DeviceSignalDigit6:
		case DeviceSignalDigit7:
		case DeviceSignalDigit8:
		case DeviceSignalDigit9:
			digit = '0' + signal - DeviceSignalDigit0;
			dtmfEvent = signal - DeviceSignalDigit0;
			break;
		default:
			cpDebug(LOG_DEBUG, "Unknown DTMF Signal %d", signal );
			return;
	}

	if( audioStack )
	{
		cpDebug(LOG_DEBUG, "DTMF %c sent out in RTP ", digit );
		audioStack->transmitEvent( dtmfEvent );
	}
	else
	{
		cpDebug(LOG_DEBUG, "No audio stack existing to send DTMF event" );
	}
}

int AssistDevice::getRtpPort()
{
	//this is an arbitrarily defined number
	const int HIGH_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
	const int LOW_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

	int port = 0;
	int lowPort = LOW_RTP_PORT;

	// create a rtp session if there is no existing session alread this Rtp session will be idle
	if ( audioStack == 0 )
	{
		audioStack = new RtpSession(0);
	}

	int jitterNew=UaConfiguration::instance()->getJitterBuffer();
	while ( 1 )
	{// resever a rtp port
		port=reserveRtpPort(audioStack,lowPort, HIGH_RTP_PORT,jitterNew);
//		port = audioStack->reserveRtpPort(lowPort, HIGH_RTP_PORT);
		lowPort = (port > lowPort) ? port : lowPort;

		// attempt to reserve a rtcp port on port number higher than the rtp port
		if ( port != 0 && audioStack->reserveRtcpPort(port + 1, 0) != 0 )
		{
			cpDebug(LOG_DEBUG, "Reserved rtp/rtcp ports %d/%d", port, port+1 );
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

	return port;
}

int
AssistDevice::reserveRtpPort(RtpSession *audioStack,int localMin, int localMax, int jitterNew)
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

void AssistDevice::releaseRtpPort()
{
	// destroy the rtp session if one exists
	if ( audioStack != 0 )
	{
//		if(type==as_virtual_device )
		if(type == as_virtual_device && pbxDtmfMode==as_pbx_dtmf_rfc2883 )
			audioStack->unsetDTMFInterface( _DTMFInterface );

		int port = audioStack->releaseRtpPort();
		cpDebug(LOG_DEBUG, "rtp port %d released", port );
		port = audioStack->releaseRtcpPort();
		cpDebug(LOG_DEBUG, "rtcp port %d released", port );
		delete audioStack;
		audioStack = 0;
	}

	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}

}

void AssistDevice::provideCallInfo(string callerNum, string calleeNum, string reason)
{
	cpDebug(LOG_DEBUG, "Caller is '%s',\t\tCallee is '%s',\t\treason is '%s'\r\n" , 
		callerNum.c_str(), calleeNum.c_str(), reason.c_str() );
}

int AssistDevice::audioStart (const HardwareAudioRequest& request)
{
	cpDebug(LOG_DEBUG, ">> Establishing audio on device %s" , myDeviceName );
	cpDebug(LOG_DEBUG, ">> Listening on port: %d", request.localPort );
	cpDebug(LOG_DEBUG, ">> Sending to host  : %s", request.remoteHost );
	cpDebug(LOG_DEBUG, ">> Sending to port  : %d", request.remotePort );
	cpDebug(LOG_DEBUG, ">> RTP packet size  : %d ms", request.rtpPacketSize );

	openDeviceFile();
	
	int jitterNew=UaConfiguration::instance()->getJitterBuffer();
	if ( audioStack == 0 )
	{
		int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
		int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
		cpDebug(LOG_DEBUG, "Remote RTCP port : %d", remoteRtcpPort );
		cpDebug(LOG_DEBUG, "Local RTCP port  : %d", localRtcpPort );

		const char* remoteHost = 0;
		if ( request.remotePort != 0 )
			remoteHost = request.remoteHost;
		audioStack = new RtpSession(remoteHost, request.remotePort,  request.localPort, remoteRtcpPort, localRtcpPort, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

#if 0
		if(type==as_hardware_device)
		{
			if(dsp->law == as_g711_u_law)
				as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
			else if(dsp->law == as_g711_a_law)
				as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
		}
#endif

	}
	else
	{
		cpDebug(LOG_DEBUG, "Modifying existing audio connection" );

		//need to determine what mode RtpSession is in
		RtpSessionState sessionState = audioStack->getSessionState();
		switch ( sessionState )
		{
			case rtp_session_sendonly:
				cpDebug(LOG_DEBUG, "Session sendonly" );
				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpDebug(LOG_DEBUG, "turning on receive" );
					cpDebug(LOG_DEBUG, "local host: %s", request.localHost);
					cpDebug(LOG_DEBUG, "local port: %d", request.localPort );
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

				}

//				if(type==as_hardware_device)
//					as_set_law(myFd, rtpPayloadPCMU, myDeviceName );
#if 0
				if(type==as_hardware_device)
				{
					if(dsp->law == as_g711_u_law)
						as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
					else if(dsp->law == as_g711_a_law)
						as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
				}
#endif

				break;
			case rtp_session_recvonly:
				cpDebug(LOG_DEBUG, "Session recvonly" );
				//turn on the send
				cpDebug(LOG_DEBUG, "turning on the send" );
				cpDebug(LOG_DEBUG, "remote host: %s", request.remoteHost );
				cpDebug(LOG_DEBUG, "remote port: %d", request.remotePort );
				if ( request.remotePort != 0 )
				{
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setTransmiter(request.remoteHost, request.remotePort,  request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

				}
				
//				if(type==as_hardware_device)
//					as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
#if 0
				if(type==as_hardware_device)
				{
					if(dsp->law == as_g711_u_law)
						as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
					else if(dsp->law == as_g711_a_law)
						as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
				}
#endif				
				break;
			case rtp_session_sendrecv:
				cpDebug(LOG_DEBUG, "Session sendrecv both, RTP parameters can not be changed" );
				return 0;
				break;

			default:
				if ( request.remotePort != 0 && request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendrecv );
					cpDebug(LOG_DEBUG, "Session state is sendrecv" );
				}
				else if ( request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_recvonly );
					cpDebug(LOG_DEBUG, "Session state is recvonly" );
				}
				else if ( request.remotePort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendonly );
					cpDebug(LOG_DEBUG, "Session state is sendonly" );
				}
				else
				{
					cpLog( LOG_ERR, "Session state is undefined" );
				}

				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpDebug(LOG_DEBUG, "Turn on recv" );
					audioStack->setReceiver( request.localPort, request.localPort + 1, 0,  rtpPayloadPCMU, rtpPayloadPCMU, jitterNew );

//					if(type==as_hardware_device)
//						as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
#if 0
					if(type==as_hardware_device)
					{
						if(dsp->law == as_g711_u_law)
							as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
						else if(dsp->law == as_g711_a_law)
							as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
					}
#endif

				}

				//turn on the send
				if ( request.remotePort != 0 )
				{
					cpDebug(LOG_DEBUG, "Turn on send" );
					audioStack->setTransmiter( request.remoteHost, request.remotePort, request.remotePort + 1,  rtpPayloadPCMU, rtpPayloadPCMU );

//					if(type==as_hardware_device)
	//					as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
#if 0	
					if(type==as_hardware_device)
					{
						if(dsp->law == as_g711_u_law)
							as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
						else if(dsp->law == as_g711_a_law)
							as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
					}
#endif

				}

				break;
		}
	}

	if ( request.sendRingback )
		startSendRingback();
	else
		stopSendRingback();

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

	if(type == as_virtual_device && pbxDtmfMode==as_pbx_dtmf_rfc2883 )
	{
		_DTMFInterface = new RtpDevice < AssistDevice > ( this );
		audioStack->setDTMFInterface( _DTMFInterface );
	}
	
	// inRtpPkt = 0;
	if( inRtpPkt )
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}
#if 0 
	audioStack->setApiFormat (rtpPayloadPCMU, RESID_RTP_RATE);
	audioStack->setNetworkFormat (rtpPayloadPCMU, RESID_RTP_RATE);
#else
	audioStack->setApiFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);
	audioStack->setNetworkFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);
#endif
#if 1
	if(!as_tran)
	{
		UaConfiguration *uaConf=UaConfiguration::instance();
		if(uaConf->getVad())
		{
			as_tran=new AssistRtpTransVad(audioStack,rtpPayloadPCMU,rtpPayloadPCMU,1);
			cpDebug(LOG_DEBUG, ":::::: [VAD] enabled ::::::");
		}
		else
		{
			as_tran=new AssistRtpTransmitter(audioStack);
			cpDebug(LOG_DEBUG, ":::::: [VAD] disabled ::::::");
		}
	}
	assert(as_tran);
#endif
//	deviceMutex.unlock();
	return 0;
} // end audioStart()


/*
*
*/
int AssistDevice::audioStop()
{
	cpDebug(LOG_DEBUG, "***   Stopping audio on device %s  ***" ,myDeviceName);

	// mark audio as deactivated.
	audioActive = false;
	audioHalfActive = false;
	vusleep(200);

	// make hardware calls to stop audio
//	deviceMutex.lock();

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

	if (as_tran)
	{
		delete as_tran;
		as_tran = 0;
	}
	if (audioStack)
	{
//		if(type==as_virtual_device) 
		if(type == as_virtual_device && pbxDtmfMode==as_pbx_dtmf_rfc2883 )
			audioStack->unsetDTMFInterface( _DTMFInterface );
		
		delete audioStack;
		audioStack = 0;
	}

	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}
//	status = pbx_audio_stop;

	closeDeviceFile();
//	deviceMutex.unlock();
	return 0;
} // end audioStop()


int AssistDevice::audioSuspend ()
{
//	deviceMutex.lock();

	cpDebug(LOG_INFO, "Suspending audio on device %s" , myDeviceName );
	cpDebug(LOG_INFO, "Setting all RTP/RTCP ports to 0");
	if ( audioStack != 0 )
	{ //set rtp session to inactive
		audioStack->setSessionState(rtp_session_inactive);
#if 0		
		audioStack->setReceiver(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU);
#endif
		int jitterNew=UaConfiguration::instance()->getJitterBuffer();

		audioStack->setReceiver(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU);

	}

	audioActive = false;
	audioHalfActive = false;

//	deviceMutex.unlock();
	return 0;
} // end audioSuspend()


int AssistDevice::audioResume (const HardwareAudioRequest& request)
{
//	deviceMutex.lock();

	if ( audioStack == 0 )
	{
		cpLog( LOG_ERR, "Try to resume an existing audio channel, but no existing audio channel");
//		deviceMutex.unlock();
		return 0;
	}

	cpDebug(LOG_INFO, "Resuming audio on device %s" , myDeviceName );
	cpDebug(LOG_INFO, "Listening on port: %d", request.localPort);
	cpDebug(LOG_INFO, "Sending to host: %s", request.remoteHost);
	cpDebug(LOG_INFO, "Sending to port: %d", request.remotePort);
	cpDebug(LOG_INFO, "RTP packet size: %d", request.rtpPacketSize);

    //resuming audio channel
    //int remoteRtcpPort=(request.remotePort>0)?request.remotePort+1:0;
    //int localRtcpPort=(request.localPort>0)?request.localPort+1:0;
	const char* remoteHost = 0;
	if ( request.remotePort != 0 )
		remoteHost = request.remoteHost;

	int jitterNew=UaConfiguration::instance()->getJitterBuffer();
	if ( request.localPort != 0 && request.remotePort != 0 )
	{        //full duplex audio
		audioStack->setSessionState(rtp_session_sendrecv);
#if 0		
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
#endif

		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);


//		if(type==as_hardware_device)
//			as_set_law( myFd, rtpPayloadPCMU,  myDeviceName);
#if 0
		if(type==as_hardware_device)
		{
			if(dsp->law == as_g711_u_law)
				as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
			else if(dsp->law == as_g711_a_law)
				as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
		}
#endif		
		audioActive = true;
		audioHalfActive = false;
	}
	else if ( request.localPort != 0 )
	{//receive only
		audioStack->setSessionState(rtp_session_sendrecv);
#if 0		
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
#endif
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

//		if(type==as_hardware_device)
//			as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
#if 0
		if(type==as_hardware_device)
		{
			if(dsp->law == as_g711_u_law)
				as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
			else if(dsp->law == as_g711_a_law)
				as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
		}
#endif

		audioActive = false;
		audioHalfActive = true;
	}
	else if ( request.remotePort != 0 )
	{ //transmit only
		audioStack->setSessionState(rtp_session_sendrecv);
#if 0		
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
#endif
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

//		if(type==as_hardware_device)
//			as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
#if 0
		if(type==as_hardware_device)
		{
			if(dsp->law == as_g711_u_law)
				as_set_law(myFd, rtpPayloadPCMU, myDeviceName);
			else if(dsp->law == as_g711_a_law)
				as_set_law(myFd, rtpPayloadPCMA, myDeviceName);
		}
#endif

		audioActive = false;
		audioHalfActive = true;
	}


	if ( request.sendRingback )
		startSendRingback();
	else
		stopSendRingback();

//	deviceMutex.unlock();
	return 0;
} // end audioResume()


Sptr<SipCallId> AssistDevice::getCallId( ) const
{
	return myCallId;
}

void AssistDevice::setAudioStack(RtpSession *_audioStack)
{
	if( _audioStack == 0 )
		cpDebug(LOG_DEBUG, "Clear Audio Stack on device %s", myDeviceName );
	audioStack = _audioStack;
}

void AssistDevice::setCallId( const Sptr<SipCallId> newCallId )
{
	if( newCallId == 0 )
		cpDebug(LOG_DEBUG, "Clear Call Id");

	myCallId = newCallId;

	if( myCallId == 0 )
		cpDebug(LOG_DEBUG, "Call Id is cleared");
}

Sptr<DigitCollector> AssistDevice::getDigitCollector( ) 
{
	return myDigitCollector;
}

HardwareStatusType AssistDevice::getHardwareStatus()
{
	HardwareStatusType hardwareStatus = HARDWARE_UNAVAILABLE;

	if ( audioActive == true )
		cpDebug(LOG_DEBUG, "audio active true" );
	else
		cpDebug(LOG_DEBUG, "audio active false" );

	if ( audioHalfActive == true )
		cpDebug(LOG_DEBUG, "audio half active true" );
	else
		cpDebug(LOG_DEBUG, "audio half active false" );
#if 0
	if ( callWaitingIdList->empty() )
	{
		cpDebug(LOG_DEBUG, "call waiting list empty" );
		if ( callId == 0 )
		{
			hardwareStatus = HARDWARE_AVAILABLE;
		}
		else
		{
			if ( audioActive && !audioHalfActive )
				hardwareStatus = HARDWARE_CALLWAITING_ALLOWED;
			else if ( !audioActive && audioHalfActive )
				hardwareStatus = HARDWARE_UNAVAILABLE;
		}
	}
	else
#endif		
	{
		if ( myCallId == 0 )
//			hardwareStatus = HARDWARE_CALLWAITING_ALLOWED;
			hardwareStatus = HARDWARE_AVAILABLE;
	}

	return hardwareStatus;
}


bool AssistDevice::isMyHardware(const SipCallId& callid)
{
	if ( myCallId == 0 )
		return false;

	return ( *myCallId == callid );
}

void AssistDevice::recvRTPDTMF( int digit, int duration )
{
	const char *phone_pad = {"0123456789*#"};
	cpDebug(LOG_DEBUG, "DTMF digit %d(duration is %d) has been received from RTP stack on %s", digit, duration, myDeviceName);
	
	if( digit>=0 && digit< 12 )
	{
		emitDtmfSignal(phone_pad[digit]);
	}
	else
	{
		cpLog(LOG_ERR, "Unrecognized DTMF key received from RTP stack");
		return;
	}
	return ;
}

//added by lijie
void AssistDevice::addCallWaitingId( const Sptr<SipCallId> newCallId )
{
    if( newCallId == 0 )
    {
        cpDebug(LOG_DEBUG, "No null id is added" );
        return ;
    }

    callWaitingIdList->push_back( newCallId );
    cpDebug(LOG_DEBUG, "added callId %s", newCallId->encode().logData());
}

Sptr<SipCallId> AssistDevice::getCallWaitingId() const
{
    if ( callWaitingIdList->empty() )
    {
        cpDebug(LOG_DEBUG, "call waiting list empty" );
        return 0;
    }

    return callWaitingIdList->front();
}


void AssistDevice::removeCallWaitingId( const SipCallId& callId )
{
    cpDebug(LOG_DEBUG, "try to remove callId %s", callId.encode().logData() );

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callId )
        {
            callWaitingIdList->erase(iter);
            cpDebug(LOG_DEBUG, "callId %s removed", callId.encode().logData() );
            break;
        }
        iter++;
    }

    return ;
}

bool AssistDevice::isOnCallWaiting(const SipCallId &callid)
{
    bool ret = false;

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callid )
        {
            ret = true;
            break;
        }
        iter++;
    }
    return ret;
}
//added ends here
#else

AssistDevice::	AssistDevice(AssistNullDevice *_agent, char *devicename, device_type_t _type, int deviceId, Sptr < Fifo < Sptr < SipProxyEvent > > > sessionQ)
	:myCallId(0),
	actionType(ActionNull),
	type(_type),
	myDeviceId(deviceId),
	callProcessingQueue(sessionQ),	
	myReadBuffer(160*14),
	myWriteBuffer(160*14),
	audioStack(0),
	as_tran(0),
	inRtpPkt(0),
	_DTMFInterface(0),
	isOn(true),
	callCount(0)
{
	memset(phonenumber, 0, AS_PBX_DTMF_BUFFER_LENGTH);
	callWaitingIdList = new CallWaitingIdList;
	myDeviceName = (char *)malloc(AS_DEVICE_NAME_LENGTH);
	memset(myDeviceName, 0 , AS_DEVICE_NAME_LENGTH);
	sprintf(myDeviceName, "%s", 	devicename);
#if 1
	dsp = assist_dsp_init(UaConfiguration::instance()->getG711Law(), UaConfiguration::instance()->getRxGain(),
		UaConfiguration::instance()->getTxGain(), DTMF_PARSE_LENGTH);
#endif

	myDigitCollector = new DigitCollector(deviceId , sessionQ );
	agent = _agent;
	as_tran=0;

	#define AS_TONE_FILE_NAME_LENGTH		64

	as_law_type_t law = UaConfiguration::instance()->getG711Law();

	char tonefile[AS_TONE_FILE_NAME_LENGTH];
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/ringback.tone", (law ==  as_g711_u_law) ?"u":"a"  );

	ringbackFd = open(tonefile , O_RDONLY);
	if ( ringbackFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	memset(tonefile,0, AS_TONE_FILE_NAME_LENGTH);
/*	changed as following, lizhijie, 2007.03.27 
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/greeting" , (law ==  as_g711_u_law) ?"u":"a"  );
*/
	sprintf(tonefile, PBX_TONES_DIR"Tone/alert/%s", UaConfiguration::instance()->getGreetingFile().c_str() );
	greetingFd = open(tonefile , O_RDONLY);
	if ( greetingFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	memset(tonefile,0, AS_TONE_FILE_NAME_LENGTH);
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/busy.tone" , (law ==  as_g711_u_law) ?"u":"a"  );
	busyFd = open(tonefile , O_RDONLY);
	if ( busyFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	memset(tonefile,0, AS_TONE_FILE_NAME_LENGTH);
	sprintf(tonefile, "%s%s.%s", PBX_TONES_DIR, "Tone/callwaiting.tone" , (law ==  as_g711_u_law) ?"u":"a"  );
	callwaitingFd = open(tonefile , O_RDONLY);
	if ( callwaitingFd == -1 )
	{
		cpLog(LOG_ERR, "can not open %s" , tonefile );
	}

	/* added, li zhijie, 2007.05.12 */
	if(_type == as_pstn_device )
	{
		PstnHwStatusMap	pstnHwMap = UaConfiguration::instance()->getPstnHwStatusMap();
		PstnHwStatusMap::iterator pstnIter = pstnHwMap.begin();
		while ( pstnIter != pstnHwMap.end() )
		{
			if( ( (deviceId-PSTN_DEVICES_EPOCH) == pstnIter->first ) &&(pstnIter->second == false) ) 
			{
				isOn = false;
				cpLog(LOG_WARNING, "PSTN %d lines is not connected", deviceId-PSTN_DEVICES_EPOCH);
			}
			pstnIter++;
		}
	}
}


AssistDevice::~AssistDevice()
{
	if(myDeviceName)
	{
		free(myDeviceName);
		myDeviceName =  0;
	}
#if 0
	assist_dsp_destory( dsp);	
#endif
}


/* send DTMF signal in the method of RFC2833 */
void AssistDevice::provideDtmf(DeviceSignalType signal)
{
	char digit = ' ';
	int dtmfEvent = 0;

	switch( signal )
	{
		case DeviceSignalDigitStar: 
			digit = '*';
			dtmfEvent = 10;
			break;
		case DeviceSignalDigitHash:
			digit = '#';
			dtmfEvent = 11;
			break;
		case DeviceSignalDigit0:
		case DeviceSignalDigit1:
		case DeviceSignalDigit2:
		case DeviceSignalDigit3:
		case DeviceSignalDigit4:
		case DeviceSignalDigit5:
		case DeviceSignalDigit6:
		case DeviceSignalDigit7:
		case DeviceSignalDigit8:
		case DeviceSignalDigit9:
			digit = '0' + signal - DeviceSignalDigit0;
			dtmfEvent = signal - DeviceSignalDigit0;
			break;
		default:
			cpDebug(LOG_DEBUG, "Unknown DTMF Signal %d", signal );
			return;
	}

	audioMutex.lock();
	if( audioStack )
	{
		cpDebug(LOG_DEBUG, "DTMF %c sent out in RTP ", digit );
		audioStack->transmitEvent( dtmfEvent );
	}
	else
	{
		cpDebug(LOG_DEBUG, "No audio stack existing to send DTMF event" );
	}
	audioMutex.unlock();
}

int AssistDevice::getRtpPort()
{
	//this is an arbitrarily defined number
	const int HIGH_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
	const int LOW_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

	int port = 0;
	int lowPort = LOW_RTP_PORT;

	// create a rtp session if there is no existing session alread this Rtp session will be idle

	audioMutex.lock();

	if ( audioStack == 0 )
	{
		audioStack = new RtpSession(0);
	}

	int jitterNew=UaConfiguration::instance()->getJitterBuffer();

	while ( 1 )
	{// resever a rtp port
		port=reserveRtpPort(lowPort, HIGH_RTP_PORT,jitterNew);
		lowPort = (port > lowPort) ? port : lowPort;

		// attempt to reserve a rtcp port on port number higher than the rtp port
		if ( port != 0 && audioStack->reserveRtcpPort(port + 1, 0) != 0 )
		{
			cpDebug(LOG_DEBUG, "Reserved rtp/rtcp ports %d/%d", port, port+1 );
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

int AssistDevice::reserveRtpPort(int localMin, int localMax, int jitterNew)
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

void AssistDevice::releaseRtpPort()
{

//modified by lijie 2005-08-23
#if 0
	// destroy the rtp session if one exists
	audioMutex.lock();
	if ( audioStack != 0 )
	{
		if(type==as_virtual_device )
			audioStack->unsetDTMFInterface( _DTMFInterface );

		int port = audioStack->releaseRtpPort();
		cpDebug(LOG_DEBUG, "rtp port %d released", port );
		port = audioStack->releaseRtcpPort();
		cpDebug(LOG_DEBUG, "rtcp port %d released", port );
		delete audioStack;
		audioStack = 0;
	}

	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}
	audioMutex.unlock();
#endif

	audioMutex.lock();
	if ( audioStack )
	{
		RtpSessionState sessionState = audioStack->getSessionState();
		if ( sessionState == rtp_session_sendonly ||
			sessionState == rtp_session_sendrecv )
		{
			if(audioStack->getRtcpTran())audioStack->transmitRTCPBYE();
		}
		
		if(type==as_virtual_device )
			audioStack->unsetDTMFInterface( _DTMFInterface );

		int port = audioStack->releaseRtpPort();
		cpDebug(LOG_DEBUG, "rtp port %d released", port );
		port = audioStack->releaseRtcpPort();
		cpDebug(LOG_DEBUG, "rtcp port %d released", port );
		delete audioStack;
		audioStack = 0;
	}

	if (inRtpPkt)
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}

	if (as_tran)
	{
		delete as_tran;
		as_tran = 0;
	}
	
	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}
	audioMutex.unlock();

}

void AssistDevice::provideCallInfo(string callerNum, string calleeNum, string reason)
{
	cpDebug(LOG_DEBUG, "Caller is '%s',\t\tCallee is '%s',\t\treason is '%s'\r\n" , 
		callerNum.c_str(), calleeNum.c_str(), reason.c_str() );
}

int AssistDevice::audioStart (const HardwareAudioRequest& request)
{
	int fd = -1;
	cpDebug(LOG_DEBUG, ">> Establishing audio on device %s" , myDeviceName );
	cpDebug(LOG_DEBUG, ">> Listening on port: %d", request.localPort );
	cpDebug(LOG_DEBUG, ">> Sending to host  : %s", request.remoteHost );
	cpDebug(LOG_DEBUG, ">> Sending to port  : %d", request.remotePort );
	cpDebug(LOG_DEBUG, ">> RTP packet size  : %d ms", request.rtpPacketSize );

	int jitterNew=UaConfiguration::instance()->getJitterBuffer();

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
		lseek(fd, 0, SEEK_SET);
	}
	actionMutex.unlock();

	audioMutex.lock();
	
	openDeviceFile();
	
	pbxDtmfMode = as_pbx_dtmf_invalide;
	if ( audioStack == 0 )
	{
		int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
		int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
		cpDebug(LOG_DEBUG, "Remote RTCP port : %d", remoteRtcpPort );
		cpDebug(LOG_DEBUG, "Local RTCP port  : %d", localRtcpPort );

		const char* remoteHost = 0;
		if ( request.remotePort != 0 )
			remoteHost = request.remoteHost;
		audioStack = new RtpSession(remoteHost, request.remotePort,  request.localPort, remoteRtcpPort, localRtcpPort, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

	}
	else
	{
		cpDebug(LOG_DEBUG, "Modifying existing audio connection" );

		//need to determine what mode RtpSession is in
		RtpSessionState sessionState = audioStack->getSessionState();
		switch ( sessionState )
		{
			case rtp_session_sendonly:
				cpDebug(LOG_DEBUG, "Session sendonly" );
				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpDebug(LOG_DEBUG, "turning on receive" );
					cpDebug(LOG_DEBUG, "local host: %s", request.localHost);
					cpDebug(LOG_DEBUG, "local port: %d", request.localPort );
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);

				}
				break;
			case rtp_session_recvonly:
				cpDebug(LOG_DEBUG, "Session recvonly" );
				//turn on the send
				cpDebug(LOG_DEBUG, "turning on the send" );
				cpDebug(LOG_DEBUG, "remote host: %s", request.remoteHost );
				cpDebug(LOG_DEBUG, "remote port: %d", request.remotePort );
				if ( request.remotePort != 0 )
				{
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setTransmiter(request.remoteHost, request.remotePort,  request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

				}
				break;
			case rtp_session_sendrecv:
				cpDebug(LOG_DEBUG, "Session sendrecv both, RTP parameters can not be changed" );
				audioMutex.unlock();
				return 0;
				break;
			default:
				if ( request.remotePort != 0 && request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendrecv );
					cpDebug(LOG_DEBUG, "Session state is sendrecv" );
				}
				else if ( request.localPort != 0 )
				{
					audioStack->setSessionState( rtp_session_recvonly );
					cpDebug(LOG_DEBUG, "Session state is recvonly" );
				}
				else if ( request.remotePort != 0 )
				{
					audioStack->setSessionState( rtp_session_sendonly );
					cpDebug(LOG_DEBUG, "Session state is sendonly" );
				}
				else
				{
					cpLog( LOG_ERR, "Session state is undefined" );
				}

				//turn on the receive
				if ( request.localPort != 0 )
				{
					cpDebug(LOG_DEBUG, "Turn on recv" );
					audioStack->setReceiver( request.localPort, request.localPort + 1, 0,  rtpPayloadPCMU, rtpPayloadPCMU, jitterNew );
				}

				//turn on the send
				if ( request.remotePort != 0 )
				{
					cpDebug(LOG_DEBUG, "Turn on send" );
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

	if(type == as_virtual_device)
	{
		_DTMFInterface = new RtpDevice < AssistDevice > ( this );
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

	if(!as_tran)
	{
		UaConfiguration *uaConf=UaConfiguration::instance();
		if(uaConf->getVad())
		{
			as_tran=new AssistRtpTransVad(audioStack,rtpPayloadPCMU,rtpPayloadPCMU,1);
			cpDebug(LOG_DEBUG, ":::::: [VAD] enabled ::::::");
		}
		else
		{
			as_tran=new AssistRtpTransmitter(audioStack);
			cpDebug(LOG_DEBUG, ":::::: [VAD] disabled ::::::");
		}
	}
	assert(as_tran);

	gettimeofday (&prevTime, 0);
	
	audioMutex.unlock();
	
	return 0;
} // end audioStart()


/*
*
*/
int AssistDevice::audioStop()
{
	cpDebug(LOG_DEBUG, "***   Stopping audio on device %s  ***" ,myDeviceName);

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
			if(audioStack->getRtcpTran())audioStack->transmitRTCPBYE();
		}
	}

	if (inRtpPkt)
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}

	if (as_tran)
	{
		delete as_tran;
		as_tran = 0;
	}
	if (audioStack)
	{
		if(type==as_virtual_device) 
			audioStack->unsetDTMFInterface( _DTMFInterface );
		
		delete audioStack;
		audioStack = 0;
	}

	if ( _DTMFInterface )
	{
		delete _DTMFInterface;
		_DTMFInterface = 0;
	}

	closeDeviceFile();

	audioMutex.unlock();
	
	return 0;
} // end audioStop()


int AssistDevice::audioSuspend ()
{

	cpDebug(LOG_INFO, "Suspending audio on device %s" , myDeviceName );
	cpDebug(LOG_INFO, "Setting all RTP/RTCP ports to 0");

	audioMutex.lock();

	if ( audioStack != 0 )
	{ //set rtp session to inactive
		audioStack->setSessionState(rtp_session_inactive);
		int jitterNew=UaConfiguration::instance()->getJitterBuffer();
		audioStack->setReceiver(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, jitterNew);
		audioStack->setTransmiter(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU);
	}

	audioActive = false;
	audioHalfActive = false;

	audioMutex.unlock();
	return 0;
} // end audioSuspend()


int AssistDevice::audioResume (const HardwareAudioRequest& request)
{

	int fd = -1;
	
	cpDebug(LOG_INFO, "Resuming audio on device %s" , myDeviceName );
	cpDebug(LOG_INFO, "Listening on port: %d", request.localPort);
	cpDebug(LOG_INFO, "Sending to host: %s", request.remoteHost);
	cpDebug(LOG_INFO, "Sending to port: %d", request.remotePort);
	cpDebug(LOG_INFO, "RTP packet size: %d", request.rtpPacketSize);

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

	int jitterNew=UaConfiguration::instance()->getJitterBuffer();
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

	gettimeofday (&prevTime, 0);
		
	audioMutex.unlock();
	
	return 0;
	
} // end audioResume()


Sptr<SipCallId> AssistDevice::getCallId( ) const
{
	return myCallId;
}

void AssistDevice::setAudioStack(RtpSession *_audioStack)
{
	if( _audioStack == 0 )
		cpDebug(LOG_DEBUG, "Clear Audio Stack on device %s", myDeviceName );
	audioStack = _audioStack;
}

void AssistDevice::setCallId( const Sptr<SipCallId> newCallId )
{
	if( newCallId == 0 )
		cpDebug(LOG_DEBUG, "Clear Call Id");

	myCallId = newCallId;

	if( myCallId == 0 )
		cpDebug(LOG_DEBUG, "Call Id is cleared");
}

Sptr<DigitCollector> AssistDevice::getDigitCollector( ) 
{
	return myDigitCollector;
}

HardwareStatusType AssistDevice::getHardwareStatus()
{
	HardwareStatusType hardwareStatus = HARDWARE_UNAVAILABLE;

	if(isOn == false)/* added, lizhijie, 2007.05.12 */
		return hardwareStatus;
	
	if(myCallId == 0)
		hardwareStatus = HARDWARE_AVAILABLE;
	else
	{
		bool bCallWait = UaConfiguration::instance()->getCallWaitingOn(phonenumber);
		if(bCallWait)
			hardwareStatus = HARDWARE_CALLWAITING_ALLOWED;
			
	}
	
	return hardwareStatus;
}


bool AssistDevice::isMyHardware(const SipCallId& callid)
{
	if ( myCallId == 0 )
		return false;

	return ( *myCallId == callid );
}

void AssistDevice::recvRTPDTMF( int digit, int duration )
{
	const char *phone_pad = {"0123456789*#"};
	cpDebug(LOG_DEBUG, "DTMF digit %d(duration is %d) has been received from RTP stack on %s", digit, duration, myDeviceName);

	if( digit>=0 && digit< 12 )
	{
		emitDtmfSignal(phone_pad[digit]);
	}
	else
	{
		cpLog(LOG_ERR, "Unrecognized DTMF key received from RTP stack");
		return;
	}
	return ;
}

void AssistDevice::addCallWaitingId( const Sptr<SipCallId> newCallId )
{
    if( newCallId == 0 )
    {
        cpDebug(LOG_DEBUG, "No null id is added" );
        return ;
    }

    callWaitingIdList->push_back( newCallId );
    cpDebug(LOG_DEBUG, "added callId %s", newCallId->encode().logData());
}

Sptr<SipCallId> AssistDevice::getCallWaitingId() const
{
    if ( callWaitingIdList->empty() )
    {
        cpDebug(LOG_DEBUG, "call waiting list empty" );
        return 0;
    }

    return callWaitingIdList->front();
}


void AssistDevice::removeCallWaitingId( const SipCallId& callId )
{
    cpDebug(LOG_DEBUG, "try to remove callId %s", callId.encode().logData() );

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callId )
        {
            callWaitingIdList->erase(iter);
            cpDebug(LOG_DEBUG, "callId %s removed", callId.encode().logData() );
            break;
        }
        iter++;
    }

    return ;
}

bool AssistDevice::isOnCallWaiting(const SipCallId &callid)
{
    bool ret = false;

    CallWaitingIdList::iterator iter = callWaitingIdList->begin();

    while ( iter != callWaitingIdList->end() )
    {
        if ( *(*iter) == callid )
        {
            ret = true;
            break;
        }
        iter++;
    }
    return ret;
}		

//return -1 means that no invalid fd exists
int AssistDevice::getSendBackTone(char *ringData, int size)
{
	static const int rtpTime = UaConfiguration::instance()->getNetworkRtpRate()*1000; 
	
	int fd = -1;
       int interval;
       int cc = 0;
	long timS;
	long timR;	
       struct timeval nowTime;      

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

       if(interval<rtpTime)
       {
       	actionMutex.unlock();
              return 0;
        }

        timS=interval/rtpTime;
        timR=(interval%rtpTime)/1000;

        if(timS <= 0)
		timS=1;
	  do
	  {
		cc = read(fd, ringData, RESID_RTP_RATE);
              while ( cc < size )
	       {
              	lseek(fd, 0, SEEK_SET);
	              cc += read( fd, ringData + cc, size - cc);              
	       }
        	timS--; 
         	as_tran->transmitRaw(ringData, cc);

	   }while(timS > 0);

          if(timR > 9)
          {
	   	cc = read(fd, ringData, RESID_RTP_RATE);               

              while ( cc < size )
		{
			lseek(fd, 0, SEEK_SET);
			cc += read( fd, ringData + cc, size - cc);              
              }
              as_tran->transmitRaw(ringData, cc); 
         }
		   
         prevTime = nowTime;

	  actionMutex.unlock();
        
         return 0;
	
}

string  AssistDevice::getAllTransferUrl()
{
	char *keyPhone;
	string valuePhone;

	keyPhone = phonenumber;
	valuePhone =  UaConfiguration::instance()->getAllTransfer(keyPhone);

	return valuePhone;
	
}
string  AssistDevice::getTimeOutTransferUrl()
{
	char *keyPhone;
	string valuePhone;

	keyPhone = phonenumber;
	valuePhone =  UaConfiguration::instance()->getTimeOutTransfer(keyPhone);

	return valuePhone;
}
string  AssistDevice::getBusyTransferUrl()
{
	char *keyPhone;
	string valuePhone;

	keyPhone = phonenumber;
	valuePhone =  UaConfiguration::instance()->getBusyTransfer(keyPhone);

	return valuePhone;
}

bool   AssistDevice::getCallWaitingOn()
{
	return UaConfiguration::instance()->getCallWaitingOn(phonenumber);
}

void AssistDevice::setPhoneNumber(char *phone)
{	
	memset(phonenumber, 0 , AS_PBX_DTMF_BUFFER_LENGTH);
	sprintf( phonenumber, "%s", phone );
}

const char* AssistDevice::getPhoneNumber()
{
	
	string num = UaConfiguration::instance()->getLocalCountry()+UaConfiguration::instance()->getLocalDistrict()+UaConfiguration::instance()->getLocalDevice()+phonenumber;
	return num.c_str();
	
}

void AssistDevice::setCallerId(string _callerId)
{
	callerNumber = _callerId;
	cpDebug(LOG_INFO, "CallerId  %s : %s",  callerNumber.c_str(), _callerId.c_str() );
}

#endif
