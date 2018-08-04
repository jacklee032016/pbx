/*
 * $Id: RtpEvent.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "cpLog.h"
#include "Rtp.hxx"
#include "RtpEvent.hxx"
#include "RtpPacket.hxx"


RtpEvent::RtpEvent()
{
    _DTMFInterface = 0;
    _DTMFType = rtpPayloadDTMF_RFC2833;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return ;
}


RtpEvent::~RtpEvent()
{
    return ;
}


void RtpEvent::setDTMFType( const RtpPayloadType DTMFType )
{
    switch( DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
        case( rtpPayloadCiscoRtp ):
            _DTMFType = DTMFType;
        default:
            cpLog(LOG_ERR, "Can't set RtpEvent to type %d", DTMFType );
            return;
    }

    return;
}


void RtpEvent::sendEvent( const DTMFEvent event )
{
    switch( _DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            sendEventRFC2833( event );
            break;
        case( rtpPayloadCiscoRtp ):
            //sendEventCiscoRTP( event );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return ;
    }

    return;
}


void RtpEvent::sendEventRFC2833( const DTMFEvent event )
{

    return;
}


void RtpEvent::recvEvent( RtpPacket* p )
{
    if ( _DTMFInterface == 0 )
        return ;

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
            return ;
    }

    return ;
}


void RtpEvent::recvEventRFC2833( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadDTMF_RFC2833);

    RtpEventDTMFRFC2833* e = NULL;

    if ( p->getPayloadUsage() != 0 )
        e = reinterpret_cast < RtpEventDTMFRFC2833* > (p->getPayloadLoc());

    if ( e == NULL )
    {
        if ( _keyEvent == KeyEventOn )
        {
            // off detection
            cpDebug(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
            sendToDTMFInterface ( _DTMFEvent );
            _keyEvent = KeyEventNULL;
        }
    }
    else if ( _keyEvent != KeyEventOn && !e->edge )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent( e->event );
        cpDebug(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }

    return ;
}

void RtpEvent::recvEventCiscoRTP( RtpPacket* p )
{
    assert(p->getPayloadType() == rtpPayloadCiscoRtp);

    RtpEventDTMFCiscoRtp* e = reinterpret_cast < RtpEventDTMFCiscoRtp* > (p->getPayloadLoc());

    if ( _keyEvent != KeyEventOn && e->digitType == 1 )
    {
        // on detection
        _keyEvent = KeyEventOn;
        _DTMFEvent = DTMFToEvent(e->digitCode);
        cpDebug(LOG_DEBUG_STACK, "DTMF on detected: %d", _DTMFEvent);
    }
    else if ( _keyEvent == KeyEventOn && e->digitType == 0 && e->edge == 0 )
    {
        // off detection
        cpDebug(LOG_DEBUG_STACK, "DTMF off detected: %d", _DTMFEvent);
        sendToDTMFInterface( _DTMFEvent );
        _keyEvent = KeyEventNULL;
    }

    return ;
}


void RtpEvent::sendToDTMFInterface( const DTMFEvent e )
{
    cpDebug(LOG_DEBUG_STACK, "Adding event %d to fifo", e);
    if ( _DTMFInterface != 0 )
        _DTMFInterface->sendDTMF( e, 20 );
    return ;
}


DTMFEvent RtpEvent::DTMFToEvent( int event )
{
    DTMFEvent result;

    switch ( event )
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
