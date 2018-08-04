#ifndef SIPURISUPPORTEDENTRY_HXX
#define SIPURISUPPORTEDENTRY_HXX
/*
 * $Id: SipUriSupportedEntry.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <string>

namespace Vocal
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
 
} // namespace Vocal


#endif
