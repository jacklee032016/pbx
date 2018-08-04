/*
* $Id: tpTrunk30.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <iostream>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include "cpLog.h"

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "NtpTime.hxx"
#include "ixjuser.h"

#define ULAW_PAYLOAD 240


int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG);

    RtpPayloadType payloadType = rtpPayloadPCMU;
    char* buffer = new char [512];
    char* outbuffer = new char [512];
    int cc;

    if (argc != 4)
    {
        cerr << "Usage: tpTrunk host rtp_tran_port rtp_recv_port\n";
        return 1;
    }

    // open quicknet card
    // char* szDevice = "/dev/ixj0";
    char* szDevice = "/dev/phone0";
    printf("Opening Phone Jack (%s)\n", szDevice);
    int ixj = open(szDevice, O_RDWR);
    if (ixj < 0)
    {
        perror("Error opening voice card");
        return -1;
    }


    // open RTP stack
    RtpSession stack (argv[1], atoi(argv[2]), atoi(argv[3]),
                      atoi(argv[2]) + 1, atoi(argv[3]) + 1,
                      payloadType, -1);
    RtpPacket* inPacket = NULL;
    RtpPacket* outPacket = stack.createPacket(160);
    outPacket->setPayloadUsage (160);


    // wait for off hook
    printf("Waiting for hookswitch\n");
    if (ioctl(ixj, IXJCTL_HOOKSTATE) == 0)
        ioctl(ixj, IXJCTL_RING);
    while (ioctl(ixj, IXJCTL_HOOKSTATE) == 0);


    // open audio path
    printf("Starting audio\n");
    ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
    //ioctl(ixj,IXJCTL_FRAME,20);
    ioctl(ixj, IXJCTL_REC_START);
    ioctl(ixj, IXJCTL_PLAY_START);
    ioctl(ixj, IXJCTL_AEC_START);

    // continue audio path
    while (ioctl(ixj, IXJCTL_HOOKSTATE))
    {
        // receive 20ms packet r
        while (1)
        {
            if (ioctl(ixj, IXJCTL_HOOKSTATE) == 0)
                return 0;
            inPacket = stack.receive ();
            if (!inPacket) continue;
            if (inPacket->getPayloadUsage() != 160)
            {
                cerr << "RX";
                continue;
            }
            break;
        }
        memcpy (outbuffer, inPacket->getPayloadLoc(), 160);

        // read from card first 30ms part
        cc = read(ixj, buffer, 240);
        if (cc != 240) cerr << "M";

        // send 20ms packet x
        memcpy (outPacket->getPayloadLoc(), buffer, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);


        // receive 20ms packet s
        while (1)
        {
            if (ioctl(ixj, IXJCTL_HOOKSTATE) == 0)
                return 0;
            inPacket = stack.receive ();
            if (!inPacket) continue;
            if (inPacket->getPayloadUsage() != 160)
            {
                cerr << "SX";
                continue;
            }
            break;
        }
        memcpy (outbuffer + 160, inPacket->getPayloadLoc(), 160);


        // write to card first 30ms part
        write (ixj, outbuffer, 240);

        // read from card second 30ms part
        cc = read(ixj, buffer + 240, 240);
        if (cc != 240) cerr << "N";

        // send 20ms packet y
        memcpy (outPacket->getPayloadLoc(), buffer + 160, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);

        // receive 20ms packet t
        while (1)
        {
            inPacket = stack.receive ();
            if (!inPacket) continue;
            if (inPacket->getPayloadUsage() != 160)
            {
                cerr << "TX";
                continue;
            }
            break;
        }
        memcpy (outbuffer + 160 + 160, inPacket->getPayloadLoc(), 160);

        // write to card second 30ms part
        write (ixj, outbuffer + 240, 240);

        // send 20ms packet z
        memcpy (outPacket->getPayloadLoc(), buffer + 160 + 160, 160);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 160);
        stack.transmit (outPacket);
        //        cerr <<"z";


        // send and receive RTCP packet
        // seperate to debug when packets are sent and recv
        /*
                if (stack.checkIntervalRTCP())
                {
                    stack.transmitRTCP();
                    cerr <<"A";
                }
                if (!stack.receiveRTCP())
                    cerr <<"B";
        */

    }
    stack.transmitRTCPBYE();

    ioctl(ixj, IXJCTL_REC_STOP);
    ioctl(ixj, IXJCTL_PLAY_STOP);
    ioctl(ixj, IXJCTL_AEC_STOP);
    close(ixj);

    return 0;
}
