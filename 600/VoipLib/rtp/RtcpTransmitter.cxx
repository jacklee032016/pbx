/*
 * $Id: RtcpTransmitter.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
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

#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "NtpTime.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"


const int RtcpTransmitter::RTCP_INTERVAL = 5000;


/* ----------------------------------------------------------------- */
/* --- RtcpTransmitter Constructor --------------------------------- */
/* ----------------------------------------------------------------- */

RtcpTransmitter::RtcpTransmitter (const char* remoteHost,
                                  int remoteMinPort,
                                  int remoteMaxPort,
                                  RtcpReceiver* receiver)
{
    NetworkAddress netAddress;

    if ( remoteHost )
    {
        netAddress.setPort(remoteMinPort);
        netAddress.setHostName(remoteHost);
    }

    if (receiver)
    {
        myStack = receiver->getUdpStack();
        myStack->setDestination(&netAddress);
        remoteAddr = netAddress;
        //        myStack->connectPorts();
        freeStack = false;
    }
    else
    {
        myStack = new UdpStack(&netAddress, remoteMinPort, remoteMaxPort,
                               sendonly) ;
        remoteAddr = netAddress;
        //        myStack->connectPorts();
        freeStack = true;
    }

    constructRtcpTransmitter ();
}

RtcpTransmitter::RtcpTransmitter (const char* remoteHost,
                                  int remotePort,
                                  RtcpReceiver* receiver)
{
    NetworkAddress netAddress;

    if ( remoteHost )
    {
        netAddress.setHostName(remoteHost);
        netAddress.setPort(remotePort);
    }

    if (receiver)
    {
        myStack = receiver->getUdpStack();
        myStack->setDestination(&netAddress);
        remoteAddr = netAddress;
        //        myStack->connectPorts();
        freeStack = false;
    }
    else
    {
        myStack = new UdpStack(&netAddress, remotePort, remotePort,
                               sendonly) ;
        remoteAddr = netAddress;
        //        myStack->connectPorts();
        freeStack = true;
    }

    constructRtcpTransmitter ();
}


void RtcpTransmitter::constructRtcpTransmitter ()
{
    tran = NULL;
    recv = NULL;
    rtcpRecv = NULL;
    SDESInfo = NULL;

    // prepare for rtcp timing intervals
    nextInterval = getNtpTime();
    updateInterval();
}


RtcpTransmitter::~RtcpTransmitter ()
{
    if (freeStack)
    {
        delete myStack;
        myStack = 0;
    }

    if ((tran) && (SDESInfo))
    {
        delete SDESInfo;
        SDESInfo = NULL;
    }

    tran = NULL;
    recv = NULL;
    rtcpRecv = NULL;
}

void
RtcpTransmitter::setRemoteAddr (const NetworkAddress& theAddr)
{
    remoteAddr = theAddr;
}

/* --- send packet functions --------------------------------------- */

int RtcpTransmitter::transmit (RtcpPacket* p)
{
    //    NetworkAddress* toAddress = myStack->getDestinationHost();
    // transmit packet
    //int result = sendto (socketFD, p->getPacketData(), p->getTotalUsage(),
    //             0, (struct sockaddr*) &rxAddress, sizeof(rxAddress));
    //    myStack->transmit (p->getPacketData(), p->getTotalUsage());
    myStack->transmitTo ((char*)p->getPacketData(),
                         p->getTotalUsage(),
                         &remoteAddr);
    /*
        if (toAddress)
        {
            delete toAddress;
            toAddress = NULL;
        }
    */



    // exit with sucess
    return 0;
}


void RtcpTransmitter::updateInterval ()
{
    // RTCP_INTERVAL random offset between (.5 to 1.5)
    int delayMs = RTCP_INTERVAL * (500 + rand() / (RAND_MAX / 1000)) / 1000;
    nextInterval = nextInterval + delayMs;
}


int RtcpTransmitter::checkInterval ()
{
    if (getNtpTime() > nextInterval)
    {
        // prepare for next interval
        updateInterval();
        return 1;
    }

    // time not up yet
    return 0;
}




/* --- SR/RR RTCP report packets------------------------------------ */

int RtcpTransmitter::addSR (RtcpPacket* p, int npadSize)
{
    // header
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (p->freeData());
    int usage = p->allocData (sizeof(RtcpHeader));

    header->version = RTP_VERSION;
    header->padding = (npadSize > 0) ? 1 : 0;
    header->count = 0;
    header->type = (tran) ? rtcpTypeSR : rtcpTypeRR;

    NtpTime nowNtp = getNtpTime();

    // sender information
    if (tran)
    {
        //cpLog (LOG_DEBUG_STACK, "RTCP: Making Sender Info");
        RtcpSender* senderInfo = reinterpret_cast < RtcpSender* > (p->freeData());
        usage += p->allocData (sizeof(RtcpSender));

        int diffNtp = 0;
        if (nowNtp > tran->seedNtpTime)
            diffNtp = nowNtp - tran->seedNtpTime;
        else
            if (tran->seedNtpTime > nowNtp)
                diffNtp = tran->seedNtpTime - nowNtp;

        RtpTime diffRtp = (diffNtp * tran->networkFormat_clockRate) / 1000;
        senderInfo->ssrc = htonl(tran->ssrc);
        senderInfo->ntpTimeSec = htonl(nowNtp.getSeconds());
        senderInfo->ntpTimeFrac = htonl(nowNtp.getFractional());
        senderInfo->rtpTime = htonl(tran->seedRtpTime + diffRtp);
        senderInfo->packetCount = htonl(tran->packetSent);
        senderInfo->octetCount = htonl(tran->payloadSent);
    }
    else
    {
        RtcpChunk* chunk = reinterpret_cast < RtcpChunk* > (p->freeData());
        usage += p->allocData (sizeof(RtpSrc));
        chunk->ssrc = 0 ;  /* if recv only, give src 0 for receiver for now */
    }

    // report blocks
    if ((rtcpRecv) && (rtcpRecv->getTranInfoCount() > 0))
    {
        //cpLog (LOG_DEBUG_STACK, "RTCP: Making Report Block");
        RtpTranInfo* tranInfo = NULL;
        RtpReceiver* recvInfoSpec = NULL;
        RtcpReport* reportBlock = NULL;
        for (int i = 0; i < rtcpRecv->getTranInfoCount(); i++)
        {
            tranInfo = rtcpRecv->getTranInfoList(i);
            recvInfoSpec = tranInfo->recv;

            // only receieved RTCP packets from transmitter
            if (recvInfoSpec == NULL)
                continue;

            // don't report on probation transmitters
            if (recvInfoSpec->probation < 0)
                continue;

            //cpLog (LOG_DEBUG_STACK, "RTCP:  Report block for src %d",
            //       recvInfoSpec->ssrc);
            reportBlock = reinterpret_cast < RtcpReport* > (p->freeData());
            usage += p->allocData (sizeof(RtcpReport));

            reportBlock->ssrc = htonl(recvInfoSpec->ssrc);
            reportBlock->fracLost = calcLostFrac(tranInfo);
            u_int32_t lost = (calcLostCount(tranInfo)) & 0xffffff;
            reportBlock->cumLost[2] = lost & 0xff;
            reportBlock->cumLost[1] = (lost & 0xff00) >> 8;
            reportBlock->cumLost[0] = (lost & 0xff0000) >> 16;
            reportBlock->recvCycles = htons(recvInfoSpec->recvCycles);
            reportBlock->lastSeqRecv = htons(recvInfoSpec->prevSeqRecv);

            // fracational
            // reportBlock->jitter = htonl((u_int32_t)recvInfoSpec->jitter);

            // interger
            //            if (recvInfoSpec->jitter > 0)
            reportBlock->jitter = htonl(recvInfoSpec->jitter >> 4);


            reportBlock->lastSRTimeStamp = htonl(tranInfo->lastSRTimestamp);

            // reportBlock->lastSRDelay in the unit of 1/65536 of sec ??
            // currently it is in ms
            if (tranInfo->lastSRTimestamp == 0)
                reportBlock->lastSRDelay = 0;
            else
            {
                NtpTime thenNtp = tranInfo->recvLastSRTimestamp;
                //                NtpTime thenNtp ((tranInfo->lastSRTimestamp >> 16) |
                //                                 (nowNtp.getSeconds() & 0xffff0000),
                //                                 tranInfo->lastSRTimestamp << 16);
                reportBlock->lastSRDelay = 0;
                if (nowNtp > thenNtp)
                    reportBlock->lastSRDelay = htonl(nowNtp - thenNtp);
                else
                    reportBlock->lastSRDelay = 0;
            }
            // next known transmitter
            header->count++;
        }
    }

    // profile-specific extensions
    // future: not implemented


    // padding
    if (npadSize > 0)
    {
        // future: not implemented
        assert (0);
    }

    // overall packet must ends on 32-bit count
    assert (usage % 4 == 0);

    header->length = htons((usage / 4) - 1);
    //cpLog (LOG_DEBUG_STACK, "RTCP:  SR/RR packet used %d bytes/ %d words",
    //       usage, usage/4);
    return usage;
}



u_int32_t RtcpTransmitter::calcLostFrac (RtpTranInfo* s)
{
    /* from A.3 of RFC 1889 - RTP/RTCP Standards */

    RtpReceiver* r = s->recv;

    u_int32_t expected = ((r->recvCycles + r->prevSeqRecv) - r->seedSeq + 1);
    u_int32_t expected_interval, received_interval, lost_interval;

    expected_interval = expected - s->expectedPrior;
    s->expectedPrior = expected;
    received_interval = r->packetReceived - s->receivedPrior;
    s->receivedPrior = r->packetReceived;
    lost_interval = expected_interval - received_interval;

    u_int32_t fraction;
    if (expected_interval == 0 || lost_interval <= 0) fraction = 0;
    else fraction = (lost_interval << 8) / expected_interval;

    return fraction;
}


u_int32_t RtcpTransmitter::calcLostCount (RtpTranInfo* s)
{
    /* from A.3 of RFC 1889 - RTP/RTCP Standards */

    RtpReceiver* r = s->recv;

    u_int32_t expected = ((r->recvCycles + r->prevSeqRecv) - r->seedSeq + 1);
    return expected - r->packetReceived;
}




/* --- SDES RTCP packet -------------------------------------------- */

int RtcpTransmitter::addSDES (RtcpPacket* p, RtcpSDESType item, int npadSize)
{
    if (!tran) return -1;

    RtcpSDESType list[2];
    list[0] = item;
    list[1] = rtcpSdesEnd;
    return addSDES (p, list, npadSize);
}


int RtcpTransmitter::addSDES (RtcpPacket* p, int npadSize)
{
    if (!tran) return -1;

    RtcpSDESType list[8];
    int i = 0;

    if (strlen(getSdesCname()) > 0) list[i++] = rtcpSdesCname;
    if (strlen(getSdesName()) > 0) list[i++] = rtcpSdesName;
    if (strlen(getSdesEmail()) > 0) list[i++] = rtcpSdesEmail;
    if (strlen(getSdesPhone()) > 0) list[i++] = rtcpSdesPhone;
    if (strlen(getSdesLoc ()) > 0) list[i++] = rtcpSdesLoc ;
    if (strlen(getSdesTool ()) > 0) list[i++] = rtcpSdesTool ;
    if (strlen(getSdesNote ()) > 0) list[i++] = rtcpSdesNote ;

    list[i] = rtcpSdesEnd;
    return addSDES (p, list, npadSize);
}


int RtcpTransmitter::addSDES (RtcpPacket* p, RtcpSDESType* SDESlist,
                              int npadSize)
{
    if (!tran) return -1;

    // header
    //cpLog (LOG_DEBUG_STACK, "RTCP: Making SDES packet");
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (p->freeData());
    int usage = p->allocData (sizeof(RtcpHeader));

    header->version = RTP_VERSION;
    header->padding = (npadSize > 0) ? 1 : 0;
    header->count = 1;
    header->type = rtcpTypeSDES;

    // only sends the sender's SDE

    // SDES chunk
    RtcpChunk* chunk = reinterpret_cast < RtcpChunk* > (p->freeData());
    usage += p->allocData (sizeof(RtpSrc));
    /*
    cout << "sizeof(RtcpChunk) =" << sizeof(RtcpChunk) << endl; // ?? should be 7
    */
    chunk->ssrc = htonl(tran->ssrc);

    // SDES items
    RtcpSDESItem* item = NULL;
    for (int i = 0; SDESlist[i] != rtcpSdesEnd; i++)
    {
        //cpLog (LOG_DEBUG_STACK, "RTCP:  Adding SDES %d", SDESlist[i]);
        item = reinterpret_cast < RtcpSDESItem* > (p->freeData());
        usage += p->allocData (sizeof(RtcpSDESItem) - 1);

        int len = 0;

        switch (SDESlist[i])
        {
            case rtcpSdesCname:
            strcpy(&(item->startOfText), getSdesCname());
            len = strlen(getSdesCname());
            //cpLog (LOG_DEBUG_STACK, "RTCP:  SDES Item Length: %d", len);
            //cpLog (LOG_DEBUG_STACK, "RTCP:  SDES Item Value: %s", getSdesCname());
            break;
            case rtcpSdesName:
            strcpy(&(item->startOfText), getSdesName());
            len = strlen(getSdesName());
            break;
            case rtcpSdesEmail:
            strcpy(&(item->startOfText), getSdesEmail());
            len = strlen(getSdesEmail());
            break;
            case rtcpSdesPhone:
            strcpy(&(item->startOfText), getSdesPhone());
            len = strlen(getSdesPhone());
            break;
            case rtcpSdesLoc:
            strcpy(&(item->startOfText), getSdesLoc());
            len = strlen(getSdesLoc());
            break;
            case rtcpSdesTool:
            strcpy(&(item->startOfText), getSdesTool());
            len = strlen(getSdesTool());
            break;
            case rtcpSdesNote:
            strcpy(&(item->startOfText), getSdesNote());
            len = strlen(getSdesNote());
            break;
            case rtcpSdesPriv:
            // future: not implemented
            assert (0);
            break;
            default:
            cpLog (LOG_ERR, "RtcpTransmitter:  SDES type unknown");
            assert (0);
            break;
        }
        item->type = SDESlist[i];

        // strlen removes the null that was copied
        item->length = len;
        usage += p->allocData (item->length);
    }

    // ending SDES item
    item = reinterpret_cast < RtcpSDESItem* > (p->freeData());
    usage += p->allocData (sizeof(RtcpSDESItem) - 1);
    item->type = rtcpSdesEnd;
    item->length = 0;

    // padding
    if (npadSize > 0)
    {
        // future: not implemented
        assert (0);
    }

    // end packet on 32-bit count
    if (usage % 4 != 0)
    {
        //cpLog (LOG_DEBUG_STACK, "RTCP:  SDES padded by: %d", 4-usage%4);
        usage += p->allocData (4 - usage % 4);
    }

    header->length = htons((usage / 4) - 1);
    //cpLog (LOG_DEBUG_STACK, "RTCP:  SDES packet used %d bytes/ %d words", usage, usage/4);
    return usage;
}



/* --- BYE RTCP packet --------------------------------------------- */

int RtcpTransmitter::addBYE (RtcpPacket* p, char* reason, int npadSize)
{
    if (!tran) return -1;

    RtpSrc list[1];
    list[0] = tran->getSSRC();
    return addBYE (p, list, 1, reason, npadSize);
}


int RtcpTransmitter::addBYE (RtcpPacket* p, RtpSrc* lst, int count,
                             char* reason, int npadSize)
{
    assert (count > 0);

    // header
    //cpLog (LOG_DEBUG_STACK, "RTCP: Making BYE packet");
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (p->freeData());
    int usage = p->allocData (sizeof(RtcpHeader));

    header->version = RTP_VERSION;
    header->padding = (npadSize > 0) ? 1 : 0;
    header->count = count;
    header->type = rtcpTypeBYE;

    // transmitter leaving
    RtpSrc* s = NULL;
    for (int i = 0; i < count; i++)
    {
        s = reinterpret_cast < RtpSrc* > (p->freeData());
        usage += p->allocData (sizeof(RtpSrc));
        *s = htonl(lst[i]);
        //cpLog (LOG_DEBUG_STACK, "RTCP:  SRC: %d", list[i]);
    }

    // reason - optional

    if (reason)
    {
        RtcpBye* byeReason = reinterpret_cast < RtcpBye* > (p->freeData());
        usage += p->allocData (sizeof(RtcpBye) - 1);

        byeReason->length = strlen(reason);
        strncpy (&(byeReason->startOfText), reason, byeReason->length);
        usage += p->allocData (byeReason->length);
        //cpLog (LOG_DEBUG_STACK, "RTCP:  Reason: %s", reason);
    }

    // padding

    if (npadSize > 0)
    {
        // future: not implemented
        assert (0);
    }

    // end packet on 32-bit count
    if (usage % 4 != 0)
    {
        //cpLog (LOG_DEBUG_STACK, "RTCP:  BYE padded by: %d", 4-usage%4);
        usage += p->allocData (4 - usage % 4);
    }

    header->length = htons((usage / 4) - 1);
    //cpLog (LOG_DEBUG_STACK, "RTCP:  BYE packet used %d bytes/ %d words",
    //       usage, usage/4);
    return usage;
}



/* --- APP RTCP packet --------------------------------------------- */

int RtcpTransmitter::addAPP (RtcpPacket* packet, int npadSize)
{
    // future: not implemented
    assert (0);
    return -1;
}



/* --- SDES Information -------------------------------------------- */

void RtcpTransmitter::setSdesCname ()
{
    char user[64] = "unknown_user";
    char hn[64] = "uknown_host";
    char cnameres [64 + 20 + 64];

#if 0
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL)
    {
        strncpy(user, pw->pw_name, sizeof(user));
        user[63] = (char)0;
    }
#endif  // 0
    gethostname (hn, sizeof(hn));
    sprintf (cnameres, "%s.%d@%s", user, getpid(), hn);

    assert (strlen(cnameres) < 255);
    strcpy (SDESInfo->cname, cnameres);
}


void RtcpTransmitter::setSdesName (char* text)
{
    assert (strlen(text) < 255);
    strcpy (SDESInfo->name, text);
}

void RtcpTransmitter::setSdesEmail (char* text)
{
    assert (strlen(text) < 255);
    strcpy (SDESInfo->email, text);
}

void RtcpTransmitter::setSdesPhone (char* text)
{
    assert (strlen(text) < 256);
    strcpy (SDESInfo->phone, text);
}

void RtcpTransmitter::setSdesLoc (char* text)
{
    assert (strlen(text) < 255);
    strcpy (SDESInfo->loc, text);
}

void RtcpTransmitter::setSdesTool (char* text)
{
    assert (strlen(text) < 255);
    strcpy (SDESInfo->tool, text);
}

void RtcpTransmitter::setSdesNote (char* text)
{
    assert (strlen(text) < 255);
    strcpy (SDESInfo->note, text);
}


char* RtcpTransmitter::getSdesCname ()
{
    return SDESInfo->cname;
}

char* RtcpTransmitter::getSdesName ()
{
    return SDESInfo->name;
}

char* RtcpTransmitter::getSdesEmail ()
{
    return SDESInfo->email;
}

char* RtcpTransmitter::getSdesPhone ()
{
    return SDESInfo->phone;
}

char* RtcpTransmitter::getSdesLoc ()
{
    return SDESInfo->loc;
}

char* RtcpTransmitter::getSdesTool ()
{
    return SDESInfo->tool;
}

char* RtcpTransmitter::getSdesNote ()
{
    return SDESInfo->note;
}




/* --- misc functions ---------------------------------------------- */

void RtcpTransmitter::setRTPtran (RtpTransmitter* s)
{
    if (SDESInfo == NULL)
        SDESInfo = new SDESdata;

    tran = s;
}

void RtcpTransmitter::setRTPrecv (RtpReceiver* s)
{
    recv = s;
}

void RtcpTransmitter::setRTCPrecv (RtcpReceiver* s)
{
    rtcpRecv = s;
}

int RtcpTransmitter::getPort ()
{
    return myStack->getTxPort();
};

int RtcpTransmitter::getSocketFD ()
{
    return myStack->getSocketFD();
};
