/*
* $Id: HostMatchTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
// test VTest class

#include "VTest.hxx"
#include "HostMatch.hxx"


bool test_00()
{
    return hostsEqual("mail", "mail.utanet.at");
}

bool test_01()
{
    return !hostsEqual("www.yahoo.com", "www.utanet.at");
}


bool test_02()
{
    return !hostsEqual("www", "mail");
}

bool test_03()
{
    return hostsIntersect("www", "www.utanet.at");
}

bool test_04()
{
    return hostsIntersect("mail", "www");
}

bool test_05()
{
    return hostsIntersect("mail", "195.70.253.8");
}

bool test_06()
{
    return hostsIntersect("www", "213.90.34.33");
}

bool test_07()
{
    return hostsEqual("www.utanet.at", "213.90.34.33");
}

bool test_08()
{
    return !hostsIntersect("workstation", "millikan");
}

bool test_09()
{
    return !hostsIntersect("213.90.34.33", "195.70.253.8");
}

bool test_10()
{
    return !hostsEqual("213.90.34.33", "213.90.34.34");
}

bool test_11()
{
    return !hostsEqual("213.90.34.33", "213.90.34.");
}

bool test_12()
{
    return hostsEqual("213.90.34.33", "213.90.34.33");
}

bool test_13()
{
    return !hostsIntersect("213.90.34.33", "zxvohihewwthoihaoisdhiqotw.");
}

bool test_14()
{
    return !hostsEqual("adsihocxh 38h 3", "zxvohihewwthoihaoisdhiqotw.");
}

bool test_15()
{
    return hostsEqual("213.90.34.33", "213.90.34.33");
}

// test addresses only too

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "HostMatch test 0");
    myTest.test(1, test_01(), "HostMatch test 1");
    myTest.test(1, test_02(), "HostMatch test 2");
    myTest.test(1, test_03(), "HostMatch test 3");
    myTest.test(1, test_04(), "HostMatch test 4");
    myTest.test(1, test_05(), "HostMatch test 5");
    myTest.test(1, test_06(), "HostMatch test 6");
    myTest.test(1, test_07(), "HostMatch test 7");
    myTest.test(1, test_08(), "HostMatch test 8");
    myTest.test(1, test_09(), "HostMatch test 9");
    myTest.test(1, test_10(), "HostMatch test 10");
    myTest.test(1, test_11(), "HostMatch test 11");
    myTest.test(1, test_12(), "HostMatch test 12");
    myTest.test(1, test_13(), "HostMatch test 13");
    myTest.test(1, test_14(), "HostMatch test 14");
    myTest.test(1, test_15(), "HostMatch test 15");

    return myTest.exitCode();
}
