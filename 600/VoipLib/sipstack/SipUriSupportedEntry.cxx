/*
 * $Id: SipUriSupportedEntry.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "SipUriSupportedEntry.hxx"

using namespace Vocal;

SipUriSupportedEntry::SipUriSupportedEntry()
{
    sipUriSupportedIndex = 0;
    snmpAdminString = "";
}


SipUriSupportedEntry::SipUriSupportedEntry(int index, string snmpadmin)
{
    sipUriSupportedIndex = index;
    snmpAdminString = snmpadmin;
}


SipUriSupportedEntry& 
SipUriSupportedEntry::operator= (const SipUriSupportedEntry& src)
{
    if ( &src != this)
    {
        sipUriSupportedIndex = src.sipUriSupportedIndex;
        snmpAdminString = src.snmpAdminString;

    }
    return (*this);
}


SipUriSupportedEntry::~SipUriSupportedEntry()
{
}


string 
SipUriSupportedEntry::getSnmpAdminString()
{
    return snmpAdminString;

}


unsigned int 
SipUriSupportedEntry::getUriIndex()
{
    return sipUriSupportedIndex;
}
