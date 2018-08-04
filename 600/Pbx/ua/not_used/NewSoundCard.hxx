#include "VMutex.h"
#include "Data.hxx"
#include "CircularBuffer.hxx"

using Vocal::ADT::CircularBuffer;

#ifdef WIN32
#include <Mmsystem.h>
#include "ASoundCardWin.hxx"
#endif

enum NewSoundCardFormat
{
    NewSoundCardUlaw,
    NewSoundCardSigned16LE,
    NewSoundCardInvalid
};


class NewSoundCard
{
    public:
        NewSoundCard( const char* deviceName );

        int  open ();
        void write ( const unsigned char* data, int samples );
        int  read ( unsigned char* data, int samples );
        int  close();
        int  reopen();
        NewSoundCardFormat getFormat() const;
        int getFd() const;

    private:
        int myFD;
        mutable VMutex deviceMutex;
        Data myDeviceName;
        NewSoundCardFormat myFormat;

#ifdef WIN32
        //AND:Win32 specific members (handles, etc...)
        ASoundCardWin	m_aSoundCardWinIn;
        ASoundCardWin	m_aSoundCardWinOut;
#endif

        CircularBuffer<unsigned char> myReadBuffer;
        CircularBuffer<unsigned char> myWriteBuffer;

};


