/*
* $Id: test_recv.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
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
    int res1;

    socketFD = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    memset((char*) &(localAddr),0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localAddr.sin_port = htons(10100);

    res1 = bind( socketFD, (struct sockaddr*) &(localAddr), sizeof(localAddr));

    char buf[256];
    int bufSize = 256;
    memset (buf, 0, bufSize);
    struct sockaddr_in xSrc;
    unsigned int lenSrc=sizeof(xSrc);

    int len = recvfrom( socketFD,
                        (char *)buf, 
                        bufSize,
                        0, /*flags */
                        (struct sockaddr*)&xSrc,
                        (socklen_t *) &lenSrc);

    cout << "len=" << len << ", received: " << buf << endl;

    if (len == -1)
    {
        cout << "recvfrom() failed: " << errno << " : " << strerror(errno) << endl;
    }

    return 0;
}




