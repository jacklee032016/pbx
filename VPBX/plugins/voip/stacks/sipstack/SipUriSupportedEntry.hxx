#ifndef SIPURISUPPORTEDENTRY_HXX
#define SIPURISUPPORTEDENTRY_HXX
/*
* $Log: SipUriSupportedEntry.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipUriSupportedEntry.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <string>

namespace Assist
{
    
///
class SipUriSupportedEntry
{

    public:
        ///
        SipUriSupportedEntry();
        ///
        SipUriSupportedEntry(int, string );
        ///
        ~SipUriSupportedEntry();
        ///
        string getSnmpAdminString();
        ///
        unsigned int getUriIndex();

        SipUriSupportedEntry& operator= (const SipUriSupportedEntry& src) ;

    private:
        unsigned int sipUriSupportedIndex;
        string snmpAdminString;
};
 
} 

#endif
