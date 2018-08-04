/*
* $Log: test_timing.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: test_timing.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, July 1999
// ---------------------------------------------------------------- //

#include <stdlib.h>
#include <unistd.h>
#include <fstream>

#include "cpLog.h"

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "VTime.hxx"

#ifdef __vxworks
#include "VThread.hxx"
#endif

#ifndef __vxworks
int main (int argc, char *argv[])
#else
int sampleUsageMain (char* rmtHost, int rmtPort, int lclPort,
                     int rmtRtcpPort, int lclRtcpPort,
                     int apiPktSize, int netPktSize)
#endif
{
    // setup logfile
    //    cpLogSetLabel ("sampleUsage");
    //    cpLogSetPriority (LOG_DEBUG);

    RtpPayloadType payloadType = rtpPayloadPCMU;

#ifndef __vxworks
    if (argc != 8)
    {
        cerr << "Usage:\n";
        cerr << "sampleUsage host rtpTxPort rtpRxPort rtcpTxPort rtcpRxPort\n";
        return 1;
    }
#endif

    // receiving on localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
#ifndef __vxworks
    RtpSession stack (argv[1], atoi(argv[2]), atoi(argv[3]),
                      atoi(argv[4]), atoi(argv[5]),
                      payloadType);
#else
RtpSession stack ( rmtHost, rmtPort, lclPort,
    rmtRtcpPort, lclRtcpPort, payloadType);
#endif

    RtpPacket* inPacket = NULL;
    RtpPacket* outPacket = stack.createPacket();
    strncpy (outPacket->getPayloadLoc(), "data", 5);

    int apSize, npSize;

    // acutally usage is 5, but just pretending a big packet
#ifndef __vxworks
    apSize = atoi(argv[6]);
    npSize = atoi(argv[7]);
#else
    apSize = apiPktSize;
    npSize = netPktSize;
#endif

    outPacket->setPayloadUsage (npSize);
    stack.setApiPktSize (apSize);
    stack.setNetworkPktSize (npSize);

    struct timeval curtime[500];

    //    for (int i = 0; 1 || i<500; i++)
    for (int i = 0; i < 500; i++)
    {
        gettimeofday(curtime + i, NULL);

        // receive a packet
        inPacket = stack.receive ();

        stack.transmit (outPacket);
    }

    for (int j = 1; j < 500; j++)
    {
        cout << curtime[j].tv_usec - curtime[j - 1].tv_usec << endl;
    }
    return 0;
}
