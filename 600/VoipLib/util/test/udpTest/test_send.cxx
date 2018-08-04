/*
* $Id: test_send.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include <iostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int main (int argc, char* argv[])
{
    int socketFD;
    struct sockaddr_in localAddr; 
    struct sockaddr_in remoteAddr; 
    int res1;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr),0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(10110);

    res1 = bind( socketFD, (struct sockaddr*) &(localAddr), sizeof(localAddr));

    memset((char*) &(remoteAddr),0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    struct hostent* host = gethostbyname (argv[1]);
    memcpy((char*)&(remoteAddr).sin_addr.s_addr,
           host->h_addr_list[0], // take the first entry 
           host->h_length);
    remoteAddr.sin_port = htons(10100);

    sendto( socketFD, "hello!", 7, 0,  
          (struct sockaddr*)&remoteAddr, sizeof(sockaddr_in));

    return 0;
}




