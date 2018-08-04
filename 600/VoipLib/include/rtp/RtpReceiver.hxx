#ifndef RTPRECEIVER_HXX
#define RTPRECEIVER_HXX

/*
 * $Id: RtpReceiver.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


#include <sys/types.h>
#include <map>

#include "Sptr.hxx"
#include "Fifo.h"

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "RtpEvent.hxx"
#include "NtpTime.hxx"
#include "RtpEventReceiver.hxx"

#include "NetworkAddress.h"

class RtpPacket;
class RtcpReceiver;


/** 
    The receiving side of the RTP session.

    @see RtpSession
*/
class RtpReceiver : public RtpEventReceiver
{
    public:

        /// using specified port
        RtpReceiver (int localPort,
                     RtpPayloadType apiFormat = rtpPayloadPCMU,
                     RtpPayloadType networkFormat = rtpPayloadPCMU,
                     int jitterNew = 0);
        /// using port range
        RtpReceiver (int localMinPort, int localMaxPort,
                     RtpPayloadType apiFormat = rtpPayloadPCMU,
                     RtpPayloadType networkFormat = rtpPayloadPCMU,
                     int jitterNew = 0);
        ///Giving UdpStack ptr
        RtpReceiver (UdpStack* udp, RtpPayloadType apiFormat = rtpPayloadPCMU,
                     RtpPayloadType networkFormat = rtpPayloadPCMU,
                     int jitterNew = 0);

        /// consturctor init (don't call this function)
        void constructRtpReceiver (RtpPayloadType apiFormat,
                                   RtpPayloadType networkFormat,
                                   int jitterNew);
        ///
        ~RtpReceiver ();


        /** Receive an RTP packet from buffer.
            Do not delete packet, it's used in next recive call as replacement
            @return NULL if not time to play packet
         **/
        RtpPacket* receive ();


        /** Receives an RTP packet from network into buffer
            @return NULL invalid packet, else packet pointer
         **/
        RtpPacket* getPacket ();

        /** Update souce information
         *  @return 0 not valid, 1 valid
         **/
        int updateSource (RtpPacket* packet);

        /// Initalize source counters
        void initSource (RtpPacket* packet);

        /** Add source to receiver
         *  @return 1 if transmitter on probation
         **/
        int addSource (RtpPacket* packet);

        /// Remove source from receiver
        void removeSource (RtpSrc src, int flag = 0);


        /** Convert from rtp time to ntp time
         *  using inital ntp time and sampleRate
         **/
        NtpTime rtp2ntp (RtpTime time);

        /// Clears out buffer
        //void clearBuffer ();
        /// Prints out the sequencing of buffer
        //void printBuffer ();


        /// Empties pending network packets
        void emptyNetwork ();


        ///
        void setRTCPrecv (RtcpReceiver* rtcpRecv);

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
        void setCodecString (const char* codecStringInput);

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
        int getPacketReceived ()
        {
            return packetReceived;
        }
        ///
        int getPayloadReceived ()
        {
            return payloadReceived;
        }

        ///
        int getJitter ()
        {
            return jitter;
        }

        /// Port this stack is sending its singal
        int getPort ()
        {
            return myStack->getRxPort();
        }
        /// Socket File Descriptor used for select()
        int getSocketFD ()
        {
            return myStack->getSocketFD();
        }

        /// receiver error code;
        RtpReceiverError receiverError;

        /// get ptr of my UdpStack
        UdpStack* getUdpStack()
        {
            return myStack;
        }


    private:

        /// incoming buffer size, including RTP header
        static const int IN_BUFFER_SIZE;
        /// buffer size for single pkt
        static const int RECV_BUF;
        /// max misordering tolerated
        static const int MISORDER;
        /// max number of sequental lost packets allowed
        static const int PROB_MAX;

        /// source found flag
        bool sourceSet;
        /// SRC number for this source
        RtpSrc ssrc;
        /// probation set flag
        bool probationSet;
        /// wouldn't listen to this source
        RtpSrc srcProbation;
        /// probation, 0 source valid
        int probation;

        /// inital seqence number
        RtpSeqNumber seedSeq;
        /// inital NTP timestamp
        NtpTime seedNtpTime;
        /// inital RTP timestamp
        RtpTime seedRtpTime;
        /// rtp interval
        RtpTime sampleRate;
        /// payload specific sample rate
        int baseSampleRate;
        /// previous packet
        RtpPacket* prevPacket;
        /// next packet play time
        NtpTime gotime;

        /// number of packets received
        int packetReceived;
        /// number of bytes of payload received
        int payloadReceived;

        /// last packet RtpTime received 
        RtpTime prevPacketRtpTime;
        /// last RtpTime play
        RtpTime prevRtpTime;
        /// last NtpTime play
        NtpTime prevNtpTime;
        /// previous sequence number received
        RtpSeqNumber prevSeqRecv;
        /// previous sequence numer played
        RtpSeqNumber prevSeqPlay;
        /// number of received sequence number cycles
        int recvCycles;
        /// number of played sequence number cycles
        int playCycles;

        /// relative transmission time for prev packet
        int transit;
        /// estimated jitter
        int jitter;

        /// jitter time in ms time
        int jitterTime;
        /// jitter buffer
        //map<RtpSeqNumber, RtpPacket*> jitterBuffer;

        /// additional SDES and RTCP information
        RtcpReceiver* rtcpRecv;


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
        int apiFormat_payloadSize;   // api_pktSampleSize*apiFormat_perSampleSize
        int networkFormat_payloadSize;   // network_pktSampleSize * networkFormat_perSampleSize

        /// inital jitter size
        int jitterSeed;

        /// silence packet template
        char* silenceCodec;

        /// my UDP stack
        UdpStack* myStack;
        /// should I free my stack?
        bool freeStack;


        /// buffer incoming rtp pkt from network
        char inBuff[24576];
        int inPos;
        int playPos;
};



#endif // RTPRECEIVER_HXX
