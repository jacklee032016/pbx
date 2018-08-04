/*
* $Log: maxPlaying.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: maxPlaying.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
// for only sending out packets

#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include "rtpTypes.h"
#include "RtpSession.hxx"
#include "TimeTracker.hxx"

int main (int argc, char *argv[])
{
    // allocate and initialize default configuration values
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "vvs-tabletennis";
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

    for (int j=0; j < max_sessions; j++)
    {
//        sleep(1);
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
        
            char outBufferPkt[480];
            struct timeval t1, t2;
        
            memset(outBufferPkt, 0, 480);
            strncpy (outBufferPkt, "data", 5);
        
            stack->setApiFormat (rtpPayloadPCMU, apSize);
            stack->setNetworkFormat (rtpPayloadPCMU, npSize);
        
            int i=0;
            long long delta;
//            int x = 1;
            int sleeptime = 16;

            gettimeofday(&t1, 0);
            T.startTime();
            while (i < numPkts)
            {

                stack->transmitRaw (outBufferPkt, 160);
                i++;
        
                // send and receive RTCP packet
                if (stack->checkIntervalRTCP())
                {
                    stack->transmitRTCP();
                }
                if (stack->receiveRTCP() == 0 )
                {
                }

                if (sleeptime > 0)
                {
                    usleep (sleeptime * 1000);  // 20 msec
//                    cerr << "sleeptime = " << sleeptime << endl;
                }
                else
                {
//                    cerr << "no sleep" << endl;
                }

                gettimeofday(&t2, 0);
                delta = ((t2.tv_sec - t1.tv_sec) * 1000000 +
                       (t2.tv_usec - t1.tv_usec))/1000;
                sleeptime = (i+1)*20 - delta;
                if (i == (numPkts-2)) sleeptime=0;
                
        
/*
                gettimeofday(&t2, 0);
                delta = ((t2.tv_sec - t1.tv_sec) * 1000 +
                       (t2.tv_usec/1000 - t1.tv_usec/1000));

                x = deta/20 - i;
                for (int k = 0; k < x; k++)
                {
                    // to Catch up the packets sending due to usleep drifting
                    stack->transmitRaw (outBufferPkt, 160);
                    i++;
                    cerr << "c" ;
                }
                cerr << endl;
*/
        
            }
        

            T.endTime();

            sleep (9);
            stack->transmitRTCPBYE();
            delete stack;
            stack = NULL;

            cout << "End rtp session " << j << endl;
            return 3;
        }
    }

    return 0;
}
