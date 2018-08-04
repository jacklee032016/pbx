/*
* $Id: HostMatch.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <deque>
#include <string>
#include "HostMatch.hxx"
#include <iostream>
#include <cassert>
#include <set>
#include "NetworkAddress.h"
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <algorithm>

class SimpleAddress
{
    public:
        SimpleAddress(const char* addr);
        ostream& print(ostream& s) const;

        friend ostream& operator<< ( ostream& s,
                                     const SimpleAddress& obj );

        friend bool operator<( const SimpleAddress& lhs ,
                               const SimpleAddress& rhs );

        friend bool operator==( const SimpleAddress& lhs ,
                                const SimpleAddress& rhs );


    private:
        char myAddr[4];
};


bool operator<( const SimpleAddress& lhs ,
                const SimpleAddress& rhs )
{
    if (memcmp(lhs.myAddr, rhs.myAddr, 4) < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool operator==( const SimpleAddress& lhs ,
                 const SimpleAddress& rhs )
{
    if (memcmp(lhs.myAddr, rhs.myAddr, 4) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}


ostream& operator<< ( ostream& s, const SimpleAddress& obj )
{
    return obj.print(s);
}


SimpleAddress::SimpleAddress(const char* addr)
{
    memcpy(myAddr, addr, 4);
}


ostream& SimpleAddress::print(ostream& s) const
{
    unsigned int x;
    memcpy(&x, myAddr, 4);
    in_addr y;
    y.s_addr = x;
    s << inet_ntoa(y);
    return s;
}


deque < SimpleAddress > getAddrList(const string& host)
{
    deque < SimpleAddress > list;

    char buf[2048];
    struct addrinfo *res = 0;

    int retval =
        NetworkAddress::getHostByName(host.c_str(), res);

    if (retval != NetworkAddress::getHostLookupOK)
    {
        return list;
    }
    while (res) {
       if (!getnameinfo(res->ai_addr, res->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST))
               list.push_back(SimpleAddress(buf));
       res = res->ai_next;
    }
    freeaddrinfo(res);

#if 0
    char** addrPtr;

    addrPtr = ent.h_addr_list;

    while (addrPtr && *addrPtr)
    {
        list.push_back(SimpleAddress(*addrPtr));
        addrPtr++;
    }
#endif

    return list;
}

set < SimpleAddress > getAddrSet(const string& host)
{
    set < SimpleAddress > list;

    char buf[2048];

#ifdef WIN32
    // Handle the case where host is a dotted ip address; getHostByName does
    // not do this properly on WIN32.
    unsigned long addr = inet_addr(host.c_str());
    if (addr != INADDR_NONE) 
    {
	char * c = (char*)&addr;
	list.insert(SimpleAddress(c));
	return list;
    }
#endif
    struct addrinfo *res = 0;
    int retval =
        NetworkAddress::getHostByName(host.c_str(), res);

    if (retval != NetworkAddress::getHostLookupOK)
    {
        return list;
    }
    while (res) {
       if (!getnameinfo(res->ai_addr, res->ai_addrlen, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST))
               list.insert(SimpleAddress(buf));
       res = res->ai_next;
    }
    freeaddrinfo(res);
    return list;
}


bool hostsEqual(const string& first, const string& second)
{
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    if (firstSet.size() == 0)
    {
        return false;
    }

    return firstSet == secondSet;
}


int hostsCompare(const string& first, const string& second)
{
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    if (firstSet < secondSet)
    {
        return -1;
    }
    else if (firstSet > secondSet)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


bool hostsIntersect(const string& first, const string& second)
{
    set < SimpleAddress > firstSet;
    set < SimpleAddress > secondSet;

    firstSet = getAddrSet(first);
    secondSet = getAddrSet(second);

    deque < SimpleAddress > result;

    // set_intersection is part of <algorithm> .  It calculates the
    // intersection between firstSet and secondSet

    set_intersection(firstSet.begin(), firstSet.end(),
                     secondSet.begin(), secondSet.end(),
                     back_inserter(result));

    if (result.size() > 0)
    {
        return true;
    }
    else
    {
        return false;
    }

    assert(0);
    return false;
}
