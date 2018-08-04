/*
* $Id: test_connect.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
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
    localAddr.sin_port = htons(5000);

    memset((char*) &(remoteAddr), 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname ("carver");
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           host->h_addr_list[0],  // take the first entry
           host->h_length);
    remoteAddr.sin_port = htons(5010);

    memset((char*) &dummyAddr, 0, sizeof(dummyAddr));
    dummyAddr.sa_family = AF_UNSPEC;

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));
    cout << "res1=" << res1 << endl;
    cout << "errno=" << errno << endl;

    close(socketFD);
    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    localAddr.sin_port = htons(5030);
    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));
    cout << "res1=" << res1 << endl;
    cout << "errno=" << errno << endl;

    res2 = connect(socketFD, (struct sockaddr*) & (remoteAddr),
                   sizeof(remoteAddr));

    /*
        while (1)
{
}
    */
    res3 = connect(socketFD, (struct sockaddr*) & (dummyAddr),
                   sizeof(dummyAddr));

    return 0;
}
