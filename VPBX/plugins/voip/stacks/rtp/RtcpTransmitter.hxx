#ifndef RTCPTRANSMITTER_HXX
#define RTCPTRANSMITTER_HXX
/*
* $Log: RtcpTransmitter.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: RtcpTransmitter.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/


#include "Rtcp.hxx"

#include <sys/types.h>

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "NetworkAddress.h"

class RtcpReceiver;
class RtcpPacket;

/** Class to transmit RTCP packets.  Called inside RtpSession.
    @see RtpSession
    
 */
class RtcpTransmitter
{
    public:
        /** Constructor creating outgoing RTCP stack using port ranges
            @param remoteHost DNS name receiving the packets
            @param port associated port
         **/
        RtcpTransmitter (const char* remoteHost, int remoteMinPort,
                         int remoteMaxPort, RtcpReceiver* receiver);
        /** Constructor creating outgoing RTCP stack using specified port
            @param remoteHost DNS name receiving the packets
            @param port associated port
         **/
        RtcpTransmitter (const char* remoteHost, int remotePort,
                         RtcpReceiver* receiver);
        /// consturctor init (don't call this function)
        void constructRtcpTransmitter ();

        /// Deconstructor
        ~RtcpTransmitter ();


        /** Transmits RTCP packet to remoteHost/port.
            Doesn't remove packet from memory.
            Returns -1 failure or number of bytes sent on success
         **/
        int transmit (RtcpPacket* packet);

        /// set timer to next interval
        void updateInterval ();

        /// Check if time to send RTCP packet.  Returns 1 then time up
        int checkInterval ();


        /// Adds SR packet into compound packet
        int addSR (RtcpPacket* packet, int npadSize = 0);

        /// Adds specificed SDES item to compound packet
        int addSDES (RtcpPacket* packet, RtcpSDESType item, int npadSize = 0);

        /// Adds all known SDES items to compound packet
        int addSDES (RtcpPacket* packet, int npadSize = 0);

        /// Adds specificed SDES items in SDESlist, which ends with RTCP_SDES_END
        int addSDES (RtcpPacket* packet, RtcpSDESType* SDESlist, int npadSize = 0);

        /** Adds BYE packet using transmitter's SRC numbers
            @param reason optional text, null-term
         **/
        int addBYE (RtcpPacket* packet, char* reason = NULL, int npadSize = 0);

        /** Adds BYE packet using specified list of SRC numbers
            @param reason optional text, null-term
            @param count number of items in list
         **/
        int addBYE (RtcpPacket* packet, RtpSrc* list, int count,
                    char* reason = NULL, int npadSize = 0);

        /// future: not implemented
        int addAPP (RtcpPacket* packet, int newpadbyeSize = 0);


        /// Used for calculating RR information
        u_int32_t calcLostFrac (RtpTranInfo* source);

        /// Used for calculating RR information
        u_int32_t calcLostCount (RtpTranInfo* source);

        ///
        void setSdesCname ();
        ///
        void setSdesName (char* text);
        ///
        void setSdesEmail (char* text);
        ///
        void setSdesPhone (char* text);
        ///
        void setSdesLoc (char* text);
        ///
        void setSdesTool (char* text);
        ///
        void setSdesNote (char* text);
        ///
        char* getSdesCname ();
        ///
        char* getSdesName ();
        ///
        char* getSdesEmail ();
        ///
        char* getSdesPhone ();
        ///
        char* getSdesLoc ();
        ///
        char* getSdesTool ();
        ///
        char* getSdesNote ();

        /// Sets RTP transmitter if one is used
        void setRTPtran (RtpTransmitter* tran);
        /// Sets RTCP transmitter if one is used
        void setRTPrecv (RtpReceiver* recv);
        /// Sets RTCP receiver if one is used
        void setRTCPrecv (RtcpReceiver* rtcpRecv);

        /// Port this tramsitter is sending it singal
        int getPort ();
        /// Socket File Descriptor used for select()
        int getSocketFD ();

        /// get the ptr of my UdpStack
        UdpStack* getUdpStack()
        {
            return myStack;
        }

        ///
        NetworkAddress* getRemoteAddr ()
        {
            return &remoteAddr;
        }

        ///
        void setRemoteAddr (const NetworkAddress& theAddr) ;


    private:
        /// Next time to submit RTCP packet
        NtpTime nextInterval;

        /// Transmission interval in ms
        static const int RTCP_INTERVAL;

        /** Transmitter SDES information
         *  data stored as null-term strings
         **/
        SDESdata* SDESInfo;

        ///
        RtpTransmitter* tran;
        ///
        RtpReceiver* recv;
        ///
        RtcpReceiver* rtcpRecv;

        /// my UDP stack
        UdpStack* myStack;

        ///
        bool freeStack;

        NetworkAddress remoteAddr;
};


#endif // RTCPTRANSMITTER_HXX
