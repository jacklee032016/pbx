#ifndef SIPURISUPPORTEDTABLE_HXX
#define SIPURISUPPORTEDTABLE_HXX
/*
 * $Id: SipUriSupportedTable.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <iostream>
#include <map>

#include "global.h"
#include "SipUriSupportedEntry.hxx"


namespace Vocal
{
    
///
class SipUriSupportedTable
{
    public:
        ///
        SipUriSupportedTable();
        ///
        SipUriSupportedTable(const SipUriSupportedTable& src);
        ///
        const SipUriSupportedTable& operator=(const SipUriSupportedTable& src);
        ///
        ~SipUriSupportedTable();
        ///
        int getSize();
        ///
        SipUriSupportedEntry getEntry(int);

    private:

        map < int, SipUriSupportedEntry > uriSupported;
        /*SipUriMap;
          SipUriMap urisupported;*/
        int size;
};
 
} // namespace Vocal

#endif
