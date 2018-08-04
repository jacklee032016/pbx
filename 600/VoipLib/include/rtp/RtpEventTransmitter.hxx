#ifndef RTPEVENTTRANSMITTER_HXX
#define RTPEVENTTRANSMITTER_HXX

/*
 * $Id: RtpEventTransmitter.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include "rtpTypes.h"
#include "RtpPacket.hxx"
#include "RtpTransmitter.hxx"


class RtpEventTransmitter
{
    public:
        ///
        RtpEventTransmitter( RtpTransmitter* rtpTran );
        ///
        virtual ~RtpEventTransmitter();

        /// send out a DTMFEvent in RTP
        void sendEvent( const DTMFEvent event );

        /// changes type of DTMF in rtp to send
        void setDTMFType( const RtpPayloadType DTMFType );

    private:
        // sending events
        void sendEventRFC2833( const DTMFEvent event );
        void sendEventCiscoRTP( const DTMFEvent event );

        // state information
        RtpTransmitter* _rtpTran;
        RtpPayloadType _DTMFType;
        KeyEvent _keyEvent;
        DTMFEvent _DTMFEvent;
};

#endif
