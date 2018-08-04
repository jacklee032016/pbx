/*
* $Id: test_udp_sendloop.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
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

    NetworkAddress target("127.0.0.1", 9000);

    UdpStack stack1(NULL, 5001, 5001, sendrecv);
    stack1.setDestination(&target);

    char buf[1024];

    for (int i = 0; i < 1000000; i++)
    {
        stack1.transmitTo(buf, 1024, &target);
    }
    return 0;

}
