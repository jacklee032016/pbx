/*
* $Log: sampleUsage.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: sampleUsage.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// --- Authors ---------------------------------------------------- //
// Kim Le, July 1999
// ---------------------------------------------------------------- //

#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "cpLog.h"
#include "rtpTypes.h"
#include "RtpSession.hxx"

#ifdef __vxworks
#include <resolvLib.h>
#include "VThread.hxx"
#endif

int main (int argc, char *argv[])
{
#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "192.168.5.11";
    int rmtPort = 6000;
    int lclPort = 5000;
    int apSize = 100;
    int npSize = 100;

#ifndef __vxworks
    // override default values if not vxworks
    if (argc == 6)
    {
        // Usage:"sampleUsage host rtpTxPort rtpRxPort
        //                    apiPktSize networkPktSize
        strcpy( rmtHost, argv[1] );
        rmtPort = atoi( argv[2] );
        lclPort = atoi( argv[3] );
        apSize = atoi( argv[4] );
        npSize = atoi( argv[5] );
    }
    else
    {
        printf("Enter remote hostname: ");
        scanf ("%s", rmtHost);
        printf("Enter remote port number: ");
        scanf ("%d", &rmtPort);
        printf("Enter local port number: ");
        scanf ("%d", &lclPort);
        printf("API packet size: ");
        scanf ("%d", &apSize);
        printf("Network packet size: ");
        scanf ("%d", &npSize);
    }
#endif

    RtpSession* stack = new RtpSession ( rmtHost, rmtPort, lclPort,
                                         rmtPort + 1, lclPort + 1, rtpPayloadPCMU, rtpPayloadPCMU, 5);

    // setup logfile
    cpLogSetLabel ("sampleUsage");
    //    cpLogSetPriority (LOG_DEBUG);

    RtpPacket* inPacket = NULL;

    RtpPacket* outPacket = stack->createPacket();

    // only 5 bytes of data, but still send a big packet
    strncpy (outPacket->getPayloadLoc(), "data", 5);

    stack->setApiFormat (rtpPayloadPCMU, apSize);
    stack->setNetworkFormat (rtpPayloadPCMU, npSize);

    for (int i = 0; i < 80; i++)
    {

        // receive a packet
        inPacket = stack->receive ();
        if (inPacket)
        {
            //            if (i<2)
            //                inPacket->printPacket();
            printf( "r" );
            delete inPacket;
            inPacket = NULL;
        }
        else
        {
            printf( "n");
        }


        // sleep between rcv and trans
        usleep (100*1000);  // 100 msec

        // send a packet
        outPacket->setRtpTime(stack->getPrevRtpTime() + npSize);
        //        if (i<2)
        //           outPacket->printPacket();
        outPacket->setPayloadUsage (npSize);

        stack->transmit (outPacket);

        printf( "t" );


        // send and receive RTCP packet
        if (stack->checkIntervalRTCP())
        {
            stack->transmitRTCP();
            printf( "T" );
        }
        if (stack->receiveRTCP() == 0 )
        {
            printf( "R" );
        }


        fflush(stdout);
    }

    stack->transmitRTCPBYE();


    if (outPacket)
    {
        delete outPacket;
        outPacket = NULL;
    }

    if (inPacket)
    {
        delete inPacket;
        inPacket = NULL;
    }

    delete stack;
    stack = NULL;

    return 0;
}
