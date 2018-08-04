/* 
 * $Log: AssistVoiceDevice.cxx,v $
 * Revision 1.1.1.1  2006/11/29 09:50:50  lizhijie
 * AS600 Pbx
 *
 * Revision 1.1.1.1  2006/07/12 16:16:48  lizhijie
 * rebuild 
 *
 * Revision 1.2  2005/09/30 09:38:07  wuaiwu
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2005/03/14 07:38:05  lizhijie
 * PBX program
 *
 * Revision 1.1  2005/02/23 05:18:12  lizhijie
 * no message
 *
 * Revision 1.1.1.1  2005/02/16 04:51:10  lizhijie
 * new pbx code
 *
 * Revision 1.3  2005/02/06 03:02:12  lizhijie
 * no message
 *
 * Revision 1.2  2005/02/05 09:52:29  lizhijie
 * debug some problems, for example ring, packet size
 *
 * $Id: AssistVoiceDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $
 */

static const char* const QuickNetDevice_cxx_Version =
    "$Id: AssistVoiceDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";

#include "global.h"

/* stdlib */
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cerrno>
#include <linux/telephony.h>
#include <linux/ixjuser.h>

#include "VTime.hxx"
#include "Rtp.hxx"
#include "VCondition.h"

/* error handling */

#include "AssistVoiceDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaConfiguration.hxx"

#include "pbx_globals.h"


using namespace Vocal;


//***************************************************************************
// description:  Constructor.  opens the quicknet device.  initialize the
//               hookstate.  resets the device to get it ready.
//***************************************************************************

AssistVoiceDevice::AssistVoiceDevice( const char* deviceName,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                                Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ )
        : ResGwDevice( deviceName, inputQ, outputQ ),
        audioStack(0),
        inRtpPkt(0),
        ringbackFd( -1),
        sendRingback(false),
        cwBeep( 0 ),
        cwCadenceOn( true ),
        cwCnt( 0 ),
        cwBeepOn( false )
{
	// initialize queues
	sessionQ = outputQ;
	myQ = inputQ;

	// open hardware device
	myFD = open(deviceName, O_RDWR);
	if (myFD < 0)
	{
		cpLog( LOG_ERR, "Cannot open %s", deviceName );
		exit(1);
	}
	close(myFD);

	myFD = open(deviceName, O_RDWR);
	if (myFD < 0)
	{
		cpLog( LOG_ERR, "Cannot re-open ", deviceName );
		exit(1);
	}

//	as_dsp_set_gain(myFD, U_LAW_CODE, GAIN_8_TIMES, GAIN_NO_CHANGE);
	as_dsp_set_gain(myFD, U_LAW_CODE,  GAIN_NO_CHANGE, GAIN_8_TIMES);
#if __WITH_SLIC__
	AS_DEVICE_STATE  state  = as_device_check_state( myFD, deviceName);
	if(state == AS_DEVICE_STATE_ONHOOK )
		hookStateOffhook = false;
	else
		hookStateOffhook = true;
#else
	hookStateOffhook = false;
#endif

#if 0
	int x = AS_ONHOOK;
	int res = ioctl( fd,  AS_CTL_HOOK, &x);
	hookStateOffhook = false;
#endif

	myDeviceName = deviceName;

#if 0
	stdinFD = fileno( stdin );
	tcgetattr( stdinFD, &initialTerm );
	struct termios termAttr = initialTerm;
	termAttr.c_lflag &= ~ICANON;
	termAttr.c_cc[VMIN] = 1;
	termAttr.c_cc[VTIME] = 0;
	if ( tcsetattr( stdinFD, TCSANOW, &termAttr ) != 0 )
	{
		cerr << "Unable to set new terminal mode."  << endl;
		exit( -1 );
	}

	if( fcntl( stdinFD, F_SETFL, O_NONBLOCK ) == -1 )
	{
		cerr << "Unable to set stdin mode."  << endl;
		exit( -1 );
	}
#endif
    // read in the call waiting beep from tone file
	cpDebug(LOG_DEBUG, "reading call waiting beep from Tone/cwbeep" );
	int cwBeepFd = open(PBX_TONES_DIR"Tone/cwbeep", O_RDONLY);
	if ( cwBeepFd == -1 )
	{
		cpLog( LOG_ERR, "can not open Tone/cwbeep" );
		return;
	}

	cwBeep = new char[CWBEEP_ON_TIME];
	int nsample = read(cwBeepFd, cwBeep, CWBEEP_ON_TIME);
	if ( nsample != CWBEEP_ON_TIME )
		cpLog( LOG_ERR, "Not enougth call waiting beep samples read" );
	else
		cpDebug(LOG_DEBUG, "call waiting beep successfully read");

	close(cwBeepFd);
}  // end Constructor


//***************************************************************************
// description:  Destructor
//***************************************************************************
AssistVoiceDevice::~AssistVoiceDevice()
{
	close(myFD);
	tcsetattr( stdinFD, TCSANOW, &initialTerm );
} // end ~AssistVoiceDevice()


//***************************************************************************
// description:  main processing loop of the hardware
//***************************************************************************
void* AssistVoiceDevice::hardwareMain (void* parms)
{
	// process forever on behalf of AssistVoiceDevice hardware
	fd_set readfds;
	struct timeval tv;
	int retval;
	int maxFd = 128;  // should be 0 then increment as needed when addToFdSet

	// reset file descriptor
	FD_ZERO(&readfds);
	addToFdSet(&readfds);
	/*** block on select for asyncronous events, but poll to process
	* audio and signal requests from endpoints in message queue
	* The following values were optimized by trial and error for
	* the quicknet ixj-0.3.18gpl drivers.
	*/

	tv.tv_sec = 0;
	tv.tv_usec = 300;
	if((retval = select(maxFd, &readfds, 0, 0, &tv)) < 0)
	{
		cpLog( LOG_ERR, "select() returned error %d", retval );
	}
	else
	{
		if(process(&readfds) < 0)
		{
			cpLog( LOG_ERR, "hardware encountered an error" );
			assert(0);
		}
	}

	if( myQ->size() > 0 )
	{
		cpDebug(LOG_DEBUG, "Signal from call control" );
		processSessionMsg( myQ->getNext() );
	}

	return 0;
} // end hardwareMain()

#if 0
void AssistVoiceDevice::processUrlInput()
{
	char keystroke = '\0';
	if( read( stdinFD, &keystroke, 1 ) == 0 )
	{
		return;
	}

	if( !hookStateOffhook )
	{
		cpLog( LOG_ERR, "The phone must be off-hook first when input SIP url" );
		return;    // Ignore input is it is not off hook
	}

	Sptr < UaDeviceEvent > event = new UaDeviceEvent( sessionQ ) ;
	assert( event != 0 );

	if(myEntryState == EntryStateEnterUrl)
	{
		// the user is entering a URL, so do something else
		switch( keystroke )
		{
			case '\x08':
			case '\x7F':  // Backspace (\b) or Delete (DEL)
			{
				if( myTextEntry.length() > 0 )
				{
				// xxx this is lame
					cout << "\b \b\b \b\b \b";
					cout.flush();
					myTextEntry.setchar(myTextEntry.length() - 1, ' ');
					myTextEntry.removeSpaces();
				}
				else
				{
					cout << "\b \b\b \b";
					cout.flush();
				}
				break;
			}

			case '\x0A':  // Line Feed (\n) -> done
			{
				cpDebug(LOG_DEBUG, "URL is %s", myTextEntry.logData() );
				event->type = DeviceEventCallUrl;
				event->text = myTextEntry;
				myTextEntry = "";
				myEntryState = EntryStateTelephoneUI;
				break;
			}

			default:
			{
				if( keystroke >= '\x20' && keystroke <= '\x7E' )
				{
				// Append a character between ' ' and '~'
					myTextEntry.setchar(myTextEntry.length(), keystroke);
				}
				else
				{
					// Treat everything else as abort
					// e.g. '\1B':  // Esc -> abort
					myTextEntry = "";
					myEntryState = EntryStateTelephoneUI;
					cout << endl;
					cpDebug(LOG_DEBUG, "Abort URL input" );
				}
				break;
			}
		}
	}

	if (event->type != DeviceEventUndefined)
	{
		assert( sessionQ != 0 );
		event->callId = callId;
		sessionQ->add( event );
	}
}
#endif

//***************************************************************************
// description:  process any events detected on the hardware (dtmf digit
//    press, onhook & offhook, etc.) and reports the events back to the session via the fifo queue.
//***************************************************************************
int AssistVoiceDevice::process (fd_set* fd)
{
	AS_DEVICE_STATE state;
#if 0	
	if( FD_ISSET( stdinFD, fd ) )
	{
		processUrlInput();
		FD_CLR( stdinFD, fd );
	}
#endif
	vusleep(0);  // needed to stabilize hookstate readings
	deviceMutex.lock();

	state = as_device_check_state(myFD, myDeviceName);
#if 0	
	if( ioctl(myFD, PHONE_HOOKSTATE) )
#endif
	if(state == AS_DEVICE_STATE_OFFHOOK )
	{// device is in the state of offhook
		if( hookStateOffhook == false )
		{// report change from onhook to offhook
			hookStateOffhook = true;
			reportEvent(sessionQ, DeviceEventHookUp);
		}

		if(as_dtmf_is_ready(myFD, myDeviceName) )
		{
			switch(as_dtmf_get_digit(myFD, myDeviceName) )
			{
				case '1'/*0x01*/:
					reportEvent(sessionQ, DeviceEventDtmf1);
					break;
				case '2' /*0x02*/:
					reportEvent(sessionQ, DeviceEventDtmf2);
					break;
				case '3'/*0x03*/:
					reportEvent(sessionQ, DeviceEventDtmf3);
					break;
				case '4'/*0x04*/:
					reportEvent(sessionQ, DeviceEventDtmf4);
					break;
				case '5'/*0x05*/:
					reportEvent(sessionQ, DeviceEventDtmf5);
					break;
				case '6'/*0x06*/:
					reportEvent(sessionQ, DeviceEventDtmf6);
					break;
				case '7'/*0x07*/:
					reportEvent(sessionQ, DeviceEventDtmf7);
					break;
				case '8'/*0x08*/:
					reportEvent(sessionQ, DeviceEventDtmf8);
					break;
				case '9'/*0x09*/:
					reportEvent(sessionQ, DeviceEventDtmf9);
					break;
				case '*'/*0x0A*/:
					reportEvent(sessionQ, DeviceEventDtmfStar);
					break;
				case '0'/*0x0B*/:
					reportEvent(sessionQ, DeviceEventDtmf0);
					break;
				case '#'/*0x0C*/:
					reportEvent(sessionQ, DeviceEventDtmfHash);
					break;
				default:
					cpLog(LOG_ERR, "Unrecognized DTMF digit from hardware");
					break;
			}
		}
	}/* end of device in the state of offhook */
	else
	{/* device is in the state of on-hook */
		if( hookStateOffhook == true )
		{
			vusleep(20000);  // needed to stabilize hookstate readings

			// read the hookstate again to make sure an onhook has been generated
			state = as_device_check_state(myFD, myDeviceName);
			if(state == AS_DEVICE_STATE_ONHOOK )
			{
				deviceMutex.unlock();
				onhookOrFlash();
				deviceMutex.lock();
			}
		}
	}
	deviceMutex.unlock();

	return 0;
} // end process()




//***************************************************************************
// description:  main processing loop for RtpThread thread
//***************************************************************************
void AssistNullDevice::processRTP ()
{
	deviceMutex.lock();
	if (audioStack == 0)
	{
		vusleep(10000);
		deviceMutex.unlock();
		return ;
	}

	RtpSessionState sessionState = audioStack->getSessionState();

	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
#if 0	
		int socketfd = audioStack->getRtpRecv()->getSocketFD();
		char phone[32];
		cpDebug(LOG_DEBUG,"try receive phone number from RTP " );
		int res = as_dsp_dtmf_detect(socketfd, U_LAW_CODE, 2048, phone, 32);
		if(res>0)
		{
			cpDebug(LOG_DEBUG,"Phone number from RTP is '%s'", phone );
		}
#else		
		inRtpPkt = audioStack->receive ();
		if (inRtpPkt)
		{
			if (inRtpPkt->getPayloadUsage() != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			char *vBuf;
			if ( cwBeepOn )
			{
				vBuf = getCwBeep();
				if ( vBuf == 0 )
					vBuf = inRtpPkt->getPayloadLoc();
			}
			else
			{
				vBuf = inRtpPkt->getPayloadLoc();
			}
#if 0			
			write (myFD, vBuf, RESID_RTP_RATE);
#endif
			// need to delete since RTP stack doesn't do it any more
			delete inRtpPkt; 
			inRtpPkt = NULL;
		}
#endif		
	}
#if 0
	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
		// cc = number of samples
		int cc;
		if ( sendRingback )
		{
			cc = getRingbackTone( outBufferPkt, RESID_RTP_RATE);
		}
		else
		{
			cc = read(myFD, outBufferPkt, RESID_RTP_RATE);
		}

		if (audioStack)
		{
			audioStack->transmitRaw (outBufferPkt, cc);
		}
	}
#endif
	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}
	deviceMutex.unlock();
}
void AssistVoiceDevice::processRTP ()
{
	deviceMutex.lock();
	if (audioStack == 0)
	{
		vusleep(10000);
		deviceMutex.unlock();
		return ;
	}

	RtpSessionState sessionState = audioStack->getSessionState();

	//only process receive if receive is enabled
	if ( sessionState == rtp_session_recvonly ||
		sessionState == rtp_session_sendrecv )
	{
		inRtpPkt = audioStack->receive ();
		if (inRtpPkt)
		{
			if (inRtpPkt->getPayloadUsage() != RESID_RTP_RATE)
				cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE");

			char *vBuf;
			if ( cwBeepOn )
			{
				vBuf = getCwBeep();
				if ( vBuf == 0 )
					vBuf = inRtpPkt->getPayloadLoc();
			}
			else
			{
				vBuf = inRtpPkt->getPayloadLoc();
			}
			write (myFD, vBuf, RESID_RTP_RATE);

			// need to delete since RTP stack doesn't do it any more
			delete inRtpPkt; 
			inRtpPkt = NULL;
		}
	}

	if ( sessionState == rtp_session_sendonly ||
		sessionState == rtp_session_sendrecv )
	{
		// cc = number of samples
		int cc;
		if ( sendRingback )
		{
			cc = getRingbackTone( outBufferPkt, RESID_RTP_RATE);
		}
		else
		{
			cc = read(myFD, outBufferPkt, RESID_RTP_RATE);
		}

		if (audioStack)
		{
			audioStack->transmitRaw (outBufferPkt, cc);
		}
	}

	if (audioStack &&
		sessionState != rtp_session_inactive &&
		sessionState != rtp_session_undefined )
	{
		audioStack->processRTCP();
	}
	deviceMutex.unlock();
}

int AssistVoiceDevice::addToFdSet (fd_set* fd)
{
	if ( ( audioActive || audioHalfActive ) && audioStack )
	{
		RtpSessionState sessionState = audioStack->getSessionState();
		if ( sessionState == rtp_session_recvonly ||
			sessionState == rtp_session_sendrecv )
		{
			FD_SET((audioStack->getRtpRecv())->getSocketFD(), fd);
			FD_SET((audioStack->getRtcpRecv())->getSocketFD(), fd);
		}
	}

	FD_SET(myFD, fd);
#if 0	
	FD_SET( stdinFD, fd );
#endif
	return 0;
} // addToFdSet()


//***************************************************************************
// description:  creates a new rtp session and reserves a rtp port
//***************************************************************************
int AssistVoiceDevice::getRtpPort()
{
	//this is an arbitrarily defined number
	const int HIGH_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
	const int LOW_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

	int port = 0;
	int lowPort = LOW_RTP_PORT;

	deviceMutex.lock();

	// create a rtp session if there is no existing session alread
	// this Rtp session will be idle
	if ( audioStack == 0 )
	{
		audioStack = new RtpSession(0);
	}

	while ( 1 )
	{
		// resever a rtp port
		port = audioStack->reserveRtpPort(lowPort, HIGH_RTP_PORT);
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

	deviceMutex.unlock();

	return port;
}


//***************************************************************************
// description: destroy the rtp session and release the rtp port
//***************************************************************************
void AssistVoiceDevice::releaseRtpPort()
{
	deviceMutex.lock();

	// destroy the rtp session if one exists
	if ( audioStack != 0 )
	{
		int port = audioStack->releaseRtpPort();
		cpDebug(LOG_DEBUG, "rtp port %d released", port );
		port = audioStack->releaseRtcpPort();
		cpDebug(LOG_DEBUG, "rtcp port %d released", port );
		delete audioStack;
		audioStack = 0;
	}

	deviceMutex.unlock();
}



//***************************************************************************
// description:  creates a new rtp session and also allocates memory for
//     incoming and outgoing rtp packet.  ioctl calls to initialize the card.
//***************************************************************************
int AssistVoiceDevice::audioStart (const HardwareAudioRequest& request)
{
	deviceMutex.lock();

	cpDebug(LOG_DEBUG, ">> Establishing audio" );
	cpDebug(LOG_DEBUG, ">> Listening on port: %d", request.localPort );
	cpDebug(LOG_DEBUG, ">> Sending to host  : %s", request.remoteHost );
	cpDebug(LOG_DEBUG, ">> Sending to port  : %d", request.remotePort );
	cpDebug(LOG_DEBUG, ">> RTP packet size  : %d ms", request.rtpPacketSize );

	if ( audioStack == 0 )
	{
		int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
		int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
		cpDebug(LOG_DEBUG, "Remote RTCP port : %d", remoteRtcpPort );
		cpDebug(LOG_DEBUG, "Local RTCP port  : %d", localRtcpPort );

		const char* remoteHost = 0;
		if ( request.remotePort != 0 )
			remoteHost = request.remoteHost;

		audioStack = new RtpSession(remoteHost, request.remotePort,
                                    request.localPort, remoteRtcpPort, localRtcpPort, 
                                    rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW);

		as_set_law(myFD, rtpPayloadPCMU, myDeviceName);

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
					cpDebug(LOG_DEBUG, "local host: %d", request.localPort );
					cpDebug(LOG_DEBUG, "local port: %d", request.localPort );
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW);
				}

				as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
				break;
			case rtp_session_recvonly:
				cpDebug(LOG_DEBUG, "Session recvonly" );
				//turn on the send
				cpDebug(LOG_DEBUG, "turning on the send" );
				cpDebug(LOG_DEBUG, "remote host: %d", request.remoteHost );
				cpDebug(LOG_DEBUG, "remote port: %d", request.remotePort );
				if ( request.remotePort != 0 )
				{
					audioStack->setSessionState(rtp_session_sendrecv);
					audioStack->setTransmiter(request.remoteHost, request.remotePort, 
						request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
				}

				as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
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
					audioStack->setReceiver( request.localPort, request.localPort + 1,
                                         0,  rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW );

					as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
				}

				//turn on the send
				if ( request.remotePort != 0 )
				{
					cpDebug(LOG_DEBUG, "Turn on send" );
					audioStack->setTransmiter( request.remoteHost, request.remotePort,
                                           request.remotePort + 1,  rtpPayloadPCMU, rtpPayloadPCMU );
					as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
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

	// inRtpPkt = 0;
	if( inRtpPkt )
	{
		delete inRtpPkt;
		inRtpPkt = 0;
	}

	audioStack->setApiFormat (rtpPayloadPCMU, RESID_RTP_RATE);
	audioStack->setNetworkFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);

	deviceMutex.unlock();
	return 0;
} // end audioStart()


//***************************************************************************
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  audioStack, rtp packets.
//***************************************************************************
int AssistVoiceDevice::audioStop()
{
	cpDebug(LOG_DEBUG, "***   Stopping audio   ***" );

	// mark audio as deactivated.
	audioActive = false;
	audioHalfActive = false;
	vusleep(200);

	// make hardware calls to stop audio
	deviceMutex.lock();

    // TBD, need to close and reopen device to stablize ipjack
    // hardware, hopefully this can be cleaned up with new driver
	close(myFD);
	myFD = open(myDeviceName, O_RDWR);
//	ioctl(myFD, IXJCTL_PORT, PORT_POTS);
	if (myFD < 0)
	{
		cpLog( LOG_ERR, "Cannot reopen %s", myDeviceName );
		exit(1);
	}

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

	if (audioStack)
	{
		delete audioStack;
		audioStack = 0;
	}

	deviceMutex.unlock();
	return 0;
} // end audioStop()


/* description: suspend the RTP transmit and receive, and also stop the */
int AssistVoiceDevice::audioSuspend ()
{
	deviceMutex.lock();

	cpDebug(LOG_INFO, "Suspending audio");
	cpDebug(LOG_INFO, "Setting all RTP/RTCP ports to 0");
	if ( audioStack != 0 )
	{ //set rtp session to inactive
		audioStack->setSessionState(rtp_session_inactive);
		audioStack->setReceiver(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW);
		audioStack->setTransmiter(0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU);
	}

	audioActive = false;
	audioHalfActive = false;

	deviceMutex.unlock();
	return 0;
} // end audioSuspend()


//***************************************************************************
// description:  resume the rtp session using the new sdp information
//               enable the quicknet card audio
//***************************************************************************
int AssistVoiceDevice::audioResume (const HardwareAudioRequest& request)
{
	deviceMutex.lock();

	if ( audioStack == 0 )
	{
		cpLog( LOG_ERR, "Try to resume an existing audio channel, but no existing audio channel");
		deviceMutex.unlock();
		return 0;
	}

	cpDebug(LOG_INFO, "Resuming audio");
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

	if ( request.localPort != 0 && request.remotePort != 0 )
	{        //full duplex audio
		audioStack->setSessionState(rtp_session_sendrecv);
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

		as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
		audioActive = true;
		audioHalfActive = false;
	}
	else if ( request.localPort != 0 )
	{//receive only
		audioStack->setSessionState(rtp_session_sendrecv);
		audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, AS_RTP_JITTER_NEW);

		as_set_law(myFD, rtpPayloadPCMU, myDeviceName);

		audioActive = false;
		audioHalfActive = true;
	}
	else if ( request.remotePort != 0 )
	{ //transmit only
		audioStack->setSessionState(rtp_session_sendrecv);
		audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);

		as_set_law(myFD, rtpPayloadPCMU, myDeviceName);
		audioActive = false;
		audioHalfActive = true;
	}


	if ( request.sendRingback )
		startSendRingback();
	else
		stopSendRingback();

	deviceMutex.unlock();
	return 0;
} // end audioResume()


int AssistVoiceDevice::getRingbackTone(char *ringData, int size)
{
    struct timeval start;      // ctam
    struct timeval finish;     // ctam
    gettimeofday (&start, 0);    // ctam

    int cc = 0;

    char tone[RESID_RTP_RATE];

    if ( ringbackFd == -1 )
    {
        cpDebug(LOG_INFO, "opening ringback tone to provide remote ringback");
        ringbackFd = open("Tone/ringback", O_RDONLY);
    }

    if ( ringbackFd == -1 )
    {
        cpLog(LOG_ERR, "can not open Tone/ringback");
        return cc;
    }

    cc = read(ringbackFd, tone, RESID_RTP_RATE);
    fflush(stdout);
#if 1
    while ( cc < size )
    {
        lseek(ringbackFd, 0, SEEK_SET);
        cc += read(ringbackFd, tone + cc, size - cc);
    }
#endif
    memcpy(ringData, tone, size);

    long diff = 0;
    while ( diff < 29500 )
    {
        if ( diff < 20000 ) vusleep(1000);
        gettimeofday (&finish, 0);   // ctam
        diff = finish.tv_usec - start.tv_usec + 1000000 * (finish.tv_sec - start.tv_sec);
    }

    return cc;
}


char* AssistVoiceDevice::getCwBeep()
{
	char *beep = 0;

	if( cwCadenceOn )
	{
		beep = cwBeep + cwCnt;
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= CWBEEP_ON_TIME )
		{
			cwCnt = 0;
			cwCadenceOn = false;
		}
	}
	else
	{
		cwCnt += RESID_RTP_RATE;
		if( cwCnt >= (CWBEEP_OFF_TIME - RESID_RTP_RATE) )
		{
			cwCnt = 0;
			cwCadenceOn = true;
		}
	}

	return beep;
}

//***************************************************************************
// description:  quicknet does not implement a flash function on their cards.
//               so, we have to implement our own, by checking the hook state
//               of the phone.  this function will tell us whether the phone
//               is being flashed or being placed onhook.
//***************************************************************************

void AssistVoiceDevice::onhookOrFlash()
{
	AS_DEVICE_STATE state;
	deviceMutex.lock();
	vusleep(20000);  // needed to stabilize hookstate readings

	state = as_device_check_state(myFD, myDeviceName);
#if 0
	if (!ioctl(myFD, PHONE_HOOKSTATE)) // onhook
#endif		
	if( state==AS_DEVICE_STATE_ONHOOK )
	{	// user is onhook or doing a flash
		hookStateOffhook = false;
		// need to wait FLASH_TIMEOUT microsec before reporting onhook
		// if user picks up before the timeout, report flash
		int flashTime = 0;

		//cpDebug(LOG_DEBUG,  "Starting Flash Timer");
		while (flashTime < FLASH_TIMEOUT) // begin testing for flash event
		{
			vusleep(50000);  // sleep for 50,000 microsec (50 ms)
			flashTime += 50000;
			state = as_device_check_state(myFD, myDeviceName);
#if 0			
			if (ioctl(myFD, PHONE_HOOKSTATE)) // user is offhook = flash!
#endif				
			if(state == AS_DEVICE_STATE_OFFHOOK)
			{
				flashTime = FLASH_TIMEOUT;  // will get us out of while-loop
				//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventFlash");
				hookStateOffhook = true;
				reportEvent(sessionQ, DeviceEventFlash);
			}
		} // end period for testing for a flash event
		//cpDebug(LOG_DEBUG,  "End Flash Timer");

		if (hookStateOffhook == false)
			// this must be an actual onhook event
		{
			//cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventHookDown");
			reportEvent(sessionQ, DeviceEventHookDown);
		}
	}
	deviceMutex.unlock();
} // end onhookOrFlash


void AssistVoiceDevice::provideDialToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start dial tone  ***" );
	deviceMutex.lock();
	as_tone_play_dial(myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideDialToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
	deviceMutex.lock();
	as_tone_play_stop(myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideRingStart()
{
	cpDebug(LOG_DEBUG, "***  Start Ringing  ***" );
	deviceMutex.lock();
	as_ring_start(myFD, myDeviceName);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideRingStop()
{
	cpDebug(LOG_DEBUG, "***  Stop Ringing  ***" );
	deviceMutex.lock();
	as_ring_stop(myFD, myDeviceName);
	deviceMutex.unlock();
}

void AssistVoiceDevice::provideLocalRingbackStart()
{
	cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
	deviceMutex.lock();
	as_tone_play_ringback(myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideLocalRingbackStop()
{
	cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
	deviceMutex.lock();
	as_tone_play_stop( myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideBusyToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start busy tone  ***" );
	deviceMutex.lock();
	as_tone_play_busy( myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop busy tone  ***" );
	deviceMutex.lock();
	as_tone_play_stop( myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideFastBusyToneStart()
{
	cpDebug(LOG_DEBUG, "***  Start fast busy tone  ***" );
	deviceMutex.lock();
	//for now using regular busy tone
	as_tone_play_busy(myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideFastBusyToneStop()
{
	cpDebug(LOG_DEBUG, "***  Stop fast busy tone  ***" );
	deviceMutex.lock();
	as_tone_play_stop( myFD);
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideCallWaitingBeepStart()
{
	cpDebug(LOG_DEBUG, "***  Start call waiting beep ***" );
	//TODO: figure out how to do this with for now, just print something
	cout << "!!!!!!! Start call waiting beep" << endl;
	deviceMutex.lock();
	as_tone_play_callwait(myFD);
#if 0	
	cwBeepOn = true;
	cwCnt = 0;
	cwCadenceOn = true;
#endif	
	deviceMutex.unlock();
}


void AssistVoiceDevice::provideCallWaitingBeepStop()
{
	cpDebug(LOG_DEBUG, "***  Stop call waiting beep ***" );

	//TODO: figure out how to do this with for now, just print something
	cout << "!!!!!!! Stop call waiting beep" << endl;
	deviceMutex.lock();
	as_tone_play_stop(myFD);
#if 0	
	cwBeepOn = false;
#endif	
	deviceMutex.unlock();
}

/* send DTMF signal in the method of RFC2833 */
void AssistVoiceDevice::provideDtmf(DeviceSignalType signal)
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


void AssistVoiceDevice::provideCallInfo(string callerNum, string calleeNum, string reason)
{
	cpDebug(LOG_DEBUG, "Caller is '%s',\t\tCallee is '%s',\t\treason is '%s'\r\n" , 
		callerNum.c_str(), calleeNum.c_str(), reason.c_str() );
}
