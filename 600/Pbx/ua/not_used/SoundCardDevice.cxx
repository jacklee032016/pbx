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

#include "global.h"

/* stdlib */
#include <iostream>
#include <fstream>
#include <cstdio>
#include <unistd.h>
#include <sys/ioctl.h>
#include <csignal>

#ifdef WIN32
#include <io.h>
#endif

/* sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef WIN32
#include <sys/soundcard.h>
#endif

#include "VTime.hxx"
#include "Rtp.hxx"
#include "codec/g711.h"
#include "VCondition.h"
#include "SoundCardDevice.hxx"
#include "UaDeviceEvent.hxx"
#include "UaConfiguration.hxx"


using namespace Vocal;


#define RESID_RTP_RATE 160      // 320 byte packet size
#define NETWORK_RTP_RATE 160    // 160 byte packet size
#define FLASH_TIMEOUT 1000000

extern "C"
{
    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );
}

//***************************************************************************
// SoundCardDevice::SoundCardDevice
// description:  Constructor.  opens the PhoneCARD device.  initialize the
//               hookstate.  resets the device to get it ready.
//***************************************************************************

void SoundCardDevice::testSoundFile()
{
    cpLog(LOG_ERR,"Start test");
    int cc = 0;
    int len;
    int inFile_fd;
    unsigned char audio_buffer[480];
    inFile_fd = open( "ULAW", O_RDONLY, 0 );
    reopenAudioHardware();
    while( ( len = read( inFile_fd, audio_buffer, 160 ) ) != -1 )
    {
        cc += len;
        if( cc > 20000 ) break;
        write( myFD, (char*)audio_buffer, 160 );
    }
#ifndef WIN32
    if( ioctl( myFD, SNDCTL_DSP_SYNC, 0 ) == -1 )
    {
        perror("SNDCTL_DSP_SYNC");
    }
#endif

    cpLog(LOG_ERR,"Stop test");
}


SoundCardDevice::SoundCardDevice( const char* deviceName,
                                  Sptr < Fifo < Sptr<SipProxyEvent> > > inputQ,
                                  Sptr < Fifo < Sptr<SipProxyEvent> > > outputQ )
    : ResGwDevice( deviceName, inputQ, outputQ ),
      audioStack( 0 ),
      inRtpPkt( 0 ),
      ringbackFd( -1 )
{
    // initialize queues
    sessionQ = outputQ;
    myQ = inputQ;

    // initialize states
    hookStateOffhook = false;
    playDialTone = false;
    sendRingback = false;

    // open audio hardware device
    if( ( myFD = openAudioHardware( deviceName ) ) == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", deviceName );
        exit( -1 );
    }

    // store the device name
    myDeviceName = deviceName;

#ifndef WIN32
    // enable single keypresses without pressing <enter> in terminal
    stdinFD = fileno( stdin );
    tcgetattr( stdinFD, &initialTerm );
    struct termios newTerm = initialTerm;
    newTerm.c_lflag &= ~ICANON;
    newTerm.c_cc[VMIN] = 0;
    newTerm.c_cc[VTIME] = 0;
    if( tcsetattr( stdinFD, TCSANOW, &newTerm ) != 0 )
    {
        cpLog( LOG_ERR, "Unable to set new terminal mode" );
        exit( -1 );
    }
#endif
} // end SoundCardDevice::SoundCardDevice()



//***************************************************************************
// SoundCardDevice::~SoundCardDevice
// description:  Destructor
//***************************************************************************

SoundCardDevice::~SoundCardDevice()
{
    closeAudioHardware();
#ifndef WIN32
    tcsetattr( stdinFD, TCSANOW, &initialTerm );
#endif
} // end SoundCardDevice::~SoundCardDevice()


//***************************************************************************
// SoundCardDevice::openAudioHardware
// description:  open audio hardware device
//***************************************************************************

int SoundCardDevice::openAudioHardware( const char* pname )
{
    deviceMutex.lock();

    cpDebug(LOG_DEBUG, "Opening audio hardware" );

    int audioFD;

    // open audio device
    assert(pname);
#ifndef WIN32
    if( ( audioFD = open( pname, O_RDWR, 0 ) ) == -1 )
    {
        perror("Open failed:");
		deviceMutex.unlock();
        return -1;
    }
#else
    if (waveInGetNumDevs() <= 0) 
    {
        cpDebug(LOG_DEBUG, "could not get any devices");
        deviceMutex.unlock();
        return -1;
    }
    if (!m_aSoundCardWinOut.Open(SoundOut)) 
    {
        perror("Open openAudioHardware failed (not found input sound card device) : ");
        deviceMutex.unlock();
        return -1;
    }
    if (!m_aSoundCardWinIn.Open(SoundIn)) 
    {
#ifndef _DEBUG
        //AND:Need for run 2 exemplars program on one computer (debug version)
        m_aSoundCardWinOut.Close();
        perror("Open openAudioHardware failed (not found output sound card device) : ");
        deviceMutex.unlock();
        return -1;
#endif
    }
    audioFD = 1;
#endif

#ifndef WIN32
    // query audio formats
    int ioctlParam = -1;
    if( ioctl( audioFD, SNDCTL_DSP_GETFMTS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_GETFMTS");
        deviceMutex.unlock();
        return -1;
    }

    // choose codec
    if( ioctlParam & AFMT_MU_LAW )
    {
        //cpDebug(LOG_DEBUG,"Setting sound card to ulaw");
        ioctlParam = AFMT_MU_LAW;
        apiCodec = rtpPayloadPCMU;
    }
    else if( ioctlParam & AFMT_S16_LE )
    {
        //cpDebug(LOG_DEBUG,"Setting sound card to linear16");
        ioctlParam = AFMT_S16_LE;
        apiCodec = rtpPayloadL16_mono;
    }
    else
    {
        cpLog(LOG_ERR,"Sound card doesn't support any known encodings");
        printf("Sound card supports the following (Mask:%d:)\n", ioctlParam);
        if( ioctlParam & AFMT_MU_LAW )     printf("  AFMT_MU_LAW\n");
        if( ioctlParam & AFMT_A_LAW )      printf("  AFMT_A_LAW\n");
        if( ioctlParam & AFMT_IMA_ADPCM )  printf("  AFMT_IMA_ADPCM\n");
        if( ioctlParam & AFMT_U8 )         printf("  AFMT_U8\n");
        if( ioctlParam & AFMT_S16_LE )     printf("  AFMT_S16_LE\n");
        if( ioctlParam & AFMT_S16_BE )     printf("  AFMT_S16_BE\n");
        if( ioctlParam & AFMT_S8 )         printf("  AFMT_S8\n");
        if( ioctlParam & AFMT_U16_LE )     printf("  AFMT_U16_LE\n");
        if( ioctlParam & AFMT_U16_BE )     printf("  AFMT_U16_BE\n");
        if( ioctlParam & AFMT_MPEG )       printf("  AFMT_MPEG\n");
        deviceMutex.unlock();
        return -1;
    }

    // setting audio device parameters
    //ioctlParam = AFMT_MU_LAW;  //test setting to ulaw
    int ioctlParamSave = ioctlParam;
    if( ioctl( audioFD, SNDCTL_DSP_SETFMT, &ioctlParam ) == -1 )
    {   
        perror("SNDCTL_DSP_SETFMT");
        deviceMutex.unlock();
        return -1;
    }
    if( ioctlParam != ioctlParamSave )
    {   
        perror("Failed to set DSP Format, sound card returned format");
        deviceMutex.unlock();
        return -1;
    }

    ioctlParam = 1;

    // set the number of channels to 1.  if this is an error, it
    // should probably not be a major failure.

    if( ioctl( audioFD, SNDCTL_DSP_CHANNELS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_CHANNELS");
        deviceMutex.unlock();
        return -1;
    }

    if( ioctlParam != 1)
    {
        cout << "warning:  unable to set audio output to mono,"
             << " may cause problems later\n";
    }

    ioctlParam = 8000;
    if( ioctl( audioFD, SNDCTL_DSP_SPEED, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_SPEED");
        deviceMutex.unlock();
        return -1;
    }
    if( ioctlParam != 8000 )
    {
        perror("Failed to set sample rate");
        deviceMutex.unlock();
        return -1;
    }
#else
    apiCodec = rtpPayloadL16_mono;
#endif
    deviceMutex.unlock();

    return audioFD;
}



//***************************************************************************
// SoundCardDevice::closeAudioHardware
// description:
//***************************************************************************

int SoundCardDevice::closeAudioHardware()
{
    deviceMutex.lock();

    cpDebug(LOG_DEBUG, "Closing audio hardware" );

#ifndef WIN32
    if( ioctl( myFD, SNDCTL_DSP_RESET, 0 ) == -1 ) 
    {
        perror("SNDCTL_DSP_RESET");
    }
#else
    m_aSoundCardWinOut.Close();
    m_aSoundCardWinIn.Close();
#endif

    deviceMutex.unlock();
#ifndef WIN32
    close( myFD );
#endif

    return 1;
}



//***************************************************************************
// SoundCardDevice::reopenAudioHardware
// description:
//***************************************************************************

int SoundCardDevice::reopenAudioHardware( )
{
    // close audio device
    if( closeAudioHardware() == -1 )
    {
        cpLog(LOG_ERR, "Fail to close %s during reopen", myDeviceName);
        return -1;
    }

    // open audio device
    if( ( myFD = openAudioHardware( myDeviceName ) ) == -1 )
    {
        cpLog(LOG_ERR, "Fail to open %s during reopen", myDeviceName);
        return -1;
    }

    return 1;
}



//***************************************************************************
// SoundCardDevice::writeToSoundCard
// description:
//***************************************************************************

void SoundCardDevice::writeToSoundCard ( const unsigned char* data,
                                         const int samples )
{
    char* dspData = 0;
    int cc = 0;
    int p = 0;
 
    // check codec encodings
    switch( apiCodec )
    {
    case rtpPayloadPCMU:
        // no conversion needed
        cc = samples;
        dspData = (char*)data;
        break;
        
    case rtpPayloadL16_mono:
        // convert sound sample from ULAW to Linear16
        cc = samples;
        for(p = 0; p < cc; p++ )
            longdataBuffer[p]  = ulaw2linear( data[p] );
        cc *= 2;
        dspData = (char*)longdataBuffer;
        break;

    default:
        cpLog(LOG_ERR,"Failed to provide correct codec encoding");
        return;
    }
#ifndef WIN32
    write( myFD, dspData, cc );
#else
    m_aSoundCardWinOut.Write(dspData, cc);
#endif
    return;
}



int SoundCardDevice::readFromSoundCard( unsigned char* data,
                                        const int samples )
{
    int cc = -1;
    int p = 0;

    //check codec encodings
    switch( apiCodec )
    {
#ifndef WIN32
    case rtpPayloadPCMU:
        // no conversion needed
        cc = 0;
        cc = read( myFD, data, samples );
        break;
#endif
    case rtpPayloadL16_mono:
        // convert sound sample from Liner16 to ULAW
#ifndef WIN32
        cc = read( myFD, (char*)longdataBuffer, samples * 2 );
#else
        cc = m_aSoundCardWinIn.Read( (char*)longdataBuffer, samples * 2 );
#endif
        for(; p < cc; p++ )
        {
            data[p]  = linear2ulaw( longdataBuffer[p] );
        }

        cc /= 2;
        break;

    default:
        cpDebug(LOG_DEBUG,"Failed to provide correct encoding");
        break;
    }
#ifndef WIN32
    if( samples != cc )
    {
        cpDebug(LOG_DEBUG,"Incomplete read from Sound Card (%d)", cc);
    }
#else
    // AND:prevent - many garbage on screen (when run 2 programs and
    // input from sound card already use one from him) :-/

    if( m_aSoundCardWinIn.IsOpen() && samples != cc )
    {
        cpLog(LOG_ERR,"Incomplete read from Sound Card (%d)", cc);
    }
#endif
    return cc;
}


//***************************************************************************
// SoundCardDevice::hardwareMain
// description:  main processing loop of the hardware
//***************************************************************************

void*
SoundCardDevice::hardwareMain( void* parms )
{
#ifndef WIN32
    // process forever on behalf of SoundCardDevice hardware
    fd_set readfds;
    struct timeval tv;
    int retval;
    int maxFd = 128;  // should be 0 then increment as needed when addToFdSet

    // reset file descriptor
    FD_ZERO( &readfds );
    addToFdSet( &readfds );

    // block on select for asyncronous events, but poll to process
    // audio and signal requests from endpoints in message queue
    tv.tv_sec = 0;
    tv.tv_usec = 300;

    if( ( retval = select( maxFd, &readfds, 0, 0, &tv ) ) < 0 )
    {
        cpLog( LOG_ERR, "select() returned error %d", retval );
    }
    else
    {
        if( process( &readfds ) < 0 )
        {
            cpLog( LOG_ERR, "Hardware encountered an error" );
            assert( 0 );
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
        cerr << "Hardware encountered an error\n";
        assert(0);
    }
#endif

    // check for signal requests from the session
    if( myQ->size() > 0 )
    {
        processSessionMsg( myQ->getNext() );
    }

    return 0;
} // end SoundCardDevice::hardwareMain()



//***************************************************************************
// SoundCardDevice::process
//
// description:  process any events detected on the hardware as well as
//               simulated device events triggered by keyboard input.
//               report these events back to the session via the fifo queue.
//***************************************************************************

int
SoundCardDevice::process( fd_set* fd )
{
    if (!fd)
    {
        return 0;
    }

    deviceMutex.lock();

    // check for events coming from the keyboard
    // initialize the event we will send
    Sptr<UaDeviceEvent> event = new UaDeviceEvent( sessionQ );
    assert( event != 0 );
    event->type = DeviceEventUndefined;

#ifndef WIN32
    char keystroke = '\0';

    if( read( stdinFD, &keystroke, 1 ) < 1 )
    {
        // didn't get any keystrokes
        deviceMutex.unlock();
        return 0;
    }
#else
    char keystroke = '\n'; // ignore
    INPUT_RECORD ir;
    unsigned long cNumRead;
    if (fd) 
    {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        ReadConsoleInput( hStdin, &ir, 1, &cNumRead);
        
        if (!ir.Event.KeyEvent.bKeyDown
            || ir.Event.KeyEvent.uChar.AsciiChar == 23 /* 0x17 */ 
            || ir.Event.KeyEvent.uChar.AsciiChar == '\n'
            || ir.Event.KeyEvent.uChar.AsciiChar == 0x0a
            || ir.Event.KeyEvent.uChar.AsciiChar == 0) 
        {
            deviceMutex.unlock();
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
        case 'q':  // shutdown
            event->type = DeviceEventShutdown;
            cpLog( LOG_NOTICE , "User requested shutdown" );
            break;
            
        case 'a':  // offhook
            hookStateOffhook = true;
            playDialTone = true;
            event->type = DeviceEventHookUp;
            break;

        case 'z':  // onhook
            if ( hookStateOffhook )
            {
                hookStateOffhook = false;
                event->type = DeviceEventHookDown;
            }
            break;

        case 'f':  // flash
            event->type = DeviceEventFlash;
            break;

        case '*':
            event->type = DeviceEventDtmfStar;
            break;

        case '#':
            event->type = DeviceEventDtmfHash;
            break;

        case '0':
            event->type = DeviceEventDtmf0;
            break;

        case '1':
            event->type = DeviceEventDtmf1;
            break;

        case '2':
            event->type = DeviceEventDtmf2;
            break;

        case '3':
            event->type = DeviceEventDtmf3;
            break;

        case '4':
            event->type = DeviceEventDtmf4;
            break;

        case '5':
            event->type = DeviceEventDtmf5;
            break;

        case '6':
            event->type = DeviceEventDtmf6;
            break;

        case '7':
            event->type = DeviceEventDtmf7;
            break;

        case '8':
            event->type = DeviceEventDtmf8;
            break;

        case '9':
            event->type = DeviceEventDtmf9;
            break;

        case 'u':
            if( hookStateOffhook )
            {
                cout << "\nEnter URL: ";
                cout.flush();
                myEntryState = EntryStateEnterUrl;
            }
            else
            {
                cpLog( LOG_ERR, "Enter 'a' first" );
            }
            break;

        case '\n':  // ignore
            break;

        default:  // no events
            cpLog( LOG_ERR, "Unknown keyboard input" );
            break;
        }
    }

    // send the event
    if( event->type != DeviceEventUndefined )
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
    
    deviceMutex.unlock();
    return 0;
} // end SoundCardDevice::process()



//***************************************************************************
// SoundCardDevice::processRTP
// description:  main processing loop for RTP
//***************************************************************************

void
SoundCardDevice::processRTP ()
{
    if( audioStack == 0 )
    {
        vusleep( 1000 );
        return;
    }

    deviceMutex.lock();

    //AND:BUGFIX:?
    if (audioStack == 0) 
    {
        deviceMutex.unlock();
        vusleep( 1000 );
        return;
    }

    bool bNothingDo = true;

    RtpSessionState sessionState = audioStack->getSessionState();

    if ( sessionState == rtp_session_undefined ) 
    {
        deviceMutex.unlock();
        vusleep( 1000 );
        return;
    }

    // receive audio
    if( sessionState == rtp_session_recvonly ||
        sessionState == rtp_session_sendrecv )
    {
        inRtpPkt = audioStack->receive();
        if( inRtpPkt )
        {
            if( inRtpPkt->getPayloadType() != rtpPayloadPCMU ||
                inRtpPkt->getPayloadUsage() != NETWORK_RTP_RATE )
            {
                cpLog(LOG_ERR,"Received from RTP stack incorrect payload type");
            }
            writeToSoundCard( (unsigned char*) inRtpPkt->getPayloadLoc(),
                              inRtpPkt->getPayloadUsage() );
            bNothingDo = false;

            // need to delete since RTP stack doesn't do it any more
            delete inRtpPkt; 
            inRtpPkt = NULL;
        }
    }

    // transmit audio
    if( sessionState == rtp_session_sendonly ||
        sessionState == rtp_session_sendrecv )
    {
        int cc;
        if( audioStack->getRtcpTran() )
        {
            if( sendRingback )
            {
                cc = getRingbackTone( dataBuffer, RESID_RTP_RATE );
#ifdef WIN32
                Sleep(15);
#endif
            }
            else 
            {
                cc = readFromSoundCard( dataBuffer, RESID_RTP_RATE );
            }
            if ((cc > 0) && audioStack) 
            {
                audioStack->transmitRaw( (char*)dataBuffer, cc );
                bNothingDo = false;
            }
            else
            {
                cpDebug(LOG_DEBUG, "failed to read data from sound card");
            }
        }
    }
#ifdef WIN32
    if (bNothingDo) 
    { //AND: Prevent processor from 100% load
        Sleep(2);
    }
#endif
    // process RTCP
    //if( audioStack && sessionState != rtp_session_inactive )
    //{
    //    audioStack->processRTCP();
    //}

    deviceMutex.unlock();

    return;
}


int
SoundCardDevice::addToFdSet( fd_set* fd )
{
    if( ( audioActive || audioHalfActive ) && audioStack )
    {
        RtpSessionState sessionState = audioStack->getSessionState();
        if( sessionState == rtp_session_recvonly ||
            sessionState == rtp_session_sendrecv )
        {
            FD_SET( ( audioStack->getRtpRecv() )->getSocketFD(), fd );
//            FD_SET( ( audioStack->getRtcpRecv() )->getSocketFD(), fd );
        }
    }

    FD_SET( myFD, fd );

    return 0;
} // end SoundCardDevice::addToFdSet()



//***************************************************************************
// SoundCardDevice::getRtpPort()
//
// description:  creates a new rtp session and reserves a rtp port
//***************************************************************************

int
SoundCardDevice::getRtpPort()
{
    deviceMutex.lock();

    //this is an arbitrarily defined number
    const int MAX_RTP_PORT = UaConfiguration::instance()->getMaxRtpPort();
    const int MIN_RTP_PORT = UaConfiguration::instance()->getMinRtpPort();

    int port = 0;
    int minPort = MIN_RTP_PORT;

    // create a rtp session if there is no existing session alread
    // this Rtp session will be idle
    if( audioStack == 0 )
    {
        audioStack = new RtpSession(0);
    }

    while( 1 )
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
// SoundCardDevice::releaseRtpPort()
//
// description: destroy the rtp session and release the rtp port
//***************************************************************************

void
SoundCardDevice::releaseRtpPort()
{
    deviceMutex.lock();

    // destroy the rtp session if one exists
    if ( audioStack != 0 )
    {
        //AND:BUGFIX: swap 2x2 bottom 4 lines

        // (or releaseRtcpPort() try use _rtp_ stack when his already
        // destroy releaseRtpPort) may be Unix to be able use class
        // after his ~destroy, but WIN32 close thread :)

        int port = audioStack->releaseRtcpPort();
        cpDebug(LOG_DEBUG, "rtcp port %d released", port );
        port = audioStack->releaseRtpPort();
        cpDebug(LOG_DEBUG, "rtp port %d released", port );
        delete audioStack;
        audioStack = 0;
    }

    deviceMutex.unlock();
}


//***************************************************************************
// SoundCardDevice::audioStart
//
// description:  creates a new rtp session and also allocates memory for
//               incoming and outgoing rtp packet.  ioctl calls to initialize
//               the quicknet card.
//***************************************************************************

int
SoundCardDevice::audioStart( const HardwareAudioRequest& request )
{
    deviceMutex.lock();

    cerr << "%%% Establishing soundcard audio" << endl;
    cerr << "%%%   Listening on port: " << request.localPort << endl;
    cerr << "%%%   Sending to host  : " << request.remoteHost << endl;
    cerr << "%%%   Sending to port  : " << request.remotePort << endl;
    cerr << "%%%   RTP packet size  : " << request.rtpPacketSize << " ms" << endl;

    // create new audioStack for this audio session
    // 0 is rtpPayloadPCUM
    // last paramter, -1, disables jitter buffer
    if( audioStack == 0 )
    {
        int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
        int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
        cerr << "%%%   Remote rtcp port : " << remoteRtcpPort << "\n";
        cerr << "%%%   Local rtcp port  : " << localRtcpPort << "\n\n";
        const char* remoteHost = 0;
        if ( request.remotePort != 0 )
            remoteHost = request.remoteHost;
        audioStack = new RtpSession( remoteHost, request.remotePort,
                                     request.localPort, remoteRtcpPort,
                                     localRtcpPort, rtpPayloadPCMU,
                                     rtpPayloadPCMU, 0 );
    }
    else
    {
        cerr << "%%% Turn half duplex into full duplex" << endl;

        //need to determine what mode RtpSession is in
        RtpSessionState sessionState = audioStack->getSessionState();
        switch ( sessionState )
        {
        case rtp_session_sendonly:  //turn on the receive
            cerr << "%%% Turn on receive" << endl;
            cerr << "%%%   local port " << request.localPort << endl;
            if ( request.localPort != 0 )
            {
                audioStack->setSessionState( rtp_session_sendrecv );
                audioStack->setReceiver( request.localPort, request.localPort + 1, 0, rtpPayloadPCMU, rtpPayloadPCMU, 0);

            }
            break;

        case rtp_session_recvonly:  //turn on the send
            cerr << "%%% Turn on send" << endl;
            cerr << "%%%   remote host " << request.remoteHost << endl;
            cerr << "%%%   remote port " << request.remotePort << endl;
            if ( request.remotePort != 0 )
            {
                audioStack->setSessionState(rtp_session_sendrecv);
                audioStack->setTransmiter(request.remoteHost, request.remotePort, request.remotePort + 1, rtpPayloadPCMU, rtpPayloadPCMU);
            }

            break;

        case rtp_session_sendrecv:
             cpDebug(LOG_DEBUG, "full duplex already");
            deviceMutex.unlock();
            return 0;
            break;

        default:
            if ( request.localPort != 0 )
            {
                cpDebug(LOG_DEBUG, "Turning on RTP receive" );
                audioStack->setSessionState( rtp_session_recvonly );
                audioStack->setReceiver( request.localPort,
                                         request.localPort + 1,
                                         0,
                                         rtpPayloadPCMU,
                                         rtpPayloadPCMU,
                                         0 );
            }
            if ( request.remotePort != 0 )
            {
                cpDebug(LOG_DEBUG, "Turning on RTP transmit" );
                if( request.localPort != 0 )
                {
                    audioStack->setSessionState( rtp_session_sendrecv );
                }
                else
                {
                    audioStack->setSessionState( rtp_session_sendonly );
                }
                audioStack->setTransmiter( request.remoteHost,
                                           request.remotePort,
                                           request.remotePort + 1,
                                           rtpPayloadPCMU,
                                           rtpPayloadPCMU );
            }
            break;
        }
    }

    if( request.sendRingback )
        startSendRingback();
    else
        stopSendRingback();


    RtpSessionState sessionState = audioStack->getSessionState();
    if( sessionState == rtp_session_sendonly ||
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

    audioStack->setApiFormat( rtpPayloadPCMU, request.rtpPacketSize*8 );
    audioStack->setNetworkFormat( rtpPayloadPCMU, request.rtpPacketSize*8 );

    deviceMutex.unlock();
    reopenAudioHardware();

    return 0;
} // end SoundCardDevice::audioStart()



//***************************************************************************
// SoundCardDevice::audioStop
//
// description:  tears down audio.  cleans up by delete objects created when
//               audioStart was called...  audioStack, rtp packets.
//***************************************************************************

int
SoundCardDevice::audioStop()
{
    if( !audioActive )
    {
        return 1;
    }

    cerr << "%%% Stopping audio" << endl;

    // mark audio as deactivated.
    audioActive = false;
    audioHalfActive = false;

    vusleep( 200 );
#ifdef WIN32
    deviceMutex.lock(); //AND: or thread must die !!!
#endif
    // should lock here, but seg faults application when unlocking below - kle

    // close RTP session
    if( audioStack )
    {
        RtpSessionState sessionState = audioStack->getSessionState();
        if( sessionState == rtp_session_sendonly ||
            sessionState == rtp_session_sendrecv )
        {
            audioStack->transmitRTCPBYE();
        }
    }

    if( inRtpPkt )
    {
        delete inRtpPkt;
        inRtpPkt = 0;
    }

    if( audioStack )
    {
        delete audioStack;
        audioStack = 0;
    }
#ifdef WIN32
    deviceMutex.unlock();
#endif
    reopenAudioHardware();

    return 0;
} // end SoundCardDevice::audioStop()



//***************************************************************************
// SoundCardDevice::audioSuspend
//
// description: suspend the RTP transmit and receive, and also stop the
//
//***************************************************************************

int
SoundCardDevice::audioSuspend ()
{
    deviceMutex.lock();

    cerr << "%%% Suspending audio" << endl;
    cerr << "Setting all RTP / RTCP ports to 0" << endl; 

    if ( audioStack != 0 )
    {
        //set rtp session to inactive
        audioStack->setSessionState( rtp_session_inactive );
        audioStack->setReceiver( 0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU, 0 );
        audioStack->setTransmiter( 0, 0, 0, rtpPayloadPCMU, rtpPayloadPCMU );
    }

    deviceMutex.unlock();
    reopenAudioHardware();

    return 0;
} // end SoundCardDevice::audioSuspend()



//***************************************************************************
// SoundCardDevice::audioResume
//
// description:  resume the rtp session using the new sdp information
//               enable the quicknet card audio
//***************************************************************************

int
SoundCardDevice::audioResume( const HardwareAudioRequest& request )
{
    deviceMutex.lock();

    if ( audioStack == 0 )
    {
        cpLog( LOG_ERR, "Try to resume an existing audio channel" );
        cpLog( LOG_ERR, "No existing audio channel" );
        deviceMutex.unlock();
        return 0;
    }

    cerr << "Resuming audio" << endl;
    cerr << "Listening on port: " << request.localPort << endl;
    cerr << "Sending to host: " << request.remoteHost << endl;
    cerr << "Sending to port: " << request.remotePort << endl;
    cerr << "RTP packet size: " << request.rtpPacketSize << endl;

    //resuming audio channel
    //int remoteRtcpPort = (request.remotePort > 0) ? request.remotePort + 1 : 0;
    //int localRtcpPort = (request.localPort > 0) ? request.localPort + 1 : 0;
    const char* remoteHost = 0;
    if( request.remotePort != 0 )
        remoteHost = request.remoteHost;

    if( request.localPort != 0 && request.remotePort != 0 )
    {
        //full duplex audio
        audioStack->setSessionState(rtp_session_sendrecv);
        audioStack->setReceiver(request.localPort, request.localPort + 1, 0,
                                rtpPayloadPCMU, rtpPayloadPCMU, 0);
        audioStack->setTransmiter(request.remoteHost, request.remotePort,
                                  request.remotePort + 1, rtpPayloadPCMU,
                                  rtpPayloadPCMU);
    }
    else if ( request.localPort != 0 )
    {
        //receive only
        audioStack->setSessionState(rtp_session_sendrecv);
        audioStack->setReceiver(request.localPort, request.localPort + 1, 0,
                                rtpPayloadPCMU, rtpPayloadPCMU, 0);
    }
    else if ( request.remotePort != 0 )
    {
        //transmit only
        audioStack->setSessionState( rtp_session_sendrecv );
        audioStack->setTransmiter( request.remoteHost, request.remotePort,
                                   request.remotePort + 1, rtpPayloadPCMU,
                                   rtpPayloadPCMU );
    }

    audioStack->setApiFormat( rtpPayloadPCMU, request.rtpPacketSize*8 );
    audioStack->setNetworkFormat( rtpPayloadPCMU, request.rtpPacketSize*8 );

    if( request.sendRingback )
        startSendRingback();
    else
        stopSendRingback();

    deviceMutex.unlock();
    reopenAudioHardware();

    return 0;
} // end SoundCardDevice::audioResume()



//***************************************************************************
// SoundCardDevice::provideTone
//
// description:  provide the tone for the soundcard by playing
//               a sound file of that particular tone.
//***************************************************************************

void
SoundCardDevice::provideTone( const ToneEmulation tone )
{
    // open ULAW encoded sound file
    int toneFD;
    switch( tone )
    {
    case DialToneEmulation:
        cpDebug(LOG_DEBUG, "*** Provide dial tone emulation ***" );
        toneFD = open( "Tone/dialtone", O_RDONLY );
        break;

    case RingbackToneEmulation:
        cpDebug(LOG_DEBUG, "*** Provide ringback tone emulation ***" );
        toneFD = open( "Tone/ringback", O_RDONLY );
        break;

    default:
        cpLog( LOG_ERR, "Unsupported tone requested" );
        toneFD = -1;
        break;
    }

    if( toneFD < 0 )
    {
        cpLog( LOG_ERR, "Cannot open tone file" );
        return;
    }


    // play until file ends or hardware event
    reopenAudioHardware();
    bool provideToneLoop = true;
    while( provideToneLoop )
    {
        int cc = read( toneFD, dataBuffer, NETWORK_RTP_RATE );
        if( cc != NETWORK_RTP_RATE )
        {
            // end of file reached
            provideToneLoop = false;
        }

        deviceMutex.lock();

        // play tone sample
        writeToSoundCard( dataBuffer, NETWORK_RTP_RATE );

#ifndef WIN32
        // watch for a key press on stdinFD
        fd_set readFdSet;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        FD_ZERO( &readFdSet );
        FD_SET(stdinFD, &readFdSet );
        select( stdinFD+1, &readFdSet, 0, 0, &tv );
        if( FD_ISSET( stdinFD, &readFdSet ) )
        {
            // something in the keyboard buffer
            provideToneLoop = false;
        }
#else
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD        inputRecord;
        DWORD        nRecords;
        if (PeekConsoleInput( hStdin, &inputRecord, 1, &nRecords)
            && nRecords)
        {
            if (inputRecord.Event.KeyEvent.bKeyDown) 
            {
                // something in the keyboard buffer
                provideToneLoop = false;
            }
        }
#endif        
        if( myQ->size() > 0 )
        {
            // need to stop tone and process endpoint msg
            provideToneLoop = false;
        }

        deviceMutex.unlock();
    }

    reopenAudioHardware();
    close( toneFD );
} // end provideTone()


int
SoundCardDevice::getRingbackTone( unsigned char *ringData, const int samples )
{
    // open tone file if first time
    if( ringbackFd == -1 )
    {
        cpDebug(LOG_DEBUG, "Opening remote ringback tone file" );
        if( ( ringbackFd = open( "Tone/ringback", O_RDONLY ) ) == -1 )
        {
            cpLog( LOG_ERR, "Fail to open Tone/ringback" );
            return 0;
        }
    }

    // read from tone file
    int cc = read( ringbackFd, (char*)ringData, samples );
    if( cc != samples )
    {
        //AND:may be need close file for reopen at next read() ?
        close(ringbackFd);
        ringbackFd = -1;
        // end of file reached
        return cc;
    }

    return cc;
}



//***************************************************************************
// SoundCardDevice::onhookOrFlash
//
// THIS FEATURE IS TURNED OFF IN THE PHONECARDDEVICE UNTIL QUICKNET
// RELEASES THE SMARTCABLE.
// description:  quicknet does not implement a flash function on their cards.
//               so, we have to implement our own, by checking the hook state
//               of the phone.  this function will tell us whether the phone
//               is being flashed or being placed onhook.
//***************************************************************************


void
SoundCardDevice::onhookOrFlash()
{
    cpDebug(LOG_DEBUG, "We know this isn't a flash!" );
}


void
SoundCardDevice::provideDialToneStart()
{
    cpDebug(LOG_DEBUG, "***  Start dial tone  ***" );
    provideTone( DialToneEmulation );
}


void
SoundCardDevice::provideDialToneStop()
{
    cpDebug(LOG_DEBUG, "***  Stop dial tone  ***" );
    if( playDialTone == true )
    {
        playDialTone = false;
        reopenAudioHardware();
    }
}


void
SoundCardDevice::provideRingStart()
{
    cpDebug(LOG_DEBUG, "***  Start Ringing  ***" );
    cout << "Incoming call..." << endl << endl;
    //TODO play some ringing sound to speaker?
    //provideTone( RingbackToneEmulation );
}


void
SoundCardDevice::provideRingStop()
{
    cpDebug(LOG_DEBUG, "***  Stop Ringing  ***" );
    reopenAudioHardware();
}


void
SoundCardDevice::provideLocalRingbackStart()
{
    cpDebug(LOG_DEBUG, "***  Start local ringback  ***" );
    provideTone( RingbackToneEmulation );
}


void
SoundCardDevice::provideLocalRingbackStop()
{
    cpDebug(LOG_DEBUG, "***  Stop local ringback  ***" );
    reopenAudioHardware();
}


void
SoundCardDevice::provideBusyToneStart()
{
    cpDebug(LOG_DEBUG, "***  Start busy tone  ***" );
}


void
SoundCardDevice::provideBusyToneStop()
{
    cpDebug(LOG_DEBUG, "***  Stop busy tone  ***" );
}


void
SoundCardDevice::provideFastBusyToneStart()
{
    cpDebug(LOG_DEBUG, "***  Start fast busy tone  ***" );
}


void
SoundCardDevice::provideFastBusyToneStop()
{
    cpDebug(LOG_DEBUG, "***  Stop fast busy tone  ***" );
}


void
SoundCardDevice::provideDtmf( DeviceSignalType signal )
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
}


void
SoundCardDevice::provideCallInfo(string, string, string)
{
}


#if 0
void
SoundCardDevice::killDialTone()
{
    if( playDialTone == true )
    {
        playDialTone = false;
        cpDebug(LOG_DEBUG, "*** Stopping tone emulation ***" );
#ifndef WIN32  //AND:Construction fall on WIN32
        deviceMutex.unlock();
        reopenAudioHardware();
        deviceMutex.unlock();
#endif
    }
}
#endif


void
SoundCardDevice::provideCallWaitingBeepStart()
{
    // Not yet implemented
    cpDebug(LOG_DEBUG, "in provideCallWaitingBeepStart() - not completed" );
}


void
SoundCardDevice::provideCallWaitingBeepStop()
{
    // Not yet implemented
    cpDebug(LOG_DEBUG, "in provideCallWaitingBeepStop() - not completed" );
}

