#ifndef RTPPACKET_HXX
#define RTPPACKET_HXX
/*
* $Log: RtpPacket.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: RtpPacket.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "rtpTypes.h"
#include  <assert.h>

/** 
    Data structure for RTP data packets.   Each RTP payload is encapsulated
    in one of these objects.

    @see RtpSession
*/
class RtpPacket
{
    public:
        /** Constructor that allocates RTP packetData
            @param newpayloadSize Payload size not including RTP header 
            @param npadSize Number of pad bytes (not implemented)
            @param csrc_count Number of contributing sources in packet
         **/
        RtpPacket (int newpayloadSize, int npadSize = 0, int csrc_count = 0);

        /** Constructor that uses already allocated memory as packetData
            @param memory already allocatd memory
            @param size includes rtpHeader and padbytes
        **/
        //RtpPacket (char* memory, int size);

        /// clones an existing packet to a new size
        RtpPacket (RtpPacket* clonePacket, int newpayloadSize);

        /// Destructor that deallocates RTP packetData memory
        ~RtpPacket ();


        /// Pointer to packet data
        char* getPacketData ()
        {
            return packetData;
        }

        /// Pointer to packet header
        RtpHeader* getHeader ()
        {
            return header;
        }

        /// Pointer to beginning of payload
        char* getPayloadLoc ();

        /// Maximum payload size
        int getPayloadSize ();

        /** Sets payload usage
            @param size doesn't include RTP header
         **/
        void setPayloadUsage (int size);

        /// Size of payload stored
        int getPayloadUsage ();

        /// Pointer to begnning of padbyte (Not implemented)
        char* getPadbyteLoc ();

        /// Sets size of payload (Not implemented)
        void setPadbyteSize (int size);

        /// Size of padbyte (Not fully implemented)
        int getPadbyteSize ();

        /// Entire size of RTP packet including header, unused, and padbyte
        int getPacketAlloc ()
        {
            return packetAlloc;
        }

        /// Size of unused memory
        int getUnused ()
        {
            return unusedSize;
        }

        /** Sets size of RTP packet including header and padbyte
            Extra memory will be set as unused memory
         **/
        void setTotalUsage (int size)
        {
            assert (size <= packetAlloc);
            unusedSize = packetAlloc - size;
        }

        /// Size of RTP packet not including unused memory
        int getTotalUsage ()
        {
            return packetAlloc - unusedSize;
        }


        ///
        void setPayloadType (RtpPayloadType payloadType);
        ///
        RtpPayloadType getPayloadType () const;

        ///
        void setSequence (RtpSeqNumber newseq);
        ///
        RtpSeqNumber getSequence ();

        ///
        RtpTime getRtpTime();
        ///
        void setRtpTime (RtpTime time);

        ///
        void setSSRC (RtpSrc src);
        ///
        RtpSrc getSSRC ();

        /// Gets number of contributing sources
        void setCSRCcount (int i);   // use with cuation
        int getCSRCcount ();

        ///
        void setCSRC (RtpSrc src, unsigned int index);

        /** index value less range 1 to csrc_count
            @param index value less range 1 to csrc_count
         **/
        RtpSrc getCSRC (unsigned int index);


        ///
        void setVersion (int i)
        {
            header->version = i;
        }
        int getVersion ()
        {
            return header->version;
        }
        ///
        void setPaddingFlag (int i)
        {
            header->padding = i;
        }
        int getPaddingFlag ()
        {
            return header->padding;
        }
        ///
        void setExtFlag (int i)
        {
            header->extension = i;
        }
        int getExtFlag ()
        {
            return header->extension;
        }
        ///
        void setMarkerFlag (int i)
        {
            header->marker = i;
        }
        int getMarkerFlag ()
        {
            return header->marker;
        }


        /// flags
        bool sequenceSet;
        bool timestampSet;

        /// valid check
        bool isValid();

        /// Print packet contents
        void printPacket ();

    private:
        /// Pointer to raw memory allocation
        char* packetData;

        /// Allocated packet size
        int packetAlloc;

        /// Size of unused memory
        int unusedSize;

        /// Easy access to header information
        RtpHeader* header;

};


#endif // RTPPACKET_HXX
