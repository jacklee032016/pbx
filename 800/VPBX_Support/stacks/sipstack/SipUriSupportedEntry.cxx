/*
* $Log: SipUriSupportedEntry.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:10  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipUriSupportedEntry.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "SipUriSupportedEntry.hxx"

using namespace Assist;

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
