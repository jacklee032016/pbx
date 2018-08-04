/*
* $Id: test_connect_send.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main ()
{
    int socketFD;
    struct sockaddr_in localAddr;
    struct sockaddr_in remoteAddr;
    struct sockaddr dummyAddr;
    //    int res1;
    int res2;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr), 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(9010);

    memset((char*) &(remoteAddr), 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname ("localhost");
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           host->h_addr_list[0],  // take the first entry
           host->h_length);
    remoteAddr.sin_port = htons(9000);

    memset((char*) &dummyAddr, 0, sizeof(dummyAddr));
    dummyAddr.sa_family = AF_UNSPEC;

    /*
        res1 = bind( socketFD, (struct sockaddr*) &(localAddr), sizeof(localAddr));
     
    */
    res2 = connect(socketFD, (struct sockaddr*) & (remoteAddr),
                   sizeof(remoteAddr));

    send( socketFD, "hello!", 7, 0 ) ;


    /*
        sendto( socketFD, "hello!", 7, 0,  
              (struct sockaddr*)&remoteAddr, sizeof(sockaddr_in));
     
        res3 = connect(socketFD, (struct sockaddr*) &(dummyAddr), 
                       sizeof(dummyAddr));
        while (1)
{
}
     
    */ 
    return 0;
}
