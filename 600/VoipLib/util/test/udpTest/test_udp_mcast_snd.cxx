/*
* $Id: test_udp_mcast_snd.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "UdpStack.hxx"
#ifdef __vxworks
#include "resolvLib.h"
#endif

// run: ./test_udp_mcast_snd 224.0.0.100:9000
// where 224.0.0.100 is the group ip address
int main(int argc, char* argv[])
{
#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    NetworkAddress target1(argv[1]);
    //    NetworkAddress target("localhost", 9000);
    //    NetworkAddress target2("lab6", 9000);

    UdpStack stack1(NULL, 5001, 5001, sendrecv);

    //    while (1) {
    stack1.transmitTo("hello", 6, &target1);
    //    stack1.transmitTo("hello", 6, &target2);

    //    }

    return 0;

}
