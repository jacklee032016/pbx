/*
* $Log: RtcpPacket.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RtcpPacket.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>


// networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "global.h"
#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "NtpTime.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"


const int RtcpPacket::PACKETSIZE = 2048;


/* ----------------------------------------------------------------- */
/* --- rtcp Packet Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtcpPacket::RtcpPacket ()
{
    packetData = NULL;

    // create memory allocation
    packetAlloc = PACKETSIZE;
    assert (packetAlloc < RTP_MTU);
    packetData = new char[packetAlloc];
    assert (packetData);
    memset (packetData, 0, packetAlloc);
    unusedSize = packetAlloc;
}


RtcpPacket::~RtcpPacket ()
{
    delete []packetData; packetData = NULL;
}



/* --- Size and Locations ------------------------------------------ */

char* RtcpPacket::getPacketData ()
{
    return packetData;
}

char* RtcpPacket::freeData ()
{
    return packetData + (packetAlloc - unusedSize);
}

int RtcpPacket::allocData (int s)
{
    if (unusedSize < s)
    {
        //cpLog (LOG_ERR, "RTCP: Out of allocated memory for RTCP packet");
        return -1;
    }
    unusedSize -= s;
    memset (freeData(), 0, s);
    return s;
}


/*     unused memory    */

int RtcpPacket::getPacketAlloc ()
{
    return packetAlloc;
}

int RtcpPacket::getUnused ()
{
    return unusedSize;
}

void RtcpPacket::setTotalUsage (int size)
{
    assert (size <= packetAlloc);
    unusedSize = packetAlloc - size;
}

int RtcpPacket::getTotalUsage ()
{
    return packetAlloc - unusedSize;
}



/* --- Packet Header functions ------------------------------------- */

int RtcpPacket::getVersion ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->version;
}

int RtcpPacket::getPadbyteFlag ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->padding;
}

int RtcpPacket::getCount ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->count;
}

RtcpType RtcpPacket::getPayloadType ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return static_cast < RtcpType > (header->type);
}


int RtcpPacket::getLength ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return ntohs(header->length);
}


void RtcpPacket::printPacket ()
{
    printBits (packetData, getTotalUsage());
    cerr << "\n-----------------------------------\n";
}
