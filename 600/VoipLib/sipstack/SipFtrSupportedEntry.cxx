/*
 * $Id: SipFtrSupportedEntry.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <string>
#include <iostream>
#include "SipFtrSupportedEntry.hxx"

using namespace Vocal;

SipFtrSupportedEntry::SipFtrSupportedEntry()
{
    sipFtrSupportedIndex = 0;
    snmpAdminString = "";
}


SipFtrSupportedEntry::SipFtrSupportedEntry(const SipFtrSupportedEntry& src)
        :
        sipFtrSupportedIndex(src.sipFtrSupportedIndex),
        snmpAdminString(src.snmpAdminString)
{}



SipFtrSupportedEntry::SipFtrSupportedEntry(int index, string snmpadmin)
{
    sipFtrSupportedIndex = index;
    snmpAdminString = snmpadmin;
}


SipFtrSupportedEntry::~SipFtrSupportedEntry ()
{
}



const SipFtrSupportedEntry& SipFtrSupportedEntry::operator=(
    const SipFtrSupportedEntry& src)
{
    if (&src != this )
    {
        sipFtrSupportedIndex = src.sipFtrSupportedIndex;
        snmpAdminString = src.snmpAdminString;
    }
    return *this;
}


string SipFtrSupportedEntry::getSnmpAdminString()
{
    return snmpAdminString;

}


unsigned int SipFtrSupportedEntry::getFtrIndex()
{
    return sipFtrSupportedIndex;
}
