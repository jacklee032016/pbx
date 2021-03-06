#ifndef RTPEVENTRECEIVER_HXX
#define RTPEVENTRECEIVER_HXX
/*
 * $Id: RtpEventReceiver.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include "RtpPacket.hxx"
#include "DTMFInterface.hxx"

/**
   A device which receives RTP events (mostly DTMF events).

   @see RtpSession
*/
class RtpEventReceiver
{
    public:
        ///
        RtpEventReceiver();
        ///
        virtual ~RtpEventReceiver();

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

        /// send message to the DTMFinterface callback.
        void eventDTMF( int a, int b )
        {
        	printf("in eventDtmf \n");
            _DTMFInterface->sendDTMF( a, b );
        }

    private:
        // reading events
        void recvEventRFC2833( RtpPacket* p );
        void recvEventCiscoRTP( RtpPacket* p );
        void sendToDTMFInterface( const DTMFEvent event, unsigned int duration );

        DTMFEvent DTMFToEvent( int event );

        // device interface
        DTMFInterface* _DTMFInterface;

        // state information
        KeyEvent _keyEvent;
        DTMFEvent _DTMFEvent;
};

#endif
