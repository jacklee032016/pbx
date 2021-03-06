/*
* $Log: tpTran.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: tpTran.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, Aug 1999
// ---------------------------------------------------------------- //

#include <unistd.h>
#include <fstream>

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"


int main (int argc, char *argv[])
{
    int result;

    if (argc != 5)
    {
        cerr << "Usage: program file host rtp_tran_port rtp_recv_port\n";
        return 1;
    }

    ifstream file(argv[1]);
    if (!file)
    {
        cerr << "Could not open file\n";
        return -1;
    }


    // open RTP stack
    RtpSession stack (argv[2], atoi(argv[3]), atoi(argv[4]),
                      atoi(argv[3]) + 1, atoi(argv[4]) + 1,
                      rtpPayloadPCMU);
    char* bufferA = new char [240];

    RtpPacket* outPacket = stack.createPacket(160);


    // continue audio path
    while (!file.eof())
    {

        // load 30ms data from file or hardwad into buffer
        file.read (bufferA, 240);

        outPacket->setPayloadUsage (240);
        outPacket->setRtpTime(stack.getPrevRtpTime() + 240);   // every 30 ms
        usleep (20*1000);

        // transmit packet
        result = stack.transmit(outPacket);
        if (result < 0)
            cerr << "x";
        else
            cerr << ".";

        // send and receive RTCP packet
        stack.processRTCP();
    }

    stack.transmitRTCPBYE();
    file.close();
    return 0;
}
