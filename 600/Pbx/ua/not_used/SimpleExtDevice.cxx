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

static const char* const SimpleExtDevice_cxx_Version 
    = "$Id: SimpleExtDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";

#include "global.h"
#include <cassert>

/* stdlib */
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>
#include <sys/time.h>

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <VTime.hxx>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* error handling */
#include <cerrno>

#include "SimpleExtDevice.hxx"
#include "UaDevice.hxx"
#include "UaConfiguration.hxx"
#include "DeviceEvent.hxx"
#include "cpLog.h"
#include "DTMFInterface.hxx"
#include "RtpEvent.hxx"
#include "VCondition.h"
                             
using namespace Vocal;

static const int ULAW_PAYLOAD = 1012;
static const int ULAW_SAMPLE_RATE = 240;
static const int RESID_RTP_RATE = 240;
static const int NETWORK_RTP_RATE = 160;


//***************************************************************************
// SimpleExtDevice::SimpleExtDevice
// description:  Constructor. Initialize the hookstate.
//               Resets the device to get it ready.
//***************************************************************************

SimpleExtDevice::SimpleExtDevice(char* hostName, int port,
                       Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                       Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ)
    : ResGwDevice( hostName, inputQ, outputQ ),
      hasPlayed(false)
{
    // initialize queues
    sessionQ = outputQ;
    myQ = inputQ;

    myHostName = hostName;
    myPortNumber = port;

    fwdFlag = false;

    hookStateOffhook = false;
    serverAvailable = false;

    prevRtpPacketTime.tv_usec = 0;

    audioStack = 0;

	digitCollector = new DigitCollector(AS_DEVICE_NOT_DEFINED, outputQ );

}  // end SimpleExtDevice::SimpleExtDevice()


//***************************************************************************
// SimpleExtDevice::~SimpleExtDevice
// description:  Destructor
//***************************************************************************


SimpleExtDevice::~SimpleExtDevice(void)
{
//    vm.sendClose();
//    close(ss);   
} // end SimpleExtDevice::~SimpleExtDevice()


//***************************************************************************
// SimpleExtDevice::hardwareMain
// description:  main processing loop of the hardware
//***************************************************************************

void*
SimpleExtDevice::hardwareMain (void* parms)
{
    // process forever on behalf of SimpleExtDevice hardware
    fd_set readfds;
    struct timeval tv;
    int retval;
    int maxFd = 128;  


    // reset file descriptor
    FD_ZERO(&readfds);
    addToFdSet(&readfds);

    // block on select for asyncronous events, but poll to process
    // audio and signal requests from endpoints in message queue
    tv.tv_sec = 0;
    tv.tv_usec = 20000;

    if ((retval = select(maxFd, &readfds, 0, 0, &tv)) < 0)
    {
        cpLog(LOG_ERR, "select() returned with an error" );
    }
    else
    {
        if (process(&readfds) < 0)
        {
            cpLog(LOG_ERR, "hardware encountered an error");
            assert(0);
        }
    }

    // check for signal requests from the session
    if (myQ->size() > 0)
    {
        processSessionMsg(myQ->getNext());
    }

    return 0;

} // end SimpleExtDevice::hardwareMain()



//***************************************************************************
// SimpleExtDevice::process
//
// description:  process any events from vmserver of type VMCP. 
//               Like play the file, start recording, stop recording etc.             
//***************************************************************************

int
SimpleExtDevice::process (fd_set* fd)
{
    deviceMutex.lock();

    if ( (hookStateOffhook == true) && (ss > 0 ))
    {
        if (FD_ISSET(ss, fd))
        {
            int Msg;
//	    Msg=vm.getMsg();

            if ( Msg < 1 )  
            {
                reportEvent(sessionQ, DeviceEventHookDown , AS_DEVICE_NOT_DEFINED);
                hookStateOffhook = false;
                deviceMutex.unlock();
                return 0;
            }
#if 0
            switch (Msg)
            {

//		case Vmcp::Close:
		{
                    cpDebug(LOG_DEBUG,"VMCP:Close");
                    reportEvent(sessionQ, DeviceEventHookDown);
                    hookStateOffhook = false;
		    vm.sendClose();
		    close(ss);
                    deviceMutex.unlock();
                    return 0;
		}
		case Vmcp::PlayFile:
                    cpDebug(LOG_DEBUG,"VMCP:Playing file %s",vm.getPlayFileName().c_str());
                    player.add(vm.getPlayFileName());
                    break;

                case Vmcp::StartPlay:
                    cpDebug(LOG_DEBUG,"VMCP:Start player");
                    if( !player.start() )
		    {
			vm.playStopped();
		    }
                    break;

                case Vmcp::RecordFile:
                    cpDebug(LOG_DEBUG,"VMCP:RecordFile");
                    recorder.open(vm.getRecordFileName());
                    break;

                case Vmcp::StartRecord:
                    cpDebug(LOG_DEBUG,"VMCP:StartRecord");
                    recorder.start();
                    break;

                case Vmcp::StopRecord:
                    cpDebug(LOG_DEBUG,"VMCP:StopRecord");
                    recorder.close();
                    break;

                case Vmcp::StopPlay:
                    cpDebug(LOG_DEBUG,"VMCP:StopPlayer");
                    player.stop();
                    break;     
	 	
		default:
		    break;               
            }
#endif

        }
    }
    deviceMutex.unlock();
    return 0;
} // end SimpleExtDevice::process()

//***************************************************************************
// SimpleExtDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************

void
SimpleExtDevice::processRTP ()
{
    if ( !audioActive )
    {
        vusleep(30000);
        return ;
    }
    if ( !hookStateOffhook )
    {
        vusleep(30000);
        return ;
    }

//    int wait = networkPktSize - (vm_gettimeofday() - nextTime);

//    if ( wait > 0 ) vusleep(wait*1000);

//    nextTime += networkPktSize;

    deviceMutex.lock();


    if (audioStack)
    {
        inRtpPkt = audioStack->receive ();
    }



    if (inRtpPkt)
    {
        packetCount++;
        recorder.write(inRtpPkt->getPayloadLoc(), inRtpPkt->getPayloadUsage());
        delete inRtpPkt;
        inRtpPkt = 0;
    }

    if (audioStack)
    {
        char buffer[10240];
        memset(buffer, 0xFE, networkPktSize*8);
        if ( !player.getData(buffer, networkPktSize*8) )
        {
//             vm.playStopped();
        }
        audioStack->transmitRaw (buffer, networkPktSize*8);
    }


    if (audioStack)
    {
        audioStack->processRTCP();
    }

    deviceMutex.unlock();
}

int
SimpleExtDevice::addToFdSet (fd_set* fd)
{
    if( hookStateOffhook && (ss > 0 ))
    {
	// set the VM controller to active
        FD_SET(ss,fd);     
        if ( audioActive )
        {
            RtpSessionState sessionState = audioStack->getSessionState();
            if ( sessionState == rtp_session_recvonly ||
                    sessionState == rtp_session_sendrecv )
            {
                FD_SET((audioStack->getRtpRecv())->getSocketFD(), fd);
                FD_SET((audioStack->getRtcpRecv())->getSocketFD(), fd);
            }
        }
    }

    return 0;
} // end SimpleExtDevice::addToFdSet()

//***************************************************************************
// SimpleExtDevice::getRtpPort()
//
// description:  creates a new rtp session and reserves a rtp port
//***************************************************************************
int
SimpleExtDevice::getRtpPort()
{
    //this is an arbitrarily defined number
    const int MAX_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
    const int MIN_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

    int port = 0;
    int minPort = MIN_RTP_PORT;

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
        port = audioStack->reserveRtpPort(minPort, MAX_RTP_PORT);
        minPort = (port > minPort) ? port : minPort;

        // attempt to reserve a rtcp port on port number higher than
        // the rtp port
        if ( port != 0 && audioStack->reserveRtcpPort(port + 1, 0) != 0 )
        {
            break;
        }

        // if not successful allocating rtcp port, increment the minimum rtp
        // port and try again.  If minPort reaches MAX_RTP_PORT, return 0
        // to indicate port allocation failed.
        minPort += 2;
        if ( minPort > MAX_RTP_PORT )
        {
            port = 0;
            break;
        }
    }

    deviceMutex.unlock();

    return port;
}

//***************************************************************************
// SimpleExtDevice::releaseRtpPort()
//
// description: destroy the rtp session and release the rtp port
//***************************************************************************
void
SimpleExtDevice::releaseRtpPort()
{
    deviceMutex.lock();

    // destroy the rtp session if one exists
    if ( audioStack != 0 )
    {
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

    deviceMutex.unlock();
}

//***************************************************************************
//***************************************************************************


//***************************************************************************
// SimpleExtDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet. 
//***************************************************************************

int
SimpleExtDevice::audioStart (const HardwareAudioRequest& request,int deviceId)
{
    if ( audioActive )
    {
        cpLog(LOG_ERR, "Audio channel is already active. Ignoring");
        return 0;
    }

    deviceMutex.lock();


    if ( audioStack == 0 )
    {
        audioStack = new RtpSession(request.remoteHost, request.remotePort,
                                    request.localPort, request.remotePort + 1,
                                    request.localPort + 1, rtpPayloadPCMU,
                                    rtpPayloadPCMU, 5);
    }
    else
    {
        RtpSessionState sessionState = audioStack->getSessionState();
        switch ( sessionState )
        {
            case rtp_session_sendonly:
                //turn on the receive
                if ( request.localPort != 0 )
                {
                    audioStack->setSessionState(rtp_session_sendrecv);
                    audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, 5);
                }
                break;

            case rtp_session_recvonly:
                //turn on the send
                if ( request.remotePort != 0 )
                {
                    audioStack->setSessionState(rtp_session_sendrecv);
                    audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
                }
                break;

            case rtp_session_sendrecv:
		deviceMutex.unlock();   
                return 0;
                break;
        
            default:
                audioStack->setSessionState(rtp_session_sendrecv);
                //turn on the send
                if ( request.remotePort != 0 )
                {
                    audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
                }
               //turn on the receive
               if ( request.localPort != 0 )
               {
                    audioStack->setReceiver(request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, 5);
               }
               break;
        }
    }

    _DTMFInterface = new RtpDevice < SimpleExtDevice > ( this );
    audioStack->setDTMFInterface( _DTMFInterface );


    // allocate RTP packet spaces
    inRtpPkt = 0;

    audioStack->setSessionState(rtp_session_sendrecv);
    audioStack->setNetworkPktSize( request.rtpPacketSize * 8);
    networkPktSize = request.rtpPacketSize;
    audioStack->setApiPktSize(request.rtpPacketSize * 8);

    packetCount = 0;

    // mark audio as active
    audioActive = true;
    hasPlayed = false;
//    nextTime = vm_gettimeofday();
    deviceMutex.unlock();

    return 0;
} // end SimpleExtDevice::audioStart()

//***************************************************************************
// SimpleExtDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  inBuffer, outBuffer, audioStack,
//               rtp packets.
//***************************************************************************

int
SimpleExtDevice::audioStop (int deviceId)
{
    //    if (!audioActive)  return 1;

    // mark audio as deactivated.
    cpDebug(LOG_DEBUG, "Audio Stop received.");
    deviceMutex.lock();
    audioActive = false;
    hookStateOffhook = false;
    reportEvent(sessionQ, DeviceEventHookDown , AS_DEVICE_NOT_DEFINED);

    player.stop();
    recorder.close();

//    vm.sendClose(); 
	
    close(ss);
    ss = -1;
    cpDebug(LOG_DEBUG,"End of session");  

    if ( audioStack )
    {
        audioStack->unsetDTMFInterface( _DTMFInterface );
        delete audioStack;
        audioStack = NULL;
    }

    if ( _DTMFInterface )
    {
        delete _DTMFInterface;
        _DTMFInterface = 0;
    }

    cpDebug(LOG_DEBUG, "Num of RTP packets received :%d", packetCount);

    deviceMutex.unlock();

    return 0;
} // end SimpleExtDevice::audioStop()



void
SimpleExtDevice::provideRingStart(int deviceId)
{
    deviceMutex.lock();

    if( ss > 0 ) 
    {
        cpLog(LOG_ERR, "Already active!!! (%d)", ss);
        deviceMutex.unlock();
        return ;
    }
    
    /* Redundancy : Try to connect to the one of the Vmservers 
     * configured in the config file.
     */
    vector<string>& vmServers=UaConfiguration::instance()->getVMServers();
     
    unsigned int i;
    for( i = 0; i < vmServers.size() ; i++ )
    {	/** Open the socket */
        cpDebug(LOG_DEBUG,"Trying %s:%d",vmServers[i].c_str(),8024);
	/** Connect to the Server */
#if 0
        if( vm.connect(vmServers[i].c_str(),8024) )  
        {
           cpDebug(LOG_DEBUG,"Server connected");
           break;
        }
        else
        {
            cpDebug(LOG_DEBUG,"Server not responding, trying the next one");
        }
#endif
    }
    if( i==vmServers.size() )
    {
         cpLog(LOG_ERR,"Can't find any live vm servers");
         /** Release the Mutex */
         deviceMutex.unlock();
         return;
    }
        
//    ss = vm.getFd();
//    vm.setSessionInfo(CallerId,CalleeId,"5000",ForwardReason,NumberOfForwards);   

    hookStateOffhook = true;
    reportEvent(sessionQ, DeviceEventHookUp , AS_DEVICE_NOT_DEFINED);

    deviceMutex.unlock();
}

void
SimpleExtDevice::provideRingStop(int deviceId)
{}


void
SimpleExtDevice::provideDialToneStart(int deviceId)
{}


void
SimpleExtDevice::provideDialToneStop(int deviceId)
{}


void
SimpleExtDevice::provideLocalRingbackStart(int deviceId)
{}



void
SimpleExtDevice::provideLocalRingbackStop(int deviceId)
{}


void
SimpleExtDevice::provideBusyToneStart(int deviceId)
{}


void
SimpleExtDevice::provideBusyToneStop(int deviceId)
{}



void
SimpleExtDevice::provideFastBusyToneStart(int deviceId)
{}



void
SimpleExtDevice::provideFastBusyToneStop(int deviceId)
{}


void
SimpleExtDevice::provideCallWaitingBeepStart(int deviceId)
{}



void
SimpleExtDevice::provideCallWaitingBeepStop(int deviceId)
{}


void
SimpleExtDevice::provideDtmf(DeviceSignalType signal,int deviceId)
{
    char digit;
    switch ( signal )
    {
        case DeviceSignalDigitStar:
            digit = '*';
            break;
        case DeviceSignalDigitHash:
            digit = '#';
            break;
        case DeviceSignalDigit0:
            digit = '0';
            break;
        case DeviceSignalDigit1:
            digit = '1';
            break;
        case DeviceSignalDigit2:
            digit = '2';
            break;
        case DeviceSignalDigit3:
            digit = '3';
            break;
        case DeviceSignalDigit4:
            digit = '4';
            break;
        case DeviceSignalDigit5:
            digit = '5';
            break;
        case DeviceSignalDigit6:
            digit = '6';
            break;
        case DeviceSignalDigit7:
            digit = '7';
            break;
        case DeviceSignalDigit8:
            digit = '8';
            break;
        case DeviceSignalDigit9:
            digit = '9';
            break;
        default:
            cpLog(LOG_ERR, "Unrecognized DTMF key received from event stack");
	    return;
    }

    deviceMutex.lock();
    cpDebug(LOG_DEBUG, "Sending %d to vmserver", digit);
//    vm.sendDtmf(digit);
    deviceMutex.unlock();

    return ;
}


void
SimpleExtDevice::recvRTPDTMF( int digit, int duration )
{
    cpDebug(LOG_DEBUG, "DTMF digit %d has been received from RTP stack", digit);

    DeviceSignalType dtmfSignal = DeviceSignalNULL;

    switch ( digit )
    {
        case 0:
            reportEvent(sessionQ, DeviceEventDtmf0 , AS_DEVICE_NOT_DEFINED);
            break;  
	case 1:
            reportEvent(sessionQ, DeviceEventDtmf1 , AS_DEVICE_NOT_DEFINED);
            break;
        case 2:
            reportEvent(sessionQ, DeviceEventDtmf2 , AS_DEVICE_NOT_DEFINED);
            break;
        case 3:
            reportEvent(sessionQ, DeviceEventDtmf3 , AS_DEVICE_NOT_DEFINED);
            break;
        case 4:
            reportEvent(sessionQ, DeviceEventDtmf4 , AS_DEVICE_NOT_DEFINED);
            break;
        case 5:
            reportEvent(sessionQ, DeviceEventDtmf5 , AS_DEVICE_NOT_DEFINED);
            break;
        case 6:
            reportEvent(sessionQ, DeviceEventDtmf6 , AS_DEVICE_NOT_DEFINED);
            break;
        case 7:
            reportEvent(sessionQ, DeviceEventDtmf7 , AS_DEVICE_NOT_DEFINED);
            break;      
        case 8:
            reportEvent(sessionQ, DeviceEventDtmf8 , AS_DEVICE_NOT_DEFINED);
            break;
        case 9:
            reportEvent(sessionQ, DeviceEventDtmf9 , AS_DEVICE_NOT_DEFINED);
            break;
        case 10:
            reportEvent(sessionQ, DeviceEventDtmfStar , AS_DEVICE_NOT_DEFINED);
            break;
        case 11:
            reportEvent(sessionQ, DeviceEventDtmfHash , AS_DEVICE_NOT_DEFINED);
            break;
	default:
       	   cpLog(LOG_ERR, "Unrecognized DTMF key received from RTP stack");         
           return;
    }

    Sptr < UaHardwareEvent > signal = new UaHardwareEvent( UaDevice::getDeviceQueue() );
    signal->type = HardwareSignalType;
    signal->signalOrRequest.signal = dtmfSignal;

    UaDevice::getDeviceQueue()->add( signal );

    return ;
}


void
SimpleExtDevice::provideCallInfo(string sCallerId, string sCalleeId, string sForwardReason)
{
    cpDebug(LOG_DEBUG, "Call from %s to %s", sCallerId.c_str(), sCalleeId.c_str());

    CallerId = sCallerId;
    CalleeId = sCalleeId;
    if ( CalleeId == "" ) CalleeId = "5000";
    if ( CallerId == "" ) CallerId = "unknown";

    cpDebug(LOG_DEBUG,"Sending call info");   
    if ( sCalleeId == "5000" )
    {
        cpDebug(LOG_DEBUG," Direct call to 5000");
        NumberOfForwards = 0;
        ForwardReason = "UNK";
    }
    else if ( sForwardReason == "" || sForwardReason == "Unknown" && !fwdFlag )
    {
	cpDebug(LOG_DEBUG," Forward reason unknown");
        NumberOfForwards = 0;
        ForwardReason = "UNK";
    }
    else
    {
	cpDebug(LOG_DEBUG," Forward No Answer");
        NumberOfForwards = 1;
        ForwardReason = "FNA";
    }

}


int SimpleExtDevice::getDeviceId( const SipCallId& callId ) 
{
	return AS_DEVICE_NOT_DEFINED;
}

Sptr<DigitCollector> SimpleExtDevice::getDigitCollector(int deviceId) 
{
	return digitCollector;
}

int SimpleExtDevice::getRtpPort(int deviceId)
{
	return getRtpPort();
}

void SimpleExtDevice::releaseRtpPort(int deviceId)
{
	releaseRtpPort();
}

HardwareStatusType SimpleExtDevice::getHardwareStatus(int deviceId)
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
	{
		if ( callId == 0 )
			hardwareStatus = HARDWARE_CALLWAITING_ALLOWED;
	}

	return hardwareStatus;
}

bool SimpleExtDevice::isMyHardware(const SipCallId& callid, int deviceId)
{
	if ( callId == 0 )
		return false;

	return ( *callId == callid );
}

