#ifndef SIPFTRSUPPORTEDENTRY_HXX
#define SIPFTRSUPPORTEDENTRY_HXX

/*
* $Log: SipFtrSupportedEntry.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipFtrSupportedEntry.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <string>
#include <iostream>

#include "global.h"

namespace Assist
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
 
}

#endif
