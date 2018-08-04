/*
 * $Id: SipUriSupportedTable.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipUriSupportedTable.hxx"
#include <iostream>
#include <map>

using namespace Vocal;


SipUriSupportedTable::SipUriSupportedTable()
{
    string ss("sip");
    string ss1("http");
    string ss2("ftp");
    string ss3("telnet");
    string ss4("mailto");
    string ss5("mgcp");
    string ss6("h323");

    SipUriSupportedEntry x(0, ss);
    SipUriSupportedEntry x1(1, ss1);
    SipUriSupportedEntry x2(2, ss2);
    SipUriSupportedEntry x3(3, ss3);
    SipUriSupportedEntry x4(4, ss4);
    SipUriSupportedEntry x5(5, ss5);

    uriSupported[0] = x;
    uriSupported[1] = x1;
    uriSupported[2] = x2;
    uriSupported[3] = x3;
    uriSupported[4] = x4;
    uriSupported[5] = x5;
}


SipUriSupportedTable::SipUriSupportedTable(const SipUriSupportedTable& src)
        :
        uriSupported(src.uriSupported),
        size(src.size)
{
}


///

const SipUriSupportedTable&
SipUriSupportedTable::operator=(const SipUriSupportedTable& src)
{
    if (&src != this)
    {
        // do the copy
        uriSupported = src.uriSupported;
        size = src.size;
    }
    return *this;
}


SipUriSupportedTable::~SipUriSupportedTable()
{
}



int
SipUriSupportedTable::getSize()
{
    return uriSupported.size();
}


SipUriSupportedEntry
SipUriSupportedTable::getEntry(int entry)
{
    map < int, SipUriSupportedEntry > ::iterator pos;
    pos = uriSupported.begin();
    SipUriSupportedEntry spe = pos->second;
    return spe;
}
