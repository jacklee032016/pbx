#ifndef RTPEVENT_HXX
#define RTPEVENT_HXX
/*
* $Log: RtpEvent.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RtpEvent.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "RtpPacket.hxx"
#include "DTMFInterface.hxx"

/**
   Class which contains an RTP event, such as a DTMF (touch tone) message.
   Many devices transmit DTMF via non-audible signals instead of actually
   sending the audio data, and the RtpEvent class is used in this context.

   @see RtpSession
*/
class RtpEvent
{
    public:
        ///
        RtpEvent();
        ///
        virtual ~RtpEvent();


        /// send out a DTMFEvent in RTP
        void sendEvent( const DTMFEvent event );

        /// receives DTMFEvent from RTP
        /// calls the registered callback fucntion
        void recvEvent( RtpPacket* p );

        ///
        const DTMFInterface* const getDTMFInterface()
        {
            return _DTMFInterface;
        }

        /// set DTMF callback function
        void setDTMFInterface( DTMFInterface* t )
        {
            _DTMFInterface = t;
        }

        /// unset DTMF callback function
        void unsetDTMFInterface( DTMFInterface* t )
        {
            _DTMFInterface = 0;
        }


        /// changes type of DTMF in rtp to send
        void setDTMFType( const RtpPayloadType DTMFType );


        /// (what does this do?)
        void eventDTMF( int a, int b )
        {
            _DTMFInterface->sendDTMF( a, b );
        }

    private:
        // sending events
        void sendEventRFC2833( const DTMFEvent event );
        void sendEventCiscoRTP( const DTMFEvent event );

        // reading events
        void recvEventRFC2833( RtpPacket* p );
        void recvEventCiscoRTP( RtpPacket* p );
        void sendToDTMFInterface( const DTMFEvent event );

        DTMFEvent DTMFToEvent( int event );

        // device interface
        DTMFInterface* _DTMFInterface;

        // state information
        RtpPayloadType _DTMFType;
        KeyEvent _keyEvent;
        DTMFEvent _DTMFEvent;
};

#endif
