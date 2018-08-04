#ifndef RTPEVENTTRANSMITTER_HXX
#define RTPEVENTTRANSMITTER_HXX
/*
* $Log: RtpEventTransmitter.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RtpEventTransmitter.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
