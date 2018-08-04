#ifndef RTPTRANSMITTER_HXX
#define RTPTRANSMITTER_HXX

/*
 * $Id: RtpTransmitter.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include <sys/types.h>
#include <map>

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "NtpTime.hxx"

#include "NetworkAddress.h"

class RtpPacket;
class RtcpReceiver;


/**
   The sending side of an RTP session.

   @see RtpSession
*/

class RtpTransmitter
{
    public:

        /// constructor using port range
        RtpTransmitter (const char* remoteHost, int remoteMinPort,
                        int remoteMaxPort,
                        RtpPayloadType apiFormat = rtpPayloadPCMU,
                        RtpPayloadType networkFormat = rtpPayloadPCMU,
                        RtpReceiver* receivePort = 0);
        /// construtor using specified port
        RtpTransmitter (const char* remoteHost, int remotePort = 5000,
                        RtpPayloadType apiFormat = rtpPayloadPCMU,
                        RtpPayloadType networkFormat = rtpPayloadPCMU,
                        RtpReceiver* receiver = 0);
        /// consturctor init (don't call this function)
        void constructRtpTransmitter (RtpPayloadType apiFormat,
                                      RtpPayloadType networkFormat);
        ///
        ~RtpTransmitter ();

        /// Creates a packet with transmitter's payload type and SRC number
        RtpPacket* createPacket (int npadSize = 0, int csrc_count = 0);

        ///
        int transmit (RtpPacket* packet, bool eventFlag = false);
        ///
        int transmitRaw (char* buffer, int data_len);

        ///
        RtpSrc getSSRC ()
        {
            return ssrc;
        }
        ///
        void setSSRC (RtpSrc src)
        {
            ssrc = src;
        }

        ///
        int getPacketSent ()
        {
            return packetSent;
        }
        ///
        int getPayloadSent ()
        {
            return payloadSent;
        }

        ///
        RtpSeqNumber getPrevSequence ()
        {
            return prevSequence;
        }


        ///
        RtpTime getPrevRtpTime ()
        {
            return prevRtpTime;
        }


        ///
        void setMarkerOnce()
        {
            markerOnce = true;
        }

        ///
        void setApiFormat (RtpPayloadType type, int no_samples, int packetSize = 0,
                           RtpPacket* p = NULL, bool print = true);
        ///
        int getApiPktSampleSize ()
        {
            return api_pktSampleSize;
        }
        ///
        void setApiPktSize (int size)
        {
            api_pktSampleSize = size;
        }
        ///
        void setNetworkFormat (RtpPayloadType type, int no_samples, int packetSize = 0,
                               RtpPacket* p = NULL, bool print = true);
        ///
        int getNetworkPktSampleSize ()
        {
            return network_pktSampleSize;
        }
        ///
        void setNetworkPktSize (int size)
        {
            network_pktSampleSize = size;
        }

        ///
        void setCodecString (const char* codecStringInput);

        /// Port this stack is sending its singal
        int getPort ()
        {
            return myStack->getTxPort();
        }
        /// Socket File Descriptor used for select()
        int getSocketFD ()
        {
            return myStack->getSocketFD();
        }

        /// transmitter error code;
        RtpTransmitterError transmitterError;

        /// get ptr of my UdpStack
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
        /// outgoing buffer size, including RTP header
        static const int OUT_BUFFER_SIZE;
        /// local SRC number
        RtpSrc ssrc;
        /// inital NTP time
        NtpTime seedNtpTime;
        /// inital RTP time
        RtpTime seedRtpTime;
        /// RTP time of previous packet
        RtpTime prevRtpTime;
        /// NTP time of previous packet
        NtpTime prevNtpTime;
        /// previous packet's sequence
        RtpSeqNumber prevSequence;

        /// numbers of packet sent
        int packetSent;
        /// total bytes of payload sent
        int payloadSent;

        /// allow RTCP stack to probe for information
        friend class RtcpTransmitter;
        /// allow RTCP stack to probe for information
        friend class RtcpReceiver;

        /// format of payload for stack
        RtpPayloadType apiFormat;
        RtpPayloadType networkFormat;
        /// codecString in case of dynamic payload type
        char codecString[32];
        /// number of samples per RTP packet on api (typical 160 or 240)
        int api_pktSampleSize;
        int network_pktSampleSize;
        /// number of bytes per sample  (typical 1 or 2, used for endian conversion)
        int apiFormat_perSampleSize;
        int networkFormat_perSampleSize;
        /// payload specific sample rate
        int apiFormat_clockRate;
        int networkFormat_clockRate;
        /// payload sizes
        int apiFormat_payloadSize;   // api_pktSampleSize*apiFormat_perSampleSize
        int networkFormat_payloadSize;   // network_pktSampleSize * networkFormat_perSampleSize

        /// marker once
        bool markerOnce;

        /// my UDP stack
        UdpStack* myStack;
        ///
        bool freeStack;

        /// buffer outgoing rtp pkt to network
        char outBuff[8192];
        int outPos;
        int recPos;

        NetworkAddress remoteAddr;
};


#endif // RTPTRANSMITTER_HXX
