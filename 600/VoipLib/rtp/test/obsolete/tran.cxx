/*
* $Id: tran.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
/*
    Sends 4000 byte packets every 500 ms
*/


#include <unistd.h>
#include <fstream>

#include "cpLog.h"

#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"


int main ()
{
    // setup logfile
    cpLogSetPriority (LOG_DEBUG);

    // sending to localhost on port 5000/5001
    RtpSession stack ("localhost", 5000, 0, 5001, 5003);
    RtpPacket* packet = NULL;

    // set SDES information
    RtcpTransmitter* rtcpTran = stack.getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");

    // send out packets
    char* data = "dataxxxx";
    for (int i = 0; i < 400; i++)
    {
        packet = stack.createPacket(4000);

        // load data into packet
        strncpy(packet->getPayloadLoc(), data, strlen(data));
        packet->setPayloadUsage(4000);
        packet->setRtpTime(stack.getPrevRtpTime() + 4000);   // every 500 ms

        // send packet
        usleep(500*1000);
        if (stack.transmit(packet))
            cerr << "x";
        else
            cerr << ".";
        delete packet; packet = NULL;

        // send and receive RTCP packet
        stack.processRTCP();
    }

    // leaving session
    stack.transmitRTCPBYE();

    return 0;
}
