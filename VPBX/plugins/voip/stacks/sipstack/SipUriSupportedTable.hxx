#ifndef SIPURISUPPORTEDTABLE_HXX
#define SIPURISUPPORTEDTABLE_HXX
/*
* $Log: SipUriSupportedTable.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUriSupportedTable.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <iostream>
#include <map>

#include "global.h"
#include "SipUriSupportedEntry.hxx"


namespace Assist
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
 
} // namespace Assist


#endif
