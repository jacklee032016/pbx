/*
 * $Id: NetworkAddressVerify.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "NetworkAddress.h"
#include "Verify.hxx"
#include "cpLog.h"

void test_00()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3700);

    test_verify(item_0 != item_1);
}

void test_01()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.100", 3400);

    test_verify(item_0 != item_1);
}


void test_02()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress item_1("192.168.5.254", 3400);

    test_verify (item_0 == item_1);
}

void test_03()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3400);

    test_verify (*item_1 == item_0);
    delete item_1;
}

void test_04()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.139", 3400);

    test_verify (!(*item_1 == item_0));
    delete item_1;
}

void test_05()
{
    // test equality

    NetworkAddress item_0("192.168.5.254", 3400);
    NetworkAddress* item_1 = new NetworkAddress("192.168.5.254", 3401);

    test_verify ( !(*item_1 == item_0));
}

void test_06()
{
    NetworkAddress na("vovida-www.cisco.com");
    NetworkAddress item_0(na.getIpName(), 5060);
    //    NetworkAddress::get
    test_verify(item_0.getHostName() == "vovida-www.cisco.com");
}

void test_07()
{
    NetworkAddress x(string("208.184.140.114"));
    test_verify(x.getHostName() == "208.184.140.114.freetelephony.com");
    cout << x.getHostName() << endl;    
}


int main()
{
//    cpLogSetPriority(LOG_DEBUG_STACK);
    test_00();
    test_01();
    test_02();
    test_03();
    test_04();
    test_05();
    test_06();
    test_07();

    return test_return_code(8);
}
