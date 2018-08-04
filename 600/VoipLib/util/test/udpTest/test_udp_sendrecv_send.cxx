/*
* $Id: test_udp_sendrecv_send.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "UdpStack.hxx"

int main ()
{

    UdpStack stack1(NULL, 9020, 9020, sendrecv);

    char buf[256];
    memset (buf, 0, 256);

    NetworkAddress sender;
    int bytesRead = stack1.receiveFrom(buf, 256, &sender);

    if (bytesRead <= 0)
    {
        cerr << "receive nothing" << endl ;
    }
    else
    {
        stack1.transmitTo(buf, bytesRead, &sender);
    }

    return 0;
}
