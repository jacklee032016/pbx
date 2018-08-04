/*
* $Id: NetworkConfig.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include <unistd.h> 
#include <errno.h> 
#include "NetworkConfig.hxx"
#include "cpLog.h"

NetworkConfig* NetworkConfig::myInstance = 0;

NetworkConfig&
NetworkConfig::instance()
{
    if(myInstance == 0)
    {
        myInstance = new NetworkConfig();
    }
    return *myInstance;
}
NetworkConfig::NetworkConfig()
{
    myAddrFamily = PF_INET;
    init();
}
int NetworkConfig::init()
{
    struct addrinfo hints;
    struct addrinfo *res;

    // Setup structures
    memset(&hints, 0, sizeof(hints));

    char hostName[256];
    if (gethostname(hostName, sizeof(hostName)) == -1)
    {
       cpLog(LOG_ERR, "Failed to get the host name");
       return -1;
    }
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    int error = getaddrinfo(hostName, 0, &hints, &res);
    if(error != 0)
    {
       cpLog(LOG_ERR, "getaddrinfo(), Failed to get the address info, reason:%s", strerror(errno));
       return -1;
    }
    struct addrinfo* myItr = res;
    int myFlg = 0;
    while(myItr)
    {
        if(myItr->ai_family == PF_INET6)
        {
            myFlg |= 0x02;
        }
        else if(myItr->ai_family == PF_INET)
        {
            myFlg |= 0x01;
        }
        cpLog(LOG_DEBUG, "Family:%d", myItr->ai_family);
        cpLog(LOG_DEBUG, "Address Len:%d" , myItr->ai_addrlen);
        cpLog(LOG_DEBUG, "Protocol:%d" , myItr->ai_protocol);
        cpLog(LOG_DEBUG, "Host Name:%s" , myItr->ai_canonname);
        myItr = myItr->ai_next;
    }
    dualStack = false;
    if((myFlg & 0x01) && (myFlg & 0x02))
    {
        myAddrFamily = PF_INET6;
        dualStack = true;
        cpLog(LOG_INFO, "*** Dual-stack supports IPv4 and IPv6 ***" );
    }
    else if((myFlg & 0x01))
    {
        myAddrFamily = PF_INET;
        cpLog(LOG_INFO, "*** IPv4 support only ***");
    }
    else if(myFlg & 0x02)
    {
        myAddrFamily = PF_INET6;
        cpLog(LOG_INFO, "*** IPv6 support only ***" );
    }
    return 0;
}
