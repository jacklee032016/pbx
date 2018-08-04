/*
* $Log: rtpRecord.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: rtpRecord.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <unistd.h>
#include <fstream>

#include "RtpSession.hxx"
#include "cpLog.h"
#include "NtpTime.hxx"

#ifdef __vxworks
#include "VThread.hxx"
#endif


int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG_HB);

    if (argc < 2)
    {
        cerr << "Usage: rtpRecord file [host]\n";
        exit (1);
    }
    ofstream file(argv[1], ios::app);

    if (!file)
    {
        cerr << "Could not open file '" << argv[1] << "'\n";
        return -1;
    }

    char* host = NULL;
    if (argc == 3)
        host = argv[2];
    else
        host = "localhost";

    // receiving on localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
    RtpSession stack (host, 9002, 9000, 9003, 9001);
    stack.setApiFormat (rtpPayloadPCMU, 160);
    stack.setNetworkFormat (rtpPayloadPCMU, 160);
    RtpPacket* p = NULL;

    // stop loop after 400 packet attempts
    NtpTime nowTime, pastTime;
    for (int i = 0; i < 999; i++)
    {
        p = stack.receive();
        if (p)
        {
            // got a packet - write data to file
            file.write(p->getPayloadLoc(), p->getPayloadUsage());
            cerr << "*";
            // simulate blocking write
#ifdef __vxworks
            sleep(10);
#else
	    usleep (10*1000);
#endif
        }
        else
        {
            // no packet received
            cerr << "x";
#ifdef __vxworks
            sleep(5);
#else
            usleep (5*1000);
#endif

        }

        // send and receive RTCP packet
        stack.processRTCP();
    }

    return 0;
}
