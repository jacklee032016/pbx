/*
* $Id: test_bind.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "errno.h"
#include <strstream>

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
    struct sockaddr dummyAddr;
    int res1, res2, res3;
    char laddr[4];
    char laddr2[4];

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    //    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //    struct hostent* host = gethostbyname ("mendel");
    laddr[0] = 192;
    laddr[1] = 168;
    laddr[2] = 5;
    laddr[3] = 28;
    memcpy((char*)&(localAddr).sin_addr.s_addr,
           laddr,  // take the first entry
           4);


    localAddr.sin_port = htons(5000);

    res1 = bind( socketFD, (struct sockaddr*) & (localAddr), sizeof(localAddr));

    if (res1 != 0)
    {
        int err = errno;
        if ( err == EADDRINUSE )
        {
            cerr << "address in use" << endl;
        }

        strstream errMsg;
        errMsg << "UdpStack<"
        << ">::UdpStack error during socket bind: ";
        errMsg << strerror(err);
        errMsg << char(0);

        clog << errMsg.str() << endl;
        cerr << "bind error" << endl;
        //            throw UdpStackException(errMsg.str());
        errMsg.freeze(false);
        //            assert(0);

    }

    memset((char*) &(remoteAddr), 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname ("ford");

    laddr2[0] = 192;
    laddr2[1] = 168;
    laddr2[2] = 66;
    laddr2[3] = 3;
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           laddr2,  // take the first entry
           4);
    /*
        memcpy((char*)&(remoteAddr).sin_addr.s_addr,
               host->h_addr_list[0], // take the first entry 
               host->h_length);
    */
    remoteAddr.sin_port = htons(7);

    res2 = sendto( socketFD, "hello!", 7, 0,
                   (struct sockaddr*) & remoteAddr, sizeof(sockaddr_in));

    cout << "res2=" << res2 << endl;
}
