/*
* $Id: tpTrunk.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
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

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "ixjuser.h"

#define ULAW_PAYLOAD 240


int main (int argc, char *argv[])
{
    RtpPayloadType payloadType = rtpPayloadPCMU;
    const int residRate = 240;
    const int trunkRate = 160;

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
    RtpSession audioStack (argv[1], atoi(argv[2]), atoi(argv[3]),
                           atoi(argv[2]) + 1, atoi(argv[3]) + 1,
                           payloadType, -1);
    RtpPacket* outPacket = audioStack.createPacket (160);
    RtpPacket* inPacket = NULL;

    char* inBuffer = new char [480];
    int inPos = 0;
    int playPos = 0;
    char* outBuffer = new char [480];
    int outPos = 0;
    int recPos = 0;

    // setup file descriptor
    fd_set fileDescriptor;
    int selMax = ixj;
    ++selMax;
    struct timeval timeout;
    int selret = 0;

    // open audio path
    ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_START);
    ioctl(ixj, IXJCTL_PLAY_START);
    ioctl(ixj, IXJCTL_AEC_START);

    // continue audio path
    cerr << "Starting audio\n";
    while (ioctl(ixj, IXJCTL_HOOKSTATE))
    {
        // set up file descriptor
        FD_ZERO (&fileDescriptor);
        FD_SET (ixj, &fileDescriptor);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        // read packet into inBuffer
        inPacket = audioStack.receive();
        if (inPacket)
        {
            if (inPacket->getPayloadUsage() != trunkRate)
                cerr << "Trunk read err " << inPacket->getPayloadUsage()
                << "instead of " << trunkRate << endl;
            else
            {
                //cerr <<"Reading from net "<<inPos<<endl;  // debug
                memcpy (inBuffer + inPos, inPacket->getPayloadLoc(),
                        inPacket->getPayloadUsage());
                inPos += inPacket->getPayloadUsage();
            }
        }


        // play packet from inBuffer
        if (inPos >= playPos + residRate)
        {
            //cerr <<"Writing to ixj "<<playPos<<endl;  // debug
            write (ixj, inBuffer + playPos, residRate);
            playPos += residRate;
        }


        // resync positions
        if (inPos == playPos)
        {
            inPos = 0;
            playPos = 0;
        }



        // read packet into outBuffer
        selret = select (selMax, &fileDescriptor, NULL, NULL, &timeout);
        if (selret <= 0)
        {
            // check if select error
            if (selret < 0) perror ("select error");
        }
        else if (FD_ISSET(ixj, &fileDescriptor) && outPos < 480)
        {
            //cerr <<"Reading from ixj "<<outPos<<endl;  // debug
            int cc = read (ixj, outBuffer + outPos, residRate);
            if (cc != residRate)
                cerr << "IXJ read err " << cc << " instead of " << residRate;
            else
                outPos += residRate;
        }


        // send packet from outBuffer
        if (outPos >= recPos + trunkRate)
        {
            //cerr <<"Sending to net "<<recPos<<endl;  // debug
            memcpy (outPacket->getPayloadLoc(), outBuffer + recPos,
                    trunkRate);
            outPacket->setPayloadUsage (trunkRate);
            outPacket->setRtpTime (audioStack.getPrevRtpTime() +
                                   trunkRate);
            audioStack.transmit (outPacket);
            recPos += trunkRate;
        }


        // resync positions
        if (outPos == recPos)
        {
            outPos = 0;
            recPos = 0;
        }
    }




    audioStack.transmitRTCPBYE();

    ioctl(ixj, IXJCTL_REC_STOP);
    ioctl(ixj, IXJCTL_PLAY_STOP);
    ioctl(ixj, IXJCTL_AEC_STOP);
    close(ixj);

    return 0;
}
