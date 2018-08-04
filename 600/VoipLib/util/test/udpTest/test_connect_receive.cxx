/*
* $Id: test_connect_receive.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "errno.h"

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
    struct sockaddr dummyAddr;
    int res1, res2, res3;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(9000);

    memset((char*) &(remoteAddr), 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname ("carver");
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           host->h_addr_list[0],  // take the first entry
           host->h_length);
    remoteAddr.sin_port = htons(9020);

    memset((char*) &dummyAddr, 0, sizeof(dummyAddr));

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));

    //    system ("netstat -a |grep udp | grep 9000");

    res2 = connect(socketFD, (struct sockaddr*) & (remoteAddr),
                   sizeof(remoteAddr));
    cout << "res2 = " << res2 << endl;

    //    system ("netstat -a |grep udp | grep 9000");


    dummyAddr.sa_family = AF_INET;
    res3 = connect(socketFD, (struct sockaddr*) & (dummyAddr),
                   sizeof(dummyAddr));
    cout << "res3 = " << res3 << endl;
    cout << " errno = " << errno << endl;

    //    system ("netstat -a |grep udp | grep 9000");

    dummyAddr.sa_family = AF_UNSPEC;
    res3 = connect(socketFD, (struct sockaddr*) & (dummyAddr),
                   sizeof(dummyAddr));
    cout << "res3 = " << res3 << endl;
    cout << " errno = " << errno << endl;

    //    system ("netstat -a |grep udp | grep 9000");


    char buf[256];
    int bufSize = 256;
    memset (buf, 0, bufSize);

    int len = recv( socketFD,
                    (char *)buf, bufSize,
                    0 /*flags */);
    cout << "len=" << len << ", received: " << buf << endl;
    /*
        while (1)
{
}
    */

    return 0;
}
