/*
* $Id: test_udp_mcast_rcv.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "UdpStack.hxx"
#ifdef __vxworks
#include "resolvLib.h"
#endif

// run: ./test_udp_mcast_rcv 224.0.0.100 host
// where 224.0.0.100 is the group IP address;
// host can be "ada.private.vovida.com" or "x.x.x.x"
//  (host cannot use "localhost")
int main(int argc, char* argv[])
{

#ifdef __vxworks
    resolvInit("192.168.5.254", "private.vovida.com", 0);
#endif

    UdpStack stack(NULL, 9000, 9000, sendrecv);


    NetworkAddress group(argv[1]);
    NetworkAddress iface(argv[2]);
    int ifaceInexe = 0;

    stack.joinMulticastGroup (group, &iface, ifaceInexe);


    char buf[256];
    NetworkAddress sender;

    while (1)
    {
        //    int bytesRead = stack.receive(buf, 256);
        int bytesRead = stack.receiveFrom(buf, 256, &sender);

        cout << "bytes read=" << bytesRead << endl;
        cout << buf << endl;
        cout << sender << endl;

    }

    cout << endl;

    return 0;
}
