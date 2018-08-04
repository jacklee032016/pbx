/*
* $Log: RtpPacket.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RtpPacket.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>
//#include <map>
//#include <utility>                     // pair

// error handling
//#include <errno.h>


// network socket
#include <netinet/in.h>                // struct socketaddr_in
//#include <sys/socket.h>
//#include <netdb.h>

#include "cpLog.h"
#include "vsock.hxx"

#include "NetworkAddress.h"
#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "rtpCodec.h"



/* ----------------------------------------------------------------- */
/* --- RtpPacket Constructor --------------------------------------- */
/* ----------------------------------------------------------------- */

RtpPacket::RtpPacket (int newpayloadSize, int npadSize, int csrc_count)
{
    packetData = NULL;
    header = NULL;

    // check given paramters
    assert (csrc_count >= 0);
    assert (csrc_count <= 15);
    assert (newpayloadSize >= 0);
    assert (npadSize >= 0);

    // create memory allocation
    packetAlloc = sizeof(RtpHeader) - sizeof(RtpSrc)
                  + csrc_count * sizeof(RtpSrc) + newpayloadSize + npadSize;
    assert (packetAlloc < RTP_MTU);
    packetData = new char[packetAlloc];
    assert (packetData);
    memset (packetData, 0, packetAlloc);

    // set private variables
    header = reinterpret_cast < RtpHeader* > (packetData);
    setPadbyteSize (npadSize);
    setPayloadUsage (0);
    assert (unusedSize == newpayloadSize);

    // set rtp header values
    header->version = RTP_VERSION;
    header->padding = (npadSize > 0) ? 1 : 0;
    header->extension = 0;
    header->count = csrc_count;
    header->marker = 0;
    header->type = rtpPayloadUndefined;
    header->sequence = 0;
    header->timestamp = 0;

    // set flags
    sequenceSet = false;
    timestampSet = false;
}


/*
RtpPacket::RtpPacket (char* memory, int size) 
{
    packetData = NULL;
    header = NULL;
 
    // check given paramters
    assert (memory);
    assert (size >= 12);
 
    // memory allocation
    packetData = memory;
 
    // set private variables
    header = reinterpret_cast<RtpHeader*>(packetData);
    packetAlloc = size;
    setPayloadUsage (getPayloadSize());
    assert (unusedSize == 0);
}
*/


// clones the rtp header
RtpPacket::RtpPacket (RtpPacket* clone, int newpayloadSize)
{
    packetData = NULL;
    header = NULL;

    // create memory allocation
    packetAlloc = sizeof(RtpHeader) - sizeof(RtpSrc)
                  + clone->getCSRCcount() * sizeof(RtpSrc)
                  + newpayloadSize + clone->getPadbyteSize();
    assert (packetAlloc < RTP_MTU);
    packetData = new char[packetAlloc];
    assert (packetData);
    memset (packetData, 0, packetAlloc);

    // set private variables
    header = reinterpret_cast < RtpHeader* > (packetData);
    setPadbyteSize (clone->getPadbyteSize());
    setPayloadUsage (0);
    assert (unusedSize == newpayloadSize);

    // clone header
    setVersion (clone->getVersion());
    setPaddingFlag (clone->getPaddingFlag());
    setExtFlag (clone->getExtFlag());
    setCSRCcount (clone->getCSRCcount());
    setMarkerFlag (clone->getMarkerFlag());
    setPayloadType (clone->getPayloadType());
    setSequence (clone->getSequence());
    setRtpTime (clone->getRtpTime());
    assert (getSequence() == clone->getSequence());

    // set flags
    sequenceSet = false;
    timestampSet = false;
}


RtpPacket::~RtpPacket ()
{
    if (packetData != NULL)
    {
        delete []packetData; packetData = NULL;
    }
    header = NULL;
}



/* --- Size and Locations ------------------------------------------ */


/*     payload          */
char* RtpPacket::getPayloadLoc ()
{
    assert (header);
    return (packetData + sizeof(RtpHeader) - sizeof(RtpSrc)
            + (header->count)*sizeof(RtpSrc));
}

int RtpPacket::getPayloadSize ()
{
    assert (header);
    return (packetAlloc - sizeof(RtpHeader) + sizeof(RtpSrc)
            - (header->count)*sizeof(RtpSrc) - getPadbyteSize());
}

void RtpPacket::setPayloadUsage (int size)
{
    if (!(size <= getPayloadSize()))
        cerr << "ERR" << size << " " << getPayloadSize();
    assert (size <= getPayloadSize());
    unusedSize = getPayloadSize() - size;
}

int RtpPacket::getPayloadUsage ()
{
    return getPayloadSize() - unusedSize;
}



/*     padbyte          */
char* RtpPacket::getPadbyteLoc ()
{
    return getPayloadLoc() + getPayloadSize();
}

void RtpPacket::setPadbyteSize (int size)
{
    // future: not implemented
    // ? write size to last octlet of packetData
}


int RtpPacket::getPadbyteSize ()
{
    // future: not implemented
    // ? read last octlet of packetData
    return 0;
}



/* --- Packet Information ------------------------------------------ */

/*     payload type     */
void RtpPacket::setPayloadType (RtpPayloadType type)
{
    assert (type >= 0);
    assert (type <= 127);
    header->type = type;
}

RtpPayloadType RtpPacket::getPayloadType () const
{
    return static_cast < RtpPayloadType > (header->type);
}



/*     sequence number  */
void RtpPacket::setSequence (RtpSeqNumber nseq)
{
    assert (header);
    sequenceSet = true;
    header->sequence = htons(nseq);
}

RtpSeqNumber RtpPacket::getSequence ()
{
    assert (header);
    return ntohs(header->sequence);
}



/*     timestamp        */
void RtpPacket::setRtpTime (RtpTime time)
{
    assert (header);
    timestampSet = true;
    header->timestamp = htonl(time);
}

RtpTime RtpPacket::getRtpTime ()
{
    assert (header);
    return ntohl(header->timestamp);
}



/*     ssrc             */
void RtpPacket::setSSRC (RtpSrc src)
{
    assert (header);
    header->ssrc = htonl(src);
}

RtpSrc RtpPacket::getSSRC ()
{
    assert (header);
    return ntohl(header->ssrc);
}



/*     csrc             */
int RtpPacket::getCSRCcount ()
{
    assert (header);
    return header->count;
}

// use with caution
void RtpPacket::setCSRCcount (int i)
{
    assert (header);
    header->count = i;
}

void RtpPacket::setCSRC (RtpSrc src, unsigned int i)
{
    assert (header);
    assert (i >= 1);
    assert (i <= header->count);

    RtpSrc* srcPtr = &(header->startOfCsrc);
    *(srcPtr + i - 1) = htonl(src);
}

RtpSrc RtpPacket::getCSRC (unsigned int i)
{
    assert (header);
    assert (i >= 1);
    assert (i <= header->count);

    RtpSrc* srcPtr = &(header->startOfCsrc);
    return ntohl(*(srcPtr + i - 1));
}


bool RtpPacket::isValid()
{
    // check packet size
    if( getTotalUsage() <= 0 )
        return false;

    // check version
    if( getVersion() != RTP_VERSION )
    {
        cpDebug(LOG_DEBUG_STACK, "Wrong RTP version" );
        return false;
    }

    // modify known payload types
    switch( getPayloadType() )
    {
        case( 13 ):
        {
            setPayloadType( rtpPayloadPCMU );
            break;
        }
        default:
            break;
    }

    return true;
}


void RtpPacket::printPacket ()
{
    //printBits (packetData, getTotalUsage());
    cerr << "\n-----------------------------------\n";
    cerr << "PacketAlloc: " << getPacketAlloc() << "  ";
    cerr << "PayloadSize: " << getPayloadSize() << "  ";
    cerr << "PayloadUsage: " << getPayloadUsage() << "  ";
    cerr << endl;
    cerr << "Unused: " << getUnused() << "  ";
    cerr << "TotalUsage: " << getTotalUsage();
    cerr << "\n-----------------------------------\n";
    cerr << getVersion() << " " << getPaddingFlag() << " " << getExtFlag() << " ";
    cerr << getCSRCcount() << " " << getMarkerFlag() << " ";
    cerr << (unsigned int)getPayloadType() << " ";
    cerr << getSequence() << "  " << getRtpTime() << "  " << getSSRC();
    cerr << "\n-----------------------------------\n";
    return ;
}
