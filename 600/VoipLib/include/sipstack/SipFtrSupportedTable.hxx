#ifndef SIPFTRSUPPORTEDTABLE_HXX
#define SIPFTRSUPPORTEDTABLE_HXX
/*
 * $Id: SipFtrSupportedTable.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <iostream>
#include <map>

#include "global.h"
#include "SipFtrSupportedEntry.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
