/*
* $Id: test_udp_receive.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "UdpStack.hxx"
#ifdef __vxworks
#include "resolvLib.h"
#endif


int main()
{

#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    //    NetworkAddress target("192.168.5.99", 5000);
    NetworkAddress target("carver", 5009);
    NetworkAddress target1("localhost", 5010);

    //    UdpStack stack(&target, 5000, 5000, recvonly);
    UdpStack stack(NULL, 9000, 9000, sendrecv);
    stack.setDestination(&target);
    //    stack.connectPorts();

    /*
        stack.setDestination(&target1);
        stack.connectPorts();
    */
    //   stack.disconnectPorts(); // ?? ports are still connected ??


    char buf[256];
    NetworkAddress sender;

    //    while(1) {
    //    int bytesRead = stack.receive(buf, 256);
    int bytesRead = stack.receiveFrom(buf, 256, &sender);

    cout << "bytes read=" << bytesRead << endl;
    cout << buf << endl;
    //    }

    cout << sender << endl;

    cout << endl;

    return 0;
}
