/*
* $Id: test_networkaddr.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "global.h"
#include "NetworkAddress.h"
#include "cpLog.h"

int main(void)
{
    //    cpLogSetPriority(LOG_DEBUG);

    NetworkAddress addr1("192.168.5.11", 8080);
    NetworkAddress addr2("192.168.22.15", 8989);
    struct sockaddr socka;
    struct sockaddr socka2;
    addr1.getSockAddr(socka);
    addr2.getSockAddr(socka2);
    cout << socka.sa_family << endl;
    for (int i = 0; i < 14; i++)
        cout << (int) socka.sa_data[i] << " " ;
    cout << endl;
    cout << socka2.sa_family << endl;
    for (int j = 0; j < 14; j++)
        cout << (int) socka2.sa_data[j] << " " ;
    cout << endl;
    /*
        NetworkAddress addr2("");
        NetworkAddress addr3("boo:8080");
        NetworkAddress addr4("192.168.5.11:8080");
     
        cout << "NetworkAddress addr1(\"192.168.5.11\", 8080);" << endl;
        cout << "NetworkAddress addr2(\"boo\", 8080);" << endl;
        cout << "NetworkAddress addr3(\"boo:8080\");" << endl;
        cout << "NetworkAddress addr4(\"192.168.5.11:8080\");" << endl << endl;
     
        cout << "addr1.getHostName()= " << addr1.getHostName() << endl;
        cout << "addr2.getHostName()= " << addr2.getHostName() << endl;
        cout << "addr3.getHostName()= " << addr3.getHostName() << endl;
        cout << "addr4.getHostName()= " << addr4.getHostName() << endl << endl;
     
        cout << "addr1.getIpName()= " << addr1.getIpName() << endl;
        cout << "addr2.getIpName()= " << addr2.getIpName() << endl;
        cout << "addr3.getIpName()= " << addr3.getIpName() << endl;
        cout << "addr4.getIpName()= " << addr4.getIpName() << endl << endl;
     
        cout << "addr1.getIp4Address()= " << dec << addr1.getIp4Address()
             << " HEX=" << hex << addr1.getIp4Address() << endl;
        cout << "addr2.getIp4Address()= " << dec << addr2.getIp4Address() 
             << " HEX=" << hex << addr2.getIp4Address() << endl;
        cout << "addr3.getIp4Address()= " << dec << addr3.getIp4Address() 
             << " HEX=" << hex << addr3.getIp4Address() << endl;
        cout << "addr4.getIp4Address()= " << dec << addr4.getIp4Address() 
             << " HEX=" << hex << addr4.getIp4Address() << endl << endl;
     
        cout << dec;
     
        cout << "addr1.getPort()= " << addr1.getPort() << endl;
        cout << "addr2.getPort()= " << addr2.getPort() << endl;
        cout << "addr3.getPort()= " << addr3.getPort() << endl;
        cout << "addr4.getPort()= " << addr4.getPort() << endl << endl;
     
        cout << "string(addr1)= " << string(addr1) << endl;
        cout << "string(addr2)= " << string(addr2) << endl;
        cout << "string(addr3)= " << string(addr3) << endl;
        cout << "string(addr4)= " << string(addr4) << endl << endl;
     
        cout << "ostream addr1= " << addr1 << endl;
        cout << "ostream addr2= " << addr2 << endl;
        cout << "ostream addr3= " << addr3 << endl;
        cout << "ostream addr4= " << addr4 << endl << endl;
    */

}
