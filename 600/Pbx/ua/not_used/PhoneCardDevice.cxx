
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


static const char* const PhoneCardDevice_cxx_Version =
    "$Id: PhoneCardDevice.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";




#include "global.h"
#include <cassert>

/* stdlib */
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>
#include <termios.h>

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <linux/ixjuser.h>
#include <linux/telephony.h>


#include "VTime.hxx"
#include "Rtp.hxx"
#include "VCondition.h"

/* error handling */
#include <cerrno>


#include "PhoneCardDevice.hxx"
#include "UaDeviceEvent.hxx"

#define ULAW_PAYLOAD 1012
#define ULAW_SAMPLE_RATE 240
#define RESID_RTP_RATE 240
#define NETWORK_RTP_RATE 160
#define FLASH_TIMEOUT 1000000

using namespace Vocal;


//***************************************************************************
// PhoneCardDevice::PhoneCardDevice
// description:  Constructor.  opens the PhoneCARD device.  initialize the
//               hookstate.  resets the device to get it ready.
//***************************************************************************

PhoneCardDevice::PhoneCardDevice( const char* deviceName,
                                  Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                                  Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ )
        : ResGwDevice( deviceName, inputQ, outputQ ),
        //      audioActive( false ),
        //      hasPlayed( false ),
        audioStack( 0 ),
        inRtpPkt( 0 ),
        ringbackFd( -1 ),
        sendRingback( false )
{
    // initialize queues
    sessionQ = outputQ;
    myQ = inputQ;
    hookStateOffhook = false;  // assume onhook for phonecard
    playDialTone = false;

    // open hardware device
    char pname[80];
    sprintf( pname, "%s", deviceName );

    myFD = open( pname, O_RDWR );

    if ( myFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot open %s", deviceName );
        exit( -1 );
    }

    // close and reopen to make sure device is not in a funny state

    ioctl( myFD, IXJCTL_REC_STOP );
    ioctl( myFD, IXJCTL_PLAY_STOP );
    ioctl( myFD, IXJCTL_AEC_STOP );

    close( myFD );
    myFD = open( deviceName, O_RDWR );
    if ( myFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot re-open ", deviceName );
        exit( 1 );
    }

    // initialize hookstate to onhook even if IXJCTL_HOOKSTATE returns
    // offhook.  onhook signifies that the cord is plugged in!

    if ( ioctl( myFD, IXJCTL_HOOKSTATE ) ) // this means that the cord is
    {	                                  // plugged in.
        cpDebug(LOG_DEBUG, "Cord is plugged into the PhoneCARD:"
               " [hardware-offhook]" );
    }
    else
    {
        cpLog( LOG_ERR, "Cord is not plugged into the PhoneCARD, or"
               " PhoneCARD error." );
        exit( -1 );
    }

    // set ring to 3 ring instead of the default of 5
    //ioctl(myFD, IXJCTL_MAXRINGS, 3);

    // store the device name.
    myDeviceName = deviceName;

    // we need to set the terminal correctly.  this will enable us to do
    // single keypresses without needing to press <enter>.

    // store stdin file descriptor
    stdinFD = fileno( stdin );
    // retrieve current term settings
    tcgetattr( stdinFD, &initialTerm );
    // change the term settings to non-canonical
    newTerm = initialTerm;
    newTerm.c_lflag &= ~ICANON;
    newTerm.c_cc[VMIN] = 0;
    newTerm.c_cc[VTIME] = 0;
    if ( tcsetattr( stdinFD, TCSANOW, &newTerm ) != 0 )
    {
        cpLog( LOG_ERR, "Unable to set new terminal mode." );
        exit( -1 );
    }
}  // end PhoneCardDevice::PhoneCardDevice()


//***************************************************************************
// PhoneCardDevice::~PhoneCardDevice
// description:  Destructor
//***************************************************************************


PhoneCardDevice::~PhoneCardDevice()
{
    close( myFD );
    tcsetattr( stdinFD, TCSANOW, &initialTerm );
} // end PhoneCardDevice::~PhoneCardDevice()


//***************************************************************************
// PhoneCardDevice::hardwareMain
// description:  main processing loop of the hardware
//***************************************************************************

void*
PhoneCardDevice::hardwareMain( void* parms )
{
    // process forever on behalf of PhoneCardDevice hardware
    fd_set readfds;
    struct timeval tv;
    int retval;
    int maxFd = 128;  // should be 0 then increment as needed when addToFdSet


    // reset file descriptor
    FD_ZERO( &readfds );
    addToFdSet( &readfds );
    //if (maxFd <= gateway.myAgent->getFd())
    //maxFd =  gateway.myAgent->getFd() + 1;

    // block on select for asyncronous events, but poll to process
    // audio and signal requests from endpoints in message queue
    tv.tv_sec = 0;
    tv.tv_usec = 300;

    if ( ( retval = select( maxFd, &readfds, 0, 0, &tv ) ) < 0 )
    {
        cpLog( LOG_ERR, "select() returned error %d", retval );
    }
    else
    {
        if ( process( &readfds ) < 0 )
        {
            cpLog( LOG_ERR, "hardware encountered an error" );
            assert( 0 );
        }
    }

    // check for signal requests from the session
    if ( myQ->size() > 0 )
    {
        processSessionMsg( myQ->getNext() );
    };
    return 0;

} // end PhoneCardDevice::hardwareMain()



//***************************************************************************
// PhoneCardDevice::process
//
// description:  process any events detected on the hardware as well as
//               simulated device events triggered by keyboard input.
//               report these events back to the session via the fifo queue.
//***************************************************************************

int
PhoneCardDevice::process( fd_set* fd )
{
    vusleep( 0 );  // needed to stabilize hookstate readings

    deviceMutex.lock();

    // check to make sure the user has the cord plugged into the
    // phonecard.  this is done through IXJCTL_HOOKSTATE for this
    // card only.  if the cord is plugged in, we assume a 'live'
    // state in which we can go onhook / offhook using the keyboard.

    if ( ioctl( myFD, IXJCTL_HOOKSTATE ) )
    {
        // user has the cord plugged in and the phonecard is in
        // the proper state.

        // now, check for events coming from the keyboard
        // initialize the event we will send
        Sptr < UaDeviceEvent > event = new UaDeviceEvent( sessionQ ) ;
        assert( event != 0 );
        event->type = DeviceEventUndefined;  // default to undefined

        // now, we need to set the flags for the stdin file descriptor.
        // this will allow us to do a non-blocking read on stdin.
        //	int stdinFD = fileno( stdin );
        //	int flags;
        char keystroke = '\0';
        int retval = 0;

        //	if( ( flags = fcntl( stdinFD, F_GETFL, 0 ) ) == -1 )
        //	    printf("fcntl err: unable to get flags\n");

        //	if( fcntl( stdinFD, F_SETFL, flags | O_NONBLOCK ) == -1)
        //	    printf("fcntl err: unable to set O_NONBLOCK\n");

        //	vusleep( 50000 ); // sleep 50ms

        retval = read( stdinFD, &keystroke, 1 );
        if ( retval < 1 )
        {
            // didn't get any keystrokes
            deviceMutex.unlock();
            return ( 0 );
        }

        // we should have a keystroke at this point.
        if ( keystroke != '\n' ) // if it's a newline, ignore it
        {
            cpDebug(LOG_DEBUG, "Keystroke read: %c", keystroke );
        }

        switch ( keystroke )
        {
            case 'q':
            {
                // shutdown
                event->type = DeviceEventShutdown;
                cpLog( LOG_NOTICE , "User requested shutdown" );
                killTone();
            }
            break;

            case 'a':
            {
                hookStateOffhook = true;
                playDialTone = true;
                event->type = DeviceEventHookUp;
            }
            break;

            case 'z':
            {
                // onhook
                if ( hookStateOffhook )  // this avoids multiple
                {                        // onhook events
                    hookStateOffhook = false;
                    event->type = DeviceEventHookDown;
                    killTone();
                }
            }
            break;

            case 'f':
            {
                event->type = DeviceEventFlash;
            }
            break;

            case '*':
            {
                event->type = DeviceEventDtmfStar;
                killTone();
            }
            break;

            case '#':
            {
                event->type = DeviceEventDtmfHash;
                killTone();
            }
            break;

            case '0':
            {
                event->type = DeviceEventDtmf0;
                killTone();
            }
            break;

            case '1':
            {
                event->type = DeviceEventDtmf1;
                killTone();
            }
            break;

            case '2':
            {
                event->type = DeviceEventDtmf2;
                killTone();
            }
            break;

            case '3':
            {
                event->type = DeviceEventDtmf3;
                killTone();
            }
            break;

            case '4':
            {
                event->type = DeviceEventDtmf4;
                killTone();
            }
            break;

            case '5':
            {
                event->type = DeviceEventDtmf5;
                killTone();
            }
            break;

            case '6':
            {
                event->type = DeviceEventDtmf6;
                killTone();
            }
            break;

            case '7':
            {
                event->type = DeviceEventDtmf7;
                killTone();
            }
            break;

            case '8':
            {
                event->type = DeviceEventDtmf8;
                killTone();
            }
            break;

            case '9':
            {
                event->type = DeviceEventDtmf9;
                killTone();
            }
            break;

            case '\n':
            {
                // ignore
            }

            break;

            default:
            {
                // no events
                cpLog( LOG_ERR, "Unknown keyboard input = %d", keystroke );
            }
            break;
        } // end switch

        // now send the event
        if ( event->type != DeviceEventUndefined )
        {
            assert( sessionQ != 0 );
            event->callId = callId;
            sessionQ->add( event );
        }
    }
    deviceMutex.unlock();
    return 0;
} // end PhoneCardDevice::process()


//***************************************************************************
// PhoneCardDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************

void
PhoneCardDevice::processRTP ()
{

    deviceMutex.lock();
    if ( audioStack == 0 )
    {
        vusleep( 10000 );
        deviceMutex.unlock();
        return ;
    }

    //if ( hookStateOffhook == false ) {vusleep(10000);  return;}

    RtpSessionState sessionState = audioStack->getSessionState();

    //only process receive if receive is enabled
    if ( sessionState == rtp_session_recvonly ||
            sessionState == rtp_session_sendrecv )
    {

        inRtpPkt = audioStack->receive();
        if ( inRtpPkt )
        {
            //cerr << "receiving" << endl;
            //vusleep(500);
            //if (ioctl(myFD,IXJCTL_HOOKSTATE)==0)  return;
            if ( inRtpPkt->getPayloadUsage() != RESID_RTP_RATE )
                cpDebug(LOG_DEBUG, "RtpStack API size doesn't match RESID_RTP_RATE" );
            write( myFD, inRtpPkt->getPayloadLoc(), RESID_RTP_RATE );

            // need to delete since RTP stack doesn't do it any more
            delete inRtpPkt; inRtpPkt = NULL;
        }
    }
    if ( sessionState == rtp_session_sendonly ||
            sessionState == rtp_session_sendrecv )
    {
        int cc;
        //if (ioctl(myFD,IXJCTL_HOOKSTATE)==0)  return;
        {
            if ( sendRingback )
                cc = getRingbackTone( outBufferPkt, RESID_RTP_RATE );
            else
                cc = read( myFD, outBufferPkt, RESID_RTP_RATE );
        }
        if ( audioStack )
        {
            audioStack->transmitRaw( outBufferPkt, cc );
        }
    }

    if ( audioStack && sessionState != rtp_session_inactive )
    {
        audioStack->processRTCP();
    }
    deviceMutex.unlock();
}

int
PhoneCardDevice::addToFdSet( fd_set* fd )
{
    if ( audioActive )
    {
        RtpSessionState sessionState = audioStack->getSessionState();
        if ( sessionState == rtp_session_recvonly ||
                sessionState == rtp_session_sendrecv )
        {
            FD_SET( ( audioStack->getRtpRecv() )->getSocketFD(), fd );
            FD_SET( ( audioStack->getRtcpRecv() )->getSocketFD(), fd );
        }
    }

    FD_SET( myFD, fd );

    return 0;
} // end PhoneCardDevice::addToFdSet()

//***************************************************************************
// PhoneCardDevice::getRtpPort()
//
// description:  creates a new rtp session and reserves a rtp port
//***************************************************************************
int
PhoneCardDevice::getRtpPort()
{
    //this is an arbitrarily defined number
    const int MAX_RTP_PORT = 2 << 16-1;
    const int MIN_RTP_PORT = 10000;

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
// PhoneCardDevice::releaseRtpPort()
//
// description: destroy the rtp session and release the rtp port
//***************************************************************************
void
PhoneCardDevice::releaseRtpPort()
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
//***************************************************************************


//***************************************************************************
// PhoneCardDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet.  ioctl calls to initialize
//               the quicknet card.
//***************************************************************************

int
PhoneCardDevice::audioStart( const HardwareAudioRequest& request )
{
    deviceMutex.lock();

    cerr << "%%%   Establishing audio   %%%\n";
    cerr << "%%%   Listening on port: " << request.localPort << "\n";
    cerr << "%%%   Sending to host: " << request.remoteHost << "\n";
    cerr << "%%%   Sending to port: " << request.remotePort << "\n";
    cerr << "%%%   RTP packet size: " << request.rtpPacketSize << " ms\n";

    // TBD, once stable, change asserts to checks which return errorcodes

    // create new audioStack for this audio session
    // 0 is rtpPayloadPCUM
    // last paramter, -1, disables jitter buffer

    if ( audioStack == 0 )
    {
        int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
        int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
        cerr << "%%%   remote rtcp port : " << remoteRtcpPort << "\n";
        cerr << "%%%   local rtcp port  : " << localRtcpPort << "\n\n";
        const char* remoteHost = 0;
        if ( request.remotePort != 0 )
            remoteHost = request.remoteHost;
        audioStack = new RtpSession( remoteHost, request.remotePort,
                                     request.localPort, remoteRtcpPort,
                                     localRtcpPort, rtpPayloadPCMU,
                                     rtpPayloadPCMU, 5 );

        //if ( request.remotePort != 0 ) {
        ioctl( myFD, IXJCTL_REC_CODEC, ULAW );
        ioctl( myFD, IXJCTL_REC_START );
        //}
        //if ( request.localPort != 0 ) {
        ioctl( myFD, IXJCTL_PLAY_CODEC, ULAW );
        ioctl( myFD, IXJCTL_PLAY_START );
        //}
        ioctl( myFD, IXJCTL_AEC_START, 1 );
#if 0
        // Echo Cancellation doesn't work with VM.
        if (request.echoCancellation)
        {
            ioctl(myFD, IXJCTL_AEC_START, 1);
            cerr << "echoCancellation is on" << endl;
        }
        else
        {
            cpDebug(LOG_DEBUG, "echoCancellation is off");
            ioctl(myFD, IXJCTL_AEC_START, 0);
        }

        ioctl(myFD, IXJCTL_MIXER, 0x0C03);   // 06 is too soft
        ioctl(myFD, IXJCTL_MIXER, 0x0F01);   // 02 is too soft
#endif
    }
    else
    {
        cerr << "%%%% we are turning half duplex into full duplex" << endl;
        //need to determine what mode RtpSession is in
        RtpSessionState sessionState = audioStack->getSessionState();
        switch ( sessionState )
        {
            case rtp_session_sendonly:
            //turn on the receive
            if ( request.localPort != 0 )
            {
                cerr << "%%%%turning on receive" << endl;
                cerr << "%%%%local host " << request.localPort << endl;
                cerr << "%%%%local port " << request.localPort << endl;
                audioStack->setSessionState( rtp_session_sendrecv );
                audioStack->setReceiver( request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, 5);
            }
            //restart the quicknet card
            ioctl(myFD, IXJCTL_REC_STOP);
            ioctl(myFD, IXJCTL_PLAY_STOP);
            ioctl(myFD, IXJCTL_REC_CODEC, ULAW);
            ioctl(myFD, IXJCTL_REC_START);
            ioctl(myFD, IXJCTL_PLAY_CODEC, ULAW);
            ioctl(myFD, IXJCTL_PLAY_START);
            break;
            case rtp_session_recvonly:
            //turn on the send
            cerr << "%%%%turning on the send" << endl;
            cerr << "%%%%remote host " << request.remoteHost << endl;
            cerr << "%%%%remote port " << request.remotePort << endl;
            if ( request.remotePort != 0 )
            {
                audioStack->setSessionState(rtp_session_sendrecv);
                audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
            }
            //restart the quicknet card
            ioctl(myFD, IXJCTL_REC_STOP);
            ioctl(myFD, IXJCTL_PLAY_STOP);
            ioctl(myFD, IXJCTL_REC_CODEC, ULAW);
            ioctl(myFD, IXJCTL_REC_START);
            ioctl(myFD, IXJCTL_PLAY_CODEC, ULAW);
            ioctl(myFD, IXJCTL_PLAY_START);
            break;
            case rtp_session_sendrecv:
            return 0;
            break;
            default:
            audioStack->setSessionState( rtp_session_sendrecv );
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
    else if ( sessionState == rtp_session_sendrecv )
    {
        audioActive = true;
        audioHalfActive = false;
    }
    else
    {
        audioActive = false;
        audioActive = true;
    }

    inRtpPkt = 0;

    audioStack->setApiFormat( rtpPayloadPCMU, RESID_RTP_RATE );
    audioStack->setNetworkFormat( rtpPayloadPCMU, request.rtpPacketSize * 8 );


    //    hasPlayed = false;

#if 0
    // Echo Cancellation doesn't work with VM.
    if (request.echoCancellation)
    {
        ioctl(myFD, IXJCTL_AEC_START, 1);
        cerr << "echoCancellation is on" << endl;
    }
    else
    {
        cpDebug(LOG_DEBUG, "echoCancellation is off");
        ioctl(myFD, IXJCTL_AEC_START, 0);
    }

    ioctl(myFD, IXJCTL_MIXER, 0x0C03);   // 06 is too soft
    ioctl(myFD, IXJCTL_MIXER, 0x0F01);   // 02 is too soft
#endif
    deviceMutex.unlock();
    return 0;
} // end PhoneCardDevice::audioStart()

//***************************************************************************
// PhoneCardDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  audioStack, rtp packets.
//***************************************************************************

int
PhoneCardDevice::audioStop()
{
    cpDebug(LOG_DEBUG, "***   Stopping audio   ***" );

    if ( !audioActive )
        return 1;

    // mark audio as deactivated.
    audioActive = false;
    audioHalfActive = false;
    //    hasPlayed = false;

    vusleep( 200 );

    // make hardware calls to stop audio
    deviceMutex.lock();
    ioctl( myFD, IXJCTL_REC_STOP );
    ioctl( myFD, IXJCTL_PLAY_STOP );
    ioctl( myFD, IXJCTL_AEC_STOP );

    // TBD, need to close and reopen device to stablize ipjack
    // hardware, hopefully this can be cleaned up with new driver
#if 0
    close( myFD );
    myFD = open( myDeviceName, O_RDWR );
    if ( myFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot reopen %s", myDeviceName );
        exit( 1 );
    }
#endif


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

    if ( inRtpPkt )
    {
        delete inRtpPkt;
        inRtpPkt = 0;
    }

    if ( audioStack )
    {
        delete audioStack;
        audioStack = 0;
    }
    deviceMutex.unlock();

    return 0;
} // end PhoneCardDevice::audioStop()

//***************************************************************************
// PhoneCardDevice::audioSuspend
//
// description: suspend the RTP transmit and receive, and also stop the
//
//***************************************************************************

int
PhoneCardDevice::audioSuspend ()
{
    deviceMutex.lock();

    cpDebug(LOG_INFO, "Suspending audio" );
    cpDebug(LOG_INFO, "Setting all RTP/RTCP ports to 0" );


    if ( audioStack != 0 )
    {
        //set rtp session to inactive
        audioStack->setSessionState( rtp_session_inactive );
        audioStack->setReceiver( 0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, 5 );
        audioStack->setTransmiter( 0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU );
    }

    ioctl( myFD, IXJCTL_REC_STOP );
    ioctl( myFD, IXJCTL_PLAY_STOP );
    ioctl( myFD, IXJCTL_AEC_STOP );

    deviceMutex.unlock();
    return 0;
} // end PhoneCardDevice::audioSuspend()

//***************************************************************************
// PhoneCardDevice::audioResume
//
// description:  resume the rtp session using the new sdp information
//               enable the quicknet card audio
//***************************************************************************

int
PhoneCardDevice::audioResume( const HardwareAudioRequest& request )
{
    deviceMutex.lock();

    if ( audioStack == 0 )
    {
        cpLog( LOG_ERR, "Try to resume an existing audio channel" );
        cpLog( LOG_ERR, "No existing audio channel" );
        deviceMutex.unlock();
        return 0;
    }

    cpDebug(LOG_INFO, "Resuming audio" );
    cpDebug(LOG_INFO, "Listening on port: %d", request.localPort );
    cpDebug(LOG_INFO, "Sending to host: %s", request.remoteHost );
    cpDebug(LOG_INFO, "Sending to port: %d", request.remotePort );
    cpDebug(LOG_INFO, "RTP packet size: %d", request.rtpPacketSize );

    //resuming audio channel
    //int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
    //int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
    const char* remoteHost = 0;
    if ( request.remotePort != 0 )
        remoteHost = request.remoteHost;

    if ( request.localPort != 0 && request.remotePort != 0 )
    {
        //full duplex audio
        audioStack->setSessionState(rtp_session_sendrecv);
        audioStack->setReceiver(request.localPort, request.localPort + 1, 0,
                                rtpPayloadPCMU, rtpPayloadPCMU, 5);
        audioStack->setTransmiter(request.remoteHost, request.remotePort,
                                  request.remotePort + 1, rtpPayloadPCMU,
                                  rtpPayloadPCMU);
        ioctl( myFD, IXJCTL_PLAY_CODEC, ULAW );
        ioctl( myFD, IXJCTL_PLAY_START );
        ioctl( myFD, IXJCTL_REC_CODEC, ULAW );
        ioctl( myFD, IXJCTL_REC_START );
    }
    else if ( request.localPort != 0 )
    {
        //receive only
        audioStack->setSessionState(rtp_session_sendrecv);
        audioStack->setReceiver(request.localPort, request.localPort + 1, 0,
                                rtpPayloadPCMU, rtpPayloadPCMU, 5);
        ioctl( myFD, IXJCTL_PLAY_CODEC, ULAW );
        ioctl( myFD, IXJCTL_PLAY_START );
    }
    else if ( request.remotePort != 0 )
    {
        //transmit only
        audioStack->setSessionState( rtp_session_sendrecv );
        audioStack->setTransmiter( request.remoteHost, request.remotePort,
                                   request.remotePort + 1, rtpPayloadPCMU,
                                   rtpPayloadPCMU );
        ioctl( myFD, IXJCTL_REC_CODEC, ULAW );
        ioctl( myFD, IXJCTL_REC_START );
    }

#if 0
    // Echo Cancellation doesn't work with VM.
    if (request.echoCancellation)
    {
        ioctl(myFD, IXJCTL_AEC_START, 1);
        cerr << "echoCancellation is on" << endl;
    }
    else
    {
        cpDebug(LOG_DEBUG, "echoCancellation is off");
        ioctl(myFD, IXJCTL_AEC_START, 0);
    }

    ioctl(myFD, IXJCTL_MIXER, 0x0C03);   // 06 is too soft
    ioctl(myFD, IXJCTL_MIXER, 0x0F01);   // 02 is too soft
#endif

    if ( request.sendRingback )
        startSendRingback();
    else
        stopSendRingback();

    deviceMutex.unlock();
    return 0;
} // end PhoneCardDevice::audioResume()

//***************************************************************************
// PhoneCardDevice::provideTone
//
// description:  quicknet does not provide all the tone that are needed.
//               this function will provide the tone for quicknet by playing
//               a sound file of that particular tone.
//***************************************************************************

void
PhoneCardDevice::provideTone( PhoneCardTone tone )
{
    int size;
    int keypress = 0;
    char buff1[ULAW_SAMPLE_RATE];
    int toneFD;
    bool provideTone = true;

    switch ( tone )
    {
        case PhoneCardDialTone:
        {
            cpDebug(LOG_DEBUG, "***   Provide dial tone ***" );
            toneFD = open( "Tone/dialtone", O_RDONLY );
        }
        break;

        case PhoneCardRingbackTone:
        {
            cpDebug(LOG_DEBUG, "***   Provide ringback tone ***" );
            toneFD = open( "Tone/ringback", O_RDONLY );
        }
        break;

        default:
        {
            cpLog( LOG_ERR, "unsupported tone requested" );
            ;
            toneFD = -1;
        }
        break;

    } // end switch

    if ( toneFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot open tone file" );
        ;
        provideTone = false;
    }
    else
    {
        bool redo = true;

        while ( redo )
        {

            redo = false;

            close( myFD );

            vusleep( 10000 );


            myFD = open( myDeviceName, O_WRONLY );
            if ( myFD < 0 )
            {
                cpLog( LOG_ERR, "Cannot open device %s", myDeviceName );
                ;
                provideTone = false;
            }
            else
            {
                struct timeval start;      // ctam
                struct timeval finish;     // ctam
                gettimeofday( &start, 0 );    // ctam

                deviceMutex.lock();
                ioctl( myFD, IXJCTL_PLAY_CODEC, ULAW );
                ioctl( myFD, IXJCTL_PLAY_START );
                deviceMutex.unlock();

                gettimeofday( &finish, 0 );   // ctam

                long diff = finish.tv_usec -
                            start.tv_usec +
                            1000000 * (finish.tv_sec - start.tv_sec);

                if ( diff > 100000 )
                    redo = true;
            }
        }
    }

    // play until file ends or onhook or dtmf is ready
    while ( provideTone )
    {
        size = read( toneFD, buff1, ULAW_SAMPLE_RATE );
        fflush( stdout );
        if ( size > 0 )
        {
            deviceMutex.lock();
            // play some tone
            write( myFD, buff1, size );

            // now, we only need to watch for a key press
            keypress = cin.peek();

            if ( keypress != 0 ) // something in the keyboard buffer
            {
                provideTone = false;
            }

            if ( myQ->size() > 0 )
            {
                // need to stop tone and process endpoint msg
                provideTone = false;
            }
            deviceMutex.unlock();
        }
        else
        {
            // end of file
            provideTone = false;
        }
    } // end while provideTone

    // stop playing and close file
    deviceMutex.lock();
    ioctl( myFD, IXJCTL_AEC_STOP );
    ioctl( myFD, IXJCTL_PLAY_STOP );
    deviceMutex.unlock();
    close( myFD );

    myFD = open( myDeviceName, O_RDWR );

    if ( myFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot re-open device %s", myDeviceName );
        ;
        exit( 1 );
    }
    close( toneFD );
} // end provideTone()


// ****************
// ****************
int
PhoneCardDevice::getRingbackTone( char *ringData, int size )
{

    struct timeval start;      // ctam
    struct timeval finish;     // ctam
    gettimeofday( &start, 0 );    // ctam

    int cc = 0;

    char tone[RESID_RTP_RATE];

    if ( ringbackFd == -1 )
    {
        cpDebug(LOG_INFO, "opening ringback tone to provide remote ringback" );
        ringbackFd = open( "Tone/ringback", O_RDONLY );
    }

    if ( ringbackFd == -1 )
    {
        cpLog( LOG_ERR, "can not open Tone/ringback" );
        return cc;
    }

    cc = read( ringbackFd, tone, RESID_RTP_RATE );
    fflush( stdout );
#if 1
    while ( cc < size )
    {
        lseek( ringbackFd, 0, SEEK_SET );
        cc += read( ringbackFd, tone + cc, size - cc );
    }
#endif
    memcpy( ringData, tone, size );

    long diff = 0;
    while ( diff < 29500 )
    {
        if ( diff < 20000 ) vusleep( 1000 );
        gettimeofday( &finish, 0 );   // ctam
        diff = finish.tv_usec - start.tv_usec + 1000000 * (finish.tv_sec - start.tv_sec);
    }

    return cc;
}

//***************************************************************************
// PhoneCardDevice::onhookOrFlash
//
// THIS FEATURE IS TURNED OFF IN THE PHONECARDDEVICE UNTIL QUICKNET
// RELEASES THE SMARTCABLE.
// description:  quicknet does not implement a flash function on their cards.
//               so, we have to implement our own, by checking the hook state
//               of the phone.  this function will tell us whether the phone
//               is being flashed or being placed onhook.
//***************************************************************************

void
PhoneCardDevice::onhookOrFlash()
{
    cpDebug(LOG_DEBUG, "We know this isn't a flash!" );
}


// turn it off for now ----------------------------------------------------
#if 0
void
PhoneCardDevice::onhookOrFlash()
{
    deviceMutex.lock();
    vusleep(20000);  // needed to stabilize hookstate readings

    if (!ioctl(myFD, IXJCTL_HOOKSTATE)) // onhook
    {
        // user is onhook or doing a flash
        hookStateOffhook = false;
        // need to wait FLASH_TIMEOUT microsec before reporting onhook
        // if user picks up before the timeout, report flash
        int flashTime = 0;

        /*debug*/
        cpDebug(LOG_DEBUG,  "Starting Flash Timer");
        while (flashTime < FLASH_TIMEOUT) // begin testing for flash event
        {
            vusleep(50000);  // sleep for 50,000 microsec (50 ms)
            flashTime += 50000;
            if (ioctl(myFD, IXJCTL_HOOKSTATE)) // user is offhook = flash!
            {
                flashTime = FLASH_TIMEOUT;  // will get us out of while-loop
                /*debug*/
                cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventFlash");
                hookStateOffhook = true;
                reportEvent(sessionQ, DeviceEventFlash);
            }
        } // end period for testing for a flash event
        /*debug*/       cpDebug(LOG_DEBUG,  "End Flash Timer");

        if (hookStateOffhook == false)
            // this must be an actual onhook event
        {
            /*debug*/   cpDebug(LOG_DEBUG,  "Hardware Event: deviceEventHookDown");
            reportEvent(sessionQ, DeviceEventHookDown);
        }
    }
    deviceMutex.unlock();
} // end onhookOrFlash
#endif
// -------------------------------------------------------------------------

// ****************
// ****************

void
PhoneCardDevice::provideDialToneStart()
{
    playDialTone = true;
    cpDebug(LOG_DEBUG, "***  Start dial tone  ***" );
    deviceMutex.lock();
    // this ioctl works when using the phonecard
    ioctl( myFD, IXJCTL_DIALTONE );
    deviceMutex.unlock();
}

void
PhoneCardDevice::provideDialToneStop()
{
    playDialTone = false;
    cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
    deviceMutex.lock();
    // this ioctl works when using the phonecard
    ioctl( myFD, IXJCTL_CPT_STOP );
    deviceMutex.unlock();
}

void
PhoneCardDevice::provideRingStart()
{
    cpDebug(LOG_DEBUG, "***  Start Ringing  ***" );
    cout << "PHONE IS RINGING..." << endl;
    //    provideTone( PhoneCardRingbackTone );
}


void
PhoneCardDevice::provideRingStop()
{
    cpDebug(LOG_DEBUG, "***  Stop Ringing  ***" );
    cout << "PHONE HAS STOPPED RINGING..." << endl;
}

void
PhoneCardDevice::provideLocalRingbackStart()
{
    cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
    // the PhoneCARD does not support ringback.
    // so we need a wav file, but for now:
    cout << "START LOCAL RINGBACK" << endl;
    //    provideTone( PhoneCardRingbackTone );
}
;

void
PhoneCardDevice::provideLocalRingbackStop()
{
    cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
    cout << "STOP LOCAL RINGBACK" << endl;
};

void
PhoneCardDevice::provideBusyToneStart()
{
    cpDebug(LOG_DEBUG, "***  Start busy tone  ***" );
    deviceMutex.lock();
    ioctl( myFD, IXJCTL_BUSY );
    deviceMutex.unlock();
};

void
PhoneCardDevice::provideBusyToneStop()
{
    cpDebug(LOG_DEBUG, "***  Stop busy tone  ***" );
    deviceMutex.lock();
    ioctl(myFD, IXJCTL_CPT_STOP);
    deviceMutex.unlock();
};

void
PhoneCardDevice::provideFastBusyToneStart()
{
    cpDebug(LOG_DEBUG, "***  Start fast busy tone  ***" );
    deviceMutex.lock();
    //for now using regular busy tone
    ioctl( myFD, IXJCTL_BUSY );
    deviceMutex.unlock();
};

void
PhoneCardDevice::provideFastBusyToneStop()
{
    cpDebug(LOG_DEBUG, "***  Stop fast busy tone  ***" );
    deviceMutex.lock();
    ioctl( myFD, IXJCTL_CPT_STOP );
    deviceMutex.unlock();
};

void
PhoneCardDevice::provideDtmf( DeviceSignalType signal )
{
    char digit = ' ';
    if ( signal >= DeviceSignalDigit0 && signal <= DeviceSignalDigit9 )
    {
        digit = '0' + signal - DeviceSignalDigit0;
    }
    else if ( signal == DeviceSignalDigitHash )
    {
        digit = '#';
    }
    else if ( signal == DeviceSignalDigitStar )
    {
        digit = '*';
    }
    cpDebug(LOG_DEBUG, "***  DTMF %c  ***", digit );
};


void
PhoneCardDevice::provideCallInfo(string, string, string)
{}
;

void
PhoneCardDevice::killTone()
{
    if ( playDialTone == true )
    {
        cpDebug(LOG_DEBUG, "killing tone on phonecard device" );
        ioctl( myFD, IXJCTL_CPT_STOP );
        playDialTone = false;
    }
};

void
PhoneCardDevice::provideCallWaitingBeepStart()
{
    // Not yet implemented by phonecard device
    cpDebug(LOG_DEBUG, "in provideCallWaitingBeepStart()" );
};

void
PhoneCardDevice::provideCallWaitingBeepStop()
{
    // Not yet implemented by phonecard device
    cpDebug(LOG_DEBUG, "in provideCallWaitingBeepStop()" );
};
