/*
* $Id: maxRecording.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "TimeTracker.hxx"
#include "support.hxx"

int main (int argc, char *argv[])
{
    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "vvs-tennis";
    int max_sessions = 100;
    int rmtPort = 7000;
    int lclPort = 5000;
    int apSize = 160;
    int npSize = 160;
    int numPkts = 10000;

    TimeTracker T(cerr);

    if (argc == 1)
    {
        cout << "Usage: ./maxRtpSessions host sessions rmt_start_port lcl_start_port num_packets" << endl;
        return 0;
    } 
    else if (argc == 6)
    {
        strcpy( rmtHost, argv[1] );
        max_sessions = atoi( argv[2] );
        rmtPort = atoi( argv[3] );
        lclPort = atoi( argv[4] );
        numPkts = atoi( argv[5] );

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
        printf("Enter number of packets sending or receiving: ");
        scanf ("%d", &numPkts);

    }

    pid_t pid;

    int seqnum1 = 0;
    int seqnum2 = 0;

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


            RtpSession* stack = new RtpSession ( rmtHost, 
                                     rmtPort + 2*j, lclPort + 2*j,
                                     rmtPort + 2*j + 1, lclPort + 2*j + 1, 
                                     rtpPayloadPCMU, rtpPayloadPCMU, 5);

            RtpPacket* inPacket = NULL;
        
            stack->setApiFormat (rtpPayloadPCMU, apSize);
            stack->setNetworkFormat (rtpPayloadPCMU, npSize);
       
            string filename = itos((unsigned int) j);
            filename.append(".data");
            ofstream file(filename.c_str(), ios::out);

            if (!file)
            {
                cerr << "Could not open file: " << filename << endl;
                return 3;
            }

            int i = 0;
            while (i < (numPkts )) // assumming allowing 5 packets lost
            {
//                cerr << "?" << endl;
                // receive a packet
                inPacket = stack->receive ();
                if (inPacket)
                {
                    seqnum2 = inPacket->getSequence();
                    if (i == 0) 
                    {
                        T.startTime();
                        seqnum1 = inPacket->getSequence();
                    }
                    i++;
//                    cerr << "get packets: " << i << endl;
                    file.write(inPacket->getPayloadLoc(), inPacket->getPayloadUsage());
                    delete inPacket;
                    inPacket = NULL;
                }
                else
                {
                }
        
                // send and receive RTCP packet
                if (stack->checkIntervalRTCP())
                {
                    stack->transmitRTCP();
                }
                if (stack->receiveRTCP() == 1)
                {
                   break;
                }
             
                usleep(1*1000);

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

            cout << "get packets: " << i << endl;
            cerr << " start seq num = " << seqnum1 << endl;
            cout << "   end seq num = " << seqnum2 << endl;
            cout << "End rtp session " << j << endl << endl;
            return 3;
//            break;
        }
    }

    return 0;
}
