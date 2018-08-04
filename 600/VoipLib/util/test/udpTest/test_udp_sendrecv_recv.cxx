/*
* $Id: test_udp_sendrecv_recv.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "UdpStack.hxx"

int main ()
{
    UdpStack stack(NULL, 9000, 9000, sendrecv);
    //    UdpStack stack(NULL, 9000, 9000, sendonly);
    //    UdpStack stack2(NULL, 9000, 9000, recvonly);
    NetworkAddress target("localhost", 9020);

    //    system ("netstat -a |grep udp | grep 9000");

    stack.transmitTo("hello", 6, &target);

    cerr << "send" << endl;

    char buf[256];
    memset (buf, 0, 256);

    NetworkAddress sender;
    //   int bytesRead = stack2.receiveFrom(buf, 256, &sender);
    int bytesRead = stack.receiveFrom(buf, 256, &sender);

    cout << "bytes read=" << bytesRead << endl;
    cout << buf << endl;
    cout << "sender is: " << sender << endl;

    return 0;
}
