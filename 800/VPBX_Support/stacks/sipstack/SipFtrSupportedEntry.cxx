/*
* $Log: SipFtrSupportedEntry.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipFtrSupportedEntry.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <string>
#include <iostream>
#include "SipFtrSupportedEntry.hxx"

using namespace Assist;

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

