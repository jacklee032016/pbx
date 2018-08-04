/*
* $Log: recv.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: recv.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, Aug 1999
// ---------------------------------------------------------------- //
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
