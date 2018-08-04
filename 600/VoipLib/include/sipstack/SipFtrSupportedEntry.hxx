#ifndef SIPFTRSUPPORTEDENTRY_HXX
#define SIPFTRSUPPORTEDENTRY_HXX
/*
 * $Id: SipFtrSupportedEntry.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include <string>
#include <iostream>

#include "global.h"

namespace Vocal
{


class SipFtrSupportedEntry
{
    public:
        SipFtrSupportedEntry();
        SipFtrSupportedEntry(int, string );
        SipFtrSupportedEntry( const SipFtrSupportedEntry& src );

        const SipFtrSupportedEntry& operator=( const SipFtrSupportedEntry& src );

        ~SipFtrSupportedEntry();
        string getSnmpAdminString();
        unsigned int getFtrIndex();

    private:
        unsigned int sipFtrSupportedIndex;
        string snmpAdminString;
};
 
} // namespace Vocal

#endif
