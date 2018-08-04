/*
* $Id: test_sendrecv_send.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <sys/time.h>
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#if defined(__svr4__)
typedef int socklen_t;
#endif

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    int res1;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(9020);

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));
    cerr << "res1= " << res1 << endl;

    /*
        fd_set netFD;
        FD_ZERO (&netFD);
        FD_SET (socketFD, &netFD);
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 300;
     
        int selret = 0;
     
        while (selret <= 0 )
        {
             selret = select (socketFD + 1,
                             &netFD, NULL, NULL, &timeout);
        }
     
    */
    char buf[256];
    int bufSize = 256;
    memset (buf, 0, bufSize);

    struct sockaddr_in xSrc;
    unsigned int lenSrc = sizeof(xSrc);

    int len;
    len = recvfrom( socketFD,
                    (char *)buf,
                    bufSize,
                    0 /*flags */,
                    (struct sockaddr*) & xSrc,
                    (socklen_t *) & lenSrc);

    if (len <= 0)
    {
        cerr << "receive nothing" << endl ;
    }
    else
    {
        sendto( socketFD, buf, len, 0,
                (struct sockaddr*)&xSrc, sizeof(sockaddr_in));
    }


    return 0;
}
