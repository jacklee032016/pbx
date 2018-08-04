/*
* $Id: test_udp.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "NetworkAddress.h"
#include "UdpStack.hxx"
#include "cpLog.h"
#include "VTime.hxx"
#include <unistd.h>
#include <stdio.h>

#ifdef __vxworks
#include "VThread.hxx"
#include <resolvLib.h>
#endif


// yet to be tested: multicase

int main(int argc, char *argv[])
{
    cpLogSetPriority(LOG_DEBUG);

    char outMsg[100] = "abcdedfg";
    char inMsg[100] = "";

    // allocate and initialize/hardcode configuration parameters
    // note, you may need to create two executable, each with the
    // others address info
    char rmtHost[256] = "192.168.5.100";
    int rmtPort = 7000;
    int lclPort = 8000;

#ifndef __vxworks
    // if not VxWorks, set config parameters at runtime;
    printf("Enter remote hostname: ");
    scanf ("%s", rmtHost);
    printf("Enter remote port number: ");
    scanf ("%d", &rmtPort);
    printf("Enter local port number: ");
    scanf ("%d", &lclPort);
#else
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif // __vxworks

    NetworkAddress remoteHost(rmtHost, rmtPort);
    UdpStack stack(&remoteHost, lclPort, lclPort, sendrecv);

    stack.connectPorts();

    int len;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    for (int i = 0; i < 1000; i++)
    {
        // every time need to reset FD_SET
        fd_set netFD;
        FD_ZERO (&netFD);
        FD_SET (stack.getSocketFD(), &netFD);

        int selret = select (stack.getSocketFD() + 1,
                             &netFD, NULL, NULL, &timeout);

        if (selret > 0)
        {
            len = stack.receive(inMsg, 100);
            if (len)
            {
                printf("r");
                inMsg[len] = '\n';
            }
            else
            {
                printf( " rcv empty pkt ");
            }
        }
        else
        {
            printf(" no  pkt ");
        }

        usleep (50000);  // 50 msec
        stack.transmit(outMsg, 100);
        printf("t");
        fflush(stdout);
    }
}
