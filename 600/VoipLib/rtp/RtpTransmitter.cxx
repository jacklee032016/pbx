/*
 * $Id: RtpTransmitter.cxx,v 1.3 2007/06/05 11:33:25 lizhijie Exp $
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


// network socket
//#include <netinet/in.h>                // struct socketaddr_in
//#include <sys/socket.h>
//#include <netdb.h>

#include "cpLog.h"
#include "vsock.hxx"
#include "NetworkAddress.h"

#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "rtpCodec.h"

const int RtpTransmitter::OUT_BUFFER_SIZE = 8192;


/* ----------------------------------------------------------------- */
/* --- RtpTransmitter Constructor ---------------------------------- */
/* ----------------------------------------------------------------- */

RtpTransmitter::RtpTransmitter (const char* remoteHost,
                                int remoteMinPort, int remoteMaxPort,
                                RtpPayloadType newApiFormat,
                                RtpPayloadType newNetworkFormat,
                                RtpReceiver* receiver)
{
    NetworkAddress netAddress;

    if(  remoteHost )
    {
        netAddress.setHostName(remoteHost);
        netAddress.setPort(remoteMinPort);
    }

    if(  receiver )
    {
        myStack = receiver->getUdpStack();
        myStack->setDestination(&netAddress);
        remoteAddr = netAddress;
        // assume that RTP will always use send instead of sendto
        // so that it can only receive from/send to the same remote port
        //	myStack->connectPorts();
        freeStack = false;
    }
    else
    {
        myStack = new UdpStack (&netAddress, remoteMinPort,
                                remoteMaxPort, sendonly) ;
        remoteAddr = netAddress;
        //	myStack->connectPorts();
        freeStack = true;
    }
	printf("in construct remote %s\n",remoteAddr.getIpName().c_str());
    constructRtpTransmitter (newApiFormat, newNetworkFormat);
}


RtpTransmitter::RtpTransmitter (const char* remoteHost, int remotePort,
                                RtpPayloadType newApiFormat,
                                RtpPayloadType newNetworkFormat,
                                RtpReceiver* receiver)
{
    NetworkAddress netAddress;

    if(  remoteHost )
    {
        netAddress.setHostName(remoteHost);
        netAddress.setPort(remotePort);
    }

    if(  receiver )
    {
        myStack = receiver->getUdpStack();
        myStack->setDestination(&netAddress);
        remoteAddr = netAddress;
		cpDebug(LOG_DEBUG, "*******************set remote addr  = %s***********\n", remoteAddr.getIpName().c_str());
        //	myStack->connectPorts();
        freeStack = false;
    }
    else
    {
        myStack = new UdpStack (&netAddress, remotePort,
                                remotePort, sendonly) ;
        remoteAddr = netAddress;
		cpDebug(LOG_DEBUG, "*******************set remote addr  = %s***********\n", remoteAddr.getIpName().c_str());
        //	myStack->connectPorts();
        freeStack = true;
    }
    constructRtpTransmitter (newApiFormat, newNetworkFormat);
}


void RtpTransmitter::constructRtpTransmitter (RtpPayloadType newApiFormat,
        RtpPayloadType newNetworkFormat)
{
    outPos = 0;
    recPos = 0;
    memset (outBuff, 0, OUT_BUFFER_SIZE);

    // set format and baseSampleRate
    setApiFormat(newApiFormat, 160, 0, NULL, false);
    setNetworkFormat(newNetworkFormat, 160, 0, NULL, false);

    // set private variables
    ssrc = generateSRC();
    seedNtpTime = getNtpTime();
    seedRtpTime = generate32();
    prevNtpTime = seedNtpTime;
    prevRtpTime = seedRtpTime;
    prevSequence = generate32();

    // set counters
    packetSent = 0;
    payloadSent = 0;
    codecString[0] = '\0';

    cpDebug(LOG_DEBUG_STACK, "Constructed ssrc = %d", ssrc);
}


RtpTransmitter::~RtpTransmitter ()
{
    if( freeStack)
    {
        delete myStack;
        myStack = NULL;
    }
    cpDebug(LOG_DEBUG_STACK, "Closed ssrc = %d", ssrc);
}

void
RtpTransmitter::setRemoteAddr (const NetworkAddress& theAddr)
{
	 remoteAddr = theAddr;
}

/* --- send packet functions --------------------------------------- */

RtpPacket* RtpTransmitter::createPacket (int npadSize, int csrc_count)
{
    // create packet
    RtpPacket* packet = new RtpPacket (apiFormat_payloadSize, npadSize, csrc_count);
    assert (packet);

    // load packet
    packet->setSSRC (ssrc);
    packet->setPayloadType (apiFormat);

    return packet;
}


// takes api RTP packet and send to network
// assumes payload size is already set
int RtpTransmitter::transmit(RtpPacket* packet, bool eventFlag )
{
    if( !packet )
    {
        cpLog(LOG_ERR,"Attempting to transmit a NULL rtp packet");
        return -1;
    }

    RtpPacket* p = packet;

    // convert codec
    if( p->getPayloadType() != networkFormat  &&  !eventFlag )
    {
#ifndef __sparc
        // replace p with a new packet
        p = convertRtpPacketCodec (networkFormat, packet);
        assert (packet->getSequence() == p->getSequence());
#endif
    }

    rtp_htonl(p);


    // finish packet
    if( p->getPayloadUsage() != networkFormat_payloadSize  &&  !eventFlag )
    {
        cpDebug(LOG_DEBUG_STACK,"Sending mismatched packetSize(%d) to networkFormatSize(%d)",
              p->getPayloadUsage(), networkFormat_payloadSize);
    }
    if( !p->timestampSet )
        p->setRtpTime( prevRtpTime + network_pktSampleSize );
    if( !p->sequenceSet )
        p->setSequence( prevSequence + 1 );
    if( p->getPayloadUsage() < 0  ||  p->getPayloadUsage() > 1012 )
    {
        cpDebug(LOG_DEBUG_STACK,"Invalid data packet size %d", p->getPayloadUsage());
        return -1;
    }

    //set marker once
    if( markerOnce )
    {
        cpDebug(LOG_DEBUG_STACK,"Setting marker bit once");
        p->setMarkerFlag(1);
        markerOnce = false;
    }

    // for packet reuse
    p->timestampSet = false;
    p->sequenceSet = false;

    // transmit packet
    try
    {
        myStack->transmitTo( (char*)p->getHeader(), p->getTotalUsage(), &remoteAddr );
	//printf("send to %s\n",remoteAddr.getIpName().c_str());
    }
    catch( UdpStackExceptionConectionRefused& )
    {
        cpLog (LOG_ERR,"Connection refused");
        return -1;
    }
   
    // update counters
    packetSent++;
    prevSequence = p->getSequence();
    if( !eventFlag )
    {
        payloadSent += p->getPayloadUsage();
        prevNtpTime = getNtpTime();
        prevRtpTime = p->getRtpTime();
    }

    // set up return value
    int result = p->getPayloadUsage();

    // delete newly created packet
    if( ( p != packet )  &&  p )
    {
        delete p; p = NULL;
    }

    // exit with success
    return result;
}


// takes rawdata, buffers it, and send network packets
int RtpTransmitter::transmitRaw (char* data, int len)
{
    assert (data);
    assert(len >= 0);

    int len1;

    char *data_buf = 0;
    char *buffer = 0;
    // convert codec
    if( apiFormat != networkFormat)
    {
        buffer = new char[1012];
        len = convertCodec(apiFormat, networkFormat, data, buffer, len);
        data_buf = buffer;
    }
    else
    {
       data_buf = data;
    }

    assert (data_buf);

    // write packet to output buffer
    if( (outPos + len) < OUT_BUFFER_SIZE)
    {
        memcpy (outBuff + outPos, data_buf, len);
        outPos += len;
    }
    else
    {
        // circular memory copy
        len1 = OUT_BUFFER_SIZE - outPos;
        memcpy (outBuff + outPos, data_buf, len1);
        memcpy (outBuff, data_buf + len1, len - len1);
        outPos = len - len1;
    }


    // check if enough data to send out packet
    int packetSize = networkFormat_payloadSize;
    if(  (outPos == 0) && (recPos == 0) )
    {
        cpLog (LOG_DEBUG_STACK, "Buffer is empty");
        return 0;
    }
    if(  ((outPos + OUT_BUFFER_SIZE - recPos) % OUT_BUFFER_SIZE) < packetSize )
    {
        cpLog (LOG_DEBUG_STACK, "Not enough data for a network packet.  Need %d", packetSize);
        return 0;
    }


    // send out packets from buffer
    int result = 0;

    // create packet
    RtpPacket* p = new RtpPacket (networkFormat_payloadSize);
    assert (p);
    p->setSSRC (ssrc);
    p->setPayloadType (networkFormat);
    p->setPayloadUsage (packetSize);

    //fill packets
    while ( ((outPos + OUT_BUFFER_SIZE - recPos) % OUT_BUFFER_SIZE) >= packetSize )
    {
        if(  (recPos + packetSize) < OUT_BUFFER_SIZE)
        {
            memcpy (p->getPayloadLoc(), outBuff + recPos, packetSize);
            recPos += packetSize;
        }
        else
        {
            // circular memory write
            len1 = OUT_BUFFER_SIZE - recPos;
            memcpy (p->getPayloadLoc(), outBuff + recPos, len1);
            memcpy (p->getPayloadLoc() + len1, outBuff, packetSize - len1);
            recPos = packetSize - len1;
        }
        // finish packet
        result += transmit(p);
    }
    if( p) delete p;
    p = 0;

    if( buffer != 0 )
    {
	delete [] buffer;
    }

    // exit with success
    return result;
}



void RtpTransmitter::setNetworkFormat (RtpPayloadType newtype, int no_samples, int packetSize,
                                       RtpPacket* p, bool print)
{
    networkFormat = newtype;
    network_pktSampleSize = no_samples;
    networkFormat_perSampleSize = 1;

    switch (newtype)
    {
        case rtpPayloadPCMU:
        case rtpPayloadPCMA:
        if( print)
	{
			cpDebug(LOG_DEBUG, "Setting network format to: PCMU %d", no_samples);
        }
        networkFormat_clockRate = 8000;
        break;
        case rtpPayloadL16_mono:
        if( print)
        	{
			cpDebug(LOG_DEBUG, "Setting network format to: L16 %d", no_samples);
        	}
        networkFormat_clockRate = 44100;
        networkFormat_perSampleSize = 2;
        break;
        case rtpPayloadG729:
        if( print)
		{
			cpDebug(LOG_DEBUG, "Setting network format to: G729 %d", no_samples);
        	}
        networkFormat_clockRate = 8000;
        break;
        default:
	{
        cpLog(LOG_ERR, "networkFormat: codec(%d) samples(%d) packetSize(%d)", (int)networkFormat, no_samples, packetSize);
        networkFormat_clockRate = 8000;
        }
    }
    if( p )
        networkFormat_payloadSize = p->getPayloadUsage();
    else
        networkFormat_payloadSize = network_pktSampleSize * networkFormat_perSampleSize;

    if( packetSize != 0 )
        networkFormat_payloadSize = packetSize;
}


void RtpTransmitter::setApiFormat (RtpPayloadType newtype, int no_samples, int packetSize,
                                   RtpPacket* p, bool print)
{
    apiFormat = newtype;
    api_pktSampleSize = no_samples;
    apiFormat_perSampleSize = 1;

    switch( newtype )
    {
    case rtpPayloadPCMU:
    case rtpPayloadPCMA:
        if( print )
		{
			cpDebug(LOG_DEBUG, "Setting api format to: PCMU %d", no_samples);
        	}
        apiFormat_clockRate = 8000;
        break;
    case rtpPayloadL16_mono:
        if( print )
		{
			cpDebug(LOG_DEBUG, "Setting api format to: L16 %d", no_samples);
        	}
        apiFormat_clockRate = 44100;
        apiFormat_perSampleSize = 2;
        break;
    case rtpPayloadG729:
        if( print )
		{
			cpDebug(LOG_DEBUG, "Setting api format to: G729 %d", no_samples);
        	}
        apiFormat_clockRate = 8000;
        break;
    default:
	{
        cpLog(LOG_ERR, "apiFormat: codec(%d) samples(%d) packetSize(%d)", (int)apiFormat, no_samples, packetSize);
        apiFormat_clockRate = 8000;
    	}
    }

    if( p )
        apiFormat_payloadSize = p->getPayloadUsage();
    else
        apiFormat_payloadSize = api_pktSampleSize * apiFormat_perSampleSize;

    if( packetSize != 0 )
        apiFormat_payloadSize = packetSize;
}

void RtpTransmitter::setCodecString(const char* codecStringInput)
{
    strncpy(codecString, codecStringInput, strlen(codecStringInput) + 1);
    cpDebug(LOG_DEBUG, "set CodecString %s", codecString);
}

