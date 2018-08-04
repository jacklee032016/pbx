/*
* $Log: SipFtrSupportedTable.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipFtrSupportedTable.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include "SipFtrSupportedTable.hxx"
#include <map>

using namespace Assist;


sipFtrSupportedTable::sipFtrSupportedTable()
{
    string ss("fsip");
    string ss1("fhttp");
    string ss2("fftp");
    string ss3("ftelnet");
    string ss4("fmailto");
    string ss5("fmgcp");
    string ss6("fh323");

    SipFtrSupportedEntry x(0, ss);
    SipFtrSupportedEntry x1(1, ss1);
    SipFtrSupportedEntry x2(2, ss2);
    SipFtrSupportedEntry x3(3, ss3);
    SipFtrSupportedEntry x4(4, ss4);
    SipFtrSupportedEntry x5(5, ss5);

    ftrsupported[0] = x;
    ftrsupported[1] = x1;
    ftrsupported[2] = x2;
    ftrsupported[3] = x3;
    ftrsupported[4] = x4;
    ftrsupported[5] = x5;
}


sipFtrSupportedTable::sipFtrSupportedTable(const sipFtrSupportedTable& src)
        :
        ftrsupported(src.ftrsupported),
        size(src.size)
{}



const sipFtrSupportedTable&
sipFtrSupportedTable::operator=(const sipFtrSupportedTable& src)
{
    if (this != &src)
    {
        ftrsupported = src.ftrsupported;
        size = src.size;
    }
    return *this;
}


sipFtrSupportedTable::~sipFtrSupportedTable()
{
}


int sipFtrSupportedTable::getSize()
{
    return ftrsupported.size();
}

SipFtrSupportedEntry sipFtrSupportedTable::getEntry(int entry)
{
    map < int, SipFtrSupportedEntry > ::iterator pos;
    pos = ftrsupported.begin();
    SipFtrSupportedEntry spe = pos->second;
    return spe;
}

/*
static SipUriSupportedEntry uriEntry;
int size;
}
 
*/
