/*
* $Log: tran.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: tran.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, Aug 1999
// ---------------------------------------------------------------- //
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
