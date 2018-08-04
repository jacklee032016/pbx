#include <iostream>
#include <fstream>
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

#ifndef WIN32
#include <sys/soundcard.h>
#endif

#include "NewSoundCard.hxx"
#include "cpLog.h"
#include <cassert>
#include "Lock.hxx"

using Vocal::Threads::Lock;

extern "C"
{
    unsigned char linear2ulaw( int pcm_val );
    int ulaw2linear( unsigned char u_val );
}


NewSoundCard::NewSoundCard( const char* deviceName )
    : myFD(-1),
      myDeviceName(deviceName),
      myFormat(NewSoundCardInvalid),
      myReadBuffer(2048),
      myWriteBuffer(2048)
{
    assert(deviceName);
}


int NewSoundCard::reopen()
{
    int retval;
    retval = close();
    retval = retval | open();
    return retval;
}

int 
NewSoundCard::open()
{
    deviceMutex.lock();

    cpDebug(LOG_DEBUG, "Opening audio hardware" );

    // open audio device
    LocalScopeAllocator lo;

#ifndef WIN32
    if( ( myFD = ::open( myDeviceName.getData(lo), O_RDWR, 0 ) ) == -1 )
    {
        perror("Open failed:");
	deviceMutex.unlock();
	myFD = -1;
	return -1;
    }
#else
    if (waveInGetNumDevs() <= 0) 
    {
        cpDebug(LOG_DEBUG, "could not get any devices");
        deviceMutex.unlock();
	myFD = -1;
	return -1;
    }
    if (!m_aSoundCardWinOut.Open(SoundOut)) 
    {
        perror("Open openAudioHardware failed (not found input sound card device) : ");
        deviceMutex.unlock();
	myFD = -1;
	return -1;
    }
    if (!m_aSoundCardWinIn.Open(SoundIn)) 
    {
#ifndef _DEBUG
        //AND:Need for run 2 exemplars program on one computer (debug version)
        m_aSoundCardWinOut.Close();
        perror("Open openAudioHardware failed (not found output sound card device) : ");
        deviceMutex.unlock();
	myFD = -1;
	return -1;
#endif
    }
    myFD = 1;
#endif

#ifndef WIN32
    // query audio formats
    int ioctlParam = -1;
    if( ioctl( myFD, SNDCTL_DSP_GETFMTS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_GETFMTS");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }

    // choose codec
    if( ioctlParam & AFMT_MU_LAW )
    {
        //cpDebug(LOG_DEBUG,"Setting sound card to ulaw");
        ioctlParam = AFMT_MU_LAW;
	myFormat = NewSoundCardUlaw;

    }
    else if( ioctlParam & AFMT_S16_LE )
    {
        //cpDebug(LOG_DEBUG,"Setting sound card to linear16");
        ioctlParam = AFMT_S16_LE;
	myFormat = NewSoundCardSigned16LE;
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
	myFD = -1;
        return -1;
    }

    // setting audio device parameters
    //ioctlParam = AFMT_MU_LAW;  //test setting to ulaw
    int ioctlParamSave = ioctlParam;
    if( ioctl( myFD, SNDCTL_DSP_SETFMT, &ioctlParam ) == -1 )
    {   
        perror("SNDCTL_DSP_SETFMT");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }
    if( ioctlParam != ioctlParamSave )
    {   
        perror("Failed to set DSP Format, sound card returned format");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }

    ioctlParam = 1;

    // set the number of channels to 1.  if this is an error, it
    // should probably not be a major failure.

    if( ioctl( myFD, SNDCTL_DSP_CHANNELS, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_CHANNELS");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }

    if( ioctlParam != 1)
    {
        cout << "warning:  unable to set audio output to mono,"
             << " may cause problems later\n";
    }

    ioctlParam = 8000;
    if( ioctl( myFD, SNDCTL_DSP_SPEED, &ioctlParam ) == -1 )
    {
        perror("SNDCTL_DSP_SPEED");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }
    if( ioctlParam != 8000 )
    {
        perror("Failed to set sample rate");
        deviceMutex.unlock();
	myFD = -1;
        return -1;
    }
#else
    myFormat = NewSoundCardSigned16LE;
#endif
    deviceMutex.unlock();
    return 0;
}


void NewSoundCard::write ( const unsigned char* data,
			   int samples )
{
    signed short longdataBuffer[480];
    unsigned char dataBuffer[480];

    int cc = 0;
    int p = 0;
 
    // check codec encodings
    switch( myFormat )
    {
    case NewSoundCardUlaw:
        // no conversion needed
	myWriteBuffer.put(data, samples);
        break;
        
    case NewSoundCardSigned16LE:
        // convert sound sample from ULAW to Linear16
        for(p = 0; p < samples; p++ )
            longdataBuffer[p]  = ulaw2linear( data[p] );
        samples *= 2;
        myWriteBuffer.put((unsigned char*)longdataBuffer, samples);
        break;

    default:
        cpLog(LOG_ERR,"Failed to provide correct codec encoding");
        return;
    }

    deviceMutex.lock();
    cc = myWriteBuffer.get(dataBuffer, samples);
#ifndef WIN32
    audio_buf_info info;
    ::ioctl(myFD, SNDCTL_DSP_GETOSPACE, &info);

    {
        int bytecount = info.fragments * info.fragsize;
        
        if (cc > bytecount)
        {
//            cout << "write will block" << endl;
        }
    }

    {
        if (myWriteBuffer.bytesUsed() > 100)
        {
//            cout << "bytesUsed: " << myWriteBuffer.bytesUsed() << endl;
        }
    }

    {
        int bufbytes = 0;
        ::ioctl(myFD, SNDCTL_DSP_GETODELAY, &bufbytes);

        if(bufbytes > 256)
        {
//            cout << "bufbytes: " << bufbytes << endl;
        }
    }

    ::write( myFD, dataBuffer, cc );

    

#else
    m_aSoundCardWinOut.Write(reinterpret_cast<char*>(dataBuffer), cc);
#endif
    deviceMutex.unlock();
    return;
}


int NewSoundCard::read( unsigned char* data,
			const int samples )
{
    signed short longdataBuffer[480];
#ifndef WIN32
    unsigned char dataBuffer[480];
#endif
    
    int cc = -1;
    int p = 0;

    //check codec encodings
    switch( myFormat )
    {
#ifndef WIN32
    case NewSoundCardUlaw:
        // no conversion needed
        cc = 0;
	deviceMutex.lock();
        audio_buf_info info;
        ::ioctl(myFD, SNDCTL_DSP_GETISPACE, &info);

        {
            int bytecount = info.fragments * info.fragsize;

            if (samples > bytecount)
            {
//                cout << "read: " << bytecount << endl;
            }
        }

        cc = ::read( myFD, dataBuffer, samples );


	myReadBuffer.put(dataBuffer, cc);
	deviceMutex.unlock();

	cc = myReadBuffer.get(data, samples);

        break;
#endif
    case NewSoundCardSigned16LE:
        // convert sound sample from Liner16 to ULAW
	deviceMutex.lock();
#ifndef WIN32
        cc = ::read( myFD, (char*)longdataBuffer, samples * 2 );
#else
        cc = m_aSoundCardWinIn.Read( (char*)longdataBuffer, samples * 2 );
#endif
	deviceMutex.unlock();
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


int 
NewSoundCard::close()
{
    int retval = 0;
    deviceMutex.lock();

    cpDebug(LOG_DEBUG, "Closing audio hardware" );

#ifndef WIN32
    if( ioctl( myFD, SNDCTL_DSP_RESET, 0 ) == -1 ) 
    {
        perror("SNDCTL_DSP_RESET");
    }
    retval = ::close( myFD );

#else
    m_aSoundCardWinOut.Close();
    m_aSoundCardWinIn.Close();
#endif

    deviceMutex.unlock();

    myFD = -1;
    return retval;
}


NewSoundCardFormat
NewSoundCard::getFormat() const
{
    Lock mylock_(deviceMutex);
    return myFormat;
}


int 
NewSoundCard::getFd() const
{
    Lock mylock_(deviceMutex);
    return myFD;
}


