/*
* $Log: tpRecv.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: tpRecv.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, July 1999
// ---------------------------------------------------------------- //

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
    int cc;
    char* buffer [480];
    char* silentBuffer [480];
    memset (silentBuffer, 0, 480);

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
                      payloadType);
    RtpPacket* inPacket = NULL;


    // open audio path
    ioctl(ixj, IXJCTL_PLAY_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_CODEC, ULAW);
    ioctl(ixj, IXJCTL_REC_START);
    ioctl(ixj, IXJCTL_PLAY_START);
    ioctl(ixj, IXJCTL_AEC_START);

    // continue audio path
    int i = 0;
    while (ioctl(ixj, IXJCTL_HOOKSTATE))
    {

        inPacket = stack.receive ();
        if (inPacket)
        {
            i++;
            if (inPacket->getPayloadUsage() != 240)
                cerr << "Partial\n";
            cc = write(ixj, inPacket->getPayloadLoc(), 240);
            cerr << ".";
        }
        else
        {
            //cc = read(ixj, buffer, 240);
            //write (ixj, buffer, 240);
        }

        stack.processRTCP();
    }
    stack.transmitRTCPBYE();

    ioctl(ixj, IXJCTL_REC_STOP);
    ioctl(ixj, IXJCTL_PLAY_STOP);
    //ioctl(ixj, IXJCTL_AEC_STOP);
    close(ixj);

    return 0;
}
