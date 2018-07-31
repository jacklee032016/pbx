#ifndef RTCPPACKET_HXX
#define RTCPPACKET_HXX
/*
* $Log: RtcpPacket.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: RtcpPacket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/


#include "Rtcp.hxx"
#include "rtpTypes.h"


/**  Compound RTCP control packet.  These are sent and received by the
     RTP stack to send RTCP statistics.

     @see RtpSession
 */
class RtcpPacket
{
    public:
        ///
        RtcpPacket ();
        ///
        ~RtcpPacket ();

        /// Pointer to beginning of memory of packet
        char* getPacketData ();

        /** Pointer to beginning of free mememory of packet.
            Must call allocData() afterwards saying how much you used
         **/
        char* freeData ();

        /// Increases packet memory usage by newSize. Returns newSize
        int allocData (int newSize);

        /// Total mem allocated in packet
        int getPacketAlloc();

        /// Unused memory in payload area
        int getUnused ();

        /** Sets total unused memory in packet.
            Useful when receiving packet into buffer
         **/
        void setTotalUsage (int size);

        /** Total unused memory in packet.
            Useful when transmitting packet
         **/
        int getTotalUsage ();

        ///
        int getVersion ();
        ///
        int getPadbyteFlag ();
        ///
        int getCount ();
        ///
        RtcpType getPayloadType ();
        ///
        int getLength ();

        /// debug
        void printPacket ();

    private:
        /// Default RTCP packet size
        static const int PACKETSIZE;

        /// Pointer to raw packet memory
        char* packetData;

        /// Allocated raw packet memory size
        int packetAlloc;

        /// Amount of unused packet memory
        int unusedSize;
};


#endif // RTCPPACKET_HXX
