/*
* $Log: rtpPlay.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: rtpPlay.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

// --- Authors ---------------------------------------------------- //
// Cullen Jennings, April 1999
// Kim Le, June 1999
// ---------------------------------------------------------------- //

#include "global.h"
#include "RtpSession.hxx"
#include <fstream>
#include <iostream>

#ifdef __vxworks
#include "VThread.hxx"
#endif
#include "cpLog.h"

int main (int argc, char *argv[])
{
    cpLogSetPriority (LOG_DEBUG_HB);

    if (argc < 2)
    {
        cerr << "Usage: rtpPlay file [host]\n";
        exit (1);
    }

    ifstream file(argv[1]);
    if (!file)
    {
        cerr << "Could not open file\n";
        return -1;
    }

    char* host = NULL;
    if (argc >= 3)
        host = argv[2];
    else
        host = "localhost";

    int serverPort = 9000;
    if (argc >= 4)
        serverPort = atoi(argv[3]);

    // sending to localhost on port 5000/5001
    // use seperate RTCP ports for localhost usage
    RtpSession stack (host, serverPort, 9002, serverPort+1, 9003);
    //ApiFormat is application codec and timing rate
    // 160 = 20ms for PCMU
    // 240 = 30ms for PCMU
    stack.setApiFormat (rtpPayloadPCMU, 160);
    //NetworkFormat is network codec and timing rate
    // 160 = 20ms for PCMU
    stack.setNetworkFormat (rtpPayloadPCMU, 160);
    RtpPacket* packet = NULL;

    // set SDES information - optional
    RtcpTransmitter* rtcpTran = stack.getRtcpTran();
    rtcpTran->setSdesEmail("development@vovida.com");


    // send out data until end of file
    int result;
    int prevRtpTime = 4294960000;
    int i = 0;
    while (!file.eof())
    {
        packet = stack.createPacket();

        // load data from file into packet
        file.read (packet->getPayloadLoc(), packet->getPayloadSize());
        packet->setPayloadUsage (file.gcount());

        // create silence gap every 10 packets
        if( i++ % 10 == 0 )
        {
            cerr <<"s";
            prevRtpTime += 160;

            // load new data from file into packet
            file.read (packet->getPayloadLoc(), packet->getPayloadSize());
            packet->setPayloadUsage (file.gcount());
        }

        packet->setRtpTime( prevRtpTime );
        prevRtpTime += 160;

        // simulating blocking read call
        usleep (10*1000);
        result = stack.transmit(packet);
        if (result < 0)
            cerr << "x";
        else
            cerr << ".";
        delete packet; packet = NULL;

        // send and receive RTCP packet
        //stack.processRTCP();
    }
    file.close();

    // leaving session
#ifdef __vxworks
    sleep(50);
#else
    usleep (50*1000);
#endif
    stack.transmitRTCPBYE();

    RtpTransmitter* tran = stack.getRtpTran();
    cerr << endl;
    cerr << "Statistics for SRC " << tran->getSSRC() << endl;
    cerr << "Packets sent: " << tran->getPacketSent() << "     ";
    cerr << "Payload sent: " << tran->getPayloadSent() << "     ";
    cerr << endl;

    return 0;
}
