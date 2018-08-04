/*
* $Id: test_createUdp.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "NetworkAddress.h"
#include "UdpStack.hxx"
#include "cpLog.h"

int main(void)
{
    //    cpLogSetPriority(LOG_DEBUG);

    NetworkAddress ipaddr("boo:8080");

    UdpStack udpStack(NULL, 9000);

    udpStack.setDestination(&ipaddr);

    cout << "getRxPort()= " << udpStack.getRxPort() << endl;
    cout << "getTxPort()= " << udpStack.getRxPort() << endl;
    cout << "getDestinationPort()= " << udpStack.getDestinationPort() << endl;

    cout << "getDestinationHost()= " << udpStack.getDestinationHost() << endl;

    cout << "getName= " << udpStack.getName() << endl << endl;

    cout << "getSocketFD()= " << udpStack.getSocketFD() << endl << endl;

    return 0;
}
