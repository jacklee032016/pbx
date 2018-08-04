/*
 * $Id: RtcpReceiver.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
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
#include <map>

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




/* ----------------------------------------------------------------- */
/* --- rtpcReceiver Constructor ------------------------------------ */
/* ----------------------------------------------------------------- */

RtcpReceiver::RtcpReceiver (int localMinPort, int localMaxPort)
{
    myStack = new UdpStack(NULL, localMinPort, localMaxPort);
    freeStack = true;

    constructRtcpReceiver();
}

RtcpReceiver::RtcpReceiver (int localPort)
{
    myStack = new UdpStack(NULL, localPort) ;
    freeStack = true;

    constructRtcpReceiver();
}

RtcpReceiver::RtcpReceiver (UdpStack* udp)
{
    myStack = udp;
    freeStack = false;

    constructRtcpReceiver();
}


void RtcpReceiver::constructRtcpReceiver ()
{

    packetReceived = 0;

    accumOneWayDelay = 0;
    avgOneWayDelay = 0;
    accumRoundTripDelay = 0;
    avgRoundTripDelay = 0;
}


RtcpReceiver::~RtcpReceiver ()
{
    if (freeStack)
    {
        delete myStack;
        myStack = 0;
    }

    // must remove each transmitter block and each SDES info
    map < RtpSrc, RtpTranInfo* > ::iterator s = tranInfoList.begin();
    while (s != tranInfoList.end())
    {
        removeTranInfo((s->second)->ssrc);
        s = tranInfoList.begin();
    }
    //cpDebug(LOG_DEBUG_STACK, "RTCP: Receiver removed");
}



/* --- receive packet functions ------------------------------------ */


RtcpPacket* RtcpReceiver::getPacket ()
{
    // check for network activity
    fd_set netFD;
    FD_ZERO (&netFD);
    FD_SET (myStack->getSocketFD(), &netFD);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    int selret = select (myStack->getSocketFD() + 1,
                         &netFD, NULL, NULL, &timeout);
    if (selret <= 0)
    {
        // select error
        if (selret < 0) perror ("rtcpReceiver select");
        else
            // no network activity
            return NULL;
    }
    else
    {
        //cpDebug(LOG_DEBUG_STACK, "Get RTCP packet");
    }

    // create packet

    RtcpPacket* p = new RtcpPacket ();

    // receive packet
    //unsigned int fromLen = sizeof (txAddress);
    //int len = recvfrom (socketFD, p->getPacketData(), p->getPacketAlloc(),
    //                    0, (struct sockaddr*) &txAddress, &fromLen);
    //    int len = myStack->receive  (p->getPacketData(), p->getPacketAlloc());
    NetworkAddress sender;
    int len = myStack->receiveFrom (p->getPacketData(),
                                    p->getPacketAlloc(),
                                    &sender);
    //cpDebug(LOG_DEBUG_STACK, "RTCP receiveFrom: %s : %d", sender.getIpName().c_str(),
    //          sender.getPort());

    // ? RtpTime arrival = generateTime ();
    if (len <= 0)
    {
        delete p;
        p = NULL;
        return NULL;
    }
    p->setTotalUsage (len);

    // kudo
    //    p->printPacket();

    // check packet
    if (!isValid(p))
    {
        delete p;
        p = NULL;
        return NULL;
    }
    return p;
}



int RtcpReceiver::isValid (RtcpPacket* p)
{
    char* begin = reinterpret_cast < char* > (p->getPacketData());
    char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
    RtcpHeader* middle = reinterpret_cast < RtcpHeader* > (begin);

    // check if known payload type for first packet
    if (middle->type != rtcpTypeSR && middle->type != rtcpTypeRR)
        return 0;

    // check padbyte
    if (middle->padding)
        return 0;

    // check header lengths
    while (begin < end && (int)middle->version == RTP_VERSION)
    {
        begin += (ntohs(middle->length) + 1) * sizeof(RtpSrc);
        middle = reinterpret_cast < RtcpHeader* > (begin);
    }

    if (begin != end)
        return 0;

    // exit with success
    cpDebug(LOG_DEBUG_STACK, "RTCP packet is valid");
    //    cout << "RTCP packet is valid" << endl;
    return 1;
}



int RtcpReceiver::readRTCP (RtcpPacket* p)
{

    char* begin = reinterpret_cast < char* > (p->getPacketData());
    char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
    RtcpHeader* middle = NULL;
    int ret = 0;

    while (begin < end)
    {
        middle = reinterpret_cast < RtcpHeader* > (begin);
        switch (middle->type)
        {
            case (rtcpTypeSR):
                        case (rtcpTypeRR):
                            readSR (middle);
            break;
            case (rtcpTypeSDES):
                        readSDES (middle);
            break;
            case (rtcpTypeBYE):
                       if ( readBYE (middle) == 0)
                       {
                           ret = 1;
                       }
            break;
            case (rtcpTypeAPP):
                        readAPP (middle);
            break;
            default:
            cpLog (LOG_ERR, "RTCP: Unknown RTCP type");
            break;
        }
        begin += (ntohs(middle->length) + 1) * sizeof(u_int32_t);
    }
    return ret;
}



RtcpHeader* RtcpReceiver::findRTCP (RtcpPacket* p, RtcpType type)
{
    char* begin = reinterpret_cast < char* > (p->getPacketData());
    char* end = reinterpret_cast < char* > (begin + p->getTotalUsage());
    RtcpHeader* middle = NULL;

    while (begin < end)
    {
        middle = reinterpret_cast < RtcpHeader* > (begin);
        if (type == static_cast < RtcpType > (middle->type))
            return middle;
        begin += (ntohs(middle->length) + 1) * sizeof(u_int32_t);
    }

    // packet type not found
    cpLog (LOG_ERR, "RTCP: Type found here: %d", (int)type);
    return NULL;
}



/* --- Read SR RTCP packet ----------------------------------------- */

int RtcpReceiver::readSR (RtcpPacket* p)
{
    RtcpHeader* head = findRTCP (p, rtcpTypeSR);
    if (head == NULL) head = findRTCP (p, rtcpTypeRR);
    if (head == NULL) return -1;

    readSR (head);

    // read next RR packet if found
    // future: - ?

    return 0;
}


void RtcpReceiver::readSR (RtcpHeader* head)
{
    char* middle = NULL;

    NtpTime nowNtp = getNtpTime();

    // read SR block
    if (head->type == rtcpTypeSR)
    {
        RtcpSender* senderBlock = reinterpret_cast < RtcpSender* >
                                  ((char*)head + sizeof(RtcpHeader));
        RtpTranInfo* s = findTranInfo(ntohl(senderBlock->ssrc));
        s->lastSRTimestamp = (ntohl(senderBlock->ntpTimeSec) << 16 |
                              ntohl(senderBlock->ntpTimeFrac) >> 16);
        s->recvLastSRTimestamp = nowNtp;

        //printSR (senderBlock);  // - debug

        packetReceived++;

        NtpTime thenNtp ( ntohl(senderBlock->ntpTimeSec),
                          ntohl(senderBlock->ntpTimeFrac) );

        accumOneWayDelay += (nowNtp - thenNtp);

        avgOneWayDelay = accumOneWayDelay / packetReceived;

        middle = (char*)head + sizeof(RtcpHeader) + sizeof(RtcpSender);
    }
    else
    {
        // move over blank SR header
        middle = (char*)head + sizeof(RtcpHeader);

        // move over the ssrc of packet sender
        RtpSrc* sender = reinterpret_cast < RtpSrc* > (middle);
        RtpSrc ssrc;

        ssrc = ntohl(*sender);
        middle += sizeof(RtpSrc);

        packetReceived++;
    }


    // read RR blocks
    RtcpReport* block = reinterpret_cast < RtcpReport* > (middle);
    for (int i = head->count; i > 0; i--)
    {
        //printRR (block);  // - debug

        // - ? what these are if the count is more than 1??
        NtpTime thenNtp (ntohl(block->lastSRTimeStamp) >> 16,
                         ntohl(block->lastSRTimeStamp) << 16 );

        NtpTime nowNtp1 (nowNtp.getSeconds() & 0x0000FFFF, 
                         nowNtp.getFractional() & 0xFFFF0000);
        accumRoundTripDelay += ((nowNtp1 - thenNtp)
                                - ntohl(block->lastSRDelay));

        avgRoundTripDelay = accumRoundTripDelay / packetReceived;

        ++block;
    }

    // handle profile specific extensions
    // - ?
}




void RtcpReceiver::printSR (RtcpSender* p)
{
    cerr << "Got SR from " << ntohl(p->ssrc) << endl;
    cerr << "  NTP time: " << ntohl(p->ntpTimeSec) << " ";
    cerr << ntohl(p->ntpTimeFrac) << endl;
    cerr << "  RTP time: " << ntohl(p->rtpTime) << endl;
    cerr << "  Packets sent: " << ntohl(p->packetCount);
    cerr << "    Payload sent: " << ntohl(p->octetCount) << endl;
}


void RtcpReceiver::printRR (RtcpReport* p)
{
    cerr << "Got RR for " << ntohl(p->ssrc) << endl;
    cerr << "  Lost Frac: " << p->fracLost;
    u_int32_t lost = (p->cumLost[0] << 16) | (p->cumLost[1] << 8) | p->cumLost[2];
    lost = lost & (0xffffff);
    cerr << "  Lost count: " << lost;
    cerr << "  Cycles: " << ntohs(p->recvCycles);
    cerr << "  Last seq: " << ntohs(p->lastSeqRecv) << endl;
    cerr << "  Jitter: " << ntohl(p->jitter) << "  ";
    cerr << "Last SR: " << ntohl(p->lastSRDelay) << endl;
}



/* --- Read SDES packet -------------------------------------------- */

int RtcpReceiver::readSDES (RtcpPacket* p)
{
    RtcpHeader* head = findRTCP (p, rtcpTypeSDES);
    if (head == NULL) return -1;

    readSDES (head);

    // read next SDES packet if found
    // future: - ?

    return 0;
}


void RtcpReceiver::readSDES (RtcpHeader* head)
{
    char* begin = reinterpret_cast < char* > ((char*)head + sizeof(RtcpHeader));
    RtcpChunk* middle = reinterpret_cast < RtcpChunk* > (begin);

    RtcpSDESItem* item = NULL;
    RtcpSDESItem* nextitem = NULL;
    RtpSrc ssrc;

    for (int i = head->count; i > 0; i--)
    {
        ssrc = ntohl(middle->ssrc);

        for (item = &(middle->startOfItem); item->type; item = nextitem)
        {
            addSDESItem(ssrc, item);
            nextitem = reinterpret_cast < RtcpSDESItem* >
                       ((char*)item + sizeof(RtcpSDESItem) - 1 + item->length);
        }

        middle = reinterpret_cast < RtcpChunk* > (item);
    }
}



void RtcpReceiver::addSDESItem (RtpSrc src, RtcpSDESItem* item)
{
    RtpTranInfo* s = findTranInfo(src);

    switch (item->type)
    {
        case rtcpSdesCname:
        strncpy ((s->SDESInfo).cname, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesName:
        strncpy ((s->SDESInfo).name, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesEmail:
        strncpy ((s->SDESInfo).email, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesPhone:
        strncpy ((s->SDESInfo).phone, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesLoc:
        strncpy ((s->SDESInfo).loc, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesTool:
        strncpy ((s->SDESInfo).tool, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesNote:
        strncpy ((s->SDESInfo).note, &(item->startOfText), item->length + 1);
        break;
        case rtcpSdesPriv:
        // future: not implemented
        default:
        cpLog (LOG_ERR, "RtcpReceiver: SDES type unknown");
        break;
    }

    /*
    // - debug
    cerr <<"Update "<<src<<" with "<< (int) item->type <<" "<< (int) item->length;
    char output [255];
    memset (output, 0, 255);
    strncpy (output, &(item->startOfText), item->length+1);
    cerr << endl <<output<<endl;
    cerr <<"_SDES_";
    */
}



/* --- Read BYE packet --------------------------------------------- */

int RtcpReceiver::readBYE (RtcpPacket* p)
{
    RtcpHeader* head = findRTCP (p, rtcpTypeBYE);
    if (head == NULL) return -1;

    readBYE (head);

    // read next BYE packet if found
    // future: - ?

    return 0;
}


int RtcpReceiver::readBYE (RtcpHeader* head)
{

    //    char* end = reinterpret_cast<char*>
    //                ((char*)head + sizeof(RtpSrc) * (ntohs(head->length) + 1));
    RtpSrc* src = reinterpret_cast < RtpSrc* >
                  ((char*)head + sizeof(RtcpHeader));


    for (int i = head->count; i > 0; i--)
    {
        cpDebug(LOG_DEBUG_STACK, "readRtcpBye for %d", ntohl(*src) );
        //       cerr << "readRtcpBye for " << ntohl(*src) << endl;
        removeTranInfo (ntohl(*src++));
    }

    return 0;

    //TODO Convert this to cpLog if necessary
#if 0
    // print reason
    char* middle = reinterpret_cast < char* > (src);
    //    if (middle != end)
    {
        RtcpBye* reason = reinterpret_cast < RtcpBye* > (middle);
        cerr << "   Reason: ";    // - debug
        cerr.write(&(reason->startOfText), (int) reason->length);    // - debug
        cerr << endl;    // - debug
    }
    //cerr <<"_BYE_";  // - debug

#endif
}



/* --- Read APP packet --------------------------------------------- */

int RtcpReceiver::readAPP (RtcpPacket* p)
{
    RtcpHeader* head = findRTCP (p, rtcpTypeAPP);
    if (head == NULL) return -1;

    readAPP (head);

    // read next APP packet if found
    // future: - ?

    return 0;
}


void RtcpReceiver::readAPP (RtcpHeader* head)
{
    // future: not implemented
    assert (0);
}



/* --- known transmitter list functions ---------------------------- */

RtpTranInfo* RtcpReceiver::addTranInfo (RtpSrc src, RtpReceiver* recv)
{

    //    cout << "adding: " << src << endl;

    if (recv) assert (src == recv->ssrc);

    RtpTranInfo* s = new RtpTranInfo;
    s->recv = recv;
    s->ssrc = src;
    s->expectedPrior = 0;
    s->receivedPrior = 0;

    //    cout << "adding ptr: " << s << endl;

    if (addTranFinal (s))
    {
        delete s;  // - ?
        s = findTranInfo (src);
        assert (s->recv == NULL);  // - ?
        s->recv = recv;
    }
    return s;
}


int RtcpReceiver::addTranFinal (RtpTranInfo* s)
{
    // add transmitter to listing
    pair < map < RtpSrc, RtpTranInfo* > ::iterator, bool > result =
        tranInfoList.insert (pair < RtpSrc, RtpTranInfo* >
                             (s->ssrc, s));
    if (!result.second)
    {
        //transmitter already in listing
        return 1;
    }
    //cpLog (LOG_DEBUG_STACK, "RTCP: Transmitter add: %d", s->ssrc);
    return 0;
}



int RtcpReceiver::removeTranInfo (RtpSrc src, int flag)
{
    //    cout << "RTCP: removing: " << src << endl;

    map < RtpSrc, RtpTranInfo* > ::iterator p = tranInfoList.find(src);
    if (p == tranInfoList.end())
    {
        // src not found
        assert (0);
    }

    RtpTranInfo* info = p->second;

    //    cout << "RTCP: removing ptr: " << info << endl;

    // remove from RTP stack
    if (info->recv && !flag)
        (info->recv)->removeSource(info->ssrc, 1);
    info->recv = NULL;
    delete info; info = NULL;

    //    cout << "RTCP: done removing\n";

    // remove from receiver list
    tranInfoList.erase (p);

    //cpLog (LOG_DEBUG_STACK, "RTCP: Transmitter removed: %d", src);
    return 0;
}



RtpTranInfo* RtcpReceiver::findTranInfo (RtpSrc src)
{
    RtpTranInfo* info = NULL;

    map < RtpSrc, RtpTranInfo* > ::iterator p = tranInfoList.find(src);
    if (p == tranInfoList.end())
        // receiver not found, so add it
        info = addTranInfo(src);
    else
        info = p->second;

    return info;
}


RtpTranInfo* RtcpReceiver::getTranInfoList (int index)
{
    assert (index >= 0);
    assert (index < getTranInfoCount());

    map < RtpSrc, RtpTranInfo* > ::iterator p = tranInfoList.begin();
    for (int i = 0; i < index; i++)
        ++p;

    assert (p != tranInfoList.end());
    return p->second;
}


int RtcpReceiver::getTranInfoCount()
{
    return tranInfoList.size();
}



int RtcpReceiver::getPort ()
{
    return myStack->getRxPort();
};

int RtcpReceiver::getSocketFD ()
{
    return myStack->getSocketFD();
};
