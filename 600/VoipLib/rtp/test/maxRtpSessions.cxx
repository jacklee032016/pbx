/*
* $Id: maxRtpSessions.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
// This is a test to see how many rtp session can be established 
// between two linux boxes. It is based on sampleUsage.cxx


#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "cpLog.h"
#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "TimeTracker.hxx"


int main (int argc, char *argv[])
{

    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "vvs-softball";
    int max_sessions = 100;
    int rmtPort = 7000;
    int lclPort = 5000;
    int apSize = 160;
    int npSize = 160;

    TimeTracker T(cerr);

    if (argc == 1)
    {
        cout << "Usage: ./maxRtpSessions host sessions rmt_start_port lcl_start_port" << endl;
        return 0;
    } 
    else if (argc == 5)
    {
        strcpy( rmtHost, argv[1] );
        max_sessions = atoi( argv[2] );
        rmtPort = atoi( argv[3] );
        lclPort = atoi( argv[4] );

    }
    else
    {
        printf("Enter remote hostname: ");
        scanf ("%s", rmtHost);
        printf("Enter number of the max rtp sessions: ");
        scanf ("%d", &max_sessions);
        printf("Enter remote port number: ");
        scanf ("%d", &rmtPort);
        printf("Enter local port number: ");
        scanf ("%d", &lclPort);

    }

    pid_t pid;


    for (int j=0; j < max_sessions; j++)
    {
        if ((pid = fork()) < 0)
        {
            cerr << "Fork failed." << endl;
            return -1;
        }
        else if (pid == 0)
        {
            cout << "Get pid is: " << getpid() <<endl;
            cout << "Creating rtp session " << j << endl;

            T.startTime();

            RtpSession* stack = new RtpSession ( rmtHost, 
                                     rmtPort + 2*j, lclPort + 2*j,
                                     rmtPort + 2*j + 1, lclPort + 2*j + 1, 
                                     rtpPayloadPCMU, rtpPayloadPCMU, 5);

            // setup logfile
//            cpLogSetLabel ("maxRtpSessions");
            //    cpLogSetPriority (LOG_DEBUG);
        
            RtpPacket* inPacket = NULL;
        
            char outBufferPkt[480];
        
            // only 5 bytes of data, but still send a big packet
            strncpy (outBufferPkt, "data", 5);
        
            stack->setApiFormat (rtpPayloadPCMU, apSize);
            stack->setNetworkFormat (rtpPayloadPCMU, npSize);
        
            for (int i = 0; i < 10000; i++)
            {

                // receive a packet
                inPacket = stack->receive ();
                if (inPacket)
                {
                    //            if (i<2)
                    //                inPacket->printPacket();
//                    printf( "r" );
                    delete inPacket;
                    inPacket = NULL;
                }
                else
                {
//                    printf( "n");
                }
        
        
                // sleep between rcv and trans
                usleep (20*1000);  // 20 msec
        
                // send a packet
                //        if (i<2)
                //           outPacket->printPacket();
                stack->transmitRaw (outBufferPkt, 160);
        
//                printf( "t" );
        
        

                // send and receive RTCP packet
                if (stack->checkIntervalRTCP())
                {
                    stack->transmitRTCP();
//                    printf( "T" );
                }
                if (stack->receiveRTCP() == 0 )
                {
//                    printf( "R" );
                }

        
//                fflush(stdout);
            }
        
            stack->transmitRTCPBYE();
        
            if (inPacket)
            {
                delete inPacket;
                inPacket = NULL;
            }
        
            delete stack;
            stack = NULL;


            T.endTime();
            cout << "End rtp session " << j << endl;
            return 3;
//            break;
        }
/*
        else if (pid != 0)
        {
            cerr << "It is still parent process." << endl;
        }
*/
    }

    return 0;
}
