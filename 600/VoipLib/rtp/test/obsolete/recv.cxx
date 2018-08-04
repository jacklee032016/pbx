/*
* $Id: recv.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
/*
    Receives every 500 ms
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

    // receiving on localhost on port 5000/5001
    RtpSession stack ("localhost", 0, 5000, 5003, 5001);
    RtpPacket* packet = NULL;

    // receive packets
    while (1)
    {
        usleep (500*1000);
        packet = stack.receive();
        if (packet)
        {
            // got a packet - write data to file
            cerr << ".";
        }
        else
        {
            // no packet received
            cerr << "x";
        }

        // send and receive RTCP packet
        stack.processRTCP();
    }

    return 0;
}
