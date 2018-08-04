/*
* $Id: recvNULL.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
/*********************************************************************
 
  Simple program that listens for incoming RTP and RTCP packets
  on port 5000 and 5001.
 
**********************************************************************/


#include <unistd.h>
#include <fstream>


#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"


int main ()
{
    cerr << "Listening:  RTP (5000)   RTCP (5001)\n";

    RtpSession dataStack ("localhost", 0, 5000, 0, 5001);
    RtpReceiver* rtpStack = dataStack.getRtpRecv();
    RtcpReceiver* rtcpStack = dataStack.getRtcpRecv();
    //RtpReceiver rtpStack (5000);
    //RtcpReceiver rtcpStack (5001);

    RtpPacket* rtp = NULL;
    RtcpPacket* rtcp = NULL;

    // loop forever
    while (1)
    {
        rtp = rtpStack->getPacket();
        if (rtp)
        {
            cerr << "r";
            delete rtp;
            rtp = NULL;
        }

        rtcp = rtcpStack->getPacket();
        if (rtcp)
        {
            cerr << "c";
            delete rtcp;
            rtcp = NULL;
        }
    }

    return 0;
}
