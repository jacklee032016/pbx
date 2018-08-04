/*
 * $Id: VmcpDevice.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

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

#include "VmcpDevice.hxx"
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
// VmcpDevice::VmcpDevice
// description:  Constructor. Initialize the hookstate.
//               Resets the device to get it ready.
//***************************************************************************

VmcpDevice::VmcpDevice(char* hostName, int port,
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
}  // end VmcpDevice::VmcpDevice()


//***************************************************************************
// VmcpDevice::~VmcpDevice
// description:  Destructor
//***************************************************************************


VmcpDevice::~VmcpDevice(void)
{
    vm.sendClose();
    close(ss);   
} // end VmcpDevice::~VmcpDevice()


//***************************************************************************
// VmcpDevice::hardwareMain
// description:  main processing loop of the hardware
//***************************************************************************

void*
VmcpDevice::hardwareMain (void* parms)
{
    // process forever on behalf of VmcpDevice hardware
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

} // end VmcpDevice::hardwareMain()



//***************************************************************************
// VmcpDevice::process
//
// description:  process any events from vmserver of type VMCP. 
//               Like play the file, start recording, stop recording etc.             
//***************************************************************************

int
VmcpDevice::process (fd_set* fd)
{
    deviceMutex.lock();

    if ( (hookStateOffhook == true) && (ss > 0 ))
    {
        if (FD_ISSET(ss, fd))
        {
            int Msg;
	    Msg=vm.getMsg();

            if ( Msg < 1 )  
            {
                reportEvent(sessionQ, DeviceEventHookDown);
                hookStateOffhook = false;
                deviceMutex.unlock();
                return 0;
            }
            switch (Msg)
            {
		case Vmcp::Close:
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

        }
    }
    deviceMutex.unlock();
    return 0;
} // end VmcpDevice::process()

//***************************************************************************
// VmcpDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************

void
VmcpDevice::processRTP ()
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

    int wait = networkPktSize - (vm_gettimeofday() - nextTime);

    if ( wait > 0 ) 
    {
	vusleep(wait*1000);
#if !defined(sun)
#endif
    }

    nextTime += networkPktSize;

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
             vm.playStopped();
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
VmcpDevice::addToFdSet (fd_set* fd)
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
} // end VmcpDevice::addToFdSet()

//***************************************************************************
// VmcpDevice::getRtpPort()
//
// description:  creates a new rtp session and reserves a rtp port
//***************************************************************************
int
VmcpDevice::getRtpPort()
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
        // reserve a rtp port
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
// VmcpDevice::releaseRtpPort()
//
// description: destroy the rtp session and release the rtp port
//***************************************************************************
void
VmcpDevice::releaseRtpPort()
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
// VmcpDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet. 
//***************************************************************************

int
VmcpDevice::audioStart (const HardwareAudioRequest& request)
{
    if ( audioActive )
    {
        cpLog(LOG_ERR, "Audio channel is already active. Ignoring");
        return 0;
    }

    deviceMutex.lock();


    if ( audioStack == 0 )
    {
	cpDebug(LOG_DEBUG, "creating audiostack");
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

    _DTMFInterface = new RtpDevice < VmcpDevice > ( this );
    audioStack->setDTMFInterface( _DTMFInterface );


    // allocate RTP packet spaces
    inRtpPkt = 0;

    audioStack->setSessionState(rtp_session_sendrecv);
    audioStack->setNetworkPktSize( request.rtpPacketSize * 8);
    networkPktSize = request.rtpPacketSize;
    audioStack->setApiPktSize(request.rtpPacketSize * 8);

    packetCount = 0;

    // mark audio as active
    cpDebug(LOG_DEBUG, "setting audio active");
    nextTime = vm_gettimeofday();
    audioActive = true;
    hasPlayed = false;
    deviceMutex.unlock();

    return 0;
} // end VmcpDevice::audioStart()

//***************************************************************************
// VmcpDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  inBuffer, outBuffer, audioStack,
//               rtp packets.
//***************************************************************************

int
VmcpDevice::audioStop (void)
{
    //    if (!audioActive)  return 1;

    // mark audio as deactivated.
    cpDebug(LOG_DEBUG, "Audio Stop received.");
    deviceMutex.lock();
    audioActive = false;
    hookStateOffhook = false;
    reportEvent(sessionQ, DeviceEventHookDown);

    player.stop();
    recorder.close();

    vm.sendClose(); 
	
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
} // end VmcpDevice::audioStop()



void
VmcpDevice::provideRingStart()
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

    if(vmServers.size() == 0)
    {
	// there are NO VM servers.  This is obviously a problem
	cpLog(LOG_ALERT, "No VmServers are configured in the config file.");
	cpLog(LOG_ALERT, "Add a VmServer entry to your uavm config file.");
	exit(-1);
    }

    unsigned int i;
    for( i = 0; i < vmServers.size() ; i++ )
    {	/** Open the socket */
        cpDebug(LOG_DEBUG,"Trying to connect to vmserver %s:%d",vmServers[i].c_str(),8024);
	/** Connect to the Server */
        if( vm.connect(vmServers[i].c_str(),8024) )  
        {
           cpDebug(LOG_DEBUG,"Server connected");
           break;
        }
        else
        {
            cpDebug(LOG_DEBUG,"Server not responding, trying the next one");
        }
    }
    if( i==vmServers.size() )
    {
         cpLog(LOG_ERR,"Can't find any live vmservers");
         /** Release the Mutex */
         deviceMutex.unlock();
         return;
    }
        
    ss = vm.getFd();
    vm.setSessionInfo(CallerId,CalleeId,"5000",ForwardReason,NumberOfForwards);   

    hookStateOffhook = true;
    reportEvent(sessionQ, DeviceEventHookUp);

    deviceMutex.unlock();
}

void
VmcpDevice::provideRingStop()
{}


void
VmcpDevice::provideDialToneStart()
{}


void
VmcpDevice::provideDialToneStop()
{}


void
VmcpDevice::provideLocalRingbackStart()
{}



void
VmcpDevice::provideLocalRingbackStop()
{}


void
VmcpDevice::provideBusyToneStart()
{}


void
VmcpDevice::provideBusyToneStop()
{}



void
VmcpDevice::provideFastBusyToneStart()
{}



void
VmcpDevice::provideFastBusyToneStop()
{}


void
VmcpDevice::provideCallWaitingBeepStart()
{}



void
VmcpDevice::provideCallWaitingBeepStop()
{}


void
VmcpDevice::provideDtmf(DeviceSignalType signal)
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
    vm.sendDtmf(digit);
    deviceMutex.unlock();

    return ;
}


void
VmcpDevice::recvRTPDTMF( int digit, int duration )
{
    cpDebug(LOG_DEBUG, "DTMF digit %d has been received from RTP stack", digit);

    DeviceSignalType dtmfSignal = DeviceSignalNULL;

    switch ( digit )
    {
        case 0:
            reportEvent(sessionQ, DeviceEventDtmf0);
            break;  
	case 1:
            reportEvent(sessionQ, DeviceEventDtmf1);
            break;
        case 2:
            reportEvent(sessionQ, DeviceEventDtmf2);
            break;
        case 3:
            reportEvent(sessionQ, DeviceEventDtmf3);
            break;
        case 4:
            reportEvent(sessionQ, DeviceEventDtmf4);
            break;
        case 5:
            reportEvent(sessionQ, DeviceEventDtmf5);
            break;
        case 6:
            reportEvent(sessionQ, DeviceEventDtmf6);
            break;
        case 7:
            reportEvent(sessionQ, DeviceEventDtmf7);
            break;      
        case 8:
            reportEvent(sessionQ, DeviceEventDtmf8);
            break;
        case 9:
            reportEvent(sessionQ, DeviceEventDtmf9);
            break;
        case 10:
            reportEvent(sessionQ, DeviceEventDtmfStar);
            break;
        case 11:
            reportEvent(sessionQ, DeviceEventDtmfHash);
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
VmcpDevice::provideCallInfo(string sCallerId, string sCalleeId, string sForwardReason)
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
