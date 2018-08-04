/*
 * $Id: RtpEventReceiver.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "Rtp.hxx"
#include "RtpEventReceiver.hxx"
#include "RtpPacket.hxx"
#include "cpLog.h"


RtpEventReceiver::RtpEventReceiver()
{
    _DTMFInterface = 0;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return;
}


RtpEventReceiver::~RtpEventReceiver()

{
    return;
}


void RtpEventReceiver::recvEvent( RtpPacket* p )
{
    if( _DTMFInterface == 0 )
        return;

    switch( p->getPayloadType() )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            recvEventRFC2833( p );
            break;
        case( rtpPayloadCiscoRtp ):
            recvEventCiscoRTP( p );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return;
    }

    return;
}


void RtpEventReceiver::recvEventRFC2833( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadDTMF_RFC2833);

    assert(p->getPayloadUsage() != 0);
    assert(p->getHeader() != 0);
    RtpEventDTMFRFC2833* e = reinterpret_cast<RtpEventDTMFRFC2833*> (p->getPayloadLoc());
    assert(e != NULL);

    if( e->edge == 1)
    {
        if( _keyEvent == KeyEventOn )
        {
            // off detection
            cpDebug(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
            sendToDTMFInterface ( _DTMFEvent, e->duration );
            _keyEvent = KeyEventNULL;
        }
    }
    else // (e->edge == 0)
    {
		if (_keyEvent == KeyEventNULL)
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent( e->event );
        cpDebug(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }

    return;
}

	}
void RtpEventReceiver::recvEventCiscoRTP( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadCiscoRtp);

    RtpEventDTMFCiscoRtp* e = reinterpret_cast<RtpEventDTMFCiscoRtp*>
                              (p->getPayloadLoc());

    if( _keyEvent != KeyEventOn && e->digitType == 1 )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent(e->digitCode);
        cpDebug(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }
    else if( _keyEvent == KeyEventOn && e->digitType == 0 && e->edge == 0 )
    {
        // off detection
        cpDebug(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
        sendToDTMFInterface( _DTMFEvent, 20 );
        _keyEvent = KeyEventNULL;
    }

    return;
}


void RtpEventReceiver::sendToDTMFInterface( const DTMFEvent e, 
					    unsigned int duration )
{
    if( _DTMFInterface != 0 )
    {
        cpDebug(LOG_DEBUG_STACK, "Sending event %d to DTMFInterface", e);
        _DTMFInterface->sendDTMF( e, duration );
    }
    return ;
}


DTMFEvent RtpEventReceiver::DTMFToEvent( int event )
{
    DTMFEvent result;

    switch( event )
    {
        case 0:
        result = DTMFEventDigit0;
        break;
        case 1:
        result = DTMFEventDigit1;
        break;
        case 2:
        result = DTMFEventDigit2;
        break;
        case 3:
        result = DTMFEventDigit3;
        break;
        case 4:
        result = DTMFEventDigit4;
        break;
        case 5:
        result = DTMFEventDigit5;
        break;
        case 6:
        result = DTMFEventDigit6;
        break;
        case 7:
        result = DTMFEventDigit7;
        break;
        case 8:
        result = DTMFEventDigit8;
        break;
        case 9:
        result = DTMFEventDigit9;
        break;
        case 10:
        result = DTMFEventDigitStar;
        break;
        case 11:
        result = DTMFEventDigitHash;
        break;
        default:
        cpLog(LOG_ERR, "Unknown DTMF event during conversion");
        result = DTMFEventNULL;
    }
    return result;
}
