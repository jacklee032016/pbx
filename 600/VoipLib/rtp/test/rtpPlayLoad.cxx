/*
* $Id: rtpPlayLoad.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include <unistd.h>
#include <fstream>

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"
#ifdef __vxworks
#include "VThread.hxx"
#endif
#include "cpLog.h"

int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG_STACK);

    if (argc < 2)
    {
        cerr << "Usage: rtpPlay host port\n";
        exit (1);
    }

    char* host = argv[1];
    int port = atoi( argv[2] );


    // sending to localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
    RtpSession stack (host, port, 0, 0, 0);
    //ApiFormat is application codec and timing rate
    // 160 = 20ms for PCMU
    // 240 = 30ms for PCMU
    stack.setApiFormat (rtpPayloadPCMU, 160);
    //NetworkFormat is network codec and timing rate
    // 160 = 20ms for PCMU
    stack.setNetworkFormat (rtpPayloadPCMU, 160);
    RtpPacket* packet = stack.createPacket();
    memset (packet->getPayloadLoc(), packet->getPayloadSize(), 0);
    packet->setPayloadUsage (packet->getPayloadSize());


    // send out data until end of file
    int result;
//    for( int i = 0; i < 10000; i++ )
    while (1)
    {

        // simulating blocking read call
        usleep (18*1000);
        result = stack.transmit(packet);
        if (result < 0)
            cerr << "x";
        //else
        //    cerr << ".";
    }

    cerr<<"Done\n";
    return 0;
}
