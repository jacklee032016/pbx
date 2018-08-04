/*
 * $Id: RtpEventTransmitter.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "Rtp.hxx"
#include "RtpEventTransmitter.hxx"
#include "RtpPacket.hxx"


RtpEventTransmitter::RtpEventTransmitter( RtpTransmitter* rtpTran )
{
    _rtpTran = rtpTran;
    _DTMFType = rtpPayloadDTMF_RFC2833;
    _keyEvent = KeyEventNULL;
    _DTMFEvent = DTMFEventNULL;
    return ;
}


RtpEventTransmitter::~RtpEventTransmitter()
{
    return ;
}


void RtpEventTransmitter::setDTMFType( const RtpPayloadType DTMFType )
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


void RtpEventTransmitter::sendEvent( const DTMFEvent event )
{
    switch( _DTMFType )
    {
        case( rtpPayloadDTMF_RFC2833 ):
            sendEventRFC2833( event );
            break;
        case( rtpPayloadCiscoRtp ):
            sendEventCiscoRTP( event );
            break;
        default:
            cpLog(LOG_ERR, "Unknown rtp payload event");
            return ;
    }

    return;
}


void RtpEventTransmitter::sendEventRFC2833( const DTMFEvent event )
{
    

    return;
}



void RtpEventTransmitter::recvEventRFC2833( const DTMFEvent event )
{
    RtpEventDTMFRFC2833* e = NULL;

    if ( e->getPayloadUsage() != 0 )
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

void RtpEventTransmitter::recvEventCiscoRTP( const DTMFEvent event )
{
/*
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
*/
}
