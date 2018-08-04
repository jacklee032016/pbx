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

static const char* const NullHwDevice_cxx_Version =
    "$Id: NullHwDevice.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "global.h"
#include <cassert>

/* stdlib */
#include <iostream>
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

/* error handling */
#include <cerrno>

#include "VTime.hxx"
#include "VCondition.h"
#include "NullHwDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaConfiguration.hxx"

#define ULAW_PAYLOAD 240
#define ULAW_SAMPLE_RATE 240
#define RESID_RTP_RATE 240
#define NETWORK_RTP_RATE 160

using namespace Vocal;


//***************************************************************************
// NullHwDevice::NullHwDevice
// description:  Constructor.
//***************************************************************************

NullHwDevice::NullHwDevice( char* deviceName,
                            Sptr < Fifo < Sptr < SipProxyEvent > > > inputQ,
                            Sptr < Fifo < Sptr < SipProxyEvent > > > outputQ )
        : ResGwDevice( deviceName, inputQ, outputQ ),
        audioActive( false ),
        hasPlayed( false )
{
    // initialize queues
    sessionQ = outputQ;
    myQ = inputQ;

    // store the device name.
    myDeviceName = deviceName;

#ifndef WIN32
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
#endif
}  // end NullHwDevice::NullHwDevice()


//***************************************************************************
// QuickNetDevice::~QuickNetDevice
// description:  Destructor
//***************************************************************************


NullHwDevice::~NullHwDevice()
{
#ifndef WIN32
    close(myFD);
    tcsetattr( stdinFD, TCSANOW, &initialTerm );
#endif
} // end NullHwDevice::~NullHwDevice()


//***************************************************************************
// NullHwDevice::hardwareMain
// description:  main processing loop of the hardware
//***************************************************************************

void*
NullHwDevice::hardwareMain (void* parms)
{
#if !defined(WIN32)
    // process forever on behalf of NullHwDevice hardware
    fd_set readfds;
    struct timeval tv;
    int retval;
    // fd_set isn't implemented quite right!
    int maxFd = 8;  // should be 0 then increment as needed when addToFdSet

    // reset file descriptor
    FD_ZERO(&readfds);
    addToFdSet( &readfds );
    tv.tv_sec = 0;
    tv.tv_usec = 300;

    if ((retval = select(maxFd, &readfds, 0, 0, &tv)) < 0)
    {
        cpLog( LOG_ERR, "select() returned error %d", retval );
    }
    else if (retval > 0)
    {
        if (process(&readfds) < 0)
        {
            cpLog( LOG_ERR, "hardware encountered an error" );
            assert(0);
        }
    }
#else
    HANDLE handles [1];
    handles[0] = GetStdHandle(STD_INPUT_HANDLE);

    DWORD dwd = WaitForMultipleObjects(1, handles, false, 300); 

    fd_set* ptr = NULL;
    if (dwd == WAIT_OBJECT_0)
    {
        ptr = (fd_set*)&dwd;
    }

    if (process(ptr) < 0)
    {
        cerr << "hardware encountered an error\n";
        assert(0);
    }
#endif

    // check for signal requests from the session
    if ( myQ->size() > 0 )
    {
        processSessionMsg( myQ->getNext() );
    };

    return 0;
} // end NullHwDevice::hardwareMain()



//***************************************************************************
// NullHwDevice::process
//
// description:  process any events detected on the hardware (dtmf digit
//               press, onhook & offhook, etc.) and reports the events back
//               to the session via the fifo queue.
//***************************************************************************

int
NullHwDevice::process (fd_set* fd)
{
    vusleep(0);  // needed to stabilize hookstate readings

    Sptr < UaDeviceEvent > event = new UaDeviceEvent( sessionQ ) ;
    assert( event != 0 );

#if !defined(WIN32)
    char keystroke = '\0';

    read( stdinFD, &keystroke, 1 );

#else
    char keystroke = '\n'; // ignore
    INPUT_RECORD ir;
    unsigned long cNumRead;
    if (fd) 
    {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	ReadConsoleInput( hStdin, &ir, 1, &cNumRead);

	if (!ir.Event.KeyEvent.bKeyDown || ir.EventType != KEY_EVENT )
	{
	    return 0;
	}
	
	keystroke = ir.Event.KeyEvent.uChar.AsciiChar;
    }
#endif

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
                    // Emulate a 'z' in TelephoneUI
                    hookStateOffhook = false;
                    event->type = DeviceEventHookDown;
                }
                break;
            }
        }
    }
    else
    {
        // process keystroke
        if( keystroke != '\n' )
        {
            cpDebug(LOG_DEBUG, "Keystroke read: '%c'(%x)",
                              ( keystroke >= ' ' && keystroke <= '~' ) ? keystroke : '.',
                              keystroke );
        }

        switch( keystroke )
        {
            case 'q':
            {
                // shutdown
                event->type = DeviceEventShutdown;
                cpLog( LOG_NOTICE , "User requested shutdown" );
            }
            break;

            case 'a':  // offhook
            {
                hookStateOffhook = true;
                event->type = DeviceEventHookUp;
            }
            break;

            case 'z':  // onhook
            {
                if( hookStateOffhook )
                {
                    hookStateOffhook = false;
                    event->type = DeviceEventHookDown;
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
            }
            break;

            case '#':
            {
                event->type = DeviceEventDtmfHash;
            }
            break;

            case '0':
            {
                event->type = DeviceEventDtmf0;
            }
            break;

            case '1':
            {
                event->type = DeviceEventDtmf1;
            }
            break;

            case '2':
            {
                event->type = DeviceEventDtmf2;
            }
            break;

            case '3':
            {
                event->type = DeviceEventDtmf3;
            }
            break;

            case '4':
            {
                event->type = DeviceEventDtmf4;
            }
            break;

            case '5':
            {
                event->type = DeviceEventDtmf5;
            }
            break;

            case '6':
            {
                event->type = DeviceEventDtmf6;
            }
            break;

            case '7':
            {
                event->type = DeviceEventDtmf7;
            }
            break;

            case '8':
            {
                event->type = DeviceEventDtmf8;
            }
            break;

            case '9':
            {
                event->type = DeviceEventDtmf9;
            }
            break;

            case 'u':
            {
                if( hookStateOffhook )
                {
                    cout << "Enter URL: ";
                    cout.flush();
                    myEntryState = EntryStateEnterUrl;
                }
                else
                {
                    cpLog( LOG_ERR, "Enter 'a' first" );
                }
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
                cpLog( LOG_ERR, "Unknown keyboard input" );
                break;
            }
        } // end switch
    }

    if (event->type != DeviceEventUndefined)
    {
        assert( sessionQ != 0 );
        event->callId = callId;
#ifndef WIN32
        sessionQ->add( event );
#else
        Sptr <SipProxyEvent> proxyEvent;
        proxyEvent.dynamicCast( event );
        sessionQ->add( proxyEvent );
#endif
    }
    return 0;
} // end NullHwDevice::process()

//***************************************************************************
// NullHwDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************


void
NullHwDevice::processRTP ()
{
    #if !defined(WIN32)
    if (audioStack != 0)
    {
        //send rtp packets with silence
        memset(outBufferPkt, 0x7f, 160);
        audioStack->transmitRaw(outBufferPkt, 160);
    }
    #endif
    vusleep(20000);  // sleep for 20 milliseconds
}


int
NullHwDevice::addToFdSet (fd_set* fd)
{
    #ifndef WIN32
    FD_SET( stdinFD, fd );
    #endif

    return 0;
} // end NullHwDevice::addToFdSet()

//***************************************************************************
// NullHwDevice::getRtpPort()
//
// description:  randomly generates a rtp port
//***************************************************************************


int
NullHwDevice::getRtpPort()
{
    //this is an arbitrarily defined number
    const int MAX_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
    const int MIN_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

    int port = random();
    port = port % ( MAX_RTP_PORT - MIN_RTP_PORT ) + MIN_RTP_PORT;

    cpDebug(LOG_DEBUG, "rtp port %d is reserved", port);
    return port;
}

//***************************************************************************
// NullHwDevice::releaseRtpPort()
//
// description: for NullHwDevice, nothing needs to be done
//***************************************************************************


void
NullHwDevice::releaseRtpPort()
{
}


//***************************************************************************
// NullHwDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet.  ioctl calls to initialize
//               the quicknet card.
//***************************************************************************


int
NullHwDevice::audioStart (const HardwareAudioRequest& request)
{
    cerr << "%%%   Establishing audio   %%%\n";
    cerr << "%%%   Listening on port: " << request.localPort << "\n";
    cerr << "%%%   Sending to host: " << request.remoteHost << "\n";
    cerr << "%%%   Sending to port: " << request.remotePort << "\n";

    //if generating rtp packets, create outgoing packet and start session
    //(without RTCP).  Assume PCMU codec.
    if ( UaConfiguration::instance()->getRtpGenOn() )
    {
        #if !defined(WIN32)
        if (audioStack != 0)
            delete audioStack;

        audioStack = new RtpSession(request.remoteHost,
                                    request.remotePort,
                                    request.localPort);

        audioStack->setApiFormat (rtpPayloadPCMU, RESID_RTP_RATE);
        audioStack->setNetworkFormat (rtpPayloadPCMU, request.rtpPacketSize * 8);

        // 	audioStack->setNetworkPktSize( request.rtpPacketSize * 8);
        //	outRtpPkt = audioStack->createPacket( 0 );
        #endif
    }

    // mark audio as active

    audioActive = true;
    audioHalfActive = true;
    hasPlayed = false;

    return 0;
} // end NullHwDevice::audioStart()


//***************************************************************************
// NullHwDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  inBuffer, outBuffer, audioStack,
//               rtp packets.
//***************************************************************************


int
NullHwDevice::audioStop()
{
    cout << "%%%   Stopping audio\n";

    if (!audioActive) return 1;

    // mark audio as deactivated.
    audioActive = false;
    audioHalfActive = true;
    hasPlayed = false;

    #if !defined(WIN32)
    //reset rtp session and outgoing packet
    delete audioStack;
    audioStack = 0;
    #endif

    //delete outRtpPkt;
    //outRtpPkt = NULL;

    return 0;
} // end NullHwDevice::audioStop()


//***************************************************************************
// NullHwDevice::audioSuspend
//
// description: suspend the RTP transmit and receive, and also stop the
//
//***************************************************************************


int
NullHwDevice::audioSuspend ()
{

#if 0
    cpDebug(LOG_INFO, "Suspending audio");
    cpDebug(LOG_INFO, "Setting all RTP/RTCP ports to 0");
#endif
return 0;
}


//***************************************************************************
// NullHwDevice ::audioResume
//
// description:  resume the rtp session using the new sdp information
//               enable the quicknet card audio
//***************************************************************************


int
NullHwDevice::audioResume (const HardwareAudioRequest& request)
{
#if 0
    if ( audioStack == 0 )
    {
        cpLog( LOG_ERR, "Try to resume an existing audio channel");
        cpLog( LOG_ERR, "No existing audio channel");
        return 0;
    }

    cpDebug(LOG_INFO, "Resuming audio");
    cpDebug(LOG_INFO, "Listening on port: %d", request.localPort);
    cpDebug(LOG_INFO, "Sending to host: %s", request.remoteHost);
    cpDebug(LOG_INFO, "Sending to port: %d", request.remotePort);
    cpDebug(LOG_INFO, "RTP packet size: %d", request.rtpPacketSize);

    return 0;
#endif
return 0;
}


void
NullHwDevice::provideDialToneStart()
{
    cout << "%%%   Start dial tone\n";
}


void
NullHwDevice::provideDialToneStop()
{
    cout << "%%%   Stop dial tones\n";
}


void
NullHwDevice::provideRingStart()
{
    cout << "%%%   Start ringing\n";
}


void
NullHwDevice::provideRingStop()
{
    cout << "%%%   Stop ringing\n";
}


void
NullHwDevice::provideLocalRingbackStart()
{
    cout << "%%%   Start local ringback\n";
}


void
NullHwDevice::provideLocalRingbackStop()
{
    cout << "%%%   Stop local ringback\n";
}


void
NullHwDevice::provideBusyToneStart()
{
    cout << "%%%   Start busy start\n";
}


void
NullHwDevice::provideBusyToneStop()
{
    cout << "%%%   Stop busy stop\n";
}


void
NullHwDevice::provideFastBusyToneStart()
{
    cout << "%%%   Start fast busy start\n";
}


void
NullHwDevice::provideFastBusyToneStop()
{
    cout << "%%%   Stop fast busy stop\n";
}


void
NullHwDevice::provideCallWaitingBeepStart()
{
    cout << "%%% Start call waiting beep" << endl;
}


void
NullHwDevice::provideCallWaitingBeepStop()
{
    cout << "%%% Stop call waiting beep" << endl;
}


