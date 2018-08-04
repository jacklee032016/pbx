/*
* $Id: test_udp_send.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
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

    NetworkAddress target("millikan", 9000);
    /*
        NetworkAddress target1("192.168.5.100", 5000);
        NetworkAddress target2("192.168.5.99", 5000);
        NetworkAddress target3("192.168.5.3", 5000);
    */

    //    UdpStack stack(&target, -1, -1, sendonly);
    //    UdpStack stack(NULL, -1, -1, sendonly);

    /*
        UdpStack stack(NULL, 5003, 5003, sendrecv);
        stack.setDestination(&target);
     
        stack.connectPorts();
        stack.disconnectPorts(); // It don't dissolve the port association??
    */

    UdpStack stack1(NULL, 5001, 5001, sendrecv);
    stack1.setDestination(&target);

    stack1.transmitTo("hello", 6, &target);
    /*
        stack.transmitTo("hello", 6, &target2);
        stack.transmitTo("hello", 6, &target3);
    */

    //    stack.connectPorts();
    //    while (1) {
    //    stack.transmit("hello", 6);
    //    }
    return 0;

}
