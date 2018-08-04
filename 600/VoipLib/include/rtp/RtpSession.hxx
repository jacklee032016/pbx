#ifndef RTPSESSION_HXX
#define RTPSESSION_HXX
/*
 * $Id: RtpSession.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "DTMFInterface.hxx"

/**

   Contains everything needed for a complete RTP Session, including
   RTP/RTCP transmitter and RTP/RTCP receiver.<P>

   <b>To construct a stack:</b><p>

   <pre>
   int remotePort = 10000;
   int localPort  = 10000;
   int remoteRtcpPort = 10001;
   int localRtcpPort =  10001;
   pType = rtpPayloadPCMU;
   RtpSession&#42; rtpStack = new RtpSession ( "127.0.0.1", remotePort, localPort,
                                           remoteRtcpPort, localRtcpPort, 
                                           pType,
                                           pType, 0 );
   </pre>

   <p>
   <b>To change the send/receive state of the stack:</b><p>
   
   <pre>
   rtpStack->setSessionState(rtp_session_recvonly);
   rtpStack->setSessionState(rtp_session_sendonly);
   rtpStack->setSessionState(rtp_session_sendrecv);
   </pre>
   <p>
   <b>To receive RTP:</b><P>

   <pre>
   while(1)
   {
       RtpSessionState sessionState = rtpStack->getSessionState();

       if( (sessionState == rtp_session_recvonly || 
            sessionState == rtp_session_sendrecv))
       {
            RtpPacket&#42; packet = rtpStack->receive();
            
            ... // do something here.
            
            // at this point, the packet data (in encoded form) is
            // available via the following interfaces:
            
            // packet.getPayloadLoc() contains a ptr to the start of
            // the packet.
            
            // packet.getPayloadUsage() returns the # of valid bytes
            // pointed to by getPayloadLoc().
       }
   }
   </pre>
   <p>

   <b>In order to send packets:</b><p>
   <P>
   <pre>
   char&#42; data;
   int length;
   
   // at this point, assume data points to the raw audio data to send
   // and that length is the # of valid bytes.
   
   rtpStack->transmitRaw(data, length);
   </pre>

   
*/
class RtpSession
{
    public:
        /**@param ports All ports are optional
         * @param jitterNew = -1 to bypass jitterBuffer
         **/
        RtpSession (const char* remoteHost, int remotePort = 0, int localPort = 0,
                    int rtcpRemotePort = 0, int rtcpLocalPort = 0,
                    RtpPayloadType apiPayloadType = rtpPayloadPCMU,
                    RtpPayloadType networkPayloadType = rtpPayloadPCMU,
                    int jitterNew = 5);
        /**@param ports All ports are optional by passing 0.  Port range is
         *        availiable here.
         * @param jitterNew = -1 to bypass jitterBuffer
         **/
        RtpSession (const char* remoteHost, int remotePort, int localPort,
                    int rtcpRemotePort, int rtcpLocalPort, int portRange,
                    RtpPayloadType apiPayloadType = rtpPayloadPCMU,
                    RtpPayloadType networkPayloadType = rtpPayloadPCMU,
                    int jitterNew = 5);
        /// consturctor init (don't call this function)
        void constructRtpSession (const char* remoteHost, int remotePort,
                                  int localPort, int rtcpRemotePort, int rtcpLocalPort,
                                  int portRange, RtpPayloadType apiPayloadType,
                                  RtpPayloadType networkPayloadType, int jitterNew);
        ///
        ~RtpSession ();

        /// reserves a rtp port by instantiating a RtpReceiver object
        int reserveRtpPort(int localMin = -1, int localMax = -1);
        /// release the rtp port
        int releaseRtpPort();

        /// reserves a rtcp port by instantiating a RtcpReceiver object
        int reserveRtcpPort(int localPort, int portRange = 0);
        /// release the rtcp port
        int releaseRtcpPort();

        /**@name Session Functions
         *  Being in a particular state disables some functions.  For example,
         *  in recvonly state, the transmit function will do nothing.
         *  Availiable state are listed in rtpTyes.h
         **/
        void setSessionState (RtpSessionState state);
        ///
        RtpSessionState getSessionState ();

        /// set receivers
        int setReceiver ( int localMinPort, int rtcpLocalPort, int portRange,
                          RtpPayloadType apiFormat, RtpPayloadType networkFormat,
                          int jitterNew = 5 );

        /// set transmitters
        int setTransmiter ( const char* remoteHost, int remotePort,
                            int rtcpRemotePort, RtpPayloadType apiFormat,
                            RtpPayloadType networkFormat );

        ///
        void setApiFormat (RtpPayloadType type, int size, int packetSize = 0);
        void setNetworkFormat (RtpPayloadType type, int size, int packetSize = 0);

        ///
        void setCodecString (const char* codecStringInput);

        /// size is in number of samples
        void setApiPktSampleSize (int size);
        void setNetworkPktSampleSize (int size);
        void setApiPktSize (int size);
        void setNetworkPktSize (int size);

        ///
        int getNetworkPktSampleSize ();
        int getApiPktSampleSize ();

        ///
        int getPacketSent ();
        ///
        int getByteSent ();

        ///
        int getPacketReceived ();
        ///
        int getByteReceived ();

        ///
        int getPacketLost ();

        ///
        int getJitter ();

        ///
        int getLatency ();

        // not implemented
        //void setTypeOfService (int service);
        // not implemented
        //int getTypeOfService ();
        //@}


        /** Creates an RTP packet for transmitter
         *  @param no_samples number of samples for specified payload
         *  @param padbyteSize always 0, not fully implemented
         *  @param csrc_count number of contributing sources to packet
         **/
        RtpPacket* createPacket (int padbyteSize = 0, int csrcCount = 0);

        /** Gets the previous packet's RTP sequence number 
         **/
        RtpSeqNumber getPrevSequence();

        /** Gets the previous packet's RTP timestamp
         **/
        RtpTime getPrevRtpTime();

        // set marker flago next packet
        void setMarkerOnce();

        /** Transmits packet onto network
         *  @return -1 error, otherwise size sent on sucess
         **/
        int transmit (RtpPacket* packet);
        int transmitRaw (char* inbuffer, int len);

        /** Transmit DTMF event in RTP on network
         *  @return -1 error, otherwise 0
         **/
        int transmitEvent( int event );

        /** Receives RTP packet information from network
         *  fn will deallocate packet on next call
         *  @return -1 error in packet or network, 0 no packet received,
         *             otherwise bytes receeived
         **/
        RtpPacket* receive ();
        RtpPacket* getPacket();


        /** Checks RTCP interval, transmits and receeives if neccessary
         **/
        int processRTCP();

        /** Create and transmit compound RTCP packet
         *  @return -1 failure, else number of bytes sent on success
         **/
        int transmitRTCP ();

        /** Create and transmit a SR and BYE compound packet
         *  @return -1 failure, else number of bytes sent on success
         **/
        int transmitRTCPBYE ();

        /** receive and read compound RTCP packet
         *  @eturn 0 if no packet received, else 1 on sucess
         **/
        int receiveRTCP ();

        /** Check if time to send RTCP packet
         *  return: 1 = time to send RTCP packet
         **/
        int checkIntervalRTCP ();


        /// RTP transmitter
        RtpTransmitter* getRtpTran ()
        {
            return tran;
        }
        /// RTP recevier
        RtpReceiver* getRtpRecv ()
        {
            return recv;
        }
        /// RTCP transmitter
        RtcpTransmitter* getRtcpTran ()
        {
            return rtcpTran;
        }
        /// RTCP receiver
        RtcpReceiver* getRtcpRecv ()
        {
            return rtcpRecv;
        }


        /// Rtp event class
        RtpEvent _rtpEvent;

        /// DTMF callback function
        void setDTMFInterface ( DTMFInterface* t );
        void unsetDTMFInterface ( DTMFInterface* t );

    private:
        /// Internal pointer to RTP transmitter
        RtpTransmitter* tran;
        /// Internal pointer to RTP receiver
        RtpReceiver* recv;
        /// Internal pointer to RTCP transmitter
        RtcpTransmitter* rtcpTran;
        /// Internal pointer to RTCP receiver
        RtcpReceiver* rtcpRecv;

        /// session state
        RtpSessionState sessionState;

        /// session error code;
        RtpSessionError sessionError;
};



#endif // RTPSESSION_HXX
