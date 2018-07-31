#ifndef SIPFTRSUPPORTEDTABLE_HXX
#define SIPFTRSUPPORTEDTABLE_HXX

/*
* $Log: SipFtrSupportedTable.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipFtrSupportedTable.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <iostream>
#include <map>

#include "global.h"
#include "SipFtrSupportedEntry.hxx"

namespace Assist
{


class sipFtrSupportedTable
{
    public:
        sipFtrSupportedTable();
        ~sipFtrSupportedTable();
        sipFtrSupportedTable(const sipFtrSupportedTable& src);

        const sipFtrSupportedTable& operator=(const sipFtrSupportedTable& src);
        int getSize();

        SipFtrSupportedEntry getEntry(int);

    private:
        map < int, SipFtrSupportedEntry > ftrsupported;
        int size;
};
 
}

#endif
