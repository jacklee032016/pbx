/*
 * $Id: UdpStackVerify.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "UdpStack.hxx"
#include "Verify.hxx"
#include "VThread.hxx"
#include "Fifo.h"
#include <stdio.h>

using namespace Vocal;

Fifo<int> recv2send;

void* sendThread(void*)
{
    UdpStack send;

    for(int i = 0; i < 1; i++)
    {
	NetworkAddress dest("127.0.0.1", 2048);
	char buf[1024];
	int len;

	sprintf(buf, "%d", i);

	int retCode;
	retCode = send.transmitTo(buf, strlen(buf) + 1, &dest);
	test_verify(retCode == 0);
    }
    return 0;
}


void* recvThread(void*)
{
    UdpStack receive(0, 2048, 2048);
    for(int i = 0; i < 1; i++)
    {
	NetworkAddress sender;
	char buf[1024];
	int len;

	len = receive.receiveFrom( buf , 1024 , &sender );
	test_verify(atoi(buf) == i);
    }
    recv2send.add(0);
    return 0;
}



int main()
{
    VThread send;
    VThread receive;

    recv2send.addDelayMs(-1, 5000);

    receive.spawn(recvThread, 0);
    usleep(1000);
    send.spawn(sendThread, 0);

    test_verify(recv2send.getNext() == 0);

    receive.join();
    send.join();

    return test_return_code(3);
}
